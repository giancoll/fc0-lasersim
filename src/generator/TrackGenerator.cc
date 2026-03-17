#include "generator/TrackGenerator.hh"
#include <stdexcept>
#include <iostream>
#include <map>

#include <Garfield/TrackHeed.hh>
#include <Garfield/Sensor.hh>

// Map user-friendly names to Heed particle strings
static const std::map<std::string, std::string> kParticleMap = {
    {"muon",     "mu-"},
    {"muon+",    "mu+"},
    {"pion",     "pi-"},
    {"pion+",    "pi+"},
    {"proton",   "proton"},
    {"electron", "e-"},
    {"positron", "e+"},
};

TrackGenerator::TrackGenerator(const Config& cfg, Garfield::Sensor* sensor, TRandom3 *rand)
    : m_cfg(cfg.generator.track), m_sensor(sensor), m_rand(rand) {}

TrackGenerator::~TrackGenerator() {
    delete m_heed;
}

void TrackGenerator::Initialize() {
    m_heed = new Garfield::TrackHeed();
    m_heed->SetSensor(m_sensor);

    // Resolve particle name
    std::string heedName = m_cfg.particle;
    auto it = kParticleMap.find(m_cfg.particle);
    if (it != kParticleMap.end()) heedName = it->second;

    m_heed->SetParticle(heedName);
    m_heed->SetMomentum(m_cfg.momentumGeV * 1e9); // eV

    // Enable relevant physics processes
    m_heed->EnableMagneticField();
    m_heed->EnableElectricField();
    m_heed->EnableDeltaElectronTransport();
    m_heed->SetSteppingLimits(100., 1000., 0.1, 0.01);

    std::cout << "[TrackGenerator] Initialized: "
              << m_cfg.particle << " (Heed: " << heedName << ")"
              << " @ " << m_cfg.momentumGeV << " GeV/c\n";
}

std::vector<PrimaryElectron> TrackGenerator::Generate() {
    std::vector<PrimaryElectron> primaries;

    // Shoot the track (Garfield++/Heed uses cm; convert from mm)
    m_heed->NewTrack(m_cfg.xMm / 10., m_cfg.yMm / 10., m_cfg.zMm / 10., 0.,
                     m_cfg.dx, m_cfg.dy, m_cfg.dz);

    double xc, yc, zc, tc, ec, extra;
    int nc = 0;

    // Iterate over ionisation clusters along the track (Garfield++ returns cm; convert to mm)
    while (m_heed->GetCluster(xc, yc, zc, tc, nc, ec, extra)) {
        for (int i = 0; i < nc; ++i) {
            double xe, ye, ze, te, ee, dxe, dye, dze;
            m_heed->GetElectron(i, xe, ye, ze, te, ee, dxe, dye, dze);
            primaries.push_back({xe * 10., ye * 10., ze * 10., te, ee});
        }
    }

    std::cout << "[TrackGenerator] Generated " << primaries.size()
              << " primary electrons\n";
    return primaries;
}
