// $Id$

#include <sstream>
#include "TND280Log.hxx"
#include "TGeometryId.hxx"
#include "ND280GeomIdDef.hxx"
#include "TOADatabase.hxx"
#include "TGeomIdManager.hxx"

ClassImp(ND::TGeometryId);

ND::TGeometryId::TGeometryId() : fGeometryId(ND::GeomId::Def::kEmptyId) {}

ND::TGeometryId::TGeometryId(int i) : fGeometryId(i) {}

ND::TGeometryId::TGeometryId(const TGeometryId& geom)
    : fGeometryId(geom.fGeometryId) {}

ND::TGeometryId::~TGeometryId() {}

void ND::TGeometryId::SetFieldSafe(int val, int msb, int lsb) {
    // Make sure the bit range is valid.
    if (msb<lsb || msb>31 || lsb <0) {
        ND280Error("Invalid bit field range -- MSB: " << msb
                   << " LSB: " << lsb);
        throw EGeomIdMSBLSB();
    }
    // Make sure val is in range.
    int maxValue = (1 << (msb-lsb+1));
    if (val >= maxValue) {
        ND280Error("Value out of range " << val
                   << " Bits in field " << msb-lsb+1
                   << " Maximum value " << maxValue);
        throw EGeomIdOutOfRange();
    }
    if (val < 0) {
        ND280Error("Value out of range " << val
                   << " -- Negative values are not allowed");
        throw EGeomIdOutOfRange();
    }
    // Build a mask for this field
    long mask = ((1<<(msb-lsb+1))-1) << lsb;
    // Clear the bit field.
    fGeometryId &= ~mask;
    // Shift the value and set the field.
    fGeometryId |= ((val << lsb)&mask);
    if (!IsValid()) {
        ND280Error("Invalid id: " << fGeometryId);
        throw EGeomIdInvalid();
    }
}

int ND::TGeometryId::GetFieldSafe(int msb, int lsb) const {
    if (msb<lsb || msb>31 || lsb <0) throw EGeomIdMSBLSB();
    int field = fGeometryId >> lsb;
    int mask = (1<<(msb-lsb+1))-1;
    return (field & mask);
}

int ND::TGeometryId::GetSubsystemId() const {
    return GetFieldSafe(ND::GeomId::Def::kDetectorIdMSB,
                    ND::GeomId::Def::kDetectorIdLSB);
}

bool ND::TGeometryId::IsValid() const {
    if (fGeometryId < 0) return false;

    int field = fGeometryId >> ND::GeomId::Def::kDetectorIdLSB;
    int mask = (1<<(ND::GeomId::Def::kDetectorIdMSB
                    - ND::GeomId::Def::kDetectorIdLSB+1))-1;
    int sys = (field & mask);

    switch (sys) {
    case ND::GeomId::Def::kP0D:
    case ND::GeomId::Def::kTPC:
    case ND::GeomId::Def::kFGD:
    case ND::GeomId::Def::kDSECal:
    case ND::GeomId::Def::kTECal:
    case ND::GeomId::Def::kPECal:
    case ND::GeomId::Def::kSMRD:
    case ND::GeomId::Def::kINGRID:
    case ND::GeomId::Def::kSFG:
    case ND::GeomId::Def::kHAT:
    case ND::GeomId::Def::kTOF:
    case ND::GeomId::Def::kROOTGeoNodeId:
        return true;
    default:
        return false;
    }

    return false;
}

std::string ND::TGeometryId::GetName() const {
    return TOADatabase::Get().GeomId().GetPath(*this);
}

std::string ND::TGeometryId::GetSubsystemName() const {
    int sys = GetFieldSafe(ND::GeomId::Def::kDetectorIdMSB,
                       ND::GeomId::Def::kDetectorIdLSB);

    switch (sys) {
    case ND::GeomId::Def::kP0D: return "P0D";
    case ND::GeomId::Def::kTPC: return "TPC";
    case ND::GeomId::Def::kFGD: return "FGD";
    case ND::GeomId::Def::kDSECal: return "DSECal";
    case ND::GeomId::Def::kTECal: return "TECal";
    case ND::GeomId::Def::kPECal: return "PECal";
    case ND::GeomId::Def::kSMRD: return "SMRD";
    case ND::GeomId::Def::kINGRID: return "INGRID";
    case ND::GeomId::Def::kSFG: return "SFG";
    case ND::GeomId::Def::kHAT: return "HAT";
    case ND::GeomId::Def::kTOF: return "TOF";
    case ND::GeomId::Def::kROOTGeoNodeId: return "node";
    }

    return "unknown";
}

TVector3 ND::TGeometryId::GetPosition() const {
    TVector3 position(0,0,0);
    if (!TOADatabase::Get().GeomId().GetPosition(*this,position)) {
        ND280Warn("Position for invalid geometry id was requested");
        throw ND::EGeomIdInvalid();
    }
    return position;
}

bool operator==(const ND::TGeometryId& a, const ND::TGeometryId& b) {
    return (a.AsInt() == b.AsInt());
}

bool operator!=(const ND::TGeometryId& a, const ND::TGeometryId& b) {
    return (a.AsInt() != b.AsInt());
}

std::ostream& operator<<(std::ostream& s, const ND::TGeometryId& id) {
    try {
        ND::TOADatabase::Get().Geometry();
        std::string name = id.GetName();
        int len = name.size();
        int maxlen = 50;
        if (len > maxlen) name = "..." + name.substr(name.size()-maxlen);
        s << name;
    }
    catch (...) {
        s << id.AsInt();
    };

    return s;
}
