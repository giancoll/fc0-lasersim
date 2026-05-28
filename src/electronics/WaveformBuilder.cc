#include "electronics/WaveformBuilder.hh"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdint>
#include <limits>
#include <map>
#include <stdexcept>
#include <string>

namespace {

double clampPositive(double value) {
    return value > 0.0 ? value : 0.0;
}

int adcMaximum(int adcBits) {
    if (adcBits <= 0) return 0;
    if (adcBits >= 30) return (1 << 30) - 1;
    return (1 << adcBits) - 1;
}

std::string lowerCopy(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(),
                   [](unsigned char ch) { return static_cast<char>(std::tolower(ch)); });
    return value;
}

std::uint64_t defaultGainSeed(int runSeed) {
    return static_cast<std::uint64_t>(static_cast<std::int64_t>(runSeed)) +
           0x9e3779b97f4a7c15ULL;
}

} // namespace

WaveformBuilder::WaveformBuilder(const Config& cfg)
    : m_cfg(cfg),
      m_gainRng(cfg.electronics.polyaRandomSeed >= 0
                    ? static_cast<std::uint64_t>(cfg.electronics.polyaRandomSeed)
                    : defaultGainSeed(cfg.run.randomSeed)) {
    const std::string model = lowerCopy(m_cfg.electronics.gainModel);
    if (model == "constant") {
        m_gainModel = GainModel::Constant;
    } else if (model == "polya" || model == "mc_resmm_polya") {
        m_gainModel = GainModel::Polya;
        BuildPolyaGainTable();
    } else {
        throw std::runtime_error("WaveformBuilder: unknown electronics.gain_model: " +
                                 m_cfg.electronics.gainModel);
    }
}

WaveformEventData WaveformBuilder::Build(int eventId,
                                         const AnodeEventData& anode) {
    WaveformEventData out;
    out.eventId = eventId;
    out.nPrimaries = anode.nPrimaries;
    out.nSamples = std::max(0, m_cfg.electronics.nSamples);
    out.samplePeriodNs = m_cfg.electronics.samplingFrequencyMHz > 0.0
        ? static_cast<float>(1000.0 / m_cfg.electronics.samplingFrequencyMHz)
        : 0.0f;

    const int internalSamples =
        std::max(1, m_cfg.electronics.waveformInternalSamples);
    const double internalStepNs =
        std::max(1.0e-9, m_cfg.electronics.waveformInternalStepNs);
    const double adcStepNs = out.samplePeriodNs;
    const int adcMax = adcMaximum(m_cfg.electronics.adcBits);
    if (out.nSamples <= 0 || adcStepNs <= 0.0 || adcMax <= 0) return out;

    const auto hits = CollectLocalHits(anode);
    const auto subpads = ClusterSubPads(hits);
    if (subpads.empty()) return out;

    const int subPadsZ = std::max(1, m_cfg.electronics.subPadsX);
    const int subPadsY = std::max(1, m_cfg.electronics.subPadsY);
    const int spreadZ = std::max(0, m_cfg.electronics.spreadPadsX);
    const int spreadY = std::max(0, m_cfg.electronics.spreadPadsY);
    const double pitchZ = HATGeometry::kPadWidthZ + HATGeometry::kPadGap;
    const double pitchY = HATGeometry::kPadHeightY + HATGeometry::kPadGap;
    const double subPitchZ = pitchZ / subPadsZ;
    const double subPitchY = pitchY / subPadsY;
    const double rc = std::max(1.0e-9, m_cfg.electronics.resistiveRcNsPerMm2);
    const double kk = 1.0 / rc;
    const double pi = std::acos(-1.0);

    std::map<PadKey, std::vector<double>> currentWaveforms;

    for (const auto& sub : subpads) {
        const int col0 = sub.subZ / subPadsZ;
        const int row0 = sub.subY / subPadsY;
        const double z0 = (sub.subZ + 0.5) * subPitchZ;
        const double y0 = (sub.subY + 0.5) * subPitchY;
        const int t0digit = static_cast<int>(std::floor(sub.t / internalStepNs));
        if (t0digit >= internalSamples) continue;

        for (int padZ = col0 - spreadZ; padZ <= col0 + spreadZ; ++padZ) {
            if (padZ < 0 || padZ >= HATGeometry::kNPadsZ) continue;
            for (int padY = row0 - spreadY; padY <= row0 + spreadY; ++padY) {
                if (padY < 0 || padY >= HATGeometry::kNPadsY) continue;

                PadKey key{sub.eramId, padZ, padY};
                auto& wf = currentWaveforms[key];
                if (wf.empty()) wf.assign(internalSamples, 0.0);

                const double zMin = padZ * pitchZ - z0;
                const double zMax = padZ * pitchZ + HATGeometry::kPadWidthZ - z0;
                const double yMin = padY * pitchY - y0;
                const double yMax = padY * pitchY + HATGeometry::kPadHeightY - y0;

                for (int ts = std::max(0, t0digit); ts < internalSamples; ++ts) {
                    const double dt = (ts - t0digit) * internalStepNs;
                    if (dt <= 0.0) continue;
                    const double kt = dt * kk;
                    const double sqrt4kt = std::sqrt(4.0 * kt);
                    const double zTerm =
                        (zMin * std::exp(-(zMin * zMin) / (4.0 * kt)) -
                         zMax * std::exp(-(zMax * zMax) / (4.0 * kt))) *
                        (std::erf(-yMin / sqrt4kt) + std::erf(yMax / sqrt4kt));
                    const double yTerm =
                        (yMin * std::exp(-(yMin * yMin) / (4.0 * kt)) -
                         yMax * std::exp(-(yMax * yMax) / (4.0 * kt))) *
                        (std::erf(-zMin / sqrt4kt) + std::erf(zMax / sqrt4kt));
                    wf[ts] += sub.q * (kk / (8.0 * std::sqrt(pi) * std::pow(kt, 1.5))) *
                              (zTerm + yTerm);
                }
            }
        }
    }

    if (currentWaveforms.empty()) return out;

    const auto response = BuildResponseKernel();
    std::map<PadKey, std::vector<double>> shapedWaveforms;
    double maxAmplitude = 0.0;

    for (const auto& [key, current] : currentWaveforms) {
        auto& shaped = shapedWaveforms[key];
        shaped.assign(internalSamples, 0.0);
        for (int i = 0; i < internalSamples; ++i) {
            if (current[i] == 0.0) continue;
            for (int j = 0; j < static_cast<int>(response.size()) && i + j < internalSamples; ++j) {
                shaped[i + j] += current[i] * response[j];
            }
        }
        const auto maxIt = std::max_element(shaped.begin(), shaped.end());
        if (maxIt != shaped.end()) maxAmplitude = std::max(maxAmplitude, *maxIt);
    }

    const double normalization = m_cfg.electronics.normalizeWaveforms && maxAmplitude > 0.0
        ? static_cast<double>(adcMax) / maxAmplitude
        : m_cfg.electronics.gain;

    for (const auto& [key, shaped] : shapedWaveforms) {
        std::vector<int> adc(out.nSamples, 0);
        int peakAdc = 0;
        int peakSample = 0;
        bool hasSignal = false;

        for (int sample = 0; sample < out.nSamples; ++sample) {
            const double t = sample * adcStepNs;
            const int source = static_cast<int>(std::llround(t / internalStepNs));
            if (source < 0 || source >= internalSamples) continue;

            int value = static_cast<int>(std::llround(clampPositive(shaped[source]) * normalization));
            value = std::clamp(value, 0, adcMax);
            if (value < m_cfg.electronics.thresholdAdc) value = 0;
            adc[sample] = value;
            if (value > peakAdc) {
                peakAdc = value;
                peakSample = sample;
            }
            if (value > 0) hasSignal = true;
        }

        if (!hasSignal) continue;
        out.eramId.push_back(key.eramId);
        out.padZ.push_back(key.padZ);
        out.padY.push_back(key.padY);
        out.peakSample.push_back(peakSample);
        out.peakAdc.push_back(peakAdc);
        out.adcStart.push_back(static_cast<int>(out.adc.size()));
        out.adcLength.push_back(static_cast<int>(adc.size()));
        out.adc.insert(out.adc.end(), adc.begin(), adc.end());
    }

    out.nActivePads = static_cast<int>(out.padZ.size());
    return out;
}

std::vector<WaveformBuilder::LocalHit>
WaveformBuilder::CollectLocalHits(const AnodeEventData& anode) {
    std::vector<LocalHit> hits;
    hits.reserve(anode.z.size());

    for (std::size_t i = 0; i < anode.z.size(); ++i) {
        int eramId = -1;
        if (i < anode.eramId.size()) {
            eramId = anode.eramId[i];
        } else {
            eramId = m_geometry.GetERAMFromPosition(anode.y[i], anode.z[i]);
        }

        if (eramId < 0 || eramId >= HATGeometry::kNERAMs) continue;
        const auto& eram = m_geometry.GetERAM(eramId);
        if (eram.isQuartzWindow) continue;

        const double localY = anode.y[i] - eram.centerY + m_geometry.GetPadAreaHeightY() / 2.0;
        const double localZ = anode.z[i] - eram.centerZ + m_geometry.GetPadAreaWidthZ() / 2.0;
        if (localY < 0.0 || localY > m_geometry.GetPadAreaHeightY() ||
            localZ < 0.0 || localZ > m_geometry.GetPadAreaWidthZ()) {
            continue;
        }

        hits.push_back({eramId, localZ, localY, anode.t[i], SampleAvalancheGain()});
    }

    return hits;
}

std::vector<WaveformBuilder::SubPadSignal>
WaveformBuilder::ClusterSubPads(const std::vector<LocalHit>& hits) const {
    std::vector<SubPadSignal> subpads;

    const int subPadsZ = std::max(1, m_cfg.electronics.subPadsX);
    const int subPadsY = std::max(1, m_cfg.electronics.subPadsY);
    const int nSubCols = HATGeometry::kNPadsZ * subPadsZ;
    const int nSubRows = HATGeometry::kNPadsY * subPadsY;
    const double pitchZ = HATGeometry::kPadWidthZ + HATGeometry::kPadGap;
    const double pitchY = HATGeometry::kPadHeightY + HATGeometry::kPadGap;
    const double subPitchZ = pitchZ / subPadsZ;
    const double subPitchY = pitchY / subPadsY;
    const double mergeTime = std::max(0.0, m_cfg.electronics.subPadMergeTimeNs);

    for (const auto& hit : hits) {
        const int subZ = std::clamp(static_cast<int>(std::floor(hit.z / subPitchZ)), 0, nSubCols - 1);
        const int subY = std::clamp(static_cast<int>(std::floor(hit.y / subPitchY)), 0, nSubRows - 1);

        auto match = subpads.end();
        for (auto it = subpads.begin(); it != subpads.end(); ++it) {
            if (it->eramId == hit.eramId &&
                it->subZ == subZ &&
                it->subY == subY &&
                std::abs(hit.t - it->t) < mergeTime) {
                match = it;
                break;
            }
        }

        if (match == subpads.end()) {
            subpads.push_back({hit.eramId, subZ, subY, hit.t, hit.q, 1});
        } else {
            const double multiplicity = static_cast<double>(match->multiplicity);
            match->t = (match->t * multiplicity + hit.t) / (multiplicity + 1.0);
            match->q += hit.q;
            match->multiplicity += 1;
        }
    }

    return subpads;
}

void WaveformBuilder::BuildPolyaGainTable() {
    const int nBins = m_cfg.electronics.polyaBins;
    const double theta = m_cfg.electronics.polyaParameter;
    const double maxGainRatio = m_cfg.electronics.polyaMaxGainRatio;

    if (nBins < 2) {
        throw std::runtime_error("WaveformBuilder: electronics.polya_gain.n_bins must be >= 2");
    }
    if (theta <= -1.0) {
        throw std::runtime_error(
            "WaveformBuilder: electronics.polya_gain.polya_parameter must be > -1");
    }
    if (maxGainRatio <= 0.0) {
        throw std::runtime_error(
            "WaveformBuilder: electronics.polya_gain.max_gain_ratio must be > 0");
    }
    if (m_cfg.electronics.polyaMeanGain <= 0.0) {
        throw std::runtime_error(
            "WaveformBuilder: electronics.polya_gain.mean_gain must be > 0");
    }

    m_polyaCdf.assign(nBins, 0.0);
    m_polyaGainRatioCenter.assign(nBins, 0.0);

    const double norm = (theta + 1.0) / std::tgamma(theta + 1.0);
    double cumulative = 0.0;
    for (int i = 0; i < nBins; ++i) {
        const double ratio = static_cast<double>(i) * maxGainRatio /
                             static_cast<double>(nBins);
        const double density =
            norm * std::pow((theta + 1.0) * ratio, theta) *
            std::exp(-(theta + 1.0) * ratio);

        if (std::isfinite(density) && density > 0.0) {
            cumulative += density;
        }
        m_polyaCdf[i] = cumulative;
        m_polyaGainRatioCenter[i] =
            ratio + maxGainRatio / (2.0 * static_cast<double>(nBins));
    }

    if (cumulative <= 0.0 || !std::isfinite(cumulative)) {
        throw std::runtime_error(
            "WaveformBuilder: failed to build a valid Polya gain integral");
    }

    for (auto& value : m_polyaCdf) {
        value /= cumulative;
    }
    m_polyaCdf.back() = 1.0;
}

double WaveformBuilder::SampleAvalancheGain() {
    if (m_gainModel == GainModel::Constant) {
        return clampPositive(m_cfg.electronics.avalancheGain);
    }

    const double u = m_unitDistribution(m_gainRng);
    for (std::size_t i = 1; i < m_polyaCdf.size(); ++i) {
        if (u > m_polyaCdf[i - 1] && u < m_polyaCdf[i]) {
            return clampPositive(
                std::floor(m_polyaGainRatioCenter[i - 1] *
                           m_cfg.electronics.polyaMeanGain));
        }
    }

    return 0.0;
}

std::vector<double> WaveformBuilder::BuildResponseKernel() const {
    const double internalStepNs =
        std::max(1.0e-9, m_cfg.electronics.waveformInternalStepNs);
    const double tauSamples =
        std::max(1.0e-9, m_cfg.electronics.responseTimeNs / internalStepNs);
    const int internalSamples =
        std::max(1, m_cfg.electronics.waveformInternalSamples);
    const int kernelSamples =
        std::clamp(static_cast<int>(std::ceil(12.0 * tauSamples)), 1, internalSamples);

    std::vector<double> kernel(kernelSamples, 0.0);
    for (int j = 0; j < kernelSamples; ++j) {
        kernel[j] =
            3.0 * (std::exp(-static_cast<double>(j) / tauSamples) +
                   std::exp(-0.75 * static_cast<double>(j) / tauSamples) *
                   (std::sin((std::sqrt(7.0) * j) / (4.0 * tauSamples)) / std::sqrt(7.0) -
                    std::cos((std::sqrt(7.0) * j) / (4.0 * tauSamples))));
    }
    return kernel;
}
