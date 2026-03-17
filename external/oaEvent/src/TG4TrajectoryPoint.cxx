////////////////////////////////////////////////////////////
// $Id$
//

#include <TGeoManager.h>
#include <TClass.h>
#include <TStreamerInfo.h>

#include "TOADatabase.hxx"

#include "TG4TrajectoryPoint.hxx"

ClassImp(ND::TG4TrajectoryPoint);

ND::TG4TrajectoryPoint::TG4TrajectoryPoint() 
    : fPositionX(0), fPositionY(0), fPositionZ(0), fPositionT(0),
      fMomentumX(0), fMomentumY(0), fMomentumZ(0), fVolumeNode(0) {}

ND::TG4TrajectoryPoint::~TG4TrajectoryPoint() {}

void ND::TG4TrajectoryPoint::ls(Option_t *opt) const {
    ND::ls_header(this,opt);
    std::cout << " at: (" << fPositionX
              << "," << fPositionY
              << "," << fPositionZ
              << "," << fPositionT
              << ")--> "
              << "(" << fMomentumX
              << "," << fMomentumY
              << "," << fMomentumZ
              << ")"
              << ", node: " << fVolumeNode
              << " proc: " << GetProcessType()
              << "(" << GetProcessSubtype() << ")"
              << std::endl;
}

std::string ND::TG4TrajectoryPoint::GetVolumeName(void) const {
    std::string path("none");
    try {
        TGeoManager* geom = ND::TOADatabase::Get().Geometry();
        geom->PushPath();
        geom->CdNode(fVolumeNode);
        path = gGeoManager->GetPath();
        gGeoManager->PopPath();
    }
    catch (...) {
        ND280Trace("Geometry not found");
    }
    return path;
}
