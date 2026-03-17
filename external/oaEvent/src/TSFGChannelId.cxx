#include "TND280Log.hxx"
#include "TSFGChannelId.hxx"

#include <cstdio>

ClassImp(ND::TSFGChannelId);

//----------------------------------------------------------------------
ND::TSFGChannelId::TSFGChannelId(UInt_t id): TChannelId(id) {}

//----------------------------------------------------------------------
ND::TSFGChannelId::TSFGChannelId(const ND::TChannelId& src)
  : TChannelId(src) {}

//----------------------------------------------------------------------
ND::TSFGChannelId::~TSFGChannelId() {}

//----------------------------------------------------------------------
// Make an MC channel identifier for the SFG.
ND::TSFGChannelId::TSFGChannelId(UInt_t subDet, UInt_t projection,
                                 UInt_t u, UInt_t v) {

    if (subDet != ND::TChannelId::kSFG) {
        ND280Severe("Invalid sub-detector field: " << subDet);
    }

    SetGuardBit();
    SetSubDetector(ND::TChannelId::kSFG);

    // This is an MC channel id.
    SetField(0,kTypeMSB,kTypeLSB);

    SetField(projection,kMCProjectionMSB,kMCProjectionLSB);
    SetField(u,kMCUMSB,kMCULSB);
    SetField(v,kMCVMSB,kMCVLSB);

}

//----------------------------------------------------------------------
// Make an channel identifier for the SFG.
ND::TSFGChannelId::TSFGChannelId(UInt_t subDet, UInt_t idType,
                                 UInt_t ocb, UInt_t feb, UInt_t chan) {

    if (subDet != ND::TChannelId::kSFG) {
        ND280Severe("Invalid sub-detector field: " << subDet);
    }

    if (idType != 1) {
        ND280Severe("Invalid channel id type field: " << idType);
    }

    SetGuardBit();
    SetSubDetector(ND::TChannelId::kSFG);

    // This is a type 01 channel id.
    SetField(1,kTypeMSB,kTypeLSB);

    SetField(ocb,kCrateMSB,kCrateLSB);
    SetField(feb,kSlotMSB,kSlotLSB);
    SetField(chan,kCitirocMSB,kCitirocChannelLSB); // notice different MSB/LSB pair

}

//----------------------------------------------------------------------
// Make an channel identifier for the SFG.
ND::TSFGChannelId::TSFGChannelId(UInt_t subDet, UInt_t boardId, UInt_t chan) {

    if (subDet != ND::TChannelId::kSFG) {
        ND280Severe("Invalid sub-detector field: " << subDet);
    }

    SetGuardBit();
    SetSubDetector(ND::TChannelId::kSFG);

    // This is a type 01 channel id.
    SetField(1,kTypeMSB,kTypeLSB);

    // Yes, the MSB and LSB come from different ranges (crate and slot)
    SetField(boardId,kCrateMSB,kSlotLSB);

    // Yes, the MSB and LSB come from different ranges (CITIROC and CITIROC channel)
    SetField(chan,kCitirocMSB,kCitirocChannelLSB);
}

//----------------------------------------------------------------------
bool ND::TSFGChannelId::IsMC() const {
    if (GetSubDetector() != ND::TChannelId::kSFG) return false;
    return (0 == GetType());
}

//----------------------------------------------------------------------
int ND::TSFGChannelId::GetType() const {
    return GetField(kTypeMSB,kTypeLSB);
}

//----------------------------------------------------------------------
int ND::TSFGChannelId::GetMCProjection() const {
    return GetField(kMCProjectionMSB,kMCProjectionLSB);
}

//----------------------------------------------------------------------
int ND::TSFGChannelId::GetMC_U() const {
    return GetField(kMCUMSB,kMCULSB);
}

//----------------------------------------------------------------------
int ND::TSFGChannelId::GetMC_V() const {
    return GetField(kMCVMSB,kMCVLSB);
}

//----------------------------------------------------------------------
int ND::TSFGChannelId::GetOCB() const {
    return GetField(kCrateMSB,kCrateLSB);
}

//----------------------------------------------------------------------
int ND::TSFGChannelId::GetFEBSlot() const {
    return GetField(kSlotMSB,kSlotLSB);
}

//----------------------------------------------------------------------
int ND::TSFGChannelId::GetBoard() const {
    return GetField(kCrateMSB,kSlotLSB);
}

//----------------------------------------------------------------------
int ND::TSFGChannelId::GetFEBChannel() const {
    return GetField(kCitirocMSB,kCitirocChannelLSB);
}

//----------------------------------------------------------------------
int ND::TSFGChannelId::GetCITIROC() const {
    return GetField(kCitirocMSB,kCitirocLSB);
}

//----------------------------------------------------------------------
int ND::TSFGChannelId::GetCITIROCChannel() const {
    return GetField(kCitirocChannelMSB,kCitirocChannelLSB);
}

//----------------------------------------------------------------------
std::string ND::TSFGChannelId::AsString() const {
  const char* det = TChannelId::SubDetAsString().c_str();
  char buffer[50];
  if(IsMC()) {
      std::snprintf(buffer,50,"%7s: MC:%01d:%03d:%03d",
                    det, GetMCProjection(), GetMC_U(), GetMC_V());
  }
  else {
      std::snprintf(buffer,50,"%7s: %02d:%02d:%03d",
                    det, GetOCB(), GetFEBSlot(), GetFEBChannel());
  }
  return std::string(buffer);
}
