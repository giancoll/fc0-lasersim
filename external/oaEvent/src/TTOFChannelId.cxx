#include "TND280Log.hxx"
#include "TTOFChannelId.hxx"

#include <cstdio>

ClassImp(ND::TTOFChannelId);

//----------------------------------------------------------------------
// this is the definition of a TChannelId constructor, 
// meant to receive only one UInt_t as argument, that can be either the 
// local channel id or the global channel id
ND::TTOFChannelId::TTOFChannelId(UInt_t id): TChannelId(id) {
   
    // writing this variable explicitly to make it more clear
    int n_channels_per_feb = NChannelsPerSampic*NSampicsPerFeb;

    int feb = id / n_channels_per_feb;
    int sampic = (id % n_channels_per_feb) / NChannelsPerSampic;
    int channel = id % NChannelsPerSampic;

    SetGuardBit();
    SetSubDetector(ND::TChannelId::kTOF); // this is where fChannel is set

    SetField(1,kVersionMSB,kVersionLSB); // not MC is 1, a bit counterintuitive

    // SetFebId
    SetField(feb,kFebMSB,kFebLSB);
    // SetSampicId
    SetField(sampic,kSampicMSB,kSampicLSB);
    // SetChannelId
    SetField(channel,kChannelMSB,kChannelLSB);

}

//----------------------------------------------------------------------
ND::TTOFChannelId::TTOFChannelId(const ND::TChannelId& src)
  : TChannelId(src) {}

//----------------------------------------------------------------------
ND::TTOFChannelId::~TTOFChannelId() {}

// Make an MC channel identifier for the TOF.  The pointer to a char is there
// to make sure this doesn't collide with a real channel identifier, and
// should have a value of NULL (the value is ignored.
ND::TTOFChannelId::TTOFChannelId(char* flag, UInt_t subDet, UInt_t module,
                                 UInt_t bar, Int_t end) { // end can be negative

    if (subDet != ND::TChannelId::kTOF) {
        ND280Severe("Invalid sub-detector field: " << subDet);
    }
    // std::cout << "Calling TTOFChannelId constructor for MC. " << std::endl;

    SetGuardBit();
    SetSubDetector(ND::TChannelId::kTOF);

    // This is an MC channel id.
    SetField(0,kVersionMSB,kVersionLSB);

    SetField(module,kMCModuleMSB,kMCModuleLSB);
    SetField(bar,kMCBarMSB,kMCBarLSB);
    if (end == -1) SetField(1,kMCEndMSB,kMCEndLSB);
    else if (end == +1) SetField(2,kMCEndMSB,kMCEndLSB);
    else {
        throw std::runtime_error("Invalid TOF channel id was requested");
    }
}

// this is meant to be used outside of the class, to make a channel id from the indices
UInt_t ND::TTOFChannelId::GetLocalChannelId(int febIndex_, int sampicIndex_, int channelIndex_) {
    Int_t out{channelIndex_};
    out += sampicIndex_*NChannelsPerSampic;
    out += febIndex_*NChannelsPerSampic*NSampicsPerFeb;
    return (UInt_t) out; 
}

UInt_t ND::TTOFChannelId::GetLocalChannelId() const {

    // this number is the offset for the channel for the TOF.
    // It is set as a mask for fChannel in SetField, when calling SetField(channel,kSubDetMSB,kSubDetLSB),
    // and following calls of SetField do not change it, because the bits of other variables 
    // are lower than kSubDetMSB and kSubDetLSB. Ask Clark for better explanation ~EV

    // TODO find a way to get this from the class, not hard-coded
    int offset = 2524971008; 
    return (AsUInt() % offset);
}

//----------------------------------------------------------------------
// Make a channel identifier for the TOF, starting from the electronics DEPRECATED, WILL DELETE TODO ~EV
// ND::TTOFChannelId::TTOFChannelId(UInt_t subDet, Int_t feb, Int_t sampic, Int_t channel) :
//     TChannelId(feb * SampicNChannelsPerFeb + sampic * SampicNChannelsPerSampic + channel){

//     if(subDet != ND::TChannelId::kTOF){
//         ND280Severe("Invalid sub-detector field: " << subDet);
//     }

//     SetGuardBit();
//     SetSubDetector(ND::TChannelId::kTOF);

//     SetField(1,kVersionMSB,kVersionLSB); // not MC, a bit counterintuitive
    
//     // SetField will fail and return a severe error if number is negative

//     // SetFebId
//     SetField(feb,kFebMSB,kFebLSB);
//     // SetSampicId
//     SetField(sampic,kSampicMSB,kSampicLSB);
//     // SetChannelId
//     SetField(channel,kChannelMSB,kChannelLSB);


//     // TODO move these parameters to tofUtils, should not be called in this scope
//     // int SampicNFebs = 4;
//     // int SampicNSampics = 4;
//     // int SampicNChannelsPerSampic = 16;
//     // int SampicNChannelsPerFeb = SampicNSampics * SampicNChannelsPerSampic;
//     // int SampicNChannelsTotal = SampicNFebs * SampicNChannelsPerFeb;

//     // this is ok here, will be the only place where it's implemented
//     // fChannelId = feb * SampicNChannelsPerFeb + sampic * SampicNChannelsPerSampic + channel;
// }

bool ND::TTOFChannelId::IsMC() const {
    if (GetSubDetector() != ND::TChannelId::kTOF) return false;
    return (0 == GetField(kVersionMSB,kVersionLSB)); // TODO I would change the definition here and in the constructor, to have 1 and not 0 when MC ~EV
}

int ND::TTOFChannelId::GetMCModule() const {
    return GetField(kMCModuleMSB,kMCModuleLSB);
}

int ND::TTOFChannelId::GetMCBar() const {
    return GetField(kMCBarMSB,kMCBarLSB);
}

int ND::TTOFChannelId::GetMCEnd() const {
    int end = GetField(kMCEndMSB,kMCEndLSB);
    // std::cout << "    In GetMCEnd, end " << end << std::endl;
    if (end == 1) return -1;
    if (end == 2) return +1;
    throw std::runtime_error("Invalid TOF channel id for MCend");
}

//----------------------------------------------------------------------
int ND::TTOFChannelId::GetFeb() const {
    return GetField(kFebMSB,kFebLSB);
}

//----------------------------------------------------------------------
int ND::TTOFChannelId::GetSampic() const {
    return GetField(kSampicMSB,kSampicLSB);
}

//----------------------------------------------------------------------
int ND::TTOFChannelId::GetChannel() const {
    return GetField(kChannelMSB,kChannelLSB);
}

// UInt_t ND::TTOFChannelId::AsUInt() const {
//     return fChannelId; // can't call directly fChannelId, since it's private
// }

//----------------------------------------------------------------------
std::string ND::TTOFChannelId::AsString() const {
  const char* det = TChannelId::SubDetAsString().c_str();
  char buffer[50];
  if(IsMC()) {
      std::snprintf(buffer,30,"%7s: MC:%01d:%02d:%02d",
                    det,
                    GetMCModule(),
                    GetMCBar(),
                    GetMCEnd());
      //return std::string(buffer);
  }else{
      std::snprintf(buffer,50,"%7s; FEB:%01d; SAMPIC:%01d; CHANNEL:%02d",
                    det,
                    GetFeb(),
                    GetSampic(),
                    GetChannel());
  }
//  return ND::TChannelId::AsString();
  return std::string(buffer);
}
