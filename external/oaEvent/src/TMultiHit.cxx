////////////////////////////////////////////////////////////
//
#include <cmath>
#include <algorithm>

#include <TGeoManager.h>
#include <TGeoMatrix.h>
#include <TGeoNode.h>

#include "HEPUnits.hxx"
#include "TMultiHit.hxx"
#include "TOADatabase.hxx"

ClassImp(ND::TMultiHit);

ND::TMultiHit::TMultiHit()
    : fInitialized(false),
      fGeomId(0), fCharge(0), fTime(0),
      fPosition(0,0,0),
      fIsXHit(false), fIsYHit(false), fIsZHit(false),
      fSpread(100*unit::meter,100*unit::meter,100*unit::meter),
      fUncertainty(100*unit::meter,100*unit::meter,100*unit::meter),
      fTimeUncertainty(1*unit::ns) {
    SetBit(kCanDelete,false);
}

ND::TMultiHit::TMultiHit(const ND::TMultiHit& h)
    : ND::THit(h), fHits(h.fHits),  fInitialized(false),
      fGeomId(h.fGeomId),
      fCharge(h.fCharge), fTime(h.fTime),
      fPosition(h.fPosition),
      fIsXHit(h.fIsXHit), fIsYHit(h.fIsYHit), fIsZHit(h.fIsZHit),
      fSpread(h.fSpread),
      fUncertainty(h.fUncertainty),
      fTimeUncertainty(h.fTimeUncertainty) {
    SetBit(kCanDelete,false);
}

ND::TMultiHit::TMultiHit(iterator f, iterator l)
    : fInitialized(false),
      fGeomId(0), fCharge(0), fTime(0),
      fPosition(0,0,0),
      fIsXHit(false), fIsYHit(false), fIsZHit(false),
      fSpread(100*unit::meter,100*unit::meter,100*unit::meter),
      fUncertainty(100*unit::meter,100*unit::meter,100*unit::meter) {
    SetBit(kCanDelete,false);
    fHits.clear();
    fHits.insert(fHits.end(),f,l);
    Initialize();
}

ND::TMultiHit::~TMultiHit() {}

//////////////////////////////////////////////////
// Getter methods for ND::TMultiHit
//////////////////////////////////////////////////

ND::TGeometryId ND::TMultiHit::GetGeomId(void) const {
    if (!fInitialized) const_cast<ND::TMultiHit*>(this)->Initialize();
    return fGeomId;
}

const TVector3& ND::TMultiHit::GetPosition(void) const {
    if (!fInitialized) const_cast<ND::TMultiHit*>(this)->Initialize();
    if (size()<1) throw EMultiHitIllegal();
    return fHits.front()->GetPosition();
}

double ND::TMultiHit::GetCharge(void) const {
    if (!fInitialized) const_cast<ND::TMultiHit*>(this)->Initialize();
    return fCharge;
}

double ND::TMultiHit::GetTime(void) const {
    if (!fInitialized) const_cast<ND::TMultiHit*>(this)->Initialize();
    if (size()<1) throw EMultiHitIllegal();
    return fHits.front()->GetTime();
}

const TVector3& ND::TMultiHit::GetSpread(void) const {
    if (!fInitialized) const_cast<ND::TMultiHit*>(this)->Initialize();
    if (size()<1) throw EMultiHitIllegal();
    return fHits.front()->GetSpread();
}

const TVector3& ND::TMultiHit::GetUncertainty(void) const {
    if (!fInitialized) const_cast<ND::TMultiHit*>(this)->Initialize();
    if (size()<1) throw EMultiHitIllegal();
    return fHits.front()->GetUncertainty();
}

double ND::TMultiHit::GetTimeUncertainty(void) const {
    return 1.0*unit::ns;
}

ND::TChannelId ND::TMultiHit::GetChannelId(int i) const {
    if (!fInitialized) const_cast<ND::TMultiHit*>(this)->Initialize();
    if (i<0 || (int) size()<=i) throw ND::EHitOutOfRange();
    return fHits[i]->GetChannelId();
}

int ND::TMultiHit::GetChannelIdCount() const {
    if (!fInitialized) const_cast<ND::TMultiHit*>(this)->Initialize();
    return size();
}

bool ND::TMultiHit::IsXHit(void) const {
    if (!fInitialized) const_cast<ND::TMultiHit*>(this)->Initialize();
    if (size()<1) throw EMultiHitIllegal();
    return fHits.front()->IsXHit();
}

bool ND::TMultiHit::IsYHit(void) const {
    if (!fInitialized) const_cast<ND::TMultiHit*>(this)->Initialize();
    if (size()<1) throw EMultiHitIllegal();
    return fHits.front()->IsYHit();
}

bool ND::TMultiHit::IsZHit(void) const {
    if (!fInitialized) const_cast<ND::TMultiHit*>(this)->Initialize();
    if (size()<1) throw EMultiHitIllegal();
    return fHits.front()->IsZHit();
}

ND::TMultiHit::iterator ND::TMultiHit::begin() const {
    return fHits.begin();
}

ND::TMultiHit::iterator ND::TMultiHit::end() const {
    return fHits.end();
}

const ND::THandle<ND::TSingleHit>& ND::TMultiHit::operator [](int index) const {
    return fHits[index];
}

unsigned int ND::TMultiHit::size() const {
    return fHits.size();
}

namespace {
    bool fHitsCompare(const ND::THandle<ND::TSingleHit>& a,
                      const ND::THandle<ND::TSingleHit>& b) {
        return (a->GetTime() < b->GetTime());
    }
}

void ND::TMultiHit::Initialize() {
    fInitialized = true;
    if (size()<1) throw EMultiHitIllegal();
    std::sort(fHits.begin(),fHits.end(), fHitsCompare);
    fGeomId = fHits.front()->GetGeomId();
    fCharge = 0.0;
    // Find out if regular or alternate charge should be used for the hit
    // charge.  This is based on the type of the first valid charge in the
    // multihit.
    bool useAlternate = false;
    for (iterator h = begin(); h != end(); ++h) {
        if ((*h)->HasValidCharge()) continue;
        useAlternate = (*h)->HasAlternateCharge();
        break;
    }
    // Sum the charges.
    for (iterator h = begin(); h != end(); ++h) {
        if (fGeomId != (*h)->GetGeomId()) throw EMultiHitIllegal();
        if (not (*h)->HasValidCharge()) continue;
        if (useAlternate != (*h)->HasAlternateCharge()) continue;
        fCharge += (*h)->GetCharge();
    }
}

void ND::TMultiHit::ls(Option_t *opt) const {
    ND::THit::ls(opt);
    std::string option(opt);
    if (option.find("dump") != std::string::npos) {
        TROOT::IncreaseDirLevel();
        for (iterator v = begin();
             v != end();
             ++v) {
            (*v)->ls(opt);
        }
        TROOT::DecreaseDirLevel();
    }
}
