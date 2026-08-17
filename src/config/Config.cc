#include "config/Config.hh"
#include <filesystem>
#include <fstream>
#include <stdexcept>

using json = nlohmann::json;
namespace fs = std::filesystem;

namespace {

void ApplyJsonToConfig(const json& j, Config& cfg, const fs::path& configDir) {
    const auto resolveConfigPath = [&configDir](const std::string& value) {
        if (value.empty()) return value;
        const fs::path p(value);
        if (p.is_absolute()) return p.lexically_normal().string();
        return (configDir / p).lexically_normal().string();
    };

    // --- run ---
    if (j.contains("run")) {
        auto& r = j["run"];
        cfg.run.nEvents      = r.value("n_events",      cfg.run.nEvents);
        cfg.run.driftMethod  = r.value("drift_method",  cfg.run.driftMethod);
        cfg.run.fieldType    = r.value("field_type",    cfg.run.fieldType);
        cfg.run.outputFile   = r.value("output_file",   cfg.run.outputFile);
        cfg.run.outputFormat = r.value("output_format", cfg.run.outputFormat);
        cfg.run.randomSeed   = r.value("random_seed",   cfg.run.randomSeed);
        cfg.run.outputLevel  = r.value("output_level",  cfg.run.outputLevel);
    }

    // --- gas ---
    if (j.contains("gas")) {
        auto& g = j["gas"];
        if (g.contains("components")) cfg.gas.components = g["components"].get<std::vector<std::string>>();
        if (g.contains("fractions"))  cfg.gas.fractions  = g["fractions"].get<std::vector<double>>();
        cfg.gas.pressureAtm  = g.value("pressure_atm",  cfg.gas.pressureAtm);
        cfg.gas.temperatureK = g.value("temperature_K", cfg.gas.temperatureK);
        cfg.gas.gasFile      = resolveConfigPath(g.value("gas_file",      cfg.gas.gasFile));
        cfg.gas.saveGasFile  = resolveConfigPath(g.value("save_gas_file", cfg.gas.saveGasFile));
    }

    // --- detector ---
    if (j.contains("detector")) {
        auto& d = j["detector"];
        cfg.detector.driftLengthMm = d.value("drift_length_mm", cfg.detector.driftLengthMm);
        cfg.detector.padPitchXMm   = d.value("pad_pitch_x_mm",  cfg.detector.padPitchXMm);
        cfg.detector.padPitchYMm   = d.value("pad_pitch_y_mm",  cfg.detector.padPitchYMm);
        cfg.detector.nPadsX        = d.value("n_pads_x",        cfg.detector.nPadsX);
        cfg.detector.nPadsY        = d.value("n_pads_y",        cfg.detector.nPadsY);
        cfg.detector.padSizeXMm    = d.value("pad_size_x_mm",   cfg.detector.padSizeXMm);
        cfg.detector.padSizeYMm    = d.value("pad_size_y_mm",   cfg.detector.padSizeYMm);
        if (d.contains("tpc_bounds")) {
            auto& b = d["tpc_bounds"];
            cfg.detector.tpcXminMm = b.value("x_min_mm", cfg.detector.tpcXminMm);
            cfg.detector.tpcXmaxMm = b.value("x_max_mm", cfg.detector.tpcXmaxMm);
            cfg.detector.tpcYminMm = b.value("y_min_mm", cfg.detector.tpcYminMm);
            cfg.detector.tpcYmaxMm = b.value("y_max_mm", cfg.detector.tpcYmaxMm);
            cfg.detector.tpcZminMm = b.value("z_min_mm", cfg.detector.tpcZminMm);
            cfg.detector.tpcZmaxMm = b.value("z_max_mm", cfg.detector.tpcZmaxMm);
        }
        cfg.detector.nPadsTotal = cfg.detector.nPadsX * cfg.detector.nPadsY;
    }

    // --- electronics ---
    if (j.contains("electronics")) {
        auto& e = j["electronics"];
        cfg.electronics.samplingFrequencyMHz = e.value("sampling_frequency_MHz", cfg.electronics.samplingFrequencyMHz);
        cfg.electronics.nSamples             = e.value("n_samples",              cfg.electronics.nSamples);
        cfg.electronics.gain                 = e.value("gain",                   cfg.electronics.gain);
        cfg.electronics.shapingTimeNs        = e.value("shaping_time_ns",        cfg.electronics.shapingTimeNs);
        cfg.electronics.thresholdAdc         = e.value("threshold_adc",          cfg.electronics.thresholdAdc);
        cfg.electronics.dynamicRangeMv       = e.value("dynamic_range_mV",       cfg.electronics.dynamicRangeMv);
        cfg.electronics.adcBits              = e.value("adc_bits",               cfg.electronics.adcBits);
        cfg.electronics.responseTimeNs          = e.value("response_time_ns",              cfg.electronics.responseTimeNs);
        cfg.electronics.waveformInternalStepNs  = e.value("waveform_internal_step_ns",     cfg.electronics.waveformInternalStepNs);
        cfg.electronics.waveformInternalSamples = e.value("waveform_internal_samples",     cfg.electronics.waveformInternalSamples);
        cfg.electronics.subPadsX                = e.value("subpads_x",                     cfg.electronics.subPadsX);
        cfg.electronics.subPadsY                = e.value("subpads_y",                     cfg.electronics.subPadsY);
        cfg.electronics.spreadPadsX             = e.value("spread_pads_x",                 cfg.electronics.spreadPadsX);
        cfg.electronics.spreadPadsY             = e.value("spread_pads_y",                 cfg.electronics.spreadPadsY);
        cfg.electronics.subPadMergeTimeNs       = e.value("subpad_merge_time_ns",          cfg.electronics.subPadMergeTimeNs);
        cfg.electronics.resistiveRcNsPerMm2     = e.value("resistive_rc_ns_per_mm2",       cfg.electronics.resistiveRcNsPerMm2);
        cfg.electronics.gainModel               = e.value("gain_model",                    cfg.electronics.gainModel);
        cfg.electronics.avalancheGain           = e.value("avalanche_gain",                cfg.electronics.avalancheGain);
        cfg.electronics.polyaMeanGain           = e.value("polya_mean_gain",               cfg.electronics.polyaMeanGain);
        cfg.electronics.polyaParameter          = e.value("polya_parameter",               cfg.electronics.polyaParameter);
        cfg.electronics.polyaMaxGainRatio       = e.value("polya_max_gain_ratio",          cfg.electronics.polyaMaxGainRatio);
        cfg.electronics.polyaBins               = e.value("polya_bins",                    cfg.electronics.polyaBins);
        cfg.electronics.polyaRandomSeed         = e.value("polya_random_seed",             cfg.electronics.polyaRandomSeed);
        if (e.contains("polya_gain")) {
            const auto& p = e["polya_gain"];
            cfg.electronics.polyaMeanGain     = p.value("mean_gain",       cfg.electronics.polyaMeanGain);
            cfg.electronics.polyaParameter    = p.value("polya_parameter", cfg.electronics.polyaParameter);
            cfg.electronics.polyaMaxGainRatio = p.value("max_gain_ratio",  cfg.electronics.polyaMaxGainRatio);
            cfg.electronics.polyaBins         = p.value("n_bins",          cfg.electronics.polyaBins);
            cfg.electronics.polyaRandomSeed   = p.value("random_seed",     cfg.electronics.polyaRandomSeed);
        }
        cfg.electronics.normalizeWaveforms      = e.value("normalize_waveforms",           cfg.electronics.normalizeWaveforms);
    }

    // --- generator ---
    if (j.contains("generator")) {
        auto& gen = j["generator"];
        cfg.generator.type = gen.value("type", cfg.generator.type);

        if (gen.contains("laser")) {
            auto& l = gen["laser"];
            cfg.generator.laser.model         = l.value("model", cfg.generator.laser.model);
            const double legacyWaistMm        = l.value("beam_waist_mm", cfg.generator.laser.beamWaistXMm);
            cfg.generator.laser.beamWaistXMm  = l.value("beam_waist_x_mm", legacyWaistMm);
            cfg.generator.laser.beamWaistYMm  = l.value("beam_waist_y_mm", legacyWaistMm);
            cfg.generator.laser.pulseDurationS = l.value("pulse_duration_s", cfg.generator.laser.pulseDurationS);
            cfg.generator.laser.pulseEnergyJ  = l.value("pulse_energy_J", cfg.generator.laser.pulseEnergyJ);
            cfg.generator.laser.wavelengthNm  = l.value("wavelength_nm", cfg.generator.laser.wavelengthNm);
            cfg.generator.laser.waistSMm      = l.value("waist_s_mm", cfg.generator.laser.waistSMm);
            cfg.generator.laser.m2X           = l.value("m2_x", cfg.generator.laser.m2X);
            cfg.generator.laser.m2Y           = l.value("m2_y", cfg.generator.laser.m2Y);
            cfg.generator.laser.propagationStepMm =
                l.value("propagation_step_mm", cfg.generator.laser.propagationStepMm);
            cfg.generator.laser.pointingJitterMm =
                l.value("pointing_jitter_mm", cfg.generator.laser.pointingJitterMm);
            cfg.generator.laser.maxPrimariesPerStep =
                l.value("max_primaries_per_step", cfg.generator.laser.maxPrimariesPerStep);

            if (l.contains("optics")) {
                const auto& o = l["optics"];
                cfg.generator.laser.wavelengthNm =
                    o.value("wavelength_nm", cfg.generator.laser.wavelengthNm);
                cfg.generator.laser.pulseEnergyJ =
                    o.value("pulse_energy_J", cfg.generator.laser.pulseEnergyJ);
                if (o.contains("pulse_energy_mJ")) {
                    cfg.generator.laser.pulseEnergyJ = o["pulse_energy_mJ"].get<double>() * 1.0e-3;
                }
                cfg.generator.laser.pulseDurationS =
                    o.value("pulse_duration_s", cfg.generator.laser.pulseDurationS);
                if (o.contains("pulse_duration_ns")) {
                    cfg.generator.laser.pulseDurationS = o["pulse_duration_ns"].get<double>() * 1.0e-9;
                }
                const double opticsWaistMm =
                    o.value("waist_mm", cfg.generator.laser.beamWaistXMm);
                cfg.generator.laser.beamWaistXMm =
                    o.value("waist_x_mm", opticsWaistMm);
                cfg.generator.laser.beamWaistYMm =
                    o.value("waist_y_mm", opticsWaistMm);
                cfg.generator.laser.waistSMm =
                    o.value("waist_s_mm", cfg.generator.laser.waistSMm);
                cfg.generator.laser.m2X =
                    o.value("m2_x", cfg.generator.laser.m2X);
                cfg.generator.laser.m2Y =
                    o.value("m2_y", cfg.generator.laser.m2Y);
                cfg.generator.laser.propagationStepMm =
                    o.value("propagation_step_mm", cfg.generator.laser.propagationStepMm);
                cfg.generator.laser.pointingJitterMm =
                    o.value("pointing_jitter_mm", cfg.generator.laser.pointingJitterMm);
            }

            if (l.contains("ionization")) {
                const auto& ion = l["ionization"];
                cfg.generator.laser.maxPrimariesPerStep =
                    ion.value("max_primaries_per_step", cfg.generator.laser.maxPrimariesPerStep);
                if (ion.contains("channels")) {
                    cfg.generator.laser.ionizationChannels.clear();
                    for (const auto& c : ion["channels"]) {
                        LaserIonizationChannelConfig ch;
                        ch.name = c.value("name", ch.name);
                        ch.gasComponent = c.value("gas_component", c.value("species", ch.gasComponent));
                        ch.photonOrder = c.value("photon_order", c.value("order", ch.photonOrder));
                        ch.speciesFraction = c.value("species_fraction", c.value("fraction", ch.speciesFraction));
                        if (c.contains("species_fraction_percent")) {
                            ch.speciesFraction = c["species_fraction_percent"].get<double>() / 100.0;
                        }
                        if (c.contains("fraction_percent")) {
                            ch.speciesFraction = c["fraction_percent"].get<double>() / 100.0;
                        }
                        ch.numberDensityCm3 = c.value("number_density_cm3", ch.numberDensityCm3);
                        ch.coefficient = c.value("coefficient", c.value("sigma", ch.coefficient));
                        ch.saturate = c.value("saturate", ch.saturate);
                        ch.enabled = c.value("enabled", ch.enabled);
                        cfg.generator.laser.ionizationChannels.push_back(ch);
                    }
                }
            }
            if (l.contains("ideal"))
                cfg.generator.laser.clusterDensity =
                    l["ideal"].value("cluster_density", cfg.generator.laser.clusterDensity);
            if (l.contains("position")) {
                auto& p = l["position"];
                cfg.generator.laser.zMm      = p.value("z_mm",       cfg.generator.laser.zMm);
                cfg.generator.laser.yMm      = p.value("y_mm",       cfg.generator.laser.yMm);
                cfg.generator.laser.xStartMm = p.value("x_start_mm", cfg.generator.laser.xStartMm);
                cfg.generator.laser.xEndMm   = p.value("x_end_mm",   cfg.generator.laser.xEndMm);
            }
            if (l.contains("direction")) {
                auto& d = l["direction"];
                cfg.generator.laser.dx = d.value("dx", cfg.generator.laser.dx);
                cfg.generator.laser.dy = d.value("dy", cfg.generator.laser.dy);
                cfg.generator.laser.dz = d.value("dz", cfg.generator.laser.dz);
            }
            if (l.contains("mirror")) {
                auto& m = l["mirror"];
                cfg.generator.laser.mirror.enabled      = m.value("enabled",        cfg.generator.laser.mirror.enabled);
                cfg.generator.laser.mirror.xMm          = m.value("x_mm",           cfg.generator.laser.mirror.xMm);
                cfg.generator.laser.mirror.yMm          = m.value("y_mm",           cfg.generator.laser.mirror.yMm);
                cfg.generator.laser.mirror.zMm          = m.value("z_mm",           cfg.generator.laser.mirror.zMm);
                cfg.generator.laser.mirror.radiusMm     = m.value("radius_mm",      cfg.generator.laser.mirror.radiusMm);
                cfg.generator.laser.mirror.tiltThetaDeg = m.value("tilt_theta_deg", cfg.generator.laser.mirror.tiltThetaDeg);
                cfg.generator.laser.mirror.tiltPhiDeg   = m.value("tilt_phi_deg",   cfg.generator.laser.mirror.tiltPhiDeg);
            }
        }

        if (gen.contains("track")) {
            auto& t = gen["track"];
            cfg.generator.track.particle    = t.value("particle",     cfg.generator.track.particle);
            cfg.generator.track.momentumGeV = t.value("momentum_GeV", cfg.generator.track.momentumGeV);
            if (t.contains("position")) {
                auto& p = t["position"];
                cfg.generator.track.xMm = p.value("x_mm", cfg.generator.track.xMm);
                cfg.generator.track.yMm = p.value("y_mm", cfg.generator.track.yMm);
                cfg.generator.track.zMm = p.value("z_mm", cfg.generator.track.zMm);
            }
            if (t.contains("direction")) {
                auto& d = t["direction"];
                cfg.generator.track.dx = d.value("dx", cfg.generator.track.dx);
                cfg.generator.track.dy = d.value("dy", cfg.generator.track.dy);
                cfg.generator.track.dz = d.value("dz", cfg.generator.track.dz);
            }
        }
    }
}

} // namespace

Config Config::Load(const std::string& path) {
    std::ifstream f(path);
    if (!f.is_open())
        throw std::runtime_error("Config: cannot open file: " + path);

    const fs::path configDir = fs::absolute(fs::path(path)).parent_path();
    const auto resolveConfigPath = [&configDir](const std::string& value) {
        if (value.empty()) return value;
        const fs::path p(value);
        if (p.is_absolute()) return p.lexically_normal().string();
        return (configDir / p).lexically_normal().string();
    };

    json j = json::parse(f);
    Config cfg;
    if (j.contains("extends")) {
        const std::string basePath = (j["extends"].is_string())
            ? j["extends"].get<std::string>()
            : "";
        if (basePath.empty()) {
            throw std::runtime_error("Config: extends must be a string in " + path);
        }
        cfg = Load(resolveConfigPath(basePath));
    }
    ApplyJsonToConfig(j, cfg, configDir);

    return cfg;
}
