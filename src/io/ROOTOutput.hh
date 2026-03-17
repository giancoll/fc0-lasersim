#pragma once
#include <string>
#include <vector>

#include "io/EventWriter.hh"
#include "config/Config.hh"

class TFile;
class TTree;

/// ROOT TTree backend — writes clusters, anode hits and waveforms to a .root file.
/// output_level is cumulative: level N creates all trees from 1 to N.
class ROOTOutput : public EventWriter {
public:
    ROOTOutput() = default;
    ~ROOTOutput() override;

    void Initialize(const std::string& filename, int outputLevel,
                    const Config& cfg) override;

    void WriteEvent(const ClusterEventData&  data, std::vector<TVector3> truePositions, std::vector<TVector3> trueDirections) override;
    void WriteEvent(const AnodeEventData&    data, std::vector<TVector3> truePositions, std::vector<TVector3> trueDirections) override;

    void Finalize() override;
    bool IsInitialized() const override { return m_initialized; }

private:
    void InitClusterTree();
    void InitAnodeTree();

    TFile* m_file         = nullptr;
    TTree* m_treeCluster  = nullptr;
    TTree* m_treeAnode    = nullptr;
    TTree* m_treeWaveform = nullptr;
    int    m_level        = 1;
    bool   m_initialized  = false;

    // ── branch buffers ────────────────────────────────────────────────────────

    // level 1
    int                m_eventId    = 0;
    int                m_nElectrons = 0;
    std::vector<float> m_cl_x, m_cl_y, m_cl_z, m_cl_t, m_cl_ekin;

    // level 2
    int                m_nPrimaries = 0;
    int                m_nAnodeHits = 0;
    std::vector<float> m_an_z, m_an_y, m_an_t;
    std::vector<int>   m_an_status;
    std::vector<int>   m_an_padZ, m_an_padY;
};
