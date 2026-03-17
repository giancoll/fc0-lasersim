////////////////////////////////////////////////////////////
//
#include <cmath>

#include <TGeoManager.h>
#include <TGeoMatrix.h>
#include <TGeoNode.h>

#include "EoaCore.hxx"
#include "HEPUnits.hxx"
#include "THitSelection.hxx"
#include "TComboHit.hxx"
#include "TOADatabase.hxx"
#include "TGeomIdManager.hxx"

ClassImp(ND::TComboHit);

ND::TComboHit::TComboHit()
    : fHitsOpen(true), fGeomId(0),
      fCharge(0), fTime(0), fPosition(0,0,0),
      fSpread(100*unit::meter,100*unit::meter,100*unit::meter),
      fUncertainty(100*unit::meter,100*unit::meter,100*unit::meter),
      fTimeUncertainty(1*unit::ns),
      fIsXHit(false), fIsYHit(false), fIsZHit(false) {
    fHits.SetTitle("Cluster Hits");
    SetBit(kCanDelete,true);
}

ND::TComboHit::TComboHit(const ND::TComboHit& h) 
    : ND::THit(h), fHitsOpen(true), fHits(h.fHits), fGeomId(0),
      fCharge(0), fTime(0), fPosition(0,0,0),
      fSpread(100*unit::meter,100*unit::meter,100*unit::meter),
      fUncertainty(100*unit::meter,100*unit::meter,100*unit::meter),
      fTimeUncertainty(1*unit::ns),
      fIsXHit(false), fIsYHit(false), fIsZHit(false) {
    SetBit(kCanDelete,true);
}

ND::TComboHit::~TComboHit() {}

//////////////////////////////////////////////////
// Getter methods for ND::TComboHit
//////////////////////////////////////////////////

ND::TGeometryId ND::TComboHit::GetGeomId(void) const {
    if (fHitsOpen) CloseHits();
    return fGeomId;
}

const TVector3& ND::TComboHit::GetPosition(void) const {
    if (fHitsOpen) CloseHits();
    return fPosition;
}

double ND::TComboHit::GetCharge(void) const {
    if (fHitsOpen) CloseHits();
    return fCharge;
}

double ND::TComboHit::GetTime(void) const {
    if (fHitsOpen) CloseHits();
    return fTime;
}


const TVector3& ND::TComboHit::GetSpread(void) const {
    if (fHitsOpen) CloseHits();
    return fSpread;
}

const TVector3& ND::TComboHit::GetUncertainty(void) const {
    if (fHitsOpen) CloseHits();
    return fUncertainty;
}

double ND::TComboHit::GetTimeUncertainty(void) const {
    if (fHitsOpen) CloseHits();
    return fTimeUncertainty;
}

bool ND::TComboHit::IsXHit(void) const {
    if (fHitsOpen) CloseHits();
    return fIsXHit;
}

bool ND::TComboHit::IsYHit(void) const {
    if (fHitsOpen) CloseHits();
    return fIsYHit;
}

bool ND::TComboHit::IsZHit(void) const {
    if (fHitsOpen) CloseHits();
    return fIsZHit;
}

TGeoNode* ND::TComboHit::GetParentNode(int i) const {
    if (fHitsOpen) CloseHits();
    if (i<0) return NULL;
    if (i>=(int) fParentNodes.size()) return fParentNodes.back();
    return fParentNodes[i];
}

const ND::THitSelection& ND::TComboHit::GetHits(void) const {
    return fHits;
}

//////////////////////////////////////////////////
// Setter methods to add hits to the TComboHit.
//////////////////////////////////////////////////
void ND::TComboHit::AddHit(ND::THandle<ND::THit>& hit) {
    OpenHits();
    fHits.push_back(hit);
}

void ND::TComboHit::AddHitSelection(ND::THitSelection& hits) {
    for (ND::THitSelection::iterator h = hits.begin();
         h != hits.end();
         ++h) {
        AddHit(*h);
    }
}

void ND::TComboHit::OpenHits() {
    fHitsOpen = true;
}

// Calculate all of the combined values for the TComboHit.  This code is ugly
// since the averages and variances are calculated in individual variables,
// and not in matrices.  It's an artifact of how the code evolved [i.e. cope,
// or fix it.]
void ND::TComboHit::CloseHits() const {
    if (!fHitsOpen) return;
    if (fHits.size()<1) return;
    fHitsOpen = false;

    // Calculate the averages.
    fPosition.SetXYZ(0,0,0);
    fCharge = 0.0;
    fTime = 0.0;
    fSpread.SetXYZ(1,1,1);
    fUncertainty.SetXYZ(0,0,0);
    fTimeUncertainty = 1.0*unit::ns;
    fIsXHit = false;
    fIsYHit = false;
    fIsZHit = false;

// Choose between two possible weightings.  If TCOMBOHIT_CHARGE_WEIGHTING is
// set, then the charge weighted position is calculated, otherwise the average
// is weighted by the "Hessian" (i.e. 1/sigma).  If the individual hit
// uncertainties are proportional to 1/sqrt(charge), then both weightings give
// the same answer.
#define TCOMBOHIT_CHARGE_WEIGHTING        


    // Calculate the charge weighted average position.  It's a point of debate
    // as to whether this should calculate the charged weighted position, or
    // do a standard average (weight by the point uncertainty).  These two
    // averages will be the same if the fundamental position uncertainty is
    // proportional to 1/sqrt(charge).
    double weightXSum=0, weightYSum=0, weightZSum=0, weightTSum=0;
    for (ND::THitSelection::const_iterator h = fHits.begin();
         h != fHits.end();
         ++h) {
        double hitCharge = (*h)->GetCharge();
#ifdef TCOMBOHIT_CHARGE_WEIGHTING
        double weightX = hitCharge;
        double weightY = hitCharge;
        double weightZ = hitCharge;
        double weightT = hitCharge;
#else
        double weightX 
            = 1.0/((*h)->GetUncertainty().X()*(*h)->GetUncertainty().X());
        double weightY 
            = 1.0/((*h)->GetUncertainty().Y()*(*h)->GetUncertainty().Y());
        double weightZ 
            = 1.0/((*h)->GetUncertainty().Z()*(*h)->GetUncertainty().Z());
        double weightT = hitCharge;
#endif
        weightXSum += weightX;
        weightYSum += weightY;
        weightZSum += weightZ;
        weightTSum += weightT;
        fPosition[0] += weightX*(*h)->GetPosition().X();
        fPosition[1] += weightY*(*h)->GetPosition().Y();
        fPosition[2] += weightZ*(*h)->GetPosition().Z();
        fTime += weightT*(*h)->GetTime();
        fCharge += hitCharge;
        fIsXHit = fIsXHit || (*h)->IsXHit();
        fIsYHit = fIsYHit || (*h)->IsYHit();
        fIsZHit = fIsZHit || (*h)->IsZHit();
    }

    if (weightXSum>0) fPosition[0] = fPosition[0]/weightXSum;
    if (weightYSum>0) fPosition[1] = fPosition[1]/weightYSum;
    if (weightZSum>0) fPosition[2] = fPosition[2]/weightZSum;
    if (weightTSum>0) fTime = fTime/weightTSum;

    // The spread of the hits.
    double sprX=0, sprY=0, sprZ=0;
    // The minimum spread of any hit in the collection.
    double minSprX=1000*unit::mm, minSprY=1000*unit::mm, minSprZ=1000*unit::mm;
    // The position uncertainties.
    double uncXX=0, uncYY=0, uncZZ=0; 
    double uncXY=0, uncXZ=0, uncYZ=0; 
    // The weighting for the position uncertainties.
    double wghXX=0, wghYY=0, wghZZ=0; 
    double wghXY=0, wghXZ=0, wghYZ=0; 
    for (ND::THitSelection::const_iterator h = fHits.begin();
         h != fHits.end();
         ++h) {
        double dx = (*h)->GetPosition().X() - fPosition.X();
        double dy = (*h)->GetPosition().Y() - fPosition.Y();
        double dz = (*h)->GetPosition().Z() - fPosition.Z();
        double dt = (*h)->GetTime() - fTime;
#ifdef TCOMBOHIT_CHARGE_WEIGHTING        
        double sx = 1.0/sqrt(fCharge);
        double sy = 1.0/sqrt(fCharge);
        double sz = 1.0/sqrt(fCharge);
#else
        double sx = (*h)->GetUncertainty().X();
        double sy = (*h)->GetUncertainty().Y();
        double sz = (*h)->GetUncertainty().Z();
#endif
        uncXX += dx*dx/(sx*sx);
        wghXX += 1.0/(sx*sx);
        uncYY += dy*dy/(sy*sy);
        wghYY += 1.0/(sy*sy);
        uncZZ += dz*dz/(sz*sz);
        wghZZ += 1.0/(sz*sz);
        uncXY += dx*dy/(sx*sy);
        wghXY += 1.0/(sx*sy);
        uncXZ += dx*dz/(sx*sz);
        wghXZ += 1.0/(sx*sz);
        uncYZ += dy*dz/(sy*sz);
        wghYZ += 1.0/(sy*sz);
        fTimeUncertainty += (*h)->GetCharge()*dt*dt;
        sprX = std::max((*h)->GetSpread().X(),sprX);
        sprX = std::max(dx,sprX);
        sprY = std::max((*h)->GetSpread().Y(),sprY);
        sprY = std::max(dy,sprY);
        sprZ = std::max((*h)->GetSpread().Z(),sprZ);
        sprZ = std::max(dz,sprZ);
        minSprX = std::min(minSprX, (*h)->GetSpread().X());
        minSprY = std::min(minSprY, (*h)->GetSpread().Y());
        minSprZ = std::min(minSprZ, (*h)->GetSpread().Z());
    }
    fSpread.SetXYZ(sprX,sprY,sprZ);
    
    // The uncXX .. uncYZ variables will hold the average width of the
    // distribution [i.e. avg(x - avg(x))]
    if (fCharge>0) {
        uncXX /= wghXX;
        uncYY /= wghYY;
        uncZZ /= wghZZ;
        uncXY /= wghXY;
        uncXZ /= wghXZ;
        uncYZ /= wghYZ;
        fTimeUncertainty /= fCharge;
    }

    // Make sure that width of the distribution isn't smaller than the
    // smallest geometric element.  The "4" comes since minSpr is half of the
    // element size, and the "12" comes from assuming the real hit position is
    // spread uniformly over the element.
    uncXX = std::max(uncXX,4*minSprX*minSprX/12);
    uncYY = std::max(uncYY,4*minSprY*minSprY/12);
    uncZZ = std::max(uncZZ,4*minSprZ*minSprZ/12);

    // The uncXX .. uncYZ variables will now hold the variance of the average
    // position.  Normalize for the number of measurements where one unit of
    // charge is treated as one measurement.  That is the right thing for
    // photosensor based detectors (as long as the charge is measured in
    // photo-electrons), but I'm not sure if it is OK for TPC based
    // measurements.  This is separate from the weighting above which is used
    // to calculate the average width of the distribution.  The normalization
    // above accounts for the multiple measurements of the position.
    double norm = std::max(fCharge-1, double(fHits.size()-1));
    if (norm>0) {
        uncXX /= norm;
        uncYY /= norm;
        uncZZ /= norm;
        uncXY /= norm;
        uncXZ /= norm;
        uncYZ /= norm;
        fTimeUncertainty /= norm;
    }

    // Set the uncertainty to the width of the distribution.
    fUncertainty.SetXYZ(sqrt(uncXX),sqrt(uncYY),sqrt(uncZZ));
    fTimeUncertainty = sqrt(fTimeUncertainty);

    ND::TOADatabase::Get().GeomId().GetGeometryId(
        fPosition.X(), fPosition.Y(), fPosition.Z(), fGeomId);

    // Get the parent stack for this cluster.
    TGeoManager* geom = ND::TOADatabase::Get().Geometry();
    fParentNodes.clear();
    if (fHits.size()>0) {
        geom->PushPoint();
        geom->FindNode(fPosition.X(), fPosition.Y(), fPosition.Z());
        TGeoNode* lastParent = NULL;
        for (TGeoNode* parent = geom->GetCurrentNode();
             parent != lastParent;
             parent = geom->GetCurrentNode()) {
            fParentNodes.push_back(parent);
            lastParent = parent;
            geom->CdUp();
        }
        geom->PopPoint();
    }
}

/// Return a hit that is contributing to this cluster;
void ND::TComboHit::ls(Option_t *opt) const {
    ND::THit::ls(opt);
    TROOT::IncreaseDirLevel();
    TROOT::IndentLevel();
    std::cout << "Hits in collection: " << fHits.size() << std::endl;
    std::string option(opt);
    if (option.find("hits") != std::string::npos) {
        fHits.ls(opt);
    }
    TROOT::DecreaseDirLevel();
}

