#pragma once
#include <string>
#include <vector>
#include <nlohmann/json.hpp>

// ─────────────────────────────────────────────────────────────────────────────
// Sub-configuration structs — one per logical concern.
// All spatial quantities are in millimetres [mm].
// ─────────────────────────────────────────────────────────────────────────────

struct RunConfig {
    int         nEvents     = 1000;
    std::string driftMethod = "RKF";   // "RKF" | "Simple" | "MC"
    std::string fieldType   = "user";  // "user" | "constant"
    std::string outputFile  = "output.root";
    std::string outputFormat = "root"; // "root" | "nd280"
    int         randomSeed  = 42;
    int         outputLevel = 3;       // 1=cluster  2=anode  3=waveform
};

struct GasConfig {
    std::vector<std::string> components = {"Ar", "CF4", "iC4H10"};
    std::vector<double>      fractions  = {95.0, 3.0, 2.0};
    double pressureAtm  = 1.0;
    double temperatureK = 293.15;
    std::string gasFile     = "";  // path to pre-computed .gas file; empty = run Magboltz
    std::string saveGasFile = "";  // if non-empty, save Magboltz result here for reuse
};

/// Detector geometry — physical dimensions and pad layout [mm].
struct DetectorConfig {
    double driftLengthMm = 974.6;
    double padPitchXMm   = 7.0;
    double padPitchYMm   = 10.0;
    int    nPadsX        = 36;
    int    nPadsY        = 32;
    double padSizeXMm    = 6.5;
    double padSizeYMm    = 9.5;

    // TPC active volume bounds [mm]
    double tpcXminMm = 0.0;      // drift axis start
    double tpcXmaxMm = 974.6;   // drift axis end
    double tpcYminMm = -370.6;   // vertical (bottom)
    double tpcYmaxMm =  370.6;   // vertical (top)
    double tpcZminMm = -890.0;   // left-right (left)
    double tpcZmaxMm =  890.0;   // left-right (right)

    int    nPadsTotal = 0;       // derived: nPadsX * nPadsY
};

struct ElectronicsConfig {
    double samplingFrequencyMHz = 50.0;
    int    nSamples             = 511;
    double gain                 = 1000.0;
    double shapingTimeNs        = 25.0;
    int    thresholdAdc         = 10;
    double dynamicRangeMv       = 2000.0;
    int    adcBits              = 12;

    // Waveform model adapted from the original TPCMC per-pad signal code.
    double responseTimeNs           = 206.0;
    double waveformInternalStepNs   = 10.0;
    int    waveformInternalSamples  = 1500;
    int    subPadsX                 = 5;
    int    subPadsY                 = 5;
    int    spreadPadsX              = 3;
    int    spreadPadsY              = 3;
    double subPadMergeTimeNs        = 38.0;
    double resistiveRcNsPerMm2      = 70.0;
    double avalancheGain            = 1000.0;
    bool   normalizeWaveforms       = true;
};

/// Circular flat mirror near the cathode for laser beam reflection [mm].
struct MirrorConfig {
    bool   enabled      = false;
    double xMm          = 680.0;
    double yMm          = 0.0;
    double zMm          = 0.0;
    double radiusMm     = 50.0;
    double tiltThetaDeg = 45.0;  // polar angle of normal from +X [deg]
    double tiltPhiDeg   = 0.0;   // azimuthal angle of normal [deg]
};

struct LaserConfig {
    double zMm = 0., yMm = 0., xStartMm = 0., xEndMm = 720.;
    double dx = 1., dy = 0., dz = 0.;
    std::string model        = "ideal";
    double clusterDensity    = 1.0;   // mean primaries per mm of beam path
    MirrorConfig mirror;
};

struct TrackConfig {
    std::string particle    = "muon";
    double momentumGeV      = 1.0;
    double xMm = 0., yMm = 0., zMm = 0.;
    double dx = 0., dy = 0., dz = 1.;
};

struct GeneratorConfig {
    std::string type = "laser"; // "laser" | "track"
    LaserConfig  laser;
    TrackConfig  track;
};

// ─────────────────────────────────────────────────────────────────────────────
// Global configuration — single object passed everywhere in the simulation.
// ─────────────────────────────────────────────────────────────────────────────

struct Config {
    RunConfig         run;
    GasConfig         gas;
    DetectorConfig    detector;
    ElectronicsConfig electronics;
    GeneratorConfig   generator;

    /// Load all configuration from a JSON file.
    static Config Load(const std::string& path);
};
