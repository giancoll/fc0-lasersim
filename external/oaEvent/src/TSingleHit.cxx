////////////////////////////////////////////////////////////
// $Id$
//
#include <cmath>

#include <TGeoManager.h>
#include <TGeoBBox.h>
#include <TVector3.h>

#include "TGeomIdManager.hxx"
#include "TSingleHit.hxx"
#include "TOADatabase.hxx"
#include "TTPCPadManager.hxx"
#include "THATPadManager.hxx"
#include "HEPUnits.hxx"
#include "ND280GeomId.hxx"

ClassImp(ND::TSingleHit);

ND::TSingleHit::TSingleHit()
    : fGeomId(0), fCharge(0), fTime(0), fChannelId(0),
      fInitialized(false),
      fPosition(0,0,0),
      fIsXHit(false), fIsYHit(false), fIsZHit(false),
      fSpread(100*unit::meter,100*unit::meter,100*unit::meter),
      fUncertainty(100*unit::meter,100*unit::meter,100*unit::meter),
      fTimeUncertainty(1*unit::ns) {
    SetBit(kCanDelete,false);
}

ND::TSingleHit::TSingleHit(const ND::TSingleHit& h)
    : ND::THit(h), fGeomId(h.fGeomId),
      fCharge(h.fCharge), fTime(h.fTime),
      fChannelId(h.fChannelId),
      fInitialized(h.fInitialized),
      fPosition(h.fPosition),
      fIsXHit(h.fIsXHit), fIsYHit(h.fIsYHit), fIsZHit(h.fIsZHit),
      fSpread(h.fSpread),
      fUncertainty(h.fUncertainty),
      fTimeUncertainty(h.fTimeUncertainty) {
    SetBit(kCanDelete,false);
}

ND::TSingleHit::~TSingleHit() { }

//////////////////////////////////////////////////
// Getter methods for ND::TSingleHit
//////////////////////////////////////////////////

ND::TGeometryId ND::TSingleHit::GetGeomId(void) const {
    return TGeometryId(fGeomId);
}

double ND::TSingleHit::GetCharge(void) const {return fCharge;}

double ND::TSingleHit::GetTime(void) const {return fTime;}

// Return the associated channel id.
ND::TChannelId ND::TSingleHit::GetChannelId(int i) const {return fChannelId;}

// Return the number of channel ids associated with this hit.
int ND::TSingleHit::GetChannelIdCount() const {
    if (!fChannelId.IsValid()) return 0;
    return 1;
}

const TVector3& ND::TSingleHit::GetPosition(void) const {
    if (!fInitialized) const_cast<ND::TSingleHit*>(this)->Initialize();
    return fPosition;
}

const TVector3& ND::TSingleHit::GetSpread(void) const {
    if (!fInitialized) const_cast<ND::TSingleHit*>(this)->Initialize();
    return fSpread;
}

const TVector3& ND::TSingleHit::GetUncertainty(void) const {
    if (!fInitialized) const_cast<ND::TSingleHit*>(this)->Initialize();
    return fUncertainty;
}

double ND::TSingleHit::GetTimeUncertainty(void) const {
    if (!fInitialized) const_cast<ND::TSingleHit*>(this)->Initialize();
    return fTimeUncertainty;
}

bool ND::TSingleHit::IsXHit(void) const {
    if (!fInitialized) const_cast<ND::TSingleHit*>(this)->Initialize();
    return fIsXHit;
}

bool ND::TSingleHit::IsYHit(void) const {
    if (!fInitialized) const_cast<ND::TSingleHit*>(this)->Initialize();
    return fIsYHit;
}

bool ND::TSingleHit::IsZHit(void) const {
    if (!fInitialized) const_cast<ND::TSingleHit*>(this)->Initialize();
    return fIsZHit;
}

bool ND::TSingleHit::InitializeTPC() {
    TGeoManager* geom = ND::TOADatabase::Get().Geometry();
    geom->PushPath();
    ND::TOADatabase::Get().GeomId().CdId(GetGeomId());
    TGeoNode* node = geom->GetCurrentNode();

    std::string nodeName(node->GetName());
    if (nodeName.find("MM_") == std::string::npos) {
        geom->PopPath();
        return false;
    }

    if (!ND::TOADatabase::Get().GeomId().GetPosition(GetGeomId(),fPosition)) {
        geom->PopPath();
        return false;
    }

    ND::TTPCPadManager& pads = ND::TOADatabase::Get().TPCPads();

    // Find the spread.
    fSpread.SetXYZ(1*unit::meter,
                   pads.GetPadYPitch()/2,
                   pads.GetPadXPitch()/2);
    fUncertainty.SetXYZ(1*unit::mm,
                        2*fSpread.Y()/std::sqrt(12.0),
                        2*fSpread.Z()/std::sqrt(12.0));
    if (HasValidTime()) {
        fTimeUncertainty = 10*unit::ns/std::sqrt(12.0);
    }
    else {
        fTimeUncertainty = 10*unit::microsecond/std::sqrt(12.0);
    }


    // Determine if the hit is an X, Y, or Z hit.
    fIsXHit = false;
    fIsYHit = true;
    fIsZHit = true;

    geom->PopPath();
    return true;
}

bool ND::TSingleHit::InitializeHAT() {
    if (!ND::GeomId::HAT::IsHAT(GetGeomId())) return false;

    TGeoManager* geom = ND::TOADatabase::Get().Geometry();
    geom->PushPath();
    ND::TOADatabase::Get().GeomId().CdId(GetGeomId());
    TGeoNode* node = geom->GetCurrentNode();

    std::string nodeName(node->GetName());
    if (nodeName.find("ERAM_") == std::string::npos) {
        geom->PopPath();
        return false;
    }

    if (!ND::TOADatabase::Get().GeomId().GetPosition(GetGeomId(),fPosition)) {
        geom->PopPath();
        return false;
    }

    ND::THATPadManager& pads = ND::TOADatabase::Get().HATPads();

    // Find the spread.
    fSpread.SetXYZ(1*unit::meter,
                   pads.GetPadYPitch()/2,
                   pads.GetPadXPitch()/2);
    fUncertainty.SetXYZ(1*unit::mm,
                        2*fSpread.Y()/std::sqrt(12.0),
                        2*fSpread.Z()/std::sqrt(12.0));
    if (HasValidTime()) {
        fTimeUncertainty = 10*unit::ns/std::sqrt(12.0);
    }
    else {
        fTimeUncertainty = 10*unit::microsecond/std::sqrt(12.0);
    }


    // Determine if the hit is an X, Y, or Z hit.
    fIsXHit = false;
    fIsYHit = true;
    fIsZHit = true;

    geom->PopPath();
    return true;
}

bool ND::TSingleHit::InitializeSFG() {
    if (!ND::GeomId::SFG::IsSFG(GetGeomId())) return false;

    bool status = InitializeGeneric();

    if (not HasValidTime()) return status;

    /// Adjust the time uncertainty for the SFG.  This is where the
    /// timewalk uncertainty needs to be added
    if (HasAlternateCharge() > 0) {
        /// FIXME: This should be the time uncertainty for TOT hits
        if (GetCharge() < 15.0) fTimeUncertainty = 10.0*unit::ns;
        else fTimeUncertainty = 2.5*unit::ns/std::sqrt(12);
    }
    else {
        /// FIXME: This should be the uncertainty when timewalk is applied.
        fTimeUncertainty = 2.5*unit::ns/std::sqrt(12);
    }

    return status;
}

bool ND::TSingleHit::InitializeGeneric() {
    TGeoManager* geom = ND::TOADatabase::Get().Geometry();
    geom->PushPath();
    ND::TOADatabase::Get().GeomId().CdId(GetGeomId());
    TGeoNode* node = geom->GetCurrentNode();

    // Find the global position
    double local[3] = {0,0,0};
    double master[3] = {0,0,0};
    geom->LocalToMaster(local,master);
    fPosition.SetXYZ(master[0],master[1],master[2]);

    // Find the spread.
    TGeoBBox *shape = dynamic_cast<TGeoBBox*>(node->GetVolume()->GetShape());
    double spread[3] = {0,0,0};
    double localX[3] = {shape->GetDX(),0,0};
    double tmp[3];
    geom->LocalToMasterVect(localX,tmp);
    for (int i=0; i<3; ++i) {
        spread[i] = std::max(spread[i],std::abs(tmp[i]));
    }
    double localY[3] = {0,shape->GetDY(),0};
    geom->LocalToMasterVect(localY,tmp);
    for (int i=0; i<3; ++i) {
        spread[i] = std::max(spread[i],std::abs(tmp[i]));
    }
    double localZ[3] = {0,0,shape->GetDZ()};
    geom->LocalToMasterVect(localZ,tmp);
    for (int i=0; i<3; ++i) {
        spread[i] = std::max(spread[i],std::abs(tmp[i]));
    }

    fSpread.SetXYZ(spread[0],spread[1],spread[2]);
    fUncertainty = fSpread*(2.0/std::sqrt(12.0*fCharge));

    // Determine if the hit is an X, Y, or Z hit.
    fIsXHit = (fSpread.X() < 10*unit::cm);
    fIsYHit = (fSpread.Y() < 10*unit::cm);
    fIsZHit = (fSpread.Z() < 10*unit::cm);

    // The SMRD scintillator is long in the Z direction, but still segmented
    // so that a single SMRD hit gives a Z position measurement.  Insteand of
    // having a generalized algorithm to figure this out, just force SMRD hits
    // to be Z hits.
    if (ND::GeomId::SMRD::IsSMRD(GetGeomId())) fIsZHit = true;

    /// The SFG scintillator fibers link to a single cube, so the spread is
    /// small.  This fixes that.
    if (ND::GeomId::SFG::IsSFG(GetGeomId())) {
        if (ND::GeomId::SFG::IsXZFiber(GetGeomId())) fIsYHit = false;
        if (ND::GeomId::SFG::IsYZFiber(GetGeomId())) fIsXHit = false;
        if (ND::GeomId::SFG::IsXYFiber(GetGeomId())) fIsZHit = false;
    }

    // Determine the timing uncertainty.
    double tick = 2.5*unit::ns; // For everything but the SFG.

    /// SFG tick was expected to be 1.25 ns, but is changed to 2.5 ns
    if (ND::GeomId::SFG::IsSFG(GetGeomId())) tick = 2.5*unit::ns;

    if (HasValidTime()) {
        fTimeUncertainty = tick/std::sqrt(12.0);
    }
    else {
        fTimeUncertainty = 580*unit::ns/std::sqrt(12.0); // A nonzero value.
    }

    geom->PopPath();

    return true;
}

void ND::TSingleHit::Initialize(void) {
#ifdef TSINGLE_HIT_SAVE_POSITION
    TVector3 origPos = fPosition;
#endif

    try {
        do {
            // Try initializations looking for the first one to work.  The
            // initializations should be ordered from the most specific to the
            // most general.
            if (InitializeTPC()) break;
            if (InitializeHAT()) break;
            if (InitializeSFG()) break;
            InitializeGeneric();
        } while (false);
        fInitialized = true;
    }
    catch (std::exception& e) {
        ND280Severe("TSingleHit Exception: " << e.what());
        return;
    }
    catch (...) {
        // Just don't crash!
        ND280Severe("TSingleHit Exception: unknown");
        return;
    }

#ifdef TSINGLE_HIT_SAVE_POSITION
    if (origPos.Mag()>1*unit::mm
        && (fPosition-origPos).Mag()>1*unit::mm) {
        ND280Error("Position and node position mismatch"
                   << std::endl
                   << "  Geometry Id " << GetGeomId().AsInt() << std::endl
                   << "  Saved Position is "
                   << " " << fPosition.X()
                   << " " << fPosition.Y()
                   << " " << fPosition.Z()
                   << std::endl
                   << "  Node Position is  "
                   << " " << origPos.X()
                   << " " << origPos.Y()
                   << " " << origPos.Z());
    }
#endif
}
