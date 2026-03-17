#include <string>
#include <iomanip>
#include <sstream>

#include "TChannelId.hxx"
#include "TTFBChannelId.hxx"
#include "TSFGChannelId.hxx"
#include "TFGDChannelId.hxx"
#include "THATChannelId.hxx"
#include "TTPCChannelId.hxx"
#include "TTOFChannelId.hxx"
#include "TND280Log.hxx"

ClassImp(ND::TChannelId);

namespace {
};

//----------------------------------------------------------------------
ND::TChannelId::TChannelId(UInt_t id): fChannelId(id) {}

//----------------------------------------------------------------------
ND::TChannelId::TChannelId(const TChannelId& src): fChannelId(src.fChannelId) {}

//----------------------------------------------------------------------
ND::TChannelId::~TChannelId() {}

//----------------------------------------------------------------------
unsigned int ND::TChannelId::GetField(int msb, int lsb) const {
    if (lsb>msb || lsb<0 || 31<msb){
        ND280Error("Requesting TChannelId Field with invalid bit range"
                    << " LSB: " << lsb << " MSB: " << msb);
        throw EChannelId();
    }
    unsigned int field = fChannelId >> lsb;
    unsigned int mask = (1<<(msb-lsb+1))-1;
    return (field & mask);
}

//----------------------------------------------------------------------
void ND::TChannelId::SetField(int val, int msb, int lsb) {
    // Make sure the bit range is valid.
    if (lsb>msb || lsb<0 || 31<msb) {
        ND280Error("Trying to set channel id field with invalid bit range"
                    << " MSB: " << msb
                    << " LSB: " << lsb);
        throw EChannelId();
    }
    // Make sure val is in range.
    int maxValue = (1 << (msb-lsb+1));
    if (val >= maxValue) {
        ND280Severe("Channel id value out of range " << val
                    << " Bits in field " << msb-lsb+1
                    << " Maximum value " << maxValue);
        return;
    }
    if (val < 0) {
        ND280Severe("Channel id value out of range " << val
                   << " -- Negative values are not allowed");
        return;
    }
    // Build a mask for this field
    long mask = ((1<<(msb-lsb+1))-1) << lsb;
    // Clear the bit field.
    fChannelId &= ~mask;
    // Shift the value and set the field.
    fChannelId |= ((val << lsb)&mask);
}

//----------------------------------------------------------------------
void ND::TChannelId::SetGuardBit() {
    SetField(1,kGuardBitMSB, kGuardBitLSB);
}

//----------------------------------------------------------------------
ND::TChannelId::SubDetId ND::TChannelId::GetSubDetector() const {
    return ND::TChannelId::SubDetId(GetField(kSubDetMSB,kSubDetLSB));
}

//----------------------------------------------------------------------
void ND::TChannelId::SetSubDetector(int det) {
    if ( det < 1 || kMaxDetector <= det) {
        ND280Severe("Setting invalid sub-detector value: " << det);
    }
    SetField(det,kSubDetMSB,kSubDetLSB);
}

//----------------------------------------------------------------------
Bool_t ND::TChannelId::IsValid() const {
    if (!(fChannelId & kGuard_Mask)) {
        ND280Severe("Channel identifier has invalid guard bit");
        return false;
    }
    int det = GetSubDetector();
    if (det < 1 || kMaxDetector <= det) {
        ND280Severe("Channel identifier has invalid sub-detector: " << det);
        return false;
    }
    return true;
}

//----------------------------------------------------------------------
Bool_t ND::TChannelId::IsFGDChannel() const {
    return (kFGD == GetSubDetector());
}

//----------------------------------------------------------------------
Bool_t ND::TChannelId::IsTFBChannel() const {
    UInt_t det = GetSubDetector();
    return((det == kP0D)
           || (det == kECal)
#ifdef  USE_DEPRECATED_SubDetId
           || (det == kDSECal) || (det == kTECal) || (det == kPECal)
#endif
           || (det == kSMRD) || (det == kINGRID));
}

//----------------------------------------------------------------------
Bool_t ND::TChannelId::IsTPCChannel() const {
    return (kTPC == GetSubDetector());
}

//----------------------------------------------------------------------
Bool_t ND::TChannelId::IsSFGChannel() const {
    return (kSFG == GetSubDetector());
}

//----------------------------------------------------------------------
Bool_t ND::TChannelId::IsHATChannel() const {
    return (kHAT == GetSubDetector());
}

//----------------------------------------------------------------------
Bool_t ND::TChannelId::IsTOFChannel() const {
    return (kTOF == GetSubDetector());
}

//----------------------------------------------------------------------
std::string ND::TChannelId::SubDetAsString() const {
    std::string det = "Unset";

    switch (GetSubDetector()) {
    case kP0D:     det = "P0D";     break;
    case kTPC:     det = "TPC";     break;
    case kFGD:     det = "FGD";     break;
#ifdef  USE_DEPRECATED_SubDetId
    case kDSECal:  det = "DS-ECal"; break;
    case kTECal:   det = "T-ECal";  break;
    case kPECal:   det = "P0DECal"; break;
#else
    case kECal:    det = "ECal";    break;
#endif
    case kSMRD:    det = "SMRD";    break;
    case kINGRID:  det = "INGRID";  break;
    case kSFG:     det = "SFG";  break;
    case kHAT:     det = "HAT";  break;
    case kTOF:     det = "TOF";  break;
    default:       det = "-------";     break;
    }

    return det;
}

//----------------------------------------------------------------------
std::string ND::TChannelId::AsString() const {
    if (IsSFGChannel()) {
        ND::TSFGChannelId sfg(*this);
        return sfg.AsString();
    }

    if (IsTFBChannel()) {
        TTFBChannelId tfb(*this);
        return tfb.AsString();
    }

    if (IsFGDChannel()) {
        TFGDChannelId fgd(*this);
        return fgd.AsString();
    }

    if (IsTPCChannel()) {
        TTPCChannelId tpc(*this);
        return tpc.AsString();
    }

    if (IsHATChannel()) {
        THATChannelId hat(*this);
        return hat.AsString();
    }

    if (IsTOFChannel()) {
        TTOFChannelId tof(*this);
        return tof.AsString();
    }

    std::ostringstream buffer;
    buffer << std::setw(7) << SubDetAsString()
           << std::setw(8) << std::hex << std::setfill('0')
           << "0x" << fChannelId;
    return buffer.str();
}

std::ostream& operator<< (std::ostream& stream, const ND::TChannelId& rhs) {
  return stream << rhs.AsString();
}