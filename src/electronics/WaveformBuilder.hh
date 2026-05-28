#pragma once

#include <random>
#include <vector>

#include "config/Config.hh"
#include "detector/HATGeometry.hh"
#include "io/EventData.hh"

class WaveformBuilder {
public:
    explicit WaveformBuilder(const Config& cfg);

    WaveformEventData Build(int eventId, const AnodeEventData& anode);

private:
    enum class GainModel {
        Constant,
        Polya
    };

    struct LocalHit {
        int eramId = -1;
        double z = 0.0;
        double y = 0.0;
        double t = 0.0;
        double q = 0.0;
    };

    struct SubPadSignal {
        int eramId = -1;
        int subZ = 0;
        int subY = 0;
        double t = 0.0;
        double q = 0.0;
        int multiplicity = 0;
    };

    struct PadKey {
        int eramId = -1;
        int padZ = 0;
        int padY = 0;

        bool operator<(const PadKey& other) const {
            if (eramId != other.eramId) return eramId < other.eramId;
            if (padY != other.padY) return padY < other.padY;
            return padZ < other.padZ;
        }
    };

    std::vector<LocalHit> CollectLocalHits(const AnodeEventData& anode);
    std::vector<SubPadSignal> ClusterSubPads(const std::vector<LocalHit>& hits) const;
    std::vector<double> BuildResponseKernel() const;
    void BuildPolyaGainTable();
    double SampleAvalancheGain();

    Config m_cfg;
    HATGeometry m_geometry;
    GainModel m_gainModel = GainModel::Constant;
    std::mt19937_64 m_gainRng;
    std::uniform_real_distribution<double> m_unitDistribution{0.0, 1.0};
    std::vector<double> m_polyaCdf;
    std::vector<double> m_polyaGainRatioCenter;
};
