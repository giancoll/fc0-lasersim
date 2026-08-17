#include "generator/LaserGenerator.hh"
#include <algorithm>
#include <cctype>
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <TRandom3.h>

// Physical constants
static constexpr double kAtmPa = 101325.0;
static constexpr double kBoltzmannJPerK = 1.380649e-23;
static constexpr double kLn2 = 0.6931471805599453;

LaserGenerator::LaserGenerator(const Config& cfg, TRandom3 *rand)
    : m_cfg(cfg.generator.laser),
      m_gasCfg(cfg.gas),
      m_detCfg(cfg.detector),
      m_rand(rand) {}

LaserGenerator::~LaserGenerator() {}

void LaserGenerator::Initialize() {
    std::cout << "[LaserGenerator] Initialized: \n"
              << "  Laser position: " << m_cfg.xStartMm << " mm (start), "
              << m_cfg.yMm << " mm, " << m_cfg.zMm << " mm\n"
              << "  Beam direction: (" << m_cfg.dx << ", " << m_cfg.dy << ", " << m_cfg.dz << ")\n"
              << "  Model: " << m_cfg.model << "\n";
    if (m_cfg.model == "ideal") {
        std::cout << "  Cluster density: " << m_cfg.clusterDensity << " primaries/mm\n";
    } else if (m_cfg.model == "mpi") {
        std::cout << "  Optics: lambda=" << m_cfg.wavelengthNm
                  << " nm, E=" << m_cfg.pulseEnergyJ
                  << " J, tau=" << m_cfg.pulseDurationS
                  << " s, w0=(" << m_cfg.beamWaistXMm
                  << "," << m_cfg.beamWaistYMm
                  << ") mm, M2=(" << m_cfg.m2X
                  << "," << m_cfg.m2Y << ")\n";
        std::cout << "  Ionization channels: "
                  << m_cfg.ionizationChannels.size() << "\n";
    }

    if (m_cfg.mirror.enabled) {
        const TVector3 n = MirrorNormal();
        std::cout << "[LaserGenerator] Mirror: x=" << m_cfg.mirror.xMm << " mm, "
                  << "r=" << m_cfg.mirror.radiusMm << " mm, "
                  << "theta=" << m_cfg.mirror.tiltThetaDeg << " deg, "
                  << "phi="   << m_cfg.mirror.tiltPhiDeg   << " deg, "
                  << "normal=(" << n.X() << "," << n.Y() << "," << n.Z() << ")\n";
    }
}

double LaserGenerator::MeanPrimaries() const {
    const TVector3 origin(m_cfg.xStartMm, m_cfg.yMm, m_cfg.zMm);
    const TVector3 dir = TVector3(m_cfg.dx, m_cfg.dy, m_cfg.dz).Unit();
    const double len = (RayTpcExit(origin, dir) - origin).Mag();
    if (m_cfg.model == "ideal") return std::max(0.0, len * m_cfg.clusterDensity);
    return 0.0;
}

// ── Geometry helpers ──────────────────────────────────────────────────────────

TVector3 LaserGenerator::MirrorNormal() const {
    // Spherical convention: theta=0 → normal along +X (drift axis).
    // theta rotates away from X; phi is the azimuthal angle in the YZ plane.
    const double theta = m_cfg.mirror.tiltThetaDeg * M_PI / 180.;
    const double phi   = m_cfg.mirror.tiltPhiDeg   * M_PI / 180.;
    return TVector3(std::cos(theta),
                    std::sin(theta) * std::sin(phi),
                    std::sin(theta) * std::cos(phi)).Unit();
}

TVector3 LaserGenerator::Reflect(const TVector3& dir,
                                  const TVector3& normal) const {
    // d_ref = d_in - 2*(d_in·n)*n   (standard specular reflection)
    return dir - 2. * dir.Dot(normal) * normal;
}

bool LaserGenerator::RayMirrorIntersect(const TVector3& origin, const TVector3& dir,
                                         TVector3& hitPt) const {
    const MirrorConfig& m = m_cfg.mirror;
    const TVector3 n      = MirrorNormal();
    const TVector3 centre(m.xMm, m.yMm, m.zMm);

    const double denom = dir.Dot(n);
    if (std::abs(denom) < 1e-10) return false;  // beam parallel to mirror plane

    const double t = (centre - origin).Dot(n) / denom;
    if (t <= 0.) return false;  // mirror is behind the beam origin

    hitPt = origin + t * dir;

    // Check whether the hit point lies within the circular disk
    return (hitPt - centre).Mag2() <= m.radiusMm * m.radiusMm;
}

TVector3 LaserGenerator::RayTpcExit(const TVector3& origin,
                                     const TVector3& dir) const {
    // TPC bounding box: X=drift [xStart,xEnd], Y=vertical, Z=left-right
    const double walls[3][2] = {
        {m_detCfg.tpcXminMm, m_detCfg.tpcXmaxMm},
        {m_detCfg.tpcYminMm, m_detCfg.tpcYmaxMm},
        {m_detCfg.tpcZminMm, m_detCfg.tpcZmaxMm}
    };
    std::cout << "[RayTpcExit] Ray origin: (" << origin.X() << ", " << origin.Y() << ", " << origin.Z() << ")\n";
    std::cout << "[RayTpcExit] Ray direction: (" << dir.X() << ", " << dir.Y() << ", " << dir.Z() << ")\n";
    std::cout << "[RayTpcExit] TPC bounds: X=[" << walls[0][0] << "," << walls[0][1] << "], "
              << "Y=[" << walls[1][0] << "," << walls[1][1] << "], "
              << "Z=[" << walls[2][0] << "," << walls[2][1] << "]\n";
    const double orig[3] = {origin.X(), origin.Y(), origin.Z()};
    const double d[3]    = {dir.X(),    dir.Y(),    dir.Z()};


    double tExit = 1e15;

    for (int ax = 0; ax < 3; ++ax) {
        for (int side = 0; side < 2; ++side) {
            if (std::abs(d[ax]) < 1e-12) continue;
            double t = (walls[ax][side] - orig[ax]) / d[ax];
            if (t > 1e-6 && t < tExit) tExit = t;
        }
    }

    std::cout << "[RayTpcExit] tExit: " << tExit << "\n";
    std::cout << "[RayTpcExit] Exit point: (" << origin.X() + tExit * dir.X() << ", "
              << origin.Y() + tExit * dir.Y() << ", "
              << origin.Z() + tExit * dir.Z() << ")\n";
    return origin + tExit * dir;
}

TVector3 LaserGenerator::MakePerp(const TVector3& v) const {
    TVector3 ref(0., 1., 0.);
    if (std::abs(v.Dot(ref)) > 0.9) ref.SetXYZ(1., 0., 0.);
    return v.Cross(ref).Unit();
}

void LaserGenerator::GenerateFlatSegment(const TVector3& origin, const TVector3& dir,
                                          double segLen,
                                          std::vector<PrimaryElectron>& out) {
    if (segLen <= 0.) return;
    const TVector3 d = dir.Unit();
    const double step = 1. / std::max(1.0e-12, m_cfg.clusterDensity);
    const int    n    = static_cast<int>(segLen / step);
    for (int i = 0; i < n; ++i) {
        const double s = (i + 0.5) * step;
        PrimaryElectron pe;
        pe.x    = origin.X() + s * d.X();
        pe.y    = origin.Y() + s * d.Y();
        pe.z    = origin.Z() + s * d.Z();
        pe.t    = 0.;
        pe.ekin = 0.;
        out.push_back(pe);
    }
}

std::pair<double, double> LaserGenerator::BeamRadiiMm(double opticalSMm) const {
    const double wavelengthCm = std::max(1.0e-30, m_cfg.wavelengthNm * 1.0e-7);
    const double w0xCm = std::max(1.0e-12, m_cfg.beamWaistXMm * 0.1);
    const double w0yCm = std::max(1.0e-12, m_cfg.beamWaistYMm * 0.1);
    const double sxCm = (opticalSMm - m_cfg.waistSMm) * 0.1;
    const double syCm = sxCm;
    const double zRxCm = M_PI * w0xCm * w0xCm /
                         (std::max(1.0e-12, m_cfg.m2X) * wavelengthCm);
    const double zRyCm = M_PI * w0yCm * w0yCm /
                         (std::max(1.0e-12, m_cfg.m2Y) * wavelengthCm);
    const double wxCm = w0xCm * std::sqrt(1.0 + (sxCm / zRxCm) * (sxCm / zRxCm));
    const double wyCm = w0yCm * std::sqrt(1.0 + (syCm / zRyCm) * (syCm / zRyCm));
    return {wxCm * 10.0, wyCm * 10.0};
}

double LaserGenerator::PeakIntensityWPerCm2(double opticalSMm) const {
    if (m_cfg.pulseEnergyJ <= 0.0 || m_cfg.pulseDurationS <= 0.0) return 0.0;

    const auto [wxMm, wyMm] = BeamRadiiMm(opticalSMm);
    const double wxCm = std::max(1.0e-12, wxMm * 0.1);
    const double wyCm = std::max(1.0e-12, wyMm * 0.1);
    const double w0xCm = std::max(1.0e-12, m_cfg.beamWaistXMm * 0.1);
    const double w0yCm = std::max(1.0e-12, m_cfg.beamWaistYMm * 0.1);

    const double temporalNorm = std::sqrt(M_PI / (4.0 * kLn2));
    const double waistI0 = 2.0 * m_cfg.pulseEnergyJ /
        (M_PI * w0xCm * w0yCm * m_cfg.pulseDurationS * temporalNorm);
    return waistI0 * (w0xCm / wxCm) * (w0yCm / wyCm);
}

double LaserGenerator::TotalGasNumberDensityCm3() const {
    const double pressurePa = std::max(0.0, m_gasCfg.pressureAtm) * kAtmPa;
    const double temperatureK = std::max(1.0e-12, m_gasCfg.temperatureK);
    return pressurePa / (kBoltzmannJPerK * temperatureK) / 1.0e6;
}

double LaserGenerator::ChannelNumberDensityCm3(
        const LaserIonizationChannelConfig& channel) const {
    if (channel.numberDensityCm3 > 0.0) return channel.numberDensityCm3;

    double fraction = channel.speciesFraction;
    if (fraction <= 0.0 && !channel.gasComponent.empty()) {
        auto lower = [](std::string value) {
            std::transform(value.begin(), value.end(), value.begin(),
                           [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
            return value;
        };
        const std::string target = lower(channel.gasComponent);
        const size_t n = std::min(m_gasCfg.components.size(), m_gasCfg.fractions.size());
        for (size_t i = 0; i < n; ++i) {
            if (lower(m_gasCfg.components[i]) == target) {
                fraction = m_gasCfg.fractions[i] / 100.0;
                break;
            }
        }
    }

    if (fraction <= 0.0) return 0.0;
    return fraction * TotalGasNumberDensityCm3();
}

bool LaserGenerator::InTpcBounds(const TVector3& point) const {
    return point.X() >= m_detCfg.tpcXminMm && point.X() <= m_detCfg.tpcXmaxMm &&
           point.Y() >= m_detCfg.tpcYminMm && point.Y() <= m_detCfg.tpcYmaxMm &&
           point.Z() >= m_detCfg.tpcZminMm && point.Z() <= m_detCfg.tpcZmaxMm;
}

void LaserGenerator::GenerateMpiSegment(const TVector3& origin, const TVector3& dir,
                                        double segLen, double opticalS0Mm,
                                        std::vector<PrimaryElectron>& out) {
    if (segLen <= 0.0) return;
    if (m_cfg.ionizationChannels.empty()) return;

    const double stepMm = std::max(1.0e-6, m_cfg.propagationStepMm);
    const int nSteps = std::max(1, static_cast<int>(std::ceil(segLen / stepMm)));
    const TVector3 d = dir.Unit();
    const TVector3 e1 = MakePerp(d);
    const TVector3 e2 = d.Cross(e1).Unit();

    for (int i = 0; i < nSteps; ++i) {
        const double s0 = i * stepMm;
        if (s0 >= segLen) break;
        const double thisStepMm = std::min(stepMm, segLen - s0);
        const double sCenterMm = s0 + 0.5 * thisStepMm;
        const double opticalSMm = opticalS0Mm + sCenterMm;
        const TVector3 beamCenter = origin + sCenterMm * d;
        const auto [wxMm, wyMm] = BeamRadiiMm(opticalSMm);
        const double intensity = PeakIntensityWPerCm2(opticalSMm);
        if (intensity <= 0.0) continue;

        const double wxCm = wxMm * 0.1;
        const double wyCm = wyMm * 0.1;
        const double dsCm = thisStepMm * 0.1;

        for (const auto& channel : m_cfg.ionizationChannels) {
            if (!channel.enabled || channel.photonOrder <= 0 ||
                channel.coefficient <= 0.0) {
                continue;
            }

            const int order = channel.photonOrder;
            const double densityCm3 = ChannelNumberDensityCm3(channel);
            if (densityCm3 <= 0.0) continue;

            const double timeFactor = m_cfg.pulseDurationS *
                std::sqrt(M_PI / (4.0 * order * kLn2));
            const double areaCm2 = M_PI * wxCm * wyCm / (2.0 * order);
            const double channelIntegral = channel.coefficient *
                std::pow(intensity, order) * timeFactor;

            double mean = densityCm3 * channelIntegral * areaCm2 * dsCm;
            if (channel.saturate) {
                mean = std::min(mean, densityCm3 * areaCm2 * dsCm);
            }
            if (!std::isfinite(mean) || mean < 0.0) {
                throw std::runtime_error("LaserGenerator: non-finite MPI mean for channel " +
                                         channel.name);
            }
            if (mean <= 0.0) continue;

            int nElectrons = static_cast<int>(std::llround(m_rand->PoissonD(mean)));
            if (m_cfg.maxPrimariesPerStep > 0 &&
                nElectrons > m_cfg.maxPrimariesPerStep) {
                std::cerr << "[LaserGenerator] WARNING: channel " << channel.name
                          << " generated " << nElectrons
                          << " electrons in one step; capping at "
                          << m_cfg.maxPrimariesPerStep << "\n";
                nElectrons = m_cfg.maxPrimariesPerStep;
            }

            const double sigmaU = wxMm / (2.0 * std::sqrt(order));
            const double sigmaV = wyMm / (2.0 * std::sqrt(order));
            const double sigmaTNs =
                m_cfg.pulseDurationS * 1.0e9 /
                (2.0 * std::sqrt(2.0 * order * kLn2));

            for (int ie = 0; ie < nElectrons; ++ie) {
                const double jitterU = (m_cfg.pointingJitterMm > 0.0)
                    ? m_rand->Gaus(0.0, m_cfg.pointingJitterMm) : 0.0;
                const double jitterV = (m_cfg.pointingJitterMm > 0.0)
                    ? m_rand->Gaus(0.0, m_cfg.pointingJitterMm) : 0.0;
                const double u = m_rand->Gaus(jitterU, sigmaU);
                const double v = m_rand->Gaus(jitterV, sigmaV);
                const TVector3 point = beamCenter + u * e1 + v * e2;
                if (!InTpcBounds(point)) continue;

                PrimaryElectron pe;
                pe.x = point.X();
                pe.y = point.Y();
                pe.z = point.Z();
                pe.t = m_rand->Gaus(0.0, sigmaTNs);
                pe.ekin = 0.0;
                out.push_back(pe);
            }
        }
    }
}

void LaserGenerator::GenerateSegment(const TVector3& origin, const TVector3& dir,
                                     double segLen, double opticalS0Mm,
                                     std::vector<PrimaryElectron>& out) {
    if (m_cfg.model == "ideal") {
        GenerateFlatSegment(origin, dir, segLen, out);
    } else if (m_cfg.model == "mpi") {
        GenerateMpiSegment(origin, dir, segLen, opticalS0Mm, out);
    } else {
        throw std::runtime_error("LaserGenerator: unknown laser.model: " + m_cfg.model);
    }
}

// ── Event generation ──────────────────────────────────────────────────────────

std::vector<PrimaryElectron> LaserGenerator::Generate() {
    std::vector<PrimaryElectron> primaries;
    m_truePosition.clear();
    m_trueDirection.clear();

    const TVector3 origin(m_cfg.xStartMm, m_cfg.yMm, m_cfg.zMm);
    const TVector3 dir1 = TVector3(m_cfg.dx, m_cfg.dy, m_cfg.dz).Unit();

    TVector3 hitPt;
    if (m_cfg.mirror.enabled && RayMirrorIntersect(origin, dir1, hitPt)) {
        // Segment 1: quartz window → mirror
        const double seg1Len = (hitPt - origin).Mag();
        std::cout << "[LaserGenerator] Beam hits mirror at (" << hitPt.X() << ", " << hitPt.Y() << ", " << hitPt.Z() << ")\n";
        std::cout << "[LaserGenerator] Segment 1 length: " << seg1Len << " mm\n";
        m_truePosition.push_back(origin);
        m_trueDirection.push_back(dir1);
        GenerateSegment(origin, dir1, seg1Len, 0.0, primaries);

        // Segment 2: mirror → TPC wall (specularly reflected)
        const TVector3 dir2   = Reflect(dir1, MirrorNormal());
        m_truePosition.push_back(hitPt);
        m_trueDirection.push_back(dir2);
        const TVector3 exitPt = RayTpcExit(hitPt, dir2);
        m_truePosition.push_back(exitPt);
        m_trueDirection.push_back(dir2);
        const double seg2Len  = (exitPt - hitPt).Mag();
        std::cout << "[LaserGenerator] Exit point: (" << exitPt.X() << ", " << exitPt.Y() << ", " << exitPt.Z() << ")\n";
        std::cout << "[LaserGenerator] Segment 2 length: " << seg2Len << " mm\n";
        GenerateSegment(hitPt, dir2, seg2Len, seg1Len, primaries);

        std::cout << "[LaserGenerator] Segments: "
                    << seg1Len << " + " << seg2Len << " mm  →  "
                    << primaries.size() << " primaries\n";
    } else {
        // Beam missed the mirror — warn and fall back to straight path
        if (m_cfg.mirror.enabled) {
            std::cerr << "[LaserGenerator] WARNING: beam did not hit mirror "
                            "(check position/direction/radius). "
                            "Falling back to straight beam to TPC exit.\n";
        }
        const TVector3 exitPt = RayTpcExit(origin, dir1);
        m_truePosition.push_back(origin);
        m_trueDirection.push_back(dir1);
        m_truePosition.push_back(exitPt);
        m_trueDirection.push_back(dir1);
        const double len = (exitPt - origin).Mag();
        std::cout << "[LaserGenerator] Exit point: (" << exitPt.X() << ", " << exitPt.Y() << ", " << exitPt.Z() << ")\n";
        std::cout << "[LaserGenerator] Segment length: " << len << " mm\n";

        GenerateSegment(origin, dir1, len, 0.0, primaries);
        std::cout << "[LaserGenerator] Segment generated "
                  << primaries.size() << " primaries\n";
    }

    return primaries;
}
