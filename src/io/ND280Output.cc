#include "io/ND280Output.hh"
#include "io/TG4HitSegmentWriter.hh"

#include "TG4Trajectory.hxx"
#include "TG4PrimaryVertex.hxx"
#include "TG4PrimaryParticle.hxx"

#include <stdexcept>
#include <iostream>

#include <TFile.h>
#include <TTree.h>
#include <TLorentzVector.h>
#include <TVector3.h>

ND280Output::~ND280Output() {
    if (m_file && m_file->IsOpen()) Finalize();
}

void ND280Output::Initialize(const std::string& filename, int outputLevel,
                              const Config& cfg) {
    m_cfg = cfg;  // Store config for potential use in WriteEvent
    m_file  = new TFile(filename.c_str(), "RECREATE");
    if (!m_file || m_file->IsZombie())
        throw std::runtime_error("ND280Output: cannot create file: " + filename);

    m_tree  = new TTree("ND280Events", "ND280 Event Laser Tree");
    m_event = NULL;
    m_tree->Branch("ND280Event","ND::TND280Event",&m_event,128000,0);

    m_initialized = true;
    std::cout << "[ND280Output] Output → " << filename << "\n";

    m_geomfile = new TFile("../config/ND-geometry/7.19-baseline-2024_geom-152fe08a-4e139f2a-1f24ed5d-16993f93-f82690e8.root");
    if(!m_geomfile){
        std::cerr << "[ND280Output] Error opening geometry file.\n";
        throw std::runtime_error("ND280Output: cannot open geometry file");
    }
    m_geom = (TGeoManager*)m_geomfile->Get("ND280Geometry");
    m_convToGeom = TLorentzVector(-974.6, -788.1+17.1, -1924.75, 0); // -788.1 (center of hat) +17.1(alignement correction), time 2104.6
}

void ND280Output::WriteEvent(const ClusterEventData& data,
                              std::vector<TVector3> truePositions,
                              std::vector<TVector3> trueDirections) {
    // fill context
    ND::TND280Context context;
    context.SetRun(1);
    context.SetSubRun(0);
    context.SetEvent(data.eventId);
    //context.SetPartition(1<<16);
    context.SetPartition(ND::TND280Context::kMCData); // set MC bit to indicate this is MC data
    std::unique_ptr<ND::TND280Event> event(new ND::TND280Event(context));

    
    
    if (!event->FindDatum("truth")) {
        event->push_back(new ND::TDataVector("truth"));
    }
    if (!event->FindDatum("truth/g4Hits")) {
        event->Get<ND::TDataVector>("truth")->push_back(new ND::TDataVector("g4Hits"));
    }
    
    // fill trajectory information
    ND::TG4TrajectoryContainer *g4Traj = new ND::TG4TrajectoryContainer();
    ND::TG4Trajectory *traj = new ND::TG4Trajectory();
    traj->SetTrackId(0);
    traj->SetPDGEncoding(22);
    traj->SetParticleName("gamma");
    traj->SetParentId(-1);
    traj->SetInitialMomentum(TLorentzVector(trueDirections[0].X(), trueDirections[0].Y(), trueDirections[0].Z(), 1));
    // set true points: laser origin, mirror hit, and last point
    for (size_t i = 0; i < truePositions.size(); ++i) {
        const auto& pos = truePositions[i];
        const auto& dir = trueDirections[i];
        ND::TG4TrajectoryPoint point;
        point.SetPosition(pos.X()+m_convToGeom.X(), pos.Y()+m_convToGeom.Y(), pos.Z()+m_convToGeom.Z(), m_convToGeom.T());
        point.SetMomentum(dir.X(), dir.Y(), dir.Z());
        traj->GetTrajectoryPoints().push_back(point);
    }
    //g4Traj->push_back(traj);
    (*g4Traj)[traj->GetTrackId()] = *traj;
    //delete traj;
    event->Get<ND::TDataVector>("truth")->push_back(g4Traj);
    
    // Fill "Primary" infomration
    ND::TG4PrimaryVertexContainer* vertexContainer = new ND::TG4PrimaryVertexContainer("G4PrimVertex00","Garfield++ Primary Particle Vertex");
    ND::TG4PrimaryVertex vtx;
    std::cout << "[ND280Output] Adding primary vertex at (" << truePositions[0].X()+m_convToGeom.X() << ", " << truePositions[0].Y()+m_convToGeom.Y() << ", " << truePositions[0].Z()+m_convToGeom.Z() << ")\n";
    vtx.GetPosition().SetXYZT(truePositions[0].X()+m_convToGeom.X(), truePositions[0].Y()+m_convToGeom.Y(), truePositions[0].Z()+m_convToGeom.Z(), m_convToGeom.T());
    ND::TG4PrimaryParticle prim;
    prim.SetTrackId(0);
    prim.SetPDGCode(22);
    prim.SetTitle("photon");
    prim.GetMomentum().SetPx(trueDirections[0].X());
    prim.GetMomentum().SetPy(trueDirections[0].Y());
    prim.GetMomentum().SetPz(trueDirections[0].Z());
    vtx.GetPrimaryParticles().push_back(prim);
    vertexContainer->push_back(vtx);
    event->Get<ND::TDataVector>("truth")->push_back(vertexContainer);

    // Fill hit segments — one TG4HitSegment per primary electron
    ND::TG4HitContainer* hits = new ND::TG4HitContainer("hat");
    for (int i = 0; i < (int)data.x.size(); ++i) {
        TG4HitSegmentWriter* hit = new TG4HitSegmentWriter();
        hit->SetPrimaryId(0);
        hit->SetEnergyDeposit(20*2.68e-05/0.975); // put just enough energy for an electron: Edep = 26.8 ev
        // fixed track length of 1mm we don't really care about it for laser, we just need the middle of start and stop to be the cluster position
        hit->SetTrackLength(1.f);
        hit->SetStart(data.x[i]-1+m_convToGeom.X(), data.y[i]-1+m_convToGeom.Y(), data.z[i]-1+m_convToGeom.Z(), data.t[i]-1+m_convToGeom.T());
        hit->SetStop (data.x[i]+1+m_convToGeom.X(), data.y[i]+1+m_convToGeom.Y(), data.z[i]+1+m_convToGeom.Z(), data.t[i]+1+m_convToGeom.T());
        hit->AddContributor(0);
        hits->push_back(new ND::TG4HitSegment(*hit)); 
        delete hit;
    }
    
    std::cout << "[ND280Output] Total hits added: " << hits->size() << "\n";

    event->Get<ND::TDataVector>("truth/g4Hits")->push_back(hits);

    // Store the finished event in the tree
    m_event = event.get();
    m_tree->Fill();
    m_event = NULL;
    
}

// ─────────────────────────────────────────────────────────────────────────────

void ND280Output::WriteEvent(const AnodeEventData & data,
                              std::vector<TVector3> truePositions,
                              std::vector<TVector3> trueDirections) {

    std::cout << "[ND280Output]: There is no ND280 output at anode level." << std::endl;

}

// ─────────────────────────────────────────────────────────────────────────────
void ND280Output::Finalize() {
    if (m_file && m_file->IsOpen()) {
        m_file->WriteTObject(m_tree);
        m_file->WriteTObject(m_geom);
        m_file->Close();
        std::cout << "[ND280Output] File closed.\n";
    }
    delete m_event;
    delete m_file;
    m_file        = nullptr;
    m_tree        = nullptr;
    m_event       = nullptr;
    m_initialized = false;
}
