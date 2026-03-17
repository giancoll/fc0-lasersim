#pragma once
#include <vector>
#include "config/Config.hh"

// Forward declarations
namespace Garfield {
    class Sensor;
    class AvalancheMicroscopic;
    class AvalancheMC;
    class DriftLineRKF;
}

/// Holds the drift endpoint of a single electron after transport.
struct DriftPoint {
    double x, y, z, t; // final position and time
    int    status;      // Garfield status code
};

/// Wraps Garfield++ electron drift, supporting three methods:
///   - "RKF"    : DriftLineRKF  — fast, deterministic
///   - "MC"     : AvalancheMC   — Monte Carlo, includes diffusion
///   - "Simple" : AvalancheMicroscopic — full microscopic tracking
class DriftEngine {
public:
    DriftEngine() = default;
    ~DriftEngine();

    void Initialize(Garfield::Sensor* sensor, const Config& cfg);

    /// Drift a single electron from (x,y,z) at time t.
    /// Returns the endpoint(s); MC/microscopic can produce multiple
    /// electrons after amplification.
    std::vector<DriftPoint> DriftElectron(double x, double y, double z, double t);

    bool IsInitialized() const { return m_initialized; }

private:
    Garfield::AvalancheMicroscopic* m_avalanche = nullptr;
    Garfield::AvalancheMC*          m_avMC      = nullptr;
    Garfield::DriftLineRKF*         m_rkf       = nullptr;

    std::string m_method;
    bool        m_initialized = false;
};
