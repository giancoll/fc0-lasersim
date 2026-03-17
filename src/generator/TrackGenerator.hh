#pragma once
#include "generator/PrimaryGenerator.hh"
#include "config/Config.hh"
#include "TRandom3.h"

namespace Garfield { class Sensor; class TrackHeed; }

class TrackGenerator : public PrimaryGenerator {
public:
    explicit TrackGenerator(const Config& cfg, Garfield::Sensor* sensor, TRandom3 *rand = nullptr);
    ~TrackGenerator() override;

    void Initialize() override;
    std::vector<PrimaryElectron> Generate() override;

private:
    TRandom3            *m_rand;
    TrackConfig         m_cfg;
    Garfield::Sensor*   m_sensor = nullptr;
    Garfield::TrackHeed* m_heed  = nullptr;
};
