#include "generator/LaserGenerator.hh"
#include <cmath>
#include <iostream>
#include <TRandom3.h>

// Physical constants
static constexpr double kHnu_eV   = 4.66;                     // photon energy at 266 nm [eV]
static constexpr double kHnu_J    = 4.66 * 1.602176634e-19;   // [J]

LaserGenerator::LaserGenerator(const Config& cfg, TRandom3 *rand)
    : m_cfg(cfg.generator.laser), m_detCfg(cfg.detector), m_rand(rand) {}

LaserGenerator::~LaserGenerator() {}

void LaserGenerator::Initialize() {
    std::cout << "[LaserGenerator] Initialized: \n"
              << "  Laser position: " << m_cfg.xStartMm << " mm (start), "
              << m_cfg.yMm << " mm, " << m_cfg.zMm << " mm\n"
              << "  Beam direction: (" << m_cfg.dx << ", " << m_cfg.dy << ", " << m_cfg.dz << ")\n"
              << "  Model: " << m_cfg.model << "\n";
    if (m_cfg.model == "ideal") std::cout << "  Cluster density: " << m_cfg.clusterDensity << " primaries/mm\n";

    if (m_cfg.mirror.enabled) {
        const TVector3 n = MirrorNormal();
        std::cout << "[LaserGenerator] Mirror: x=" << m_cfg.mirror.xMm << " mm, "
                  << "r=" << m_cfg.mirror.radiusMm << " mm, "
                  << "theta=" << m_cfg.mirror.tiltThetaDeg << " deg, "
                  << "phi="   << m_cfg.mirror.tiltPhiDeg   << " deg, "
                  << "normal=(" << n.X() << "," << n.Y() << "," << n.Z() << ")\n";
    }
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
    //const double step = 1.0;  // 1 mm step
    double step = 1. / m_cfg.clusterDensity; // step size to achieve desired cluster density
    const int    n    = static_cast<int>(segLen / step);
    for (int i = 0; i < n; ++i) {
        const double s = (i + 0.5) * step;
        PrimaryElectron pe;
        pe.x    = origin.X() + s * d.X();
        pe.y    = origin.Y() + s * d.Y();
        pe.z    = origin.Z() + s * d.Z();
        pe.t    = 0.;
        pe.ekin = 0.;
        std::cout << "[GenerateFlatSegment] Adding primary at (" << pe.x << ", " << pe.y << ", " << pe.z << ")\n";
        out.push_back(pe);
    }
}

// ── Event generation ──────────────────────────────────────────────────────────

std::vector<PrimaryElectron> LaserGenerator::Generate() {
    std::vector<PrimaryElectron> primaries;

    const TVector3 origin(m_cfg.xStartMm, m_cfg.yMm, m_cfg.zMm);
    const TVector3 dir1 = TVector3(m_cfg.dx, m_cfg.dy, m_cfg.dz).Unit();

    TVector3 hitPt;
    if (RayMirrorIntersect(origin, dir1, hitPt)) {
        // Segment 1: quartz window → mirror
        const double seg1Len = (hitPt - origin).Mag();
        std::cout << "[LaserGenerator] Beam hits mirror at (" << hitPt.X() << ", " << hitPt.Y() << ", " << hitPt.Z() << ")\n";
        std::cout << "[LaserGenerator] Segment 1 length: " << seg1Len << " mm\n";
        m_truePosition.push_back(origin);
        m_trueDirection.push_back(dir1);
        if (m_cfg.model == "ideal") GenerateFlatSegment(origin, dir1, seg1Len, primaries);

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
        if (m_cfg.model == "ideal") GenerateFlatSegment(hitPt, dir2, seg2Len, primaries);

        std::cout << "[LaserGenerator] Segments: "
                    << seg1Len << " + " << seg2Len << " mm  →  "
                    << primaries.size() << " primaries\n";
    } else {
        // Beam missed the mirror — warn and fall back to straight path
        std::cerr << "[LaserGenerator] WARNING: beam did not hit mirror "
                        "(check position/direction/radius). "
                        "Falling back to straight beam to TPC exit.\n";
        const TVector3 exitPt = RayTpcExit(origin, dir1);
        m_truePosition.push_back(origin);
        m_trueDirection.push_back(dir1);
        m_truePosition.push_back(exitPt);
        m_trueDirection.push_back(dir1);
        const double len = (exitPt - origin).Mag();
        std::cout << "[LaserGenerator] Exit point: (" << exitPt.X() << ", " << exitPt.Y() << ", " << exitPt.Z() << ")\n";
        std::cout << "[LaserGenerator] Segment length: " << len << " mm\n";

        if (m_cfg.model == "ideal") GenerateFlatSegment(origin, dir1, len, primaries);
    }

    return primaries;
}
