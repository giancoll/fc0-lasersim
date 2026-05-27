#include "io/ROOTOutput.hh"
#include <stdexcept>
#include <iostream>

#include <TFile.h>
#include <TTree.h>

ROOTOutput::~ROOTOutput() {
    if (m_file && m_file->IsOpen()) Finalize();
}

void ROOTOutput::Initialize(const std::string& filename, int outputLevel,
                             const Config& /*cfg*/) {
    if (outputLevel < 1 || outputLevel > 3)
        throw std::runtime_error(
            "ROOTOutput: unknown output_level " + std::to_string(outputLevel) +
            " (must be 1, 2, or 3)");

    m_level = outputLevel;
    m_file  = new TFile(filename.c_str(), "RECREATE");
    if (!m_file || m_file->IsZombie())
        throw std::runtime_error("ROOTOutput: cannot create file: " + filename);

    InitClusterTree();
    if (m_level >= 2) InitAnodeTree();
    if (m_level >= 3) InitWaveformTree();

    m_initialized = true;
    std::cout << "[ROOTOutput] Output level " << m_level
              << " (cumulative) → " << filename << "\n";
}

// ─── Cluster level ──────────────────────────────────────────────────────────────────

void ROOTOutput::InitClusterTree() {
    m_treeCluster = new TTree("clusters",
                              "Primary electrons from Heed (cluster level)");
    m_treeCluster->Branch("eventId",    &m_eventId);
    m_treeCluster->Branch("nElectrons", &m_nElectrons);
    m_treeCluster->Branch("cl_x",       &m_cl_x);
    m_treeCluster->Branch("cl_y",       &m_cl_y);
    m_treeCluster->Branch("cl_z",       &m_cl_z);
    m_treeCluster->Branch("cl_t",       &m_cl_t);
    m_treeCluster->Branch("cl_ekin",    &m_cl_ekin);
}

void ROOTOutput::WriteEvent(const ClusterEventData& data, std::vector<TVector3> /*truePositions*/, std::vector<TVector3> /*trueDirections*/) {
    if (!m_treeCluster) return;
    m_eventId    = data.eventId;
    m_nElectrons = data.nElectrons;
    m_cl_x       = data.x;
    m_cl_y       = data.y;
    m_cl_z       = data.z;
    m_cl_t       = data.t;
    m_cl_ekin    = data.ekin;
    m_treeCluster->Fill();
}

// ─── Anode level ──────────────────────────────────────────────────────────────────

void ROOTOutput::InitAnodeTree() {
    m_treeAnode = new TTree("anode",
                            "Drift endpoints on anode plane (before electronics)");
    m_treeAnode->Branch("eventId",      &m_eventId);
    m_treeAnode->Branch("nPrimaries",   &m_nPrimaries);
    m_treeAnode->Branch("nAnodeHits",   &m_nAnodeHits);
    m_treeAnode->Branch("anode_z",      &m_an_z);    // Z = left-right [mm]
    m_treeAnode->Branch("anode_y",      &m_an_y);    // Y = vertical   [mm]
    m_treeAnode->Branch("anode_t",      &m_an_t);
    m_treeAnode->Branch("anode_status", &m_an_status);
    m_treeAnode->Branch("anode_eram",   &m_an_eramId);
    m_treeAnode->Branch("anode_padZ",   &m_an_padZ); // directly-hit pad column
    m_treeAnode->Branch("anode_padY",   &m_an_padY); // directly-hit pad row
}

void ROOTOutput::WriteEvent(const AnodeEventData& data, std::vector<TVector3> /*truePositions*/, std::vector<TVector3> /*trueDirections*/) {
    if (!m_treeAnode) return;
    m_eventId    = data.eventId;
    m_nPrimaries = data.nPrimaries;
    m_nAnodeHits = data.nAnodeHits;
    m_an_z       = data.z;
    m_an_y       = data.y;
    m_an_t       = data.t;
    m_an_status  = data.status;
    m_an_eramId  = data.eramId;
    m_an_padZ    = data.padZ;
    m_an_padY    = data.padY;
    m_treeAnode->Fill();
}

// --- Waveform level ---------------------------------------------------------

void ROOTOutput::InitWaveformTree() {
    m_treeWaveform = new TTree("waveforms",
                               "Digitized per-pad waveforms after electronics response");
    m_treeWaveform->Branch("eventId",        &m_eventId);
    m_treeWaveform->Branch("nPrimaries",     &m_nPrimaries);
    m_treeWaveform->Branch("nActivePads",    &m_nActivePads);
    m_treeWaveform->Branch("nSamples",       &m_nSamples);
    m_treeWaveform->Branch("samplePeriodNs", &m_samplePeriodNs);
    m_treeWaveform->Branch("wf_eram",        &m_wf_eramId);
    m_treeWaveform->Branch("wf_padZ",        &m_wf_padZ);
    m_treeWaveform->Branch("wf_padY",        &m_wf_padY);
    m_treeWaveform->Branch("wf_peakSample",  &m_wf_peakSample);
    m_treeWaveform->Branch("wf_peakAdc",     &m_wf_peakAdc);
    m_treeWaveform->Branch("wf_adcStart",    &m_wf_adcStart);
    m_treeWaveform->Branch("wf_adcLength",   &m_wf_adcLength);
    m_treeWaveform->Branch("wf_adc",         &m_wf_adc);
}

void ROOTOutput::WriteEvent(const WaveformEventData& data,
                            std::vector<TVector3> /*truePositions*/,
                            std::vector<TVector3> /*trueDirections*/) {
    if (!m_treeWaveform) return;
    m_eventId        = data.eventId;
    m_nPrimaries     = data.nPrimaries;
    m_nActivePads    = data.nActivePads;
    m_nSamples       = data.nSamples;
    m_samplePeriodNs = data.samplePeriodNs;
    m_wf_eramId      = data.eramId;
    m_wf_padZ        = data.padZ;
    m_wf_padY        = data.padY;
    m_wf_peakSample  = data.peakSample;
    m_wf_peakAdc     = data.peakAdc;
    m_wf_adcStart    = data.adcStart;
    m_wf_adcLength   = data.adcLength;
    m_wf_adc         = data.adc;
    m_treeWaveform->Fill();
}


// ─── Common ───────────────────────────────────────────────────────────────────

void ROOTOutput::Finalize() {
    if (m_file && m_file->IsOpen()) {
        m_file->cd();
        if (m_treeCluster)  m_treeCluster->Write();
        if (m_treeAnode)    m_treeAnode->Write();
        if (m_treeWaveform) m_treeWaveform->Write();
        m_file->Close();
        std::cout << "[ROOTOutput] File closed.\n";
    }
    delete m_file;
    m_file         = nullptr;
    m_treeCluster  = nullptr;
    m_treeAnode    = nullptr;
    m_treeWaveform = nullptr;
    m_initialized  = false;
}
