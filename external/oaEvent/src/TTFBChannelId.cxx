#include "TTFBChannelId.hxx"
#include <cstdio>

ClassImp(ND::TTFBChannelId);

//----------------------------------------------------------------------
ND::TTFBChannelId::TTFBChannelId(UInt_t id): ND::TChannelId(id) {}

//----------------------------------------------------------------------
ND::TTFBChannelId::TTFBChannelId(const ND::TChannelId& src)
  : ND::TChannelId(src) {}

//----------------------------------------------------------------------
ND::TTFBChannelId::TTFBChannelId(UInt_t subDet,  UInt_t rmm,  UInt_t tfb,
                                 UInt_t tripChip, UInt_t tripChannel, 
                                 UInt_t capacitor) {
    SetGuardBit();
    SetSubDetector(subDet);
    SetRMM(rmm);
    SetTFB(tfb);
    SetTripChip(tripChip);
    SetChannel(tripChannel);
    SetCapacitor(capacitor);
}

//----------------------------------------------------------------------
ND::TTFBChannelId::~TTFBChannelId() {}

//----------------------------------------------------------------------
std::string ND::TTFBChannelId::AsString() const {
  const char* det = ND::TChannelId::SubDetAsString().c_str();
  
  char trip = 'U';
  switch (GetTripChip()) {
  case 0:     trip = 'A';     break;
  case 1:     trip = 'B';     break;
  case 2:     trip = 'C';     break;
  case 3:     trip = 'D';     break;
  default:    trip = 'X';     break;
  }

  char cap[3];
  if (GetCapacitor() != 24) {
    std::snprintf(cap,3,"%02d",GetCapacitor());
  }
  else{
    std::sprintf(cap,"--");
  }
  char buffer[30];
  std::snprintf(buffer,30,"%7s: TFB:%02d:%02d:%2s:%c:%02d",
		det, GetRMM(), GetTFB(), cap,
		trip, GetChannel());
  
  return std::string(buffer);
}

//----------------------------------------------------------------------
ND::TTFBChannelId ND::TTFBChannelId::IgnoreCapacitor() const{
    TTFBChannelId id24(*this);
    id24.SetCapacitor(24);
    return id24;
}

//----------------------------------------------------------------------
ND::TTFBChannelId ND::TTFBChannelId::IgnoreChannel() const{
    TTFBChannelId id(this->IgnoreCapacitor());
    id.SetChannel(18);
    return id;
}

//----------------------------------------------------------------------
ND::TTFBChannelId ND::TTFBChannelId::IgnoreTripChip() const{
    TTFBChannelId id(this->IgnoreChannel());
    id.SetTripChip(0);
    return id;
}

//----------------------------------------------------------------------
ND::TTFBChannelId ND::TTFBChannelId::IgnoreTFB() const{
    TTFBChannelId id(this->IgnoreTripChip());
    id.SetTFB(0);
    return id;
}

//----------------------------------------------------------------------
UInt_t ND::TTFBChannelId::GetRMM() const {
    return GetField(kRMMBoardMSB,kRMMBoardLSB);
}

//----------------------------------------------------------------------
void ND::TTFBChannelId::SetRMM(int val) {
    SetField(val,kRMMBoardMSB,kRMMBoardLSB);
}

//----------------------------------------------------------------------
UInt_t ND::TTFBChannelId::GetTFB() const {
    return GetField(kTFBPortMSB, kTFBPortLSB);
}

//----------------------------------------------------------------------
void ND::TTFBChannelId::SetTFB(int val) {
    SetField(val, kTFBPortMSB, kTFBPortLSB);
}

//----------------------------------------------------------------------
UInt_t ND::TTFBChannelId::GetCapacitor() const {
    return GetField(kCapacitorMSB, kCapacitorLSB);
}

//----------------------------------------------------------------------
void ND::TTFBChannelId::SetCapacitor(int val) {
    SetField(val, kCapacitorMSB, kCapacitorLSB);
}

//----------------------------------------------------------------------
UInt_t ND::TTFBChannelId::GetTripChip() const {
    return GetField(kTripTMSB, kTripTLSB);
}

//----------------------------------------------------------------------
void ND::TTFBChannelId::SetTripChip(int val) {
    SetField(val, kTripTMSB, kTripTLSB);
}

//----------------------------------------------------------------------
UInt_t ND::TTFBChannelId::GetChannel() const {
    return GetField(kTripTChannelMSB, kTripTChannelLSB);
}

//----------------------------------------------------------------------
void ND::TTFBChannelId::SetChannel(int val) {
    SetField(val, kTripTChannelMSB, kTripTChannelLSB);
}

//----------------------------------------------------------------------
UInt_t ND::TTFBChannelId::GetCableId() const {
    return IgnoreCapacitor().AsUInt();
}

//----------------------------------------------------------------------
UInt_t ND::TTFBChannelId::GetTripTId() const {
    TTFBChannelId cableId(*this);
    cableId.SetChannel(0);
    cableId.SetCapacitor(0);
    return cableId.AsUInt();
}
