#include <cstdio>

// This allows deprecated methods to be used.
#define METHOD_DEPRECATED
#include "TTPCChannelId.hxx"

ClassImp(ND::TTPCChannelId);

//----------------------------------------------------------------------
ND::TTPCChannelId::TTPCChannelId(UInt_t id): ND::TChannelId(id) {}

//----------------------------------------------------------------------
ND::TTPCChannelId::TTPCChannelId(const ND::TChannelId& src)
  : ND::TChannelId(src) {}

//----------------------------------------------------------------------
ND::TTPCChannelId::~TTPCChannelId() {}

//----------------------------------------------------------------------
ND::TTPCChannelId::TTPCChannelId(UInt_t dcc, UInt_t fem, UInt_t fec, 
			     UInt_t asic, UInt_t channel) {
    SetGuardBit();
    SetSubDetector(ND::TChannelId::kTPC);
    SetDCC(dcc);
    SetFEM(fem);
    SetFEC(fec);
    SetAsic(asic);
    SetChannel(channel);
}

#ifdef USE_DEPRECATED_TTPCChannelId_Constructor
//----------------------------------------------------------------------
ND::TTPCChannelId::TTPCChannelId(UInt_t tpc, UInt_t mm, UInt_t channel) {
    SetGuardBit();
    SetSubDetector(ND::TChannelId::kTPC);
    SetTPC(tpc);
    SetMM(mm);
    SetMMChannel(channel);
}
#endif

//----------------------------------------------------------------------
std::string ND::TTPCChannelId::AsString() const {
    const char* det = TChannelId::SubDetAsString().c_str();
    char buffer[30];

    /// \todo Code needs to be translated into C++ and use sstream.  
    std::snprintf(buffer,30,"%7s: TPC:%02d:%02d:%01d:%01d:%02d",
                  det, this->GetDCC(), this->GetFEM(),this->GetFEC(), 
                  this->GetAsic(), this->GetChannel());

    return std::string(buffer);
}


//----------------------------------------------------------------------
UInt_t ND::TTPCChannelId::GetDCC() const {
    return GetField(kDCCMSB, kDCCLSB);
}

//----------------------------------------------------------------------
void ND::TTPCChannelId::SetDCC(int val) {
    SetField(val, kDCCMSB, kDCCLSB);
}

//----------------------------------------------------------------------
UInt_t ND::TTPCChannelId::GetFEM() const {
    return GetField(kFEMMSB, kFEMLSB);
}

//----------------------------------------------------------------------
void ND::TTPCChannelId::SetFEM(int val) {
    SetField(val, kFEMMSB, kFEMLSB);
}

//----------------------------------------------------------------------
UInt_t ND::TTPCChannelId::GetFEC() const {
    return GetField(kFECMSB, kFECLSB);
}

//----------------------------------------------------------------------
void ND::TTPCChannelId::SetFEC(int val) {
    SetField(val, kFECMSB, kFECLSB);
}

//----------------------------------------------------------------------
UInt_t ND::TTPCChannelId::GetAsic() const {
    return GetField(kAsicMSB,kAsicLSB);
}

//----------------------------------------------------------------------
void ND::TTPCChannelId::SetAsic(int val) {
    SetField(val, kAsicMSB,kAsicLSB);
}

//----------------------------------------------------------------------
UInt_t ND::TTPCChannelId::GetChannel() const {
    return GetField(kChanMSB, kChanLSB);
}

//----------------------------------------------------------------------
void ND::TTPCChannelId::SetChannel(int val) {
    SetField(val, kChanMSB, kChanLSB);
}

#ifdef USE_DEPRECATED_TPCCode
//----------------------------------------------------------------------
UInt_t ND::TTPCChannelId::GetTPC() const {
    return GetField(kTPCMSB,kTPCLSB);
}

//----------------------------------------------------------------------
void ND::TTPCChannelId::SetTPC(int val) {
    SetField(val,kTPCMSB,kTPCLSB);
}

//----------------------------------------------------------------------
UInt_t ND::TTPCChannelId::GetMM() const {
    return GetField(kMMegaMSB, kMMegaLSB);
}

//----------------------------------------------------------------------
void ND::TTPCChannelId::SetMM(int val) {
    SetField(val,kMMegaMSB, kMMegaLSB);
}

//----------------------------------------------------------------------
UInt_t ND::TTPCChannelId::GetMMChannel() const {
    return GetField(kMMChanMSB, kMMChanLSB);
}

//----------------------------------------------------------------------
void ND::TTPCChannelId::SetMMChannel(int val) {
    SetField(val, kMMChanMSB, kMMChanLSB);
}
#endif

