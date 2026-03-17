////////////////////////////////////////////////////////////
// $Id$
//

#include "TDataHit.hxx"
#include "HEPUnits.hxx"

ClassImp(ND::TDataHit);
ClassImp(ND::TWritableDataHit);

ND::TDataHit::TDataHit() {}

ND::TDataHit::TDataHit(const ND::TWritableDataHit& h)
    : ND::TSingleHit(h), fProxy(h.fProxy) {}

ND::TDataHit::~TDataHit() { }

int ND::TDataHit::GetDigitCount() const {
    if (fProxy.IsValid()) return 1;
    else return 0;
}

const ND::TDigitProxy& ND::TDataHit::GetDigit(int i) const {
    if (i!=0) throw ND::EHitOutOfRange();
    if (!fProxy.IsValid()) throw ND::EHitOutOfRange();
    return fProxy;
}

ND::TWritableDataHit::TWritableDataHit() {}
ND::TWritableDataHit::TWritableDataHit(const ND::TWritableDataHit& h)
  : ND::TDataHit(h) {}
ND::TWritableDataHit::~TWritableDataHit() {}

//////////////////////////////////////////////////
// Setter methods for ND::TWritableDataHit
//////////////////////////////////////////////////

void ND::TWritableDataHit::SetGeomId(ND::TGeometryId id) {
    fGeomId = id.AsInt();
}

void ND::TWritableDataHit::SetCharge(double q) {fCharge = q;}

void ND::TWritableDataHit::SetTime(double t) {fTime = t;}

void ND::TWritableDataHit::SetDigit(ND::TDigitProxy proxy) {
    fProxy = proxy;
    if (fProxy.IsValid()) {
        try {
            ND::TDigit* digit = *fProxy;
            if (digit) {
                if (fChannelId.AsUInt() == 0) {
                    fChannelId = digit->GetChannelId();
                }
                else if (fChannelId != digit->GetChannelId()
                         && digit->GetChannelId().AsUInt() != 0) {
                    ND280Error("Hit channel id disagrees with digit proxy");
                }
            }
        }
        catch (...) {}
    }
    else if (fChannelId.AsUInt() == 0) {
        ND280Error("Invalid digit used to create hit."
                   "  Use SetChannelId to set channel before SetDigit.");
    }
}

void ND::TWritableDataHit::SetChannelId(ND::TChannelId id) {fChannelId = id;}

void ND::TWritableDataHit::SetChargeValidity(bool valid) {
    ND::THit::SetChargeValidity(valid);
}

void ND::TWritableDataHit::SetTimeValidity(bool valid) {
    ND::THit::SetTimeValidity(valid);
}

void ND::TWritableDataHit::SetAlternateCharge(int flag) {
    ND::THit::SetAlternateCharge(flag);
}
