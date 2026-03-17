////////////////////////////////////////////////////////////
// $Id$
//
#include <TClass.h>
#include <TStreamerInfo.h>
#include <TDatabasePDG.h>

#include "TG4Trajectory.hxx"

ClassImp(ND::TG4Trajectory);
ClassImp(ND::TG4TrajectoryContainer);

ND::TG4Trajectory::TG4Trajectory() 
    : fTrackId(0), fParentId(0), 
      fParticleName("none"), fPDGEncoding(0),
      fInitialMomentumX(0), fInitialMomentumY(0), 
      fInitialMomentumZ(0), fInitialMomentumT(0) {}

ND::TG4Trajectory::~TG4Trajectory() { }

const TParticlePDG* ND::TG4Trajectory::GetParticle() const {
    return TDatabasePDG::Instance()->GetParticle(fPDGEncoding);
}

void ND::TG4Trajectory::ls(Option_t* opt) const {
    ND::ls_header(this,opt);
    int prec = std::cout.precision();
    std::cout.precision(1);
    TLorentzVector mom = GetInitialMomentum();
    std::cout << " Id: " << fTrackId
              << " " << fPDGEncoding << "(" << fParticleName << ")"
              << " Points: " << fPositions.size()
              << " E: " << mom.E()
              << " P: (" << mom.Px()
              << ", " << mom.Py()
              << ", " << mom.Pz() << ")"
              << " from: " << fParentId
              << std::endl;
    std::string option(opt);
    if (option.find("dump") != std::string::npos) {
        TROOT::IncreaseDirLevel();
        for (Points::const_iterator v 
                 = fPositions.begin();
             v != fPositions.end(); 
             ++v) {
            std::cout.precision(prec);
            v->ls(opt);
        };
        TROOT::DecreaseDirLevel();
    }
    std::cout.precision(prec);
}

int ND::TG4TrajectoryContainer::GetPrimaryId(int id) const {
    TG4TrajectoryContainer::const_iterator particle = find(id);

    // The particle doesn't exist in the trajectory container.
    if (particle == end()) return -1;
    
    for (;;) {
        int parentId = particle->second.GetParentId();
        if (parentId == 0) return particle->first;
        particle = find(parentId);
        if (particle == end()) return -1;
    }

    return -1;
}

ND::THandle<ND::TG4Trajectory>
ND::TG4TrajectoryContainer::GetTrajectory(int id) const {
    TG4TrajectoryContainer::const_iterator particle = find(id);
    
    // The particle doesn't exist in the trajectory container.
    if (particle == end()) return THandle<ND::TG4Trajectory>(NULL);
    
    ND::TG4Trajectory* val = const_cast<ND::TG4Trajectory*>(&(particle->second));
    return THandle<ND::TG4Trajectory>(val,false);
}

void ND::TG4TrajectoryContainer::ls(Option_t* opt) const {
    ND::TDatum::ls(opt);
    TROOT::IncreaseDirLevel();
    for (const_iterator v = begin();
         v != end(); 
         ++v) {
        v->second.ls(opt);
    };
    TROOT::DecreaseDirLevel();
}
