#include <cstdio>

#include "THATChannelId.hxx"

ClassImp(ND::THATChannelId);

//----------------------------------------------------------------------
ND::THATChannelId::THATChannelId(UInt_t id): ND::TChannelId(id) {}

//----------------------------------------------------------------------
ND::THATChannelId::THATChannelId(const ND::TChannelId& src)
  : ND::TChannelId(src) {}

//----------------------------------------------------------------------
ND::THATChannelId::~THATChannelId() {}

//----------------------------------------------------------------------
ND::THATChannelId::THATChannelId(UInt_t hat, UInt_t plate, UInt_t fem, UInt_t fec, 
			     UInt_t asic, UInt_t channel) {
    SetGuardBit();
    SetSubDetector(ND::TChannelId::kHAT);
    SetHAT(hat);
    SetPlate(plate);
    SetFEM(fem);
    SetFEC(fec);
    SetAsic(asic);
    SetChannel(channel);
}

//----------------------------------------------------------------------
std::string ND::THATChannelId::AsString() const {
    const char* det = TChannelId::SubDetAsString().c_str();
    char buffer[30];

    /// \todo Code needs to be translated into C++ and use sstream. 
    /// Only Channel has more than 2 figures 
    std::snprintf(buffer,30,"%7s: HAT:%01d:%01d:%01d:%01d:%01d:%02d",
                  det, this->GetHAT(), this->GetPlate(), this->GetFEM(),
                  this->GetFEC(), this->GetAsic(), this->GetChannel());

    return std::string(buffer);
}


//----------------------------------------------------------------------
UInt_t ND::THATChannelId::GetHAT() const {
    return GetField(kHATMSB, kHATLSB);
}

//----------------------------------------------------------------------
void ND::THATChannelId::SetHAT(int val) {
    SetField(val, kHATMSB, kHATLSB);
}

//----------------------------------------------------------------------
UInt_t ND::THATChannelId::GetPlate() const {
    return GetField(kPlateMSB, kPlateLSB);
}

//----------------------------------------------------------------------
void ND::THATChannelId::SetPlate(int val) {
    SetField(val, kPlateMSB, kPlateLSB);
}

//----------------------------------------------------------------------
UInt_t ND::THATChannelId::GetFEM() const {
    return GetField(kFEMMSB, kFEMLSB);
}

//----------------------------------------------------------------------
void ND::THATChannelId::SetFEM(int val) {
    SetField(val, kFEMMSB, kFEMLSB);
}

//----------------------------------------------------------------------
UInt_t ND::THATChannelId::GetFEC() const {
    return GetField(kFECMSB, kFECLSB);
}

//----------------------------------------------------------------------
void ND::THATChannelId::SetFEC(int val) {
    SetField(val, kFECMSB, kFECLSB);
}

//----------------------------------------------------------------------
UInt_t ND::THATChannelId::GetAsic() const {
    return GetField(kAsicMSB,kAsicLSB);
}

//----------------------------------------------------------------------
void ND::THATChannelId::SetAsic(int val) {
    SetField(val, kAsicMSB,kAsicLSB);
}

//----------------------------------------------------------------------
UInt_t ND::THATChannelId::GetChannel() const {
    return GetField(kChanMSB, kChanLSB);
}

//----------------------------------------------------------------------
void ND::THATChannelId::SetChannel(int val) {
    SetField(val, kChanMSB, kChanLSB);
}

