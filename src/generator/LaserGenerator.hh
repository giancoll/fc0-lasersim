#pragma once
#include "generator/PrimaryGenerator.hh"
#include "config/Config.hh"
#include <cmath>
#include <vector>
#include <TVector3.h>

class TRandom3;

/// UV laser track generator — simulates (1+1) REMPI ionisation of trace
/// aromatic impurities (e.g. toluene) in the TPC gas by a 266 nm pulsed laser.
///
/// Physics:
///   dNe/dz = impurityYield * (E_lambda / (hnu * A * tau))^2
///
/// Beam modes:
///   - Straight  (mirror.enabled = false): beam travels from (zMm,yMm,xStartMm)
///     in direction (dx,dy,dz) along the drift axis X to xEndMm.
///   - Reflected (mirror.enabled = true) : two-segment path.
///       Segment 1  window → mirror  (initial direction from config)
///       Segment 2  mirror → TPC wall (specularly reflected direction)
///     The mirror is a circular flat disk; its orientation is set by
///     (tiltThetaDeg, tiltPhiDeg) and its x-position along the drift axis is
///     adjustable.  Both segments ionise according to the same dNe/dz.
///   - Fake flat  (fakeIonisation = true): 1 primary electron per mm, placed
///     exactly on beam axis — for testing/calibration (no Poisson fluctuation).
///
/// Convention: X = drift axis (anode x=0), Y = vertical, Z = left-right.
///
/// TODO: beam attenuation (absorption + scattering in gas) not yet modelled.
class LaserGenerator : public PrimaryGenerator {
public:
    explicit LaserGenerator(const Config& cfg, TRandom3 *rand);
    ~LaserGenerator() override;

    void Initialize() override;
    std::vector<PrimaryElectron> Generate() override;

    /// Expected mean primaries per event for the straight-beam case.
    /// When mirror is enabled the actual mean depends on segment geometry.
    double MeanPrimaries() const;

    std::vector<TVector3> GetTruePositions()  const override { return m_truePosition; }
    std::vector<TVector3> GetTrueDirections() const override { return m_trueDirection; }

private:
    // ── Geometry helpers ──────────────────────────────────────────────────────

    /// Mirror normal unit vector from (tiltThetaDeg, tiltPhiDeg).
    TVector3 MirrorNormal() const;

    /// Specular reflection: d_ref = d_in - 2*(d_in·n)*n
    TVector3 Reflect(const TVector3& dir, const TVector3& normal) const;

    /// Ray–mirror-disk intersection.
    /// Returns true and sets hitPt if the ray hits the circular mirror face.
    bool RayMirrorIntersect(const TVector3& origin, const TVector3& dir,
                            TVector3& hitPt) const;

    /// Walk a ray forward until it hits a TPC boundary wall.
    /// Bounds: x∈[xStart,xEnd], y∈[tpcYmin,tpcYmax], z∈[tpcZmin,tpcZmax].
    TVector3 RayTpcExit(const TVector3& origin, const TVector3& dir) const;

    /// Build a unit vector perpendicular to v (for transverse beam profile).
    TVector3 MakePerp(const TVector3& v) const;

    /// Fake-flat model: 1 primary electron per mm exactly on beam axis.
    void GenerateFlatSegment(const TVector3& origin, const TVector3& dir,
                              double segLen,
                              std::vector<PrimaryElectron>& out);

    LaserConfig    m_cfg;
    DetectorConfig m_detCfg;
    TRandom3*      m_rand = nullptr;
    std::vector<TVector3> m_truePosition;
    std::vector<TVector3> m_trueDirection;
};
