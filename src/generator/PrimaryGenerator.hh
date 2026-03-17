#pragma once
#include <vector>
#include <TVector3.h>

/// A single primary ionisation electron to be drifted.
struct PrimaryElectron {
    double x, y, z; // position [cm]
    double t;        // time [ns]
    double ekin;     // kinetic energy [eV] — typically ~0 after thermalisation
};

/// Abstract base class for all primary particle generators.
/// Concrete implementations: LaserGenerator, TrackGenerator.
///
/// Pattern: the simulation loop calls Generate() once per event.
/// The generator returns a list of primary electrons ready for drifting.
class PrimaryGenerator {
public:
    virtual ~PrimaryGenerator() = default;

    /// Generate primary electrons for one event.
    virtual std::vector<PrimaryElectron> Generate() = 0;

    /// Optional: called once before the event loop.
    virtual void Initialize() {}

    /// Optional: called once after the event loop.
    virtual void Finalize() {}

    virtual std::vector<TVector3> GetTruePositions() const { return {}; }
    virtual std::vector<TVector3> GetTrueDirections() const { return {}; }
};
