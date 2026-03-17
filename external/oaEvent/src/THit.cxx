////////////////////////////////////////////////////////////
// $Id$
//
#include <TClass.h>
#include <TStreamerInfo.h>

#include <TGeoManager.h>

#include "HEPUnits.hxx"
#include "THit.hxx"
#include "TDigitProxy.hxx"
#include "TOADatabase.hxx"
#include "TGeomIdManager.hxx"
#include "TND280Log.hxx"

ClassImp(ND::THit);

ND::THit::THit() {
    SetBit(kCanDelete,false);
}

ND::THit::~THit() {}

// Get the geometry identifier for this hit.
ND::TGeometryId ND::THit::GetGeomId(void) const {throw;}

#ifdef USE_DEPRECATED_GetGeoNodeId
int ND::THit::GetGeoNodeId(void) const {
    ND280Error("Deprecated method being used");
    ND::TOADatabase::Get().Geometry();
    gGeoManager->PushPath();
    TOADatabase::Get().GeomId().CdId(GetGeomId());
    int node = gGeoManager->GetCurrentNodeId();
    gGeoManager->PopPath();
    return node;
}
#endif

// The center of the volume associated with this hit.
const TVector3& ND::THit::GetPosition(void) const {throw;}

// Return the calibrated "charge" for the hit.
double ND::THit::GetCharge(void) const {throw;}

bool ND::THit::HasValidCharge() const {return !TestBit(kInvalidCharge);}

void ND::THit::SetChargeValidity(bool valid) {
    SetBit(kInvalidCharge, !valid);
}

void ND::THit::SetAlternateCharge(int flag) {
    if (flag > 0) {
        SetBit(kAltCharge1, true);
    }
    else {
        SetBit(kAltCharge1, false);
    }
}

int ND::THit::HasAlternateCharge(void) const {
    return TestBit(kAltCharge1);
}

// Return the calibrated "time" for the hit.
double ND::THit::GetTime(void) const {throw;}

bool ND::THit::HasValidTime() const {return !TestBit(kInvalidTime);}

void ND::THit::SetTimeValidity(bool valid) {
    SetBit(kInvalidTime, !valid);
}

// Return the position spread.
const TVector3& ND::THit::GetSpread(void) const {throw;}

// Return the "uncertainty" of the hit position.  This is the position
// resolution of the hit.
const TVector3& ND::THit::GetUncertainty(void) const {throw;}

// Return the "uncertainty" for the time measurement.  This is the timing
// resolution of the sensor.
double ND::THit::GetTimeUncertainty(void) const {return 1.0*unit::ns;}

// Has X information.
bool ND::THit::IsXHit(void) const {throw;}

// Has Y information.
bool ND::THit::IsYHit(void) const {throw;}

// Has Z information.
bool ND::THit::IsZHit(void) const {throw;}

// Return a contributing hit.
ND::THandle<ND::THit> ND::THit::GetContributor(int i) const {
    throw ND::EHitOutOfRange();
}

// Return the number of hits that contribute to this hit.
int ND::THit::GetContributorCount() const {return 0;}

// Return the digit that generated this hit.
const ND::TDigitProxy& ND::THit::GetDigit(int i) const {
    throw ND::EHitOutOfRange();
}

// Return the number of digits that made this hit.
int ND::THit::GetDigitCount() const {return 0;}

// Return the associated channel id.
ND::TChannelId ND::THit::GetChannelId(int i) const {
    throw ND::EHitOutOfRange();
}

// Return the number of channel ids associated with this hit.
int ND::THit::GetChannelIdCount() const {return 0;}

// Print the hit information.
void ND::THit::ls(Option_t *opt) const {
    ND::ls_header(this,opt);
    int prec = std::cout.precision();
    std::cout << " Id: " << GetGeomId().AsInt();
    std::cout << " P: " << std::setprecision(0)
              << "(" << GetPosition().X()/unit::cm << " cm"
              << ", " << GetPosition().Y()/unit::cm << " cm"
              << ", " << GetPosition().Z()/unit::cm << " cm"
              << ", " << GetTime()/unit::ns << " ns) ";
    if (IsXHit()) std::cout << "X";
    if (IsYHit()) std::cout << "Y";
    if (IsZHit()) std::cout << "Z";
    std::cout << " Q: " << std::setprecision(1) << GetCharge() << " pe"
              << std::endl;
    std::string option(opt);
    if (option.find("dump") != std::string::npos) {
        TROOT::IncreaseDirLevel();
        TROOT::IndentLevel();
        std::cout << "Spread:      "
                  << std::setprecision(0)
                  << "(" << GetSpread().X()/unit::cm << " cm"
                  << ", " << GetSpread().Y()/unit::cm << " cm"
                  << ", " << GetSpread().Z()/unit::cm << " cm)"
                  << std::endl;
        TROOT::IndentLevel();
        std::cout << "Uncertainty: "
                  << std::setprecision(2)
                  << "(" << GetUncertainty().X()/unit::cm << " cm"
                  << ", " << GetUncertainty().Y()/unit::cm << " cm"
                  << ", " << GetUncertainty().Z()/unit::cm << " cm)"
                  << std::endl;
        TROOT::DecreaseDirLevel();
    }
    std::cout.precision(prec);
}
