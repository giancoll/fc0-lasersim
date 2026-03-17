#pragma once
#include <string>

#include "io/EventWriter.hh"
#include "config/Config.hh"
#include "detector/HATGeometry.hh"

#include "TND280Event.hxx"

#include "TGeoManager.h"

#include "TLorentzVector.h"

class TFile;
class TTree;

/// ND280 oaAnalysis backend — writes events in the ND280 TND280Event format.
/// WriteEvent() implementations are stubs pending the ND280 data model mapping.
class ND280Output : public EventWriter {
public:
    ND280Output() = default;
    ~ND280Output() override;

    void Initialize(const std::string& filename, int outputLevel,
                    const Config& cfg) override;

    void WriteEvent(const ClusterEventData&  data,
                    std::vector<TVector3> truePositions,
                    std::vector<TVector3> trueDirections) override;

    void WriteEvent(const AnodeEventData&    data,
                    std::vector<TVector3> truePositions,
                    std::vector<TVector3> trueDirections) override;

    void Finalize() override;
    bool IsInitialized() const override { return m_initialized; }

    /// Direct access to the current TND280Event for manual filling.
    ND::TND280Event* GetEvent() { return m_event; }

    /// Set sub-run ID to stamp on each written event (default -1 = use 0).
    void SetSubRunId(int id) { m_subId = id; }

private:
    TFile*              m_file     = nullptr;
    TTree*              m_tree     = nullptr;
    ND::TND280Event*    m_event    = nullptr;
    bool                m_initialized = false;
    Config              m_cfg;
    int                 m_subId    = -1;
    TFile *             m_geomfile = nullptr;
    TGeoManager *       m_geom      = nullptr;
    TLorentzVector      m_convToGeom;
    HATGeometry         m_hatGeom;

};
