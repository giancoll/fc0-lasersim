////////////////////////////////////////////////////////////
// $Id$
//

#include "TMCHit.hxx"
#include "HEPUnits.hxx"
#include "TG4VHit.hxx"

ClassImp(ND::TMCHit);
ClassImp(ND::TWritableMCHit);

ND::TMCHit::TMCHit() {}

ND::TMCHit::TMCHit(const ND::TWritableMCHit& h)
    : ND::TSingleHit(h), fContributors(h.fContributors) { }

ND::TMCHit::~TMCHit() { }

/// Print the hit information.
void ND::TMCHit::ls(Option_t *opt) const {
    ND::THit::ls(opt);
    std::string option(opt);
    if (option.find("dump") != std::string::npos) {
        TROOT::IncreaseDirLevel();
        // Print the contributors.
        for (ContributorContainer::const_iterator h = fContributors.begin();
             h != fContributors.end();
             ++h) {
	  if(*h) (*h)->ls(opt);
        }
        TROOT::DecreaseDirLevel();
    }
}

ND::TWritableMCHit::TWritableMCHit() {}
ND::TWritableMCHit::TWritableMCHit(const ND::TWritableMCHit& h) : ND::TMCHit(h) {}
ND::TWritableMCHit::~TWritableMCHit() {}

//////////////////////////////////////////////////
// Setter methods for ND::TWritableMCHit
//////////////////////////////////////////////////

void ND::TWritableMCHit::SetGeomId(ND::TGeometryId id) {
    fGeomId = id.AsInt();
}

void ND::TWritableMCHit::SetCharge(double q) {fCharge = q;}

void ND::TWritableMCHit::SetTime(double t) {fTime = t;}

void ND::TWritableMCHit::SetChannelId(ND::TChannelId id) {fChannelId = id;}

void ND::TWritableMCHit::SetChargeValidity(bool valid) {
    ND::THit::SetChargeValidity(valid);
}

void ND::TWritableMCHit::SetTimeValidity(bool valid) {
    ND::THit::SetTimeValidity(valid);
}

void ND::TWritableMCHit::SetAlternateCharge(int flag) {
    ND::THit::SetAlternateCharge(flag);
}
