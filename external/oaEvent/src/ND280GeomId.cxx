#include "TND280Log.hxx"
#include "TGeometryId.hxx"
#include "ND280GeomId.hxx"
#include "ND280GeomIdDef.hxx"

namespace {
    // Provide access to the protected setter and getter methods.
    class TSettableGeometryId: public ND::TGeometryId {
    public:
        TSettableGeometryId() {}
        explicit TSettableGeometryId(const ND::TGeometryId& id)
            : ND::TGeometryId(id) {}
        void SetField(int v, int m, int b) {
            ND::TGeometryId::SetFieldSafe(v,m,b);
        }
        int GetField(int m, int b) {
            return ND::TGeometryId::GetFieldSafe(m,b);
        }
    };
}

ND::TGeometryId ND::GeomId::Empty() {
    return TGeometryId();
}

int ND::GeomId::GetSubSystem(TGeometryId i) {
    TSettableGeometryId id(i);
    return id.GetField(ND::GeomId::Def::kDetectorIdMSB,
                       ND::GeomId::Def::kDetectorIdLSB);
}

////////////////////////////////////////////////////////////
// P0D Geometry Identifiers.
////////////////////////////////////////////////////////////

bool ND::GeomId::P0D::IsP0D(TGeometryId i) {
    TSettableGeometryId id(i);
    return (ND::GeomId::Def::kP0D
            == id.GetField(ND::GeomId::Def::kDetectorIdMSB,
                           ND::GeomId::Def::kDetectorIdLSB));
}

ND::TGeometryId ND::GeomId::P0D::Detector() {
    TSettableGeometryId id;
    id.SetField(ND::GeomId::Def::kP0D,
                ND::GeomId::Def::kDetectorIdMSB,
                ND::GeomId::Def::kDetectorIdLSB);
    id.SetField(ND::GeomId::Def::P0D::kGlobal,
                ND::GeomId::Def::P0D::kSeqIdMSB,
                ND::GeomId::Def::P0D::kSeqIdLSB);
    id.SetField(ND::GeomId::Def::P0D::Global::kP0D,
                ND::GeomId::Def::P0D::Global::kSeqIdMSB,
                ND::GeomId::Def::P0D::Global::kSeqIdLSB);
    id.SetField(0,
                ND::GeomId::Def::P0D::Global::kFieldMSB,
                ND::GeomId::Def::P0D::Global::kFieldLSB);
    return id;
}

ND::TGeometryId ND::GeomId::P0D::SuperP0Dule(int sP0Dule) {
    TSettableGeometryId id;
    id.SetField(ND::GeomId::Def::kP0D,
                ND::GeomId::Def::kDetectorIdMSB,
                ND::GeomId::Def::kDetectorIdLSB);
    id.SetField(ND::GeomId::Def::P0D::kGlobal,
                ND::GeomId::Def::P0D::kSeqIdMSB,
                ND::GeomId::Def::P0D::kSeqIdLSB);
    id.SetField(ND::GeomId::Def::P0D::Global::kSP0Dule,
                ND::GeomId::Def::P0D::Global::kSeqIdMSB,
                ND::GeomId::Def::P0D::Global::kSeqIdLSB);
    id.SetField(sP0Dule,
                ND::GeomId::Def::P0D::Global::kFieldMSB,
                ND::GeomId::Def::P0D::Global::kFieldLSB);
    return id;
}

int ND::GeomId::P0D::GetSuperP0Dule(TGeometryId i) {
    TSettableGeometryId id(i);
    if (ND::GeomId::Def::kP0D
        != id.GetField(ND::GeomId::Def::kDetectorIdMSB,
                       ND::GeomId::Def::kDetectorIdLSB)) return -1;
    if (ND::GeomId::Def::P0D::kGlobal
        != id.GetField(ND::GeomId::Def::P0D::kSeqIdMSB,
                       ND::GeomId::Def::P0D::kSeqIdLSB)) return -1;
    if (ND::GeomId::Def::P0D::Global::kSP0Dule
        != id.GetField(ND::GeomId::Def::P0D::Global::kSeqIdMSB,
                       ND::GeomId::Def::P0D::Global::kSeqIdLSB)) return -1;
    return id.GetField(ND::GeomId::Def::P0D::Global::kFieldMSB,
                       ND::GeomId::Def::P0D::Global::kFieldLSB);
}

ND::TGeometryId ND::GeomId::P0D::P0Dule(int p0dule) {
    TSettableGeometryId id;
    id.SetField(ND::GeomId::Def::kP0D,
                ND::GeomId::Def::kDetectorIdMSB,
                ND::GeomId::Def::kDetectorIdLSB);
    id.SetField(ND::GeomId::Def::P0D::kGlobal,
                ND::GeomId::Def::P0D::kSeqIdMSB,
                ND::GeomId::Def::P0D::kSeqIdLSB);
    id.SetField(ND::GeomId::Def::P0D::Global::kP0Dule,
                ND::GeomId::Def::P0D::Global::kSeqIdMSB,
                ND::GeomId::Def::P0D::Global::kSeqIdLSB);
    id.SetField(p0dule,
                ND::GeomId::Def::P0D::Global::kFieldMSB,
                ND::GeomId::Def::P0D::Global::kFieldLSB);
    return id;
}

int ND::GeomId::P0D::GetP0Dule(TGeometryId i) {
    TSettableGeometryId id(i);
    if (ND::GeomId::Def::kP0D
        != id.GetField(ND::GeomId::Def::kDetectorIdMSB,
                       ND::GeomId::Def::kDetectorIdLSB)) return -1;
    if (ND::GeomId::Def::P0D::kGlobal
        != id.GetField(ND::GeomId::Def::P0D::kSeqIdMSB,
                       ND::GeomId::Def::P0D::kSeqIdLSB)) return -1;
    if (ND::GeomId::Def::P0D::Global::kP0Dule
        != id.GetField(ND::GeomId::Def::P0D::Global::kSeqIdMSB,
                       ND::GeomId::Def::P0D::Global::kSeqIdLSB)) return -1;
    return id.GetField(ND::GeomId::Def::P0D::Global::kFieldMSB,
                       ND::GeomId::Def::P0D::Global::kFieldLSB);
}

ND::TGeometryId ND::GeomId::P0D::Target(int target) {
    TSettableGeometryId id;
    id.SetField(ND::GeomId::Def::kP0D,
                ND::GeomId::Def::kDetectorIdMSB,
                ND::GeomId::Def::kDetectorIdLSB);
    id.SetField(ND::GeomId::Def::P0D::kGlobal,
                ND::GeomId::Def::P0D::kSeqIdMSB,
                ND::GeomId::Def::P0D::kSeqIdLSB);
    id.SetField(ND::GeomId::Def::P0D::Global::kTarget,
                ND::GeomId::Def::P0D::Global::kSeqIdMSB,
                ND::GeomId::Def::P0D::Global::kSeqIdLSB);
    id.SetField(target,
                ND::GeomId::Def::P0D::Global::kFieldMSB,
                ND::GeomId::Def::P0D::Global::kFieldLSB);
    return id;
}

int ND::GeomId::P0D::GetTarget(TGeometryId i) {
    TSettableGeometryId id(i);
    if (ND::GeomId::Def::kP0D
        != id.GetField(ND::GeomId::Def::kDetectorIdMSB,
                       ND::GeomId::Def::kDetectorIdLSB)) return -1;
    if (ND::GeomId::Def::P0D::kGlobal
        != id.GetField(ND::GeomId::Def::P0D::kSeqIdMSB,
                       ND::GeomId::Def::P0D::kSeqIdLSB)) return -1;
    if (ND::GeomId::Def::P0D::Global::kTarget
        != id.GetField(ND::GeomId::Def::P0D::Global::kSeqIdMSB,
                       ND::GeomId::Def::P0D::Global::kSeqIdLSB)) return -1;
    return id.GetField(ND::GeomId::Def::P0D::Global::kFieldMSB,
                       ND::GeomId::Def::P0D::Global::kFieldLSB);
}

ND::TGeometryId ND::GeomId::P0D::ECalRadiator(int radiator) {
    TSettableGeometryId id;
    id.SetField(ND::GeomId::Def::kP0D,
                ND::GeomId::Def::kDetectorIdMSB,
                ND::GeomId::Def::kDetectorIdLSB);
    id.SetField(ND::GeomId::Def::P0D::kGlobal,
                ND::GeomId::Def::P0D::kSeqIdMSB,
                ND::GeomId::Def::P0D::kSeqIdLSB);
    id.SetField(ND::GeomId::Def::P0D::Global::kECalRadiator,
                ND::GeomId::Def::P0D::Global::kSeqIdMSB,
                ND::GeomId::Def::P0D::Global::kSeqIdLSB);
    id.SetField(radiator,
                ND::GeomId::Def::P0D::Global::kFieldMSB,
                ND::GeomId::Def::P0D::Global::kFieldLSB);
    return id;
}

int ND::GeomId::P0D::GetECalRadiator(TGeometryId i) {
    TSettableGeometryId id(i);
    if (ND::GeomId::Def::kP0D
        != id.GetField(ND::GeomId::Def::kDetectorIdMSB,
                       ND::GeomId::Def::kDetectorIdLSB)) return -1;
    if (ND::GeomId::Def::P0D::kGlobal
        != id.GetField(ND::GeomId::Def::P0D::kSeqIdMSB,
                       ND::GeomId::Def::P0D::kSeqIdLSB)) return -1;
    if (ND::GeomId::Def::P0D::Global::kECalRadiator
        != id.GetField(ND::GeomId::Def::P0D::Global::kSeqIdMSB,
                       ND::GeomId::Def::P0D::Global::kSeqIdLSB)) return -1;
    return id.GetField(ND::GeomId::Def::P0D::Global::kFieldMSB,
                       ND::GeomId::Def::P0D::Global::kFieldLSB);
}

ND::TGeometryId ND::GeomId::P0D::TargetRadiator(int targetRadiator) {
    TSettableGeometryId id;
    id.SetField(ND::GeomId::Def::kP0D,
                ND::GeomId::Def::kDetectorIdMSB,
                ND::GeomId::Def::kDetectorIdLSB);
    id.SetField(ND::GeomId::Def::P0D::kGlobal,
                ND::GeomId::Def::P0D::kSeqIdMSB,
                ND::GeomId::Def::P0D::kSeqIdLSB);
    id.SetField(ND::GeomId::Def::P0D::Global::kTargetRadiator,
                ND::GeomId::Def::P0D::Global::kSeqIdMSB,
                ND::GeomId::Def::P0D::Global::kSeqIdLSB);
    id.SetField(targetRadiator,
                ND::GeomId::Def::P0D::Global::kFieldMSB,
                ND::GeomId::Def::P0D::Global::kFieldLSB);
    return id;
}

int ND::GeomId::P0D::GetTargetRadiator(TGeometryId i) {
    TSettableGeometryId id(i);
    if (ND::GeomId::Def::kP0D
        != id.GetField(ND::GeomId::Def::kDetectorIdMSB,
                       ND::GeomId::Def::kDetectorIdLSB)) return -1;
    if (ND::GeomId::Def::P0D::kGlobal
        != id.GetField(ND::GeomId::Def::P0D::kSeqIdMSB,
                       ND::GeomId::Def::P0D::kSeqIdLSB)) return -1;
    if (ND::GeomId::Def::P0D::Global::kTargetRadiator
        != id.GetField(ND::GeomId::Def::P0D::Global::kSeqIdMSB,
                       ND::GeomId::Def::P0D::Global::kSeqIdLSB)) return -1;
    return id.GetField(ND::GeomId::Def::P0D::Global::kFieldMSB,
                       ND::GeomId::Def::P0D::Global::kFieldLSB);
}

ND::TGeometryId ND::GeomId::P0D::Bar(int P0Dule,
                                     int layer,
                                     int bar) {
    TSettableGeometryId id;
    id.SetField(ND::GeomId::Def::kP0D,
                ND::GeomId::Def::kDetectorIdMSB,
                ND::GeomId::Def::kDetectorIdLSB);
    id.SetField(ND::GeomId::Def::P0D::kScintillator,
                ND::GeomId::Def::P0D::kSeqIdMSB,
                ND::GeomId::Def::P0D::kSeqIdLSB);
    id.SetField(0,
                ND::GeomId::Def::P0D::Bar::kSP0DuleMSB,
                ND::GeomId::Def::P0D::Bar::kSP0DuleLSB);
    id.SetField(P0Dule,
                ND::GeomId::Def::P0D::Bar::kP0DuleMSB,
                ND::GeomId::Def::P0D::Bar::kP0DuleLSB);
    id.SetField(layer,
                ND::GeomId::Def::P0D::Bar::kLayerMSB,
                ND::GeomId::Def::P0D::Bar::kLayerLSB);
    id.SetField(bar,
                ND::GeomId::Def::P0D::Bar::kBarMSB,
                ND::GeomId::Def::P0D::Bar::kBarLSB);
    return id;
}

int ND::GeomId::P0D::GetBarP0Dule(TGeometryId i) {
    TSettableGeometryId id(i);
    if (ND::GeomId::Def::kP0D
        != id.GetField(ND::GeomId::Def::kDetectorIdMSB,
                       ND::GeomId::Def::kDetectorIdLSB)) return -1;
    if (ND::GeomId::Def::P0D::kScintillator
        != id.GetField(ND::GeomId::Def::P0D::kSeqIdMSB,
                       ND::GeomId::Def::P0D::kSeqIdLSB)) return -1;
    return id.GetField(ND::GeomId::Def::P0D::Bar::kP0DuleMSB,
                       ND::GeomId::Def::P0D::Bar::kP0DuleLSB);
}

int ND::GeomId::P0D::GetBarLayer(TGeometryId i) {
    TSettableGeometryId id(i);
    if (ND::GeomId::Def::kP0D
        != id.GetField(ND::GeomId::Def::kDetectorIdMSB,
                       ND::GeomId::Def::kDetectorIdLSB)) return -1;
    if (ND::GeomId::Def::P0D::kScintillator
        != id.GetField(ND::GeomId::Def::P0D::kSeqIdMSB,
                       ND::GeomId::Def::P0D::kSeqIdLSB)) return -1;
    return id.GetField(ND::GeomId::Def::P0D::Bar::kLayerMSB,
                       ND::GeomId::Def::P0D::Bar::kLayerLSB);
}

int ND::GeomId::P0D::GetBarNumber(TGeometryId i) {
    TSettableGeometryId id(i);
    if (ND::GeomId::Def::kP0D
        != id.GetField(ND::GeomId::Def::kDetectorIdMSB,
                       ND::GeomId::Def::kDetectorIdLSB)) return -1;
    if (ND::GeomId::Def::P0D::kScintillator
        != id.GetField(ND::GeomId::Def::P0D::kSeqIdMSB,
                       ND::GeomId::Def::P0D::kSeqIdLSB)) return -1;
    return id.GetField(ND::GeomId::Def::P0D::Bar::kBarMSB,
                       ND::GeomId::Def::P0D::Bar::kBarLSB);
}

////////////////////////////////////////////////////////////
// TPC Geometry Identifiers
////////////////////////////////////////////////////////////

bool ND::GeomId::TPC::IsTPC(TGeometryId i) {
    TSettableGeometryId id(i);
    return (ND::GeomId::Def::kTPC
            == id.GetField(ND::GeomId::Def::kDetectorIdMSB,
                           ND::GeomId::Def::kDetectorIdLSB));
}

ND::TGeometryId ND::GeomId::TPC::Module(int tpc) {
    TSettableGeometryId id;
    if (tpc<0 || tpc>2) {
        ND280Warn("TPC module out of range [0,2]: " << tpc);
        return id;
    }
    id.SetField(ND::GeomId::Def::kTPC,
                ND::GeomId::Def::kDetectorIdMSB,
                ND::GeomId::Def::kDetectorIdLSB);
    id.SetField(ND::GeomId::Def::TPC::kGlobal,
                ND::GeomId::Def::TPC::kSeqIdMSB,
                ND::GeomId::Def::TPC::kSeqIdLSB);
    id.SetField(ND::GeomId::Def::TPC::Global::kTPC,
                ND::GeomId::Def::TPC::Global::kSeqIdMSB,
                ND::GeomId::Def::TPC::Global::kSeqIdLSB);
    id.SetField(tpc,
                ND::GeomId::Def::TPC::Global::kFieldMSB,
                ND::GeomId::Def::TPC::Global::kFieldLSB);
    return id;
}

int ND::GeomId::TPC::GetModule(TGeometryId i) {
    TSettableGeometryId id(i);
    if (ND::GeomId::Def::kTPC
        != id.GetField(ND::GeomId::Def::kDetectorIdMSB,
                       ND::GeomId::Def::kDetectorIdLSB)) return -1;
    if (ND::GeomId::Def::TPC::kGlobal
        != id.GetField(ND::GeomId::Def::TPC::kSeqIdMSB,
                       ND::GeomId::Def::TPC::kSeqIdLSB)) return -1;
    if (ND::GeomId::Def::TPC::Global::kTPC
        != id.GetField(ND::GeomId::Def::TPC::Global::kSeqIdMSB,
                       ND::GeomId::Def::TPC::Global::kSeqIdLSB)) return -1;
    return id.GetField(ND::GeomId::Def::TPC::Global::kFieldMSB,
                       ND::GeomId::Def::TPC::Global::kFieldLSB);
}

ND::TGeometryId ND::GeomId::TPC::TPC1() {return Module(0);}
ND::TGeometryId ND::GeomId::TPC::TPC2() {return Module(1);}
ND::TGeometryId ND::GeomId::TPC::TPC3() {return Module(2);}

bool ND::GeomId::TPC::IsTPC1(TGeometryId i) {
    return (ND::GeomId::TPC::GetModule(i) == 0);
}

bool ND::GeomId::TPC::IsTPC2(TGeometryId i) {
    return (ND::GeomId::TPC::GetModule(i) == 1);
}

bool ND::GeomId::TPC::IsTPC3(TGeometryId i) {
    return (ND::GeomId::TPC::GetModule(i) == 2);
}

ND::TGeometryId ND::GeomId::TPC::MicroMega(int tpc, int half, int mm) {
    TSettableGeometryId id;
    if (tpc<0 || tpc>2) {
        ND280Warn("TPC module out of range [0,2]: " << tpc);
        return id;
    }
    if (half<0 || half>1) {
        ND280Warn("TPC half out of range [0,1]: " << half);
        return id;
    }
    if (mm<0 || mm>11) {
        ND280Warn("TPC micromega out of range [0,11]: " << mm);
        return id;
    }
    id.SetField(ND::GeomId::Def::kTPC,
                ND::GeomId::Def::kDetectorIdMSB,
                ND::GeomId::Def::kDetectorIdLSB);
    id.SetField(ND::GeomId::Def::TPC::kPad,
                ND::GeomId::Def::TPC::kSeqIdMSB,
                ND::GeomId::Def::TPC::kSeqIdLSB);
    id.SetField(tpc,
                ND::GeomId::Def::TPC::Pad::kTPCMSB,
                ND::GeomId::Def::TPC::Pad::kTPCLSB);
    id.SetField(half,
                ND::GeomId::Def::TPC::Pad::kHalfMSB,
                ND::GeomId::Def::TPC::Pad::kHalfLSB);
    id.SetField(mm,
                ND::GeomId::Def::TPC::Pad::kMMegaMSB,
                ND::GeomId::Def::TPC::Pad::kMMegaLSB);
    id.SetField(0,
                ND::GeomId::Def::TPC::Pad::kPadFlagMSB,
                ND::GeomId::Def::TPC::Pad::kPadFlagLSB);
    id.SetField(0,
                ND::GeomId::Def::TPC::Pad::kPadMSB,
                ND::GeomId::Def::TPC::Pad::kPadLSB);
    return id;
}

bool ND::GeomId::TPC::IsMicroMega(ND::TGeometryId i) {
    TSettableGeometryId id(i);
    int detector = id.GetField(ND::GeomId::Def::kDetectorIdMSB,
                               ND::GeomId::Def::kDetectorIdLSB);
    if (detector != ND::GeomId::Def::kTPC) return false;
    /// This may be a pad.
    int seqId = id.GetField(ND::GeomId::Def::TPC::kSeqIdMSB,
                            ND::GeomId::Def::TPC::kSeqIdLSB);
    if (seqId != ND::GeomId::Def::TPC::kPad) return false;
    int padFlag = id.GetField(ND::GeomId::Def::TPC::Pad::kPadFlagMSB,
                              ND::GeomId::Def::TPC::Pad::kPadFlagLSB);
    if (padFlag) return false;
    return true;
}

ND::TGeometryId ND::GeomId::TPC::Pad(int tpc, int half, int mm, int pad) {
    TSettableGeometryId id;
    if (tpc<0 || tpc>2) {
        ND280Warn("TPC module out of range [0,2]: " << tpc);
        return id;
    }
    if (half<0 || half>1) {
        ND280Warn("TPC half out of range [0,1]: " << half);
        return id;
    }
    if (mm<0 || mm>11) {
        ND280Warn("TPC micromega out of range [0,11]: " << mm);
        return id;
    }
    if (pad<0 || pad>1727) {
        ND280Warn("TPC pad out of range [0,1727]: " << pad);
        return id;
    }
    id.SetField(ND::GeomId::Def::kTPC,
                ND::GeomId::Def::kDetectorIdMSB,
                ND::GeomId::Def::kDetectorIdLSB);
    id.SetField(ND::GeomId::Def::TPC::kPad,
                ND::GeomId::Def::TPC::kSeqIdMSB,
                ND::GeomId::Def::TPC::kSeqIdLSB);
    id.SetField(tpc,
                ND::GeomId::Def::TPC::Pad::kTPCMSB,
                ND::GeomId::Def::TPC::Pad::kTPCLSB);
    id.SetField(half,
                ND::GeomId::Def::TPC::Pad::kHalfMSB,
                ND::GeomId::Def::TPC::Pad::kHalfLSB);
    id.SetField(mm,
                ND::GeomId::Def::TPC::Pad::kMMegaMSB,
                ND::GeomId::Def::TPC::Pad::kMMegaLSB);
    id.SetField(1,
                ND::GeomId::Def::TPC::Pad::kPadFlagMSB,
                ND::GeomId::Def::TPC::Pad::kPadFlagLSB);
    id.SetField(pad,
                ND::GeomId::Def::TPC::Pad::kPadMSB,
                ND::GeomId::Def::TPC::Pad::kPadLSB);
    return id;
}

int ND::GeomId::TPC::GetMicroMegaTPC(TGeometryId i) {
    return ND::GeomId::TPC::GetPadTPC(i);
}

int ND::GeomId::TPC::GetPadTPC(TGeometryId i) {
    TSettableGeometryId id(i);
    if (ND::GeomId::Def::kTPC
        != id.GetField(ND::GeomId::Def::kDetectorIdMSB,
                       ND::GeomId::Def::kDetectorIdLSB)) return -1;
    if (ND::GeomId::Def::TPC::kPad
        != id.GetField(ND::GeomId::Def::TPC::kSeqIdMSB,
                       ND::GeomId::Def::TPC::kSeqIdLSB)) return -1;
    return id.GetField(ND::GeomId::Def::TPC::Pad::kTPCMSB,
                       ND::GeomId::Def::TPC::Pad::kTPCLSB);
}

int ND::GeomId::TPC::GetMicroMegaHalf(TGeometryId i) {
    return ND::GeomId::TPC::GetPadHalf(i);
}

int ND::GeomId::TPC::GetPadHalf(TGeometryId i) {
    TSettableGeometryId id(i);
    if (ND::GeomId::Def::kTPC
        != id.GetField(ND::GeomId::Def::kDetectorIdMSB,
                       ND::GeomId::Def::kDetectorIdLSB)) return -1;
    if (ND::GeomId::Def::TPC::kPad
        != id.GetField(ND::GeomId::Def::TPC::kSeqIdMSB,
                       ND::GeomId::Def::TPC::kSeqIdLSB)) return -1;
    return id.GetField(ND::GeomId::Def::TPC::Pad::kHalfMSB,
                       ND::GeomId::Def::TPC::Pad::kHalfLSB);
}

int ND::GeomId::TPC::GetMicroMegaNumber(TGeometryId i) {
    return ND::GeomId::TPC::GetPadMicroMega(i);
}

int ND::GeomId::TPC::GetPadMicroMega(TGeometryId i) {
    TSettableGeometryId id(i);
    if (ND::GeomId::Def::kTPC
        != id.GetField(ND::GeomId::Def::kDetectorIdMSB,
                       ND::GeomId::Def::kDetectorIdLSB)) return -1;
    if (ND::GeomId::Def::TPC::kPad
        != id.GetField(ND::GeomId::Def::TPC::kSeqIdMSB,
                       ND::GeomId::Def::TPC::kSeqIdLSB)) return -1;
    return id.GetField(ND::GeomId::Def::TPC::Pad::kMMegaMSB,
                       ND::GeomId::Def::TPC::Pad::kMMegaLSB);
}

int ND::GeomId::TPC::GetPadNumber(TGeometryId i) {
    TSettableGeometryId id(i);
    if (ND::GeomId::Def::kTPC
        != id.GetField(ND::GeomId::Def::kDetectorIdMSB,
                       ND::GeomId::Def::kDetectorIdLSB)) return -1;
    if (ND::GeomId::Def::TPC::kPad
        != id.GetField(ND::GeomId::Def::TPC::kSeqIdMSB,
                       ND::GeomId::Def::TPC::kSeqIdLSB)) return -1;
    if (1 != id.GetField(ND::GeomId::Def::TPC::Pad::kPadFlagMSB,
                         ND::GeomId::Def::TPC::Pad::kPadFlagLSB)) return -1;
    return id.GetField(ND::GeomId::Def::TPC::Pad::kPadMSB,
                       ND::GeomId::Def::TPC::Pad::kPadLSB);
}

bool ND::GeomId::TPC::IsPad(ND::TGeometryId i) {
    TSettableGeometryId id(i);
    int detector = id.GetField(ND::GeomId::Def::kDetectorIdMSB,
                               ND::GeomId::Def::kDetectorIdLSB);
    if (detector != ND::GeomId::Def::kTPC) return false;
    /// This may be a pad.
    int seqId = id.GetField(ND::GeomId::Def::TPC::kSeqIdMSB,
                            ND::GeomId::Def::TPC::kSeqIdLSB);
    if (seqId != ND::GeomId::Def::TPC::kPad) return false;
    int padFlag = id.GetField(ND::GeomId::Def::TPC::Pad::kPadFlagMSB,
                              ND::GeomId::Def::TPC::Pad::kPadFlagLSB);
    if (!padFlag) return false;
    return true;
}

////////////////////////////////////////////////////////////
// FGD Geometry Identifiers.
////////////////////////////////////////////////////////////
bool ND::GeomId::FGD::IsFGD(TGeometryId i) {
    TSettableGeometryId id(i);
    return (ND::GeomId::Def::kFGD
            == id.GetField(ND::GeomId::Def::kDetectorIdMSB,
                           ND::GeomId::Def::kDetectorIdLSB));
}

ND::TGeometryId ND::GeomId::FGD::FGD(int fgd) {
    TSettableGeometryId id;
    if (fgd<0 || fgd>1) {
        ND280Warn("FGD out of range [0,1]: " << fgd);
        return id;
    }
    id.SetField(ND::GeomId::Def::kFGD,
                ND::GeomId::Def::kDetectorIdMSB,
                ND::GeomId::Def::kDetectorIdLSB);
    id.SetField(ND::GeomId::Def::FGD::kGlobal,
                ND::GeomId::Def::FGD::kSeqIdMSB,
                ND::GeomId::Def::FGD::kSeqIdLSB);
    id.SetField(ND::GeomId::Def::FGD::Global::kFGD,
                ND::GeomId::Def::FGD::Global::kSeqIdMSB,
                ND::GeomId::Def::FGD::Global::kSeqIdLSB);
    id.SetField(fgd,
                ND::GeomId::Def::FGD::Global::kFieldMSB,
                ND::GeomId::Def::FGD::Global::kFieldLSB);
    return id;
}

ND::TGeometryId ND::GeomId::FGD::FGD1() {return FGD(0);}
ND::TGeometryId ND::GeomId::FGD::FGD2() {return FGD(1);}

int ND::GeomId::FGD::GetFGD(TGeometryId i) {
    TSettableGeometryId id(i);
    if (ND::GeomId::Def::kFGD
        != id.GetField(ND::GeomId::Def::kDetectorIdMSB,
                       ND::GeomId::Def::kDetectorIdLSB)) return -1;
    if (ND::GeomId::Def::FGD::kGlobal
        != id.GetField(ND::GeomId::Def::FGD::kSeqIdMSB,
                       ND::GeomId::Def::FGD::kSeqIdLSB)) return -1;
    if (ND::GeomId::Def::FGD::Global::kFGD
        != id.GetField(ND::GeomId::Def::FGD::Global::kSeqIdMSB,
                       ND::GeomId::Def::FGD::Global::kSeqIdLSB)) return -1;
    return id.GetField(ND::GeomId::Def::FGD::Global::kFieldMSB,
                       ND::GeomId::Def::FGD::Global::kFieldLSB);
}

bool ND::GeomId::FGD::IsFGD1(TGeometryId id) {
    return (ND::GeomId::FGD::GetFGD(id) == 0);
}

bool ND::GeomId::FGD::IsFGD2(TGeometryId id) {
    return (ND::GeomId::FGD::GetFGD(id) == 1);
}

ND::TGeometryId ND::GeomId::FGD::Bar(int fgd,
                                     int module,
                                     int layer,
                                     int bar) {
    TSettableGeometryId id;
    if (fgd<0 || fgd>1) {
        ND280Warn("FGD out of range [0,1]: " << fgd);
        return id;
    }
    id.SetField(ND::GeomId::Def::kFGD,
                ND::GeomId::Def::kDetectorIdMSB,
                ND::GeomId::Def::kDetectorIdLSB);
    id.SetField(ND::GeomId::Def::FGD::kScintillator,
                ND::GeomId::Def::FGD::kSeqIdMSB,
                ND::GeomId::Def::FGD::kSeqIdLSB);
    id.SetField(fgd,
                ND::GeomId::Def::FGD::Bar::kModuleMSB,
                ND::GeomId::Def::FGD::Bar::kModuleLSB);
    id.SetField(module,
                ND::GeomId::Def::FGD::Bar::kPlaneMSB,
                ND::GeomId::Def::FGD::Bar::kPlaneLSB);
    id.SetField(layer,
                ND::GeomId::Def::FGD::Bar::kLayerMSB,
                ND::GeomId::Def::FGD::Bar::kLayerLSB);
    id.SetField(bar,
                ND::GeomId::Def::FGD::Bar::kBarMSB,
                ND::GeomId::Def::FGD::Bar::kBarLSB);
    return id;
}

int ND::GeomId::FGD::GetBarFGD(TGeometryId i) {
    TSettableGeometryId id(i);
    if (ND::GeomId::Def::kFGD
        != id.GetField(ND::GeomId::Def::kDetectorIdMSB,
                       ND::GeomId::Def::kDetectorIdLSB)) return -1;
    if (ND::GeomId::Def::FGD::kScintillator
        != id.GetField(ND::GeomId::Def::FGD::kSeqIdMSB,
                       ND::GeomId::Def::FGD::kSeqIdLSB)) return -1;
    return id.GetField(ND::GeomId::Def::FGD::Bar::kModuleMSB,
                       ND::GeomId::Def::FGD::Bar::kModuleLSB);
}

int ND::GeomId::FGD::GetBarModule(TGeometryId i) {
    TSettableGeometryId id(i);
    if (ND::GeomId::Def::kFGD
        != id.GetField(ND::GeomId::Def::kDetectorIdMSB,
                       ND::GeomId::Def::kDetectorIdLSB)) return -1;
    if (ND::GeomId::Def::FGD::kScintillator
        != id.GetField(ND::GeomId::Def::FGD::kSeqIdMSB,
                       ND::GeomId::Def::FGD::kSeqIdLSB)) return -1;
    return id.GetField(ND::GeomId::Def::FGD::Bar::kPlaneMSB,
                       ND::GeomId::Def::FGD::Bar::kPlaneLSB);
}

int ND::GeomId::FGD::GetBarLayer(TGeometryId i) {
    TSettableGeometryId id(i);
    if (ND::GeomId::Def::kFGD
        != id.GetField(ND::GeomId::Def::kDetectorIdMSB,
                       ND::GeomId::Def::kDetectorIdLSB)) return -1;
    if (ND::GeomId::Def::FGD::kScintillator
        != id.GetField(ND::GeomId::Def::FGD::kSeqIdMSB,
                       ND::GeomId::Def::FGD::kSeqIdLSB)) return -1;
    return id.GetField(ND::GeomId::Def::FGD::Bar::kLayerMSB,
                       ND::GeomId::Def::FGD::Bar::kLayerLSB);
}

int ND::GeomId::FGD::GetBarNumber(TGeometryId i) {
    TSettableGeometryId id(i);
    if (ND::GeomId::Def::kFGD
        != id.GetField(ND::GeomId::Def::kDetectorIdMSB,
                       ND::GeomId::Def::kDetectorIdLSB)) return -1;
    if (ND::GeomId::Def::FGD::kScintillator
        != id.GetField(ND::GeomId::Def::FGD::kSeqIdMSB,
                       ND::GeomId::Def::FGD::kSeqIdLSB)) return -1;
    return id.GetField(ND::GeomId::Def::FGD::Bar::kBarMSB,
                       ND::GeomId::Def::FGD::Bar::kBarLSB);
}

ND::TGeometryId ND::GeomId::FGD::Target(int target) {
    TSettableGeometryId id;
    id.SetField(ND::GeomId::Def::kFGD,
                ND::GeomId::Def::kDetectorIdMSB,
                ND::GeomId::Def::kDetectorIdLSB);
    id.SetField(ND::GeomId::Def::FGD::kGlobal,
                ND::GeomId::Def::FGD::kSeqIdMSB,
                ND::GeomId::Def::FGD::kSeqIdLSB);
    id.SetField(ND::GeomId::Def::FGD::Global::kTarget,
                ND::GeomId::Def::FGD::Global::kSeqIdMSB,
                ND::GeomId::Def::FGD::Global::kSeqIdLSB);
    id.SetField(target,
                ND::GeomId::Def::FGD::Global::kFieldMSB,
                ND::GeomId::Def::FGD::Global::kFieldLSB);
    return id;
}

int ND::GeomId::FGD::GetTarget(TGeometryId i) {
    TSettableGeometryId id(i);
    if (ND::GeomId::Def::kFGD
        != id.GetField(ND::GeomId::Def::kDetectorIdMSB,
                       ND::GeomId::Def::kDetectorIdLSB)) return -1;
    if (ND::GeomId::Def::FGD::kGlobal
        != id.GetField(ND::GeomId::Def::FGD::kSeqIdMSB,
                       ND::GeomId::Def::FGD::kSeqIdLSB)) return -1;
    if (ND::GeomId::Def::FGD::Global::kTarget
        != id.GetField(ND::GeomId::Def::FGD::Global::kSeqIdMSB,
                       ND::GeomId::Def::FGD::Global::kSeqIdLSB)) return -1;
    return id.GetField(ND::GeomId::Def::FGD::Global::kFieldMSB,
                       ND::GeomId::Def::FGD::Global::kFieldLSB);
}

ND::TGeometryId ND::GeomId::FGD::Layer(int fgd,
                                       int module,
                                       int layer) {
    TSettableGeometryId id;
    if (fgd<0 || fgd>1) {
        ND280Warn("FGD out of range [0,1]: " << fgd);
        return id;
    }
    id.SetField(ND::GeomId::Def::kFGD,
                ND::GeomId::Def::kDetectorIdMSB,
                ND::GeomId::Def::kDetectorIdLSB);
    id.SetField(ND::GeomId::Def::FGD::kGlobal,
                ND::GeomId::Def::FGD::kSeqIdMSB,
                ND::GeomId::Def::FGD::kSeqIdLSB);
    id.SetField(ND::GeomId::Def::FGD::Global::kLayer,
                ND::GeomId::Def::FGD::Global::kSeqIdMSB,
                ND::GeomId::Def::FGD::Global::kSeqIdLSB);
    id.SetField(fgd,
                ND::GeomId::Def::FGD::Global::Layer::kFGDMSB,
                ND::GeomId::Def::FGD::Global::Layer::kFGDLSB);
    id.SetField(layer,
                ND::GeomId::Def::FGD::Global::Layer::kLayerMSB,
                ND::GeomId::Def::FGD::Global::Layer::kLayerLSB);
    id.SetField(module,
                ND::GeomId::Def::FGD::Global::Layer::kModuleMSB,
                ND::GeomId::Def::FGD::Global::Layer::kModuleLSB);
    return id;
}

int ND::GeomId::FGD::GetLayerFGD(TGeometryId i) {
    TSettableGeometryId id(i);
    if (ND::GeomId::Def::kFGD
        != id.GetField(ND::GeomId::Def::kDetectorIdMSB,
                       ND::GeomId::Def::kDetectorIdLSB)) return -1;
    if (ND::GeomId::Def::FGD::kGlobal
        != id.GetField(ND::GeomId::Def::FGD::kSeqIdMSB,
                       ND::GeomId::Def::FGD::kSeqIdLSB)) return -1;
    if (ND::GeomId::Def::FGD::Global::kLayer
        != id.GetField(ND::GeomId::Def::FGD::Global::kSeqIdMSB,
                       ND::GeomId::Def::FGD::Global::kSeqIdLSB)) return -1;
    return id.GetField(ND::GeomId::Def::FGD::Global::Layer::kFGDMSB,
                       ND::GeomId::Def::FGD::Global::Layer::kFGDLSB);
}

int ND::GeomId::FGD::GetLayerModule(TGeometryId i) {
    TSettableGeometryId id(i);
    if (ND::GeomId::Def::kFGD
        != id.GetField(ND::GeomId::Def::kDetectorIdMSB,
                       ND::GeomId::Def::kDetectorIdLSB)) return -1;
    if (ND::GeomId::Def::FGD::kGlobal
        != id.GetField(ND::GeomId::Def::FGD::kSeqIdMSB,
                       ND::GeomId::Def::FGD::kSeqIdLSB)) return -1;
    if (ND::GeomId::Def::FGD::Global::kLayer
        != id.GetField(ND::GeomId::Def::FGD::Global::kSeqIdMSB,
                       ND::GeomId::Def::FGD::Global::kSeqIdLSB)) return -1;
    return id.GetField(ND::GeomId::Def::FGD::Global::Layer::kModuleMSB,
                       ND::GeomId::Def::FGD::Global::Layer::kModuleLSB);
}

int ND::GeomId::FGD::GetLayerNumber(TGeometryId i) {
    TSettableGeometryId id(i);
    if (ND::GeomId::Def::kFGD
        != id.GetField(ND::GeomId::Def::kDetectorIdMSB,
                       ND::GeomId::Def::kDetectorIdLSB)) return -1;
    if (ND::GeomId::Def::FGD::kGlobal
        != id.GetField(ND::GeomId::Def::FGD::kSeqIdMSB,
                       ND::GeomId::Def::FGD::kSeqIdLSB)) return -1;
    if (ND::GeomId::Def::FGD::Global::kLayer
        != id.GetField(ND::GeomId::Def::FGD::Global::kSeqIdMSB,
                       ND::GeomId::Def::FGD::Global::kSeqIdLSB)) return -1;
    return id.GetField(ND::GeomId::Def::FGD::Global::Layer::kLayerMSB,
                       ND::GeomId::Def::FGD::Global::Layer::kLayerLSB);
}

////////////////////////////////////////////////////////////
// ECal Geometry Identifiers.
////////////////////////////////////////////////////////////

ND::TGeometryId ND::GeomId::ECal::Module(int ecal, int clam, int module) {
    TSettableGeometryId id;
    if (ecal == ND::GeomId::Def::kDSECal) {
        if (clam != 0) {
            ND280Error("Downstream ECal with invalid clam: " << clam);
            return id;
        }
        if (module != 0) {
            ND280Error("Downstream ECal with invalid module: " << module);
            return id;
        }
    }
    else if (ecal == ND::GeomId::Def::kTECal) {
        if (clam != 0 && clam != 1) {
            ND280Error("Tracker ECal with invalid clam: " << clam);
            return id;
        }
        if (module<0 || 2<module) {
            ND280Error("Tracker ECal with invalid module: " << module);
            return id;
        }
    }
    else if (ecal == ND::GeomId::Def::kPECal) {
        if (clam != 0 && clam != 1) {
            ND280Error("P0D ECal with invalid clam: " << clam);
            return id;
        }
        if (module<0 || 2<module) {
            ND280Error("P0D ECal with invalid module: " << module);
            return id;
        }
    }
    id.SetField(ecal,
                ND::GeomId::Def::kDetectorIdMSB,
                ND::GeomId::Def::kDetectorIdLSB);
    id.SetField(ND::GeomId::Def::ECal::kGlobal,
                ND::GeomId::Def::ECal::kSeqIdMSB,
                ND::GeomId::Def::ECal::kSeqIdLSB);
    id.SetField(module*2+clam,
                ND::GeomId::Def::ECal::kModuleMSB,
                ND::GeomId::Def::ECal::kModuleLSB);

    id.SetField(ND::GeomId::Def::ECal::Global::kECal,
                ND::GeomId::Def::ECal::Global::kSeqIdMSB,
                ND::GeomId::Def::ECal::Global::kSeqIdLSB);
    id.SetField(0,
                ND::GeomId::Def::ECal::Global::kFieldMSB,
                ND::GeomId::Def::ECal::Global::kFieldLSB);

    return id;
}

ND::TGeometryId ND::GeomId::ECal::Container(int ecal, int clam, int module) {
    TSettableGeometryId id;
    if (ecal ==  ND::GeomId::Def::kTECal) {
        if (clam != 0 && clam != 1) {
            ND280Error("Tracker ECal container with invalid clam: " << clam);
            return id;
        }
        if (module<0 || 2<module) {
            ND280Error("Tracker ECal container with invalid module: " << module);
            return id;
        }
    }
    else if (ecal == ND::GeomId::Def::kPECal) {
        if (clam != 0 && clam != 1) {
            ND280Error("P0D ECal container with invalid clam: " << clam);
            return id;
        }
        if (module<0 || 2<module) {
            ND280Error("P0D ECal container with invalid module: " << module);
            return id;
        }
    }
    else {
        ND280Error("Container for non-contained ECal: " << ecal);
        return id;
    }

    id.SetField(ecal,
                ND::GeomId::Def::kDetectorIdMSB,
                ND::GeomId::Def::kDetectorIdLSB);
    id.SetField(ND::GeomId::Def::ECal::kGlobal,
                ND::GeomId::Def::ECal::kSeqIdMSB,
                ND::GeomId::Def::ECal::kSeqIdLSB);
    id.SetField(module*2+clam,
                ND::GeomId::Def::ECal::kModuleMSB,
                ND::GeomId::Def::ECal::kModuleLSB);

    id.SetField(ND::GeomId::Def::ECal::Global::kContainer,
                ND::GeomId::Def::ECal::Global::kSeqIdMSB,
                ND::GeomId::Def::ECal::Global::kSeqIdLSB);
    id.SetField(0,
                ND::GeomId::Def::ECal::Global::kFieldMSB,
                ND::GeomId::Def::ECal::Global::kFieldLSB);

    return id;
}

bool ND::GeomId::ECal::IsECal(TGeometryId i) {
    TSettableGeometryId id(i);
    int det = id.GetField(ND::GeomId::Def::kDetectorIdMSB,
                           ND::GeomId::Def::kDetectorIdLSB);
    if (det == ND::GeomId::Def::kTECal) return true;
    if (det == ND::GeomId::Def::kPECal) return true;
    if (det == ND::GeomId::Def::kDSECal) return true;
    return false;
}

int ND::GeomId::ECal::GetClam(TGeometryId i) {
    if (!ND::GeomId::ECal::IsECal(i)) return -1;
    TSettableGeometryId id(i);
    int mod = id.GetField(ND::GeomId::Def::ECal::kModuleMSB,
                          ND::GeomId::Def::ECal::kModuleLSB);
    return (mod%2);
}

int ND::GeomId::ECal::GetModule(TGeometryId i) {
    if (!ND::GeomId::ECal::IsECal(i)) return -1;
    TSettableGeometryId id(i);
    int mod = id.GetField(ND::GeomId::Def::ECal::kModuleMSB,
                          ND::GeomId::Def::ECal::kModuleLSB);
    return (mod/2);
}

int ND::GeomId::ECal::GetModuleECal(TGeometryId i) {
    TSettableGeometryId id(i);
    int det = ND::GeomId::GetSubSystem(id);
    if (det == ND::GeomId::Def::kDSECal) return det;
    else if (det == ND::GeomId::Def::kTECal) return det;
    else if (det == ND::GeomId::Def::kPECal) return det;
    else return -1;
}

int ND::GeomId::ECal::GetModuleNumber(TGeometryId i) {
    TSettableGeometryId id(i);
    if (ND::GeomId::ECal::GetModuleECal(i)<0) return -1;
    if (ND::GeomId::Def::ECal::kGlobal
        != id.GetField(ND::GeomId::Def::ECal::kSeqIdMSB,
                       ND::GeomId::Def::ECal::kSeqIdLSB)) return -1;
    if (ND::GeomId::Def::ECal::Global::kECal
        != id.GetField(ND::GeomId::Def::ECal::Global::kSeqIdMSB,
                       ND::GeomId::Def::ECal::Global::kSeqIdLSB)) return -1;

    return id.GetField(ND::GeomId::Def::ECal::kModuleMSB,
                       ND::GeomId::Def::ECal::kModuleLSB) / 2;
}

int ND::GeomId::ECal::GetModuleClam(TGeometryId i) {
    TSettableGeometryId id(i);
    if (ND::GeomId::ECal::GetModuleECal(i)<0) return -1;
    if (ND::GeomId::Def::ECal::kGlobal
        != id.GetField(ND::GeomId::Def::ECal::kSeqIdMSB,
                       ND::GeomId::Def::ECal::kSeqIdLSB)) return -1;
    if (ND::GeomId::Def::ECal::Global::kECal
        != id.GetField(ND::GeomId::Def::ECal::Global::kSeqIdMSB,
                       ND::GeomId::Def::ECal::Global::kSeqIdLSB)) return -1;

    return id.GetField(ND::GeomId::Def::ECal::kModuleMSB,
                       ND::GeomId::Def::ECal::kModuleLSB) % 2;
}

ND::TGeometryId ND::GeomId::ECal::Layer(int ecal, int clam,
                                        int module, int layer) {
    TSettableGeometryId id;
    if (ecal == ND::GeomId::Def::kDSECal) {
        if (clam != 0) {
            ND280Error("Downstream ECal with invalid clam: " << clam);
            return id;
        }
        if (module != 0) {
            ND280Error("Downstream ECal with invalid module: " << module);
            return id;
        }
    }
    else if (ecal == ND::GeomId::Def::kTECal) {
        if (clam != 0 && clam != 1) {
            ND280Error("Tracker ECal with invalid clam: " << clam);
            return id;
        }
        if (module<0 || 2<module) {
            ND280Error("Tracker ECal with invalid module: " << module);
            return id;
        }
    }
    else if (ecal == ND::GeomId::Def::kPECal) {
        if (clam != 0 && clam != 1) {
            ND280Error("P0D ECal with invalid clam: " << clam);
            return id;
        }
        if (module<0 || 2<module) {
            ND280Error("P0D ECal with invalid module: " << module);
            return id;
        }
    }
    id.SetField(ecal,
                ND::GeomId::Def::kDetectorIdMSB,
                ND::GeomId::Def::kDetectorIdLSB);
    id.SetField(ND::GeomId::Def::ECal::kGlobal,
                ND::GeomId::Def::ECal::kSeqIdMSB,
                ND::GeomId::Def::ECal::kSeqIdLSB);
    id.SetField(module*2+clam,
                ND::GeomId::Def::ECal::kModuleMSB,
                ND::GeomId::Def::ECal::kModuleLSB);
    id.SetField(ND::GeomId::Def::ECal::Global::kLayer,
                ND::GeomId::Def::ECal::Global::kSeqIdMSB,
                ND::GeomId::Def::ECal::Global::kSeqIdLSB);
    id.SetField(layer,
                ND::GeomId::Def::ECal::Global::kFieldMSB,
                ND::GeomId::Def::ECal::Global::kFieldLSB);
    return id;
}

int ND::GeomId::ECal::GetLayerECal(TGeometryId i) {
    TSettableGeometryId id(i);
    int det = ND::GeomId::GetSubSystem(id);
    if (det == ND::GeomId::Def::kDSECal) return det;
    else if (det == ND::GeomId::Def::kTECal) return det;
    else if (det == ND::GeomId::Def::kPECal) return det;
    else return -1;
}

int ND::GeomId::ECal::GetLayerClam(TGeometryId i) {
    TSettableGeometryId id(i);
    if (ND::GeomId::ECal::GetLayerECal(i)<0) return -1;
    if (ND::GeomId::Def::ECal::kGlobal
        != id.GetField(ND::GeomId::Def::ECal::kSeqIdMSB,
                       ND::GeomId::Def::ECal::kSeqIdLSB)) return -1;
    if (ND::GeomId::Def::ECal::Global::kLayer
        != id.GetField(ND::GeomId::Def::ECal::Global::kSeqIdMSB,
                       ND::GeomId::Def::ECal::Global::kSeqIdLSB)) return -1;

    return id.GetField(ND::GeomId::Def::ECal::kModuleMSB,
                       ND::GeomId::Def::ECal::kModuleLSB) % 2;
}

int ND::GeomId::ECal::GetLayerModule(TGeometryId i) {
    TSettableGeometryId id(i);
    if (ND::GeomId::ECal::GetLayerECal(i)<0) return -1;
    if (ND::GeomId::Def::ECal::kGlobal
        != id.GetField(ND::GeomId::Def::ECal::kSeqIdMSB,
                       ND::GeomId::Def::ECal::kSeqIdLSB)) return -1;
    if (ND::GeomId::Def::ECal::Global::kLayer
        != id.GetField(ND::GeomId::Def::ECal::Global::kSeqIdMSB,
                       ND::GeomId::Def::ECal::Global::kSeqIdLSB)) return -1;

    return id.GetField(ND::GeomId::Def::ECal::kModuleMSB,
                       ND::GeomId::Def::ECal::kModuleLSB) / 2;
}

int ND::GeomId::ECal::GetLayerNumber(TGeometryId i) {
    TSettableGeometryId id(i);
    if (ND::GeomId::ECal::GetLayerECal(i)<0) return -1;
    if (ND::GeomId::Def::ECal::kGlobal
        != id.GetField(ND::GeomId::Def::ECal::kSeqIdMSB,
                       ND::GeomId::Def::ECal::kSeqIdLSB)) return -1;
    if (ND::GeomId::Def::ECal::Global::kLayer
        != id.GetField(ND::GeomId::Def::ECal::Global::kSeqIdMSB,
                       ND::GeomId::Def::ECal::Global::kSeqIdLSB)) return -1;

    return id.GetField(ND::GeomId::Def::ECal::Global::kFieldMSB,
                       ND::GeomId::Def::ECal::Global::kFieldLSB);
}

ND::TGeometryId ND::GeomId::ECal::Radiator(int ecal, int clam,
                                           int module, int rad) {
    TSettableGeometryId id;
    if (ecal == ND::GeomId::Def::kDSECal) {
        if (clam != 0) {
            ND280Error("Downstream ECal with invalid clam: " << clam);
            return id;
        }
        if (module != 0) {
            ND280Error("Downstream ECal with invalid module: " << module);
            return id;
        }
    }
    else if (ecal == ND::GeomId::Def::kTECal) {
        if (clam != 0 && clam != 1) {
            ND280Error("Tracker ECal with invalid clam: " << clam);
            return id;
        }
        if (module<0 || 2<module) {
            ND280Error("Tracker ECal with invalid module: " << module);
            return id;
        }
    }
    else if (ecal == ND::GeomId::Def::kPECal) {
        if (clam != 0 && clam != 1) {
            ND280Error("P0D ECal with invalid clam: " << clam);
            return id;
        }
        if (module<0 || 2<module) {
            ND280Error("P0D ECal with invalid module: " << module);
            return id;
        }
    }
    id.SetField(ecal,
                ND::GeomId::Def::kDetectorIdMSB,
                ND::GeomId::Def::kDetectorIdLSB);
    id.SetField(ND::GeomId::Def::ECal::kGlobal,
                ND::GeomId::Def::ECal::kSeqIdMSB,
                ND::GeomId::Def::ECal::kSeqIdLSB);
    id.SetField(module*2+clam,
                ND::GeomId::Def::ECal::kModuleMSB,
                ND::GeomId::Def::ECal::kModuleLSB);
    id.SetField(ND::GeomId::Def::ECal::Global::kRadiator,
                ND::GeomId::Def::ECal::Global::kSeqIdMSB,
                ND::GeomId::Def::ECal::Global::kSeqIdLSB);
    id.SetField(rad,
                ND::GeomId::Def::ECal::Global::kFieldMSB,
                ND::GeomId::Def::ECal::Global::kFieldLSB);
    return id;
}

int ND::GeomId::ECal::GetRadiatorECal(TGeometryId i) {
    TSettableGeometryId id(i);
    int det = ND::GeomId::GetSubSystem(id);
    if (det == ND::GeomId::Def::kDSECal) return det;
    else if (det == ND::GeomId::Def::kTECal) return det;
    else if (det == ND::GeomId::Def::kPECal) return det;
    else return -1;
}

int ND::GeomId::ECal::GetRadiatorClam(TGeometryId i) {
    TSettableGeometryId id(i);
    if (ND::GeomId::ECal::GetRadiatorECal(i)<0) return -1;
    if (ND::GeomId::Def::ECal::kGlobal
        != id.GetField(ND::GeomId::Def::ECal::kSeqIdMSB,
                       ND::GeomId::Def::ECal::kSeqIdLSB)) return -1;
    if (ND::GeomId::Def::ECal::Global::kRadiator
        != id.GetField(ND::GeomId::Def::ECal::Global::kSeqIdMSB,
                       ND::GeomId::Def::ECal::Global::kSeqIdLSB)) return -1;

    return id.GetField(ND::GeomId::Def::ECal::kModuleMSB,
                       ND::GeomId::Def::ECal::kModuleLSB) % 2;
}

int ND::GeomId::ECal::GetRadiatorModule(TGeometryId i) {
    TSettableGeometryId id(i);
    if (ND::GeomId::ECal::GetRadiatorECal(i)<0) return -1;
    if (ND::GeomId::Def::ECal::kGlobal
        != id.GetField(ND::GeomId::Def::ECal::kSeqIdMSB,
                       ND::GeomId::Def::ECal::kSeqIdLSB)) return -1;
    if (ND::GeomId::Def::ECal::Global::kRadiator
        != id.GetField(ND::GeomId::Def::ECal::Global::kSeqIdMSB,
                       ND::GeomId::Def::ECal::Global::kSeqIdLSB)) return -1;

    return id.GetField(ND::GeomId::Def::ECal::kModuleMSB,
                       ND::GeomId::Def::ECal::kModuleLSB) / 2;
}

int ND::GeomId::ECal::GetRadiatorNumber(TGeometryId i) {
    TSettableGeometryId id(i);
    if (ND::GeomId::ECal::GetRadiatorECal(i)<0) return -1;
    if (ND::GeomId::Def::ECal::kGlobal
        != id.GetField(ND::GeomId::Def::ECal::kSeqIdMSB,
                       ND::GeomId::Def::ECal::kSeqIdLSB)) return -1;
    if (ND::GeomId::Def::ECal::Global::kRadiator
        != id.GetField(ND::GeomId::Def::ECal::Global::kSeqIdMSB,
                       ND::GeomId::Def::ECal::Global::kSeqIdLSB)) return -1;

    return id.GetField(ND::GeomId::Def::ECal::Global::kFieldMSB,
                       ND::GeomId::Def::ECal::Global::kFieldLSB);
}

ND::TGeometryId ND::GeomId::ECal::Bar(int ecal, int clam,
                                      int module, int layer, int bar) {
    TSettableGeometryId id;
    if (ecal == ND::GeomId::Def::kDSECal) {
        if (clam != 0) {
            ND280Error("Downstream ECal with invalid clam: " << clam);
            return id;
        }
        if (module != 0) {
            ND280Error("Downstream ECal with invalid module: " << module);
            return id;
        }
    }
    else if (ecal == ND::GeomId::Def::kTECal) {
        if (clam != 0 && clam != 1) {
            ND280Error("Tracker ECal with invalid clam: " << clam);
            return id;
        }
        if (module<0 || 2<module) {
            ND280Error("Tracker ECal with invalid module: " << module);
            return id;
        }
    }
    else if (ecal == ND::GeomId::Def::kPECal) {
        if (clam != 0 && clam != 1) {
            ND280Error("P0D ECal with invalid clam: " << clam);
            return id;
        }
        if (module<0 || 2<module) {
            ND280Error("P0D ECal with invalid module: " << module);
            return id;
        }
    }
    id.SetField(ecal,
                ND::GeomId::Def::kDetectorIdMSB,
                ND::GeomId::Def::kDetectorIdLSB);
    id.SetField(ND::GeomId::Def::ECal::kScintillator,
                ND::GeomId::Def::ECal::kSeqIdMSB,
                ND::GeomId::Def::ECal::kSeqIdLSB);
    id.SetField(module*2+clam,
                ND::GeomId::Def::ECal::kModuleMSB,
                ND::GeomId::Def::ECal::kModuleLSB);
    id.SetField(layer,
                ND::GeomId::Def::ECal::Bar::kLayerMSB,
                ND::GeomId::Def::ECal::Bar::kLayerLSB);
    id.SetField(bar,
                ND::GeomId::Def::ECal::Bar::kBarMSB,
                ND::GeomId::Def::ECal::Bar::kBarLSB);
    return id;
}

int ND::GeomId::ECal::GetBarECal(TGeometryId i) {
    TSettableGeometryId id(i);
    int det = ND::GeomId::GetSubSystem(id);
    if (det == ND::GeomId::Def::kDSECal) return det;
    else if (det == ND::GeomId::Def::kTECal) return det;
    else if (det == ND::GeomId::Def::kPECal) return det;
    else return -1;
}

int ND::GeomId::ECal::GetBarClam(TGeometryId i) {
    TSettableGeometryId id(i);
    if (ND::GeomId::ECal::GetBarECal(i)<0) return -1;
    if (ND::GeomId::Def::ECal::kScintillator
        != id.GetField(ND::GeomId::Def::ECal::kSeqIdMSB,
                       ND::GeomId::Def::ECal::kSeqIdLSB)) return -1;

    return id.GetField(ND::GeomId::Def::ECal::kModuleMSB,
                       ND::GeomId::Def::ECal::kModuleLSB) % 2;
}

int ND::GeomId::ECal::GetBarModule(TGeometryId i) {
    TSettableGeometryId id(i);
    if (ND::GeomId::ECal::GetBarECal(i)<0) return -1;
    if (ND::GeomId::Def::ECal::kScintillator
        != id.GetField(ND::GeomId::Def::ECal::kSeqIdMSB,
                       ND::GeomId::Def::ECal::kSeqIdLSB)) return -1;

    return id.GetField(ND::GeomId::Def::ECal::kModuleMSB,
                       ND::GeomId::Def::ECal::kModuleLSB) / 2;
}

int ND::GeomId::ECal::GetBarLayer(TGeometryId i) {
    TSettableGeometryId id(i);
    if (ND::GeomId::ECal::GetBarECal(i)<0) return -1;
    if (ND::GeomId::Def::ECal::kScintillator
        != id.GetField(ND::GeomId::Def::ECal::kSeqIdMSB,
                       ND::GeomId::Def::ECal::kSeqIdLSB)) return -1;

    return id.GetField(ND::GeomId::Def::ECal::Bar::kLayerMSB,
                       ND::GeomId::Def::ECal::Bar::kLayerLSB);
}

int ND::GeomId::ECal::GetBarNumber(TGeometryId i) {
    TSettableGeometryId id(i);
    if (ND::GeomId::ECal::GetBarECal(i)<0) return -1;
    if (ND::GeomId::Def::ECal::kScintillator
        != id.GetField(ND::GeomId::Def::ECal::kSeqIdMSB,
                       ND::GeomId::Def::ECal::kSeqIdLSB)) return -1;

    return id.GetField(ND::GeomId::Def::ECal::Bar::kBarMSB,
                       ND::GeomId::Def::ECal::Bar::kBarLSB);
}

////////////////////////////////////////////////////////////
// Downstream ECal Geometry Identifiers.
////////////////////////////////////////////////////////////
bool ND::GeomId::DSECal::IsDSECal(TGeometryId i) {
    TSettableGeometryId id(i);
    return (ND::GeomId::Def::kDSECal
            == id.GetField(ND::GeomId::Def::kDetectorIdMSB,
                           ND::GeomId::Def::kDetectorIdLSB));
}

ND::TGeometryId ND::GeomId::DSECal::Detector() {
    return ND::GeomId::ECal::Module(ND::GeomId::Def::kDSECal,0,0);
}

ND::TGeometryId ND::GeomId::DSECal::Layer(int layer) {
    return ND::GeomId::ECal::Layer(ND::GeomId::Def::kDSECal,0,0,layer);
}

int ND::GeomId::DSECal::GetLayer(TGeometryId i) {
    if (!ND::GeomId::DSECal::IsDSECal(i)) return -1;
    return ND::GeomId::ECal::GetLayerNumber(i);
}

ND::TGeometryId ND::GeomId::DSECal::Radiator(int rad) {
    return ND::GeomId::ECal::Radiator(ND::GeomId::Def::kDSECal,0,0,rad);
}

int ND::GeomId::DSECal::GetRadiator(TGeometryId i) {
    if (!ND::GeomId::DSECal::IsDSECal(i)) return -1;
    return ND::GeomId::ECal::GetRadiatorNumber(i);
}

ND::TGeometryId ND::GeomId::DSECal::Bar(int layer, int bar) {
    return ND::GeomId::ECal::Bar(ND::GeomId::Def::kDSECal,
                                 0,0,layer,bar);
}

int ND::GeomId::DSECal::GetBarLayer(TGeometryId i) {
    if (!ND::GeomId::DSECal::IsDSECal(i)) return -1;
    return ND::GeomId::ECal::GetBarLayer(i);
}

int ND::GeomId::DSECal::GetBarNumber(TGeometryId i) {
    if (!ND::GeomId::DSECal::IsDSECal(i)) return -1;
    return ND::GeomId::ECal::GetBarNumber(i);
}

////////////////////////////////////////////////////////////
// Tracker ECal Geometry Identifiers.
////////////////////////////////////////////////////////////
bool ND::GeomId::TECal::IsTECal(TGeometryId i) {
    TSettableGeometryId id(i);
    return (ND::GeomId::Def::kTECal
            == id.GetField(ND::GeomId::Def::kDetectorIdMSB,
                           ND::GeomId::Def::kDetectorIdLSB));
}

ND::TGeometryId ND::GeomId::TECal::Module(int clam, int module) {
    return ND::GeomId::ECal::Module(ND::GeomId::Def::kTECal,
                                    clam,module);
}

ND::TGeometryId ND::GeomId::TECal::Layer(int clam, int module, int layer) {
    return ND::GeomId::ECal::Layer(ND::GeomId::Def::kTECal,
                                   clam,module,layer);
}

ND::TGeometryId ND::GeomId::TECal::Radiator(int clam, int module, int rad) {
    return ND::GeomId::ECal::Radiator(ND::GeomId::Def::kTECal,
                                      clam,module,rad);
}

ND::TGeometryId ND::GeomId::TECal::Bar(int clam, int mod, int layer, int bar) {
    return ND::GeomId::ECal::Bar(ND::GeomId::Def::kTECal,
                                      clam,mod,layer,bar);
}

int ND::GeomId::TECal::GetModuleNumber(TGeometryId i) {
    if (!ND::GeomId::TECal::IsTECal(i)) return -1;
    return ND::GeomId::ECal::GetModuleNumber(i);
}

int ND::GeomId::TECal::GetModuleClam(TGeometryId i) {
    if (!ND::GeomId::TECal::IsTECal(i)) return -1;
    return ND::GeomId::ECal::GetModuleClam(i);
}

int ND::GeomId::TECal::GetLayerNumber(TGeometryId i) {
    if (!ND::GeomId::TECal::IsTECal(i)) return -1;
    return ND::GeomId::ECal::GetLayerNumber(i);
}

int ND::GeomId::TECal::GetLayerClam(TGeometryId i) {
    if (!ND::GeomId::TECal::IsTECal(i)) return -1;
    return ND::GeomId::ECal::GetLayerClam(i);
}

int ND::GeomId::TECal::GetLayerModule(TGeometryId i) {
    if (!ND::GeomId::TECal::IsTECal(i)) return -1;
    return ND::GeomId::ECal::GetLayerModule(i);
}

int ND::GeomId::TECal::GetRadiatorNumber(TGeometryId i) {
    if (!ND::GeomId::TECal::IsTECal(i)) return -1;
    return ND::GeomId::ECal::GetRadiatorNumber(i);
}

int ND::GeomId::TECal::GetRadiatorClam(TGeometryId i) {
    if (!ND::GeomId::TECal::IsTECal(i)) return -1;
    return ND::GeomId::ECal::GetRadiatorClam(i);
}

int ND::GeomId::TECal::GetRadiatorModule(TGeometryId i) {
    if (!ND::GeomId::TECal::IsTECal(i)) return -1;
    return ND::GeomId::ECal::GetRadiatorModule(i);
}

int ND::GeomId::TECal::GetBarNumber(TGeometryId i) {
    if (!ND::GeomId::TECal::IsTECal(i)) return -1;
    return ND::GeomId::ECal::GetBarNumber(i);
}

int ND::GeomId::TECal::GetBarLayer(TGeometryId i) {
    if (!ND::GeomId::TECal::IsTECal(i)) return -1;
    return ND::GeomId::ECal::GetBarLayer(i);
}

int ND::GeomId::TECal::GetBarClam(TGeometryId i) {
    if (!ND::GeomId::TECal::IsTECal(i)) return -1;
    return ND::GeomId::ECal::GetBarClam(i);
}

int ND::GeomId::TECal::GetBarModule(TGeometryId i) {
    if (!ND::GeomId::TECal::IsTECal(i)) return -1;
    return ND::GeomId::ECal::GetBarModule(i);
}

////////////////////////////////////////////////////////////
// P0D ECal Geometry Identifiers.
////////////////////////////////////////////////////////////
bool ND::GeomId::PECal::IsPECal(TGeometryId i) {
    TSettableGeometryId id(i);
    return (ND::GeomId::Def::kPECal
            == id.GetField(ND::GeomId::Def::kDetectorIdMSB,
                           ND::GeomId::Def::kDetectorIdLSB));
}

ND::TGeometryId ND::GeomId::PECal::Module(int clam, int module) {
    return ND::GeomId::ECal::Module(ND::GeomId::Def::kPECal,
                                    clam,module);
}

ND::TGeometryId ND::GeomId::PECal::Layer(int clam, int module, int layer) {
    return ND::GeomId::ECal::Layer(ND::GeomId::Def::kPECal,
                                   clam,module,layer);
}

ND::TGeometryId ND::GeomId::PECal::Radiator(int clam, int module, int rad) {
    return ND::GeomId::ECal::Radiator(ND::GeomId::Def::kPECal,
                                      clam,module,rad);
}

ND::TGeometryId ND::GeomId::PECal::Bar(int clam, int mod, int layer, int bar) {
    return ND::GeomId::ECal::Bar(ND::GeomId::Def::kPECal,
                                      clam,mod,layer,bar);
}

int ND::GeomId::PECal::GetModuleNumber(TGeometryId i) {
    if (!ND::GeomId::PECal::IsPECal(i)) return -1;
    return ND::GeomId::ECal::GetModuleNumber(i);
}

int ND::GeomId::PECal::GetModuleClam(TGeometryId i) {
    if (!ND::GeomId::PECal::IsPECal(i)) return -1;
    return ND::GeomId::ECal::GetModuleClam(i);
}

int ND::GeomId::PECal::GetLayerNumber(TGeometryId i) {
    if (!ND::GeomId::PECal::IsPECal(i)) return -1;
    return ND::GeomId::ECal::GetLayerNumber(i);
}

int ND::GeomId::PECal::GetLayerClam(TGeometryId i) {
    if (!ND::GeomId::PECal::IsPECal(i)) return -1;
    return ND::GeomId::ECal::GetLayerClam(i);
}

int ND::GeomId::PECal::GetLayerModule(TGeometryId i) {
    if (!ND::GeomId::PECal::IsPECal(i)) return -1;
    return ND::GeomId::ECal::GetLayerModule(i);
}

int ND::GeomId::PECal::GetRadiatorNumber(TGeometryId i) {
    if (!ND::GeomId::PECal::IsPECal(i)) return -1;
    return ND::GeomId::ECal::GetRadiatorNumber(i);
}

int ND::GeomId::PECal::GetRadiatorClam(TGeometryId i) {
    if (!ND::GeomId::PECal::IsPECal(i)) return -1;
    return ND::GeomId::ECal::GetRadiatorClam(i);
}

int ND::GeomId::PECal::GetRadiatorModule(TGeometryId i) {
    if (!ND::GeomId::PECal::IsPECal(i)) return -1;
    return ND::GeomId::ECal::GetRadiatorModule(i);
}

int ND::GeomId::PECal::GetBarNumber(TGeometryId i) {
    if (!ND::GeomId::PECal::IsPECal(i)) return -1;
    return ND::GeomId::ECal::GetBarNumber(i);
}

int ND::GeomId::PECal::GetBarLayer(TGeometryId i) {
    if (!ND::GeomId::PECal::IsPECal(i)) return -1;
    return ND::GeomId::ECal::GetBarLayer(i);
}

int ND::GeomId::PECal::GetBarClam(TGeometryId i) {
    if (!ND::GeomId::PECal::IsPECal(i)) return -1;
    return ND::GeomId::ECal::GetBarClam(i);
}

int ND::GeomId::PECal::GetBarModule(TGeometryId i) {
    if (!ND::GeomId::PECal::IsPECal(i)) return -1;
    return ND::GeomId::ECal::GetBarModule(i);
}

////////////////////////////////////////////////////////////
// SMRD Geometry Identifiers.
////////////////////////////////////////////////////////////
bool ND::GeomId::SMRD::IsSMRD(TGeometryId i) {
    TSettableGeometryId id(i);
    return (ND::GeomId::Def::kSMRD
            == id.GetField(ND::GeomId::Def::kDetectorIdMSB,
                           ND::GeomId::Def::kDetectorIdLSB));
}

ND::TGeometryId ND::GeomId::SMRD::Module(int clam, int yoke,
                                         int layer, int slot) {
    TSettableGeometryId id;
    id.SetField(ND::GeomId::Def::kSMRD,
                ND::GeomId::Def::kDetectorIdMSB,
                ND::GeomId::Def::kDetectorIdLSB);
    id.SetField(ND::GeomId::Def::SMRD::kGlobal,
                ND::GeomId::Def::SMRD::kSeqIdMSB,
                ND::GeomId::Def::SMRD::kSeqIdLSB);
    id.SetField(ND::GeomId::Def::SMRD::Global::kModule,
                ND::GeomId::Def::SMRD::Global::kSeqIdMSB,
                ND::GeomId::Def::SMRD::Global::kSeqIdLSB);
    id.SetField(clam,
                ND::GeomId::Def::SMRD::Global::Module::kClamMSB,
                ND::GeomId::Def::SMRD::Global::Module::kClamLSB);
    id.SetField(yoke,
                ND::GeomId::Def::SMRD::Global::Module::kYokeMSB,
                ND::GeomId::Def::SMRD::Global::Module::kYokeLSB);
    id.SetField(layer,
                ND::GeomId::Def::SMRD::Global::Module::kLayerMSB,
                ND::GeomId::Def::SMRD::Global::Module::kLayerLSB);
    id.SetField(slot,
                ND::GeomId::Def::SMRD::Global::Module::kSlotMSB,
                ND::GeomId::Def::SMRD::Global::Module::kSlotLSB);
    return id;
}

int ND::GeomId::SMRD::GetModuleClam(TGeometryId i) {
    TSettableGeometryId id(i);
    if (ND::GeomId::Def::kSMRD
        != id.GetField(ND::GeomId::Def::kDetectorIdMSB,
                       ND::GeomId::Def::kDetectorIdLSB)) return -1;
    if (ND::GeomId::Def::SMRD::kGlobal
        != id.GetField(ND::GeomId::Def::SMRD::kSeqIdMSB,
                       ND::GeomId::Def::SMRD::kSeqIdLSB)) return -1;
    if (ND::GeomId::Def::SMRD::Global::kModule
        != id.GetField(ND::GeomId::Def::SMRD::Global::kSeqIdMSB,
                       ND::GeomId::Def::SMRD::Global::kSeqIdLSB)) return -1;

    return id.GetField(ND::GeomId::Def::SMRD::Global::Module::kClamMSB,
                       ND::GeomId::Def::SMRD::Global::Module::kClamLSB);
}

int ND::GeomId::SMRD::GetModuleYoke(TGeometryId i) {
    TSettableGeometryId id(i);
    if (ND::GeomId::Def::kSMRD
        != id.GetField(ND::GeomId::Def::kDetectorIdMSB,
                       ND::GeomId::Def::kDetectorIdLSB)) return -1;
    if (ND::GeomId::Def::SMRD::kGlobal
        != id.GetField(ND::GeomId::Def::SMRD::kSeqIdMSB,
                       ND::GeomId::Def::SMRD::kSeqIdLSB)) return -1;
    if (ND::GeomId::Def::SMRD::Global::kModule
        != id.GetField(ND::GeomId::Def::SMRD::Global::kSeqIdMSB,
                       ND::GeomId::Def::SMRD::Global::kSeqIdLSB)) return -1;


    return id.GetField(ND::GeomId::Def::SMRD::Global::Module::kYokeMSB,
                       ND::GeomId::Def::SMRD::Global::Module::kYokeLSB);
}

int ND::GeomId::SMRD::GetModuleLayer(TGeometryId i) {
    TSettableGeometryId id(i);
    if (ND::GeomId::Def::kSMRD
        != id.GetField(ND::GeomId::Def::kDetectorIdMSB,
                       ND::GeomId::Def::kDetectorIdLSB)) return -1;
    if (ND::GeomId::Def::SMRD::kGlobal
        != id.GetField(ND::GeomId::Def::SMRD::kSeqIdMSB,
                       ND::GeomId::Def::SMRD::kSeqIdLSB)) return -1;
    if (ND::GeomId::Def::SMRD::Global::kModule
        != id.GetField(ND::GeomId::Def::SMRD::Global::kSeqIdMSB,
                       ND::GeomId::Def::SMRD::Global::kSeqIdLSB)) return -1;


    return id.GetField(ND::GeomId::Def::SMRD::Global::Module::kLayerMSB,
                       ND::GeomId::Def::SMRD::Global::Module::kLayerLSB);
}

int ND::GeomId::SMRD::GetModuleSlot(TGeometryId i) {
    TSettableGeometryId id(i);
    if (ND::GeomId::Def::kSMRD
        != id.GetField(ND::GeomId::Def::kDetectorIdMSB,
                       ND::GeomId::Def::kDetectorIdLSB)) return -1;
    if (ND::GeomId::Def::SMRD::kGlobal
        != id.GetField(ND::GeomId::Def::SMRD::kSeqIdMSB,
                       ND::GeomId::Def::SMRD::kSeqIdLSB)) return -1;
    if (ND::GeomId::Def::SMRD::Global::kModule
        != id.GetField(ND::GeomId::Def::SMRD::Global::kSeqIdMSB,
                       ND::GeomId::Def::SMRD::Global::kSeqIdLSB)) return -1;


    return id.GetField(ND::GeomId::Def::SMRD::Global::Module::kSlotMSB,
                       ND::GeomId::Def::SMRD::Global::Module::kSlotLSB);
}

ND::TGeometryId ND::GeomId::SMRD::Bar(int clam, int yoke,
                                      int layer, int slot, int bar) {
    TSettableGeometryId id;
    id.SetField(ND::GeomId::Def::kSMRD,
                ND::GeomId::Def::kDetectorIdMSB,
                ND::GeomId::Def::kDetectorIdLSB);
    id.SetField(ND::GeomId::Def::SMRD::kScintillator,
                ND::GeomId::Def::SMRD::kSeqIdMSB,
                ND::GeomId::Def::SMRD::kSeqIdLSB);
    id.SetField(clam,
                ND::GeomId::Def::SMRD::Bar::kClamMSB,
                ND::GeomId::Def::SMRD::Bar::kClamLSB);
    id.SetField(yoke,
                ND::GeomId::Def::SMRD::Bar::kYokeMSB,
                ND::GeomId::Def::SMRD::Bar::kYokeLSB);
    id.SetField(layer,
                ND::GeomId::Def::SMRD::Bar::kLayerMSB,
                ND::GeomId::Def::SMRD::Bar::kLayerLSB);
    id.SetField(slot,
                ND::GeomId::Def::SMRD::Bar::kSlotMSB,
                ND::GeomId::Def::SMRD::Bar::kSlotLSB);
    id.SetField(bar,
                ND::GeomId::Def::SMRD::Bar::kBarMSB,
                ND::GeomId::Def::SMRD::Bar::kBarLSB);
    return id;
}

int ND::GeomId::SMRD::GetBarClam(TGeometryId i) {
    TSettableGeometryId id(i);
    if (ND::GeomId::Def::kSMRD
        != id.GetField(ND::GeomId::Def::kDetectorIdMSB,
                       ND::GeomId::Def::kDetectorIdLSB)) return -1;
    if (ND::GeomId::Def::SMRD::kScintillator
        != id.GetField(ND::GeomId::Def::SMRD::kSeqIdMSB,
                       ND::GeomId::Def::SMRD::kSeqIdLSB)) return -1;

    return id.GetField(ND::GeomId::Def::SMRD::Bar::kClamMSB,
                       ND::GeomId::Def::SMRD::Bar::kClamLSB);
}

int ND::GeomId::SMRD::GetBarYoke(TGeometryId i) {
    TSettableGeometryId id(i);
    if (ND::GeomId::Def::kSMRD
        != id.GetField(ND::GeomId::Def::kDetectorIdMSB,
                       ND::GeomId::Def::kDetectorIdLSB)) return -1;
    if (ND::GeomId::Def::SMRD::kScintillator
        != id.GetField(ND::GeomId::Def::SMRD::kSeqIdMSB,
                       ND::GeomId::Def::SMRD::kSeqIdLSB)) return -1;

    return id.GetField(ND::GeomId::Def::SMRD::Bar::kYokeMSB,
                       ND::GeomId::Def::SMRD::Bar::kYokeLSB);
}

int ND::GeomId::SMRD::GetBarLayer(TGeometryId i) {
    TSettableGeometryId id(i);
    if (ND::GeomId::Def::kSMRD
        != id.GetField(ND::GeomId::Def::kDetectorIdMSB,
                       ND::GeomId::Def::kDetectorIdLSB)) return -1;
    if (ND::GeomId::Def::SMRD::kScintillator
        != id.GetField(ND::GeomId::Def::SMRD::kSeqIdMSB,
                       ND::GeomId::Def::SMRD::kSeqIdLSB)) return -1;

    return id.GetField(ND::GeomId::Def::SMRD::Bar::kLayerMSB,
                       ND::GeomId::Def::SMRD::Bar::kLayerLSB);
}

int ND::GeomId::SMRD::GetBarSlot(TGeometryId i) {
    TSettableGeometryId id(i);
    if (ND::GeomId::Def::kSMRD
        != id.GetField(ND::GeomId::Def::kDetectorIdMSB,
                       ND::GeomId::Def::kDetectorIdLSB)) return -1;
    if (ND::GeomId::Def::SMRD::kScintillator
        != id.GetField(ND::GeomId::Def::SMRD::kSeqIdMSB,
                       ND::GeomId::Def::SMRD::kSeqIdLSB)) return -1;

    return id.GetField(ND::GeomId::Def::SMRD::Bar::kSlotMSB,
                       ND::GeomId::Def::SMRD::Bar::kSlotLSB);
}

int ND::GeomId::SMRD::GetBarNumber(TGeometryId i) {
    TSettableGeometryId id(i);
    if (ND::GeomId::Def::kSMRD
        != id.GetField(ND::GeomId::Def::kDetectorIdMSB,
                       ND::GeomId::Def::kDetectorIdLSB)) return -1;
    if (ND::GeomId::Def::SMRD::kScintillator
        != id.GetField(ND::GeomId::Def::SMRD::kSeqIdMSB,
                       ND::GeomId::Def::SMRD::kSeqIdLSB)) return -1;

    return id.GetField(ND::GeomId::Def::SMRD::Bar::kBarMSB,
                       ND::GeomId::Def::SMRD::Bar::kBarLSB);
}

////////////////////////////////////////////////////////////
// INGRID Geometry Identifiers.
////////////////////////////////////////////////////////////

bool ND::GeomId::INGRID::IsINGRID(TGeometryId i) {
    TSettableGeometryId id(i);
    return (ND::GeomId::Def::kINGRID
            == id.GetField(ND::GeomId::Def::kDetectorIdMSB,
                           ND::GeomId::Def::kDetectorIdLSB));
}

ND::TGeometryId ND::GeomId::INGRID::Scintillator(
    int objType, int obj, int trk, int proj, int scinti){
    TSettableGeometryId id;
    id.SetField(ND::GeomId::Def::kINGRID,
                ND::GeomId::Def::kDetectorIdMSB,
                ND::GeomId::Def::kDetectorIdLSB);
    id.SetField(objType,
                ND::GeomId::Def::INGRID::kIngridObjIDMSB,
                ND::GeomId::Def::INGRID::kIngridObjIDLSB);
    id.SetField(obj,
                ND::GeomId::Def::INGRID::kIngridModNumMSB,
                ND::GeomId::Def::INGRID::kIngridModNumLSB);
    id.SetField(trk,
                ND::GeomId::Def::INGRID::kIngridTrkNumMSB,
                ND::GeomId::Def::INGRID::kIngridTrkNumLSB);
    id.SetField(proj,
                ND::GeomId::Def::INGRID::kIngridProjMSB,
                ND::GeomId::Def::INGRID::kIngridProjLSB);
    id.SetField(scinti,
                ND::GeomId::Def::INGRID::kIngridSciNumMSB,
                ND::GeomId::Def::INGRID::kIngridSciNumLSB);
    return id;
}

ND::TGeometryId ND::GeomId::INGRID::ModScintillator(
    int obj, int trk, int proj, int scinti){
    return ND::GeomId::INGRID::Scintillator(
        ND::GeomId::Def::INGRID::kIngridModule, obj, trk, proj, scinti);
}

ND::TGeometryId ND::GeomId::INGRID::VertVetoScintillator(int obj, int scinti){
    return ND::GeomId::INGRID::Scintillator(
        ND::GeomId::Def::INGRID::kIngridVeto, obj, 0,
        ND::GeomId::Def::INGRID::kVertical, scinti);
}

ND::TGeometryId ND::GeomId::INGRID::HorzVetoScintillator(int obj, int scinti){
    return ND::GeomId::INGRID::Scintillator(
        ND::GeomId::Def::INGRID::kIngridVeto, obj, 0,
        ND::GeomId::Def::INGRID::kHorizontal, scinti);
}

int ND::GeomId::INGRID::GetObjectType(TGeometryId i) {
    if (!IsINGRID(i)) return -1;
    TSettableGeometryId id(i);
    return id.GetField(ND::GeomId::Def::INGRID::kIngridObjIDMSB,
                       ND::GeomId::Def::INGRID::kIngridObjIDLSB);
}

bool ND::GeomId::INGRID::IsModule(TGeometryId i) {
    if (!IsINGRID(i)) return false;
    return ND::GeomId::INGRID::GetObjectType(i)
        == ND::GeomId::Def::INGRID::kIngridModule;
}


bool ND::GeomId::INGRID::IsVeto(TGeometryId i) {
    if (!IsINGRID(i)) return false;
    return ND::GeomId::INGRID::GetObjectType(i)
        == ND::GeomId::Def::INGRID::kIngridVeto;
}

int ND::GeomId::INGRID::GetProjection(TGeometryId i) {
    if (!IsINGRID(i)) return -1;
    TSettableGeometryId id(i);
    return id.GetField(ND::GeomId::Def::INGRID::kIngridProjMSB,
                       ND::GeomId::Def::INGRID::kIngridProjLSB);
}

bool ND::GeomId::INGRID::IsVertical(TGeometryId i) {
    if (!IsINGRID(i)) return false;
    return GetProjection(i) == ND::GeomId::Def::INGRID::kVertical;
}

bool ND::GeomId::INGRID::IsHorizontal(TGeometryId i) {
    if (!IsINGRID(i)) return false;
    return GetProjection(i) == ND::GeomId::Def::INGRID::kHorizontal;
}

int ND::GeomId::INGRID::GetObjectNumber(TGeometryId i) {
    if (!IsINGRID(i)) return -1;
    TSettableGeometryId id(i);
    return id.GetField(ND::GeomId::Def::INGRID::kIngridModNumMSB,
                       ND::GeomId::Def::INGRID::kIngridModNumLSB);
}

int ND::GeomId::INGRID::GetTracker(TGeometryId i) {
    if (!IsINGRID(i)) return -1;
    TSettableGeometryId id(i);
    return id.GetField(ND::GeomId::Def::INGRID::kIngridTrkNumMSB,
                       ND::GeomId::Def::INGRID::kIngridTrkNumLSB);
}


int ND::GeomId::INGRID::GetScintillator(TGeometryId i) {
    if (!IsINGRID(i)) return -1;
    TSettableGeometryId id(i);
    return id.GetField(ND::GeomId::Def::INGRID::kIngridSciNumMSB,
                       ND::GeomId::Def::INGRID::kIngridSciNumLSB);
}

////////////////////////////////////////////////////////////
// SuperFGD Geometry Identifiers.
////////////////////////////////////////////////////////////

ND::TGeometryId ND::GeomId::SFG::Detector() {
    TSettableGeometryId id;
    id.SetField(ND::GeomId::Def::kSFG,
                ND::GeomId::Def::kDetectorIdMSB,
                ND::GeomId::Def::kDetectorIdLSB);
    id.SetField(ND::GeomId::Def::SFG::kGlobal,
                ND::GeomId::Def::SFG::kSeqIdMSB,
                ND::GeomId::Def::SFG::kSeqIdLSB);
    id.SetField(0, ND::GeomId::Def::SFG::kSeqIdLSB-1, 0);
    return id;
}

bool ND::GeomId::SFG::IsSFG(ND::TGeometryId i) {
    TSettableGeometryId id(i);
    return (ND::GeomId::Def::kSFG
            == id.GetField(ND::GeomId::Def::kDetectorIdMSB,
                           ND::GeomId::Def::kDetectorIdLSB));
}

bool ND::GeomId::SFG::IsCube(ND::TGeometryId i) {
    if (!ND::GeomId::SFG::IsSFG(i)) return false;
    TSettableGeometryId id(i);
    if (ND::GeomId::Def::SFG::kCube
        != id.GetField(ND::GeomId::Def::SFG::kSeqIdMSB,
                       ND::GeomId::Def::SFG::kSeqIdLSB)) return false;
    unsigned int x = id.GetField(ND::GeomId::Def::SFG::Cube::kCubeXMSB,
                        ND::GeomId::Def::SFG::Cube::kCubeXLSB);
    unsigned int y = id.GetField(ND::GeomId::Def::SFG::Cube::kCubeYMSB,
                        ND::GeomId::Def::SFG::Cube::kCubeYLSB);
    unsigned int z = id.GetField(ND::GeomId::Def::SFG::Cube::kCubeZMSB,
                        ND::GeomId::Def::SFG::Cube::kCubeZLSB);
    if (x == ND::GeomId::Def::SFG::Cube::kYZFiberFlag) return false;
    if (y == ND::GeomId::Def::SFG::Cube::kXZFiberFlag) return false;
    if (z == ND::GeomId::Def::SFG::Cube::kXYFiberFlag) return false;
    return true;
}

bool ND::GeomId::SFG::IsFiber(ND::TGeometryId id) {
    if (ND::GeomId::SFG::IsXZFiber(id)) return true;
    if (ND::GeomId::SFG::IsYZFiber(id)) return true;
    if (ND::GeomId::SFG::IsXYFiber(id)) return true;
    return false;
}

bool ND::GeomId::SFG::IsXZFiber(ND::TGeometryId i) {
    if (!ND::GeomId::SFG::IsSFG(i)) return false;
    TSettableGeometryId id(i);
    if (ND::GeomId::Def::SFG::kCube
        != id.GetField(ND::GeomId::Def::SFG::kSeqIdMSB,
                       ND::GeomId::Def::SFG::kSeqIdLSB)) return false;
    unsigned int x = id.GetField(ND::GeomId::Def::SFG::Cube::kCubeXMSB,
                        ND::GeomId::Def::SFG::Cube::kCubeXLSB);
    unsigned int y = id.GetField(ND::GeomId::Def::SFG::Cube::kCubeYMSB,
                        ND::GeomId::Def::SFG::Cube::kCubeYLSB);
    unsigned int z = id.GetField(ND::GeomId::Def::SFG::Cube::kCubeZMSB,
                        ND::GeomId::Def::SFG::Cube::kCubeZLSB);
    if (x < ND::GeomId::Def::SFG::Cube::kYZFiberFlag
        && y == ND::GeomId::Def::SFG::Cube::kXZFiberFlag
        && z < ND::GeomId::Def::SFG::Cube::kXYFiberFlag) return true;
    return false;
}

bool ND::GeomId::SFG::IsYZFiber(ND::TGeometryId i) {
    if (!ND::GeomId::SFG::IsSFG(i)) return false;
    TSettableGeometryId id(i);
    if (ND::GeomId::Def::SFG::kCube
        != id.GetField(ND::GeomId::Def::SFG::kSeqIdMSB,
                       ND::GeomId::Def::SFG::kSeqIdLSB)) return false;
    unsigned int x = id.GetField(ND::GeomId::Def::SFG::Cube::kCubeXMSB,
                        ND::GeomId::Def::SFG::Cube::kCubeXLSB);
    unsigned int y = id.GetField(ND::GeomId::Def::SFG::Cube::kCubeYMSB,
                        ND::GeomId::Def::SFG::Cube::kCubeYLSB);
    unsigned int z = id.GetField(ND::GeomId::Def::SFG::Cube::kCubeZMSB,
                        ND::GeomId::Def::SFG::Cube::kCubeZLSB);
    if (x == ND::GeomId::Def::SFG::Cube::kYZFiberFlag
        && y < ND::GeomId::Def::SFG::Cube::kXZFiberFlag
        && z < ND::GeomId::Def::SFG::Cube::kXYFiberFlag) return true;
    return false;
}

bool ND::GeomId::SFG::IsXYFiber(ND::TGeometryId i) {
    if (!ND::GeomId::SFG::IsSFG(i)) return false;
    TSettableGeometryId id(i);
    if (ND::GeomId::Def::SFG::kCube
        != id.GetField(ND::GeomId::Def::SFG::kSeqIdMSB,
                       ND::GeomId::Def::SFG::kSeqIdLSB)) return false;
    unsigned int x = id.GetField(ND::GeomId::Def::SFG::Cube::kCubeXMSB,
                        ND::GeomId::Def::SFG::Cube::kCubeXLSB);
    unsigned int y = id.GetField(ND::GeomId::Def::SFG::Cube::kCubeYMSB,
                        ND::GeomId::Def::SFG::Cube::kCubeYLSB);
    unsigned int z = id.GetField(ND::GeomId::Def::SFG::Cube::kCubeZMSB,
                        ND::GeomId::Def::SFG::Cube::kCubeZLSB);
    if (x < ND::GeomId::Def::SFG::Cube::kYZFiberFlag
        && y < ND::GeomId::Def::SFG::Cube::kXZFiberFlag
        && z == ND::GeomId::Def::SFG::Cube::kXYFiberFlag) return true;
    return false;
}

ND::TGeometryId ND::GeomId::SFG::Cube(int x, int y, int z) {
    TSettableGeometryId id;

    if (x<0 || ND::GeomId::Def::SFG::Cube::kYZFiberFlag <= x) {
        ND280Error("X cube index is out of range: " << x);
        return id;
    }

    if (y<0 || ND::GeomId::Def::SFG::Cube::kXZFiberFlag <= y) {
        ND280Error("Y cube index is out of range: " << y);
        return id;
    }

    if (z<0 || ND::GeomId::Def::SFG::Cube::kXYFiberFlag <= z) {
        ND280Error("Z cube index is out of range: " << z);
        return id;
    }

    id.SetField(ND::GeomId::Def::kSFG,
                ND::GeomId::Def::kDetectorIdMSB,
                ND::GeomId::Def::kDetectorIdLSB);
    id.SetField(ND::GeomId::Def::SFG::kCube,
                ND::GeomId::Def::SFG::kSeqIdMSB,
                ND::GeomId::Def::SFG::kSeqIdLSB);
    id.SetField(x,
                ND::GeomId::Def::SFG::Cube::kCubeXMSB,
                ND::GeomId::Def::SFG::Cube::kCubeXLSB);
    id.SetField(y,
                ND::GeomId::Def::SFG::Cube::kCubeYMSB,
                ND::GeomId::Def::SFG::Cube::kCubeYLSB);
    id.SetField(z,
                ND::GeomId::Def::SFG::Cube::kCubeZMSB,
                ND::GeomId::Def::SFG::Cube::kCubeZLSB);

    return id;
}

ND::TGeometryId ND::GeomId::SFG::Fiber(int proj, int u, int v) {
    switch (proj) {
    case ND::GeomId::Def::SFG::Cube::kXZ:
        return ND::GeomId::SFG::XZFiber(u,v);
    case ND::GeomId::Def::SFG::Cube::kYZ:
        return ND::GeomId::SFG::YZFiber(u,v);
    case ND::GeomId::Def::SFG::Cube::kXY:
        return ND::GeomId::SFG::XYFiber(u,v);
    default:
        ND280Error("Illegal projection: " << proj);
    }
    return TGeometryId();
}

ND::TGeometryId ND::GeomId::SFG::XZFiber(int x, int z) {
    TSettableGeometryId id;
    if (x < 0 || ND::GeomId::Def::SFG::Cube::kYZFiberFlag <= x) {
        ND280Error("X fiber index is out of range: " << x);
        return id;
    }

    if (z < 0 || ND::GeomId::Def::SFG::Cube::kXYFiberFlag <= z) {
        ND280Error("Z fiber index is out of range: " << z);
        return id;
    }

    id.SetField(ND::GeomId::Def::kSFG,
                ND::GeomId::Def::kDetectorIdMSB,
                ND::GeomId::Def::kDetectorIdLSB);
    id.SetField(ND::GeomId::Def::SFG::kCube,
                ND::GeomId::Def::SFG::kSeqIdMSB,
                ND::GeomId::Def::SFG::kSeqIdLSB);
    id.SetField(x,
                ND::GeomId::Def::SFG::Cube::kCubeXMSB,
                ND::GeomId::Def::SFG::Cube::kCubeXLSB);
    id.SetField(ND::GeomId::Def::SFG::Cube::kXZFiberFlag,
                ND::GeomId::Def::SFG::Cube::kCubeYMSB,
                ND::GeomId::Def::SFG::Cube::kCubeYLSB);
    id.SetField(z,
                ND::GeomId::Def::SFG::Cube::kCubeZMSB,
                ND::GeomId::Def::SFG::Cube::kCubeZLSB);

    return id;
}

ND::TGeometryId ND::GeomId::SFG::YZFiber(int y, int z) {
    TSettableGeometryId id;
    if (y < 0 || ND::GeomId::Def::SFG::Cube::kXZFiberFlag <= y) {
        ND280Error("Y fiber index is out of range: " << y);
        return id;
    }

    if (z < 0 || ND::GeomId::Def::SFG::Cube::kXYFiberFlag <= z) {
        ND280Error("Z fiber index is out of range: " << z);
        return id;
    }

    id.SetField(ND::GeomId::Def::kSFG,
                ND::GeomId::Def::kDetectorIdMSB,
                ND::GeomId::Def::kDetectorIdLSB);
    id.SetField(ND::GeomId::Def::SFG::kCube,
                ND::GeomId::Def::SFG::kSeqIdMSB,
                ND::GeomId::Def::SFG::kSeqIdLSB);
    id.SetField(ND::GeomId::Def::SFG::Cube::kYZFiberFlag,
                ND::GeomId::Def::SFG::Cube::kCubeXMSB,
                ND::GeomId::Def::SFG::Cube::kCubeXLSB);
    id.SetField(y,
                ND::GeomId::Def::SFG::Cube::kCubeYMSB,
                ND::GeomId::Def::SFG::Cube::kCubeYLSB);
    id.SetField(z,
                ND::GeomId::Def::SFG::Cube::kCubeZMSB,
                ND::GeomId::Def::SFG::Cube::kCubeZLSB);

    return id;
}

ND::TGeometryId ND::GeomId::SFG::XYFiber(int x, int y) {
    TSettableGeometryId id;
    if (x < 0 || ND::GeomId::Def::SFG::Cube::kYZFiberFlag <= x) {
        ND280Error("X fiber index is out of range: " << x);
        return id;
    }

    if (y < 0 || ND::GeomId::Def::SFG::Cube::kXZFiberFlag <= y) {
        ND280Error("Y fiber index is out of range: " << y);
        return id;
    }

    id.SetField(ND::GeomId::Def::kSFG,
                ND::GeomId::Def::kDetectorIdMSB,
                ND::GeomId::Def::kDetectorIdLSB);
    id.SetField(ND::GeomId::Def::SFG::kCube,
                ND::GeomId::Def::SFG::kSeqIdMSB,
                ND::GeomId::Def::SFG::kSeqIdLSB);
    id.SetField(x,
                ND::GeomId::Def::SFG::Cube::kCubeXMSB,
                ND::GeomId::Def::SFG::Cube::kCubeXLSB);
    id.SetField(y,
                ND::GeomId::Def::SFG::Cube::kCubeYMSB,
                ND::GeomId::Def::SFG::Cube::kCubeYLSB);
    id.SetField(ND::GeomId::Def::SFG::Cube::kXYFiberFlag,
                ND::GeomId::Def::SFG::Cube::kCubeZMSB,
                ND::GeomId::Def::SFG::Cube::kCubeZLSB);

    return id;
}

int ND::GeomId::SFG::GetCubeX(ND::TGeometryId i) {
    if (!ND::GeomId::SFG::IsCube(i)
        && !ND::GeomId::SFG::IsFiber(i)) return -1;
    TSettableGeometryId id(i);
    return id.GetField(ND::GeomId::Def::SFG::Cube::kCubeXMSB,
                       ND::GeomId::Def::SFG::Cube::kCubeXLSB);
}

int ND::GeomId::SFG::GetCubeY(ND::TGeometryId i) {
    if (!ND::GeomId::SFG::IsCube(i)
        && !ND::GeomId::SFG::IsFiber(i)) return -1;
    TSettableGeometryId id(i);
    return id.GetField(ND::GeomId::Def::SFG::Cube::kCubeYMSB,
                       ND::GeomId::Def::SFG::Cube::kCubeYLSB);
}

int ND::GeomId::SFG::GetCubeZ(ND::TGeometryId i) {
    if (!ND::GeomId::SFG::IsCube(i)
        && !ND::GeomId::SFG::IsFiber(i)) return -1;
    TSettableGeometryId id(i);
    return id.GetField(ND::GeomId::Def::SFG::Cube::kCubeZMSB,
                       ND::GeomId::Def::SFG::Cube::kCubeZLSB);
}

int ND::GeomId::SFG::GetFiberProjection(ND::TGeometryId i) {
    if (ND::GeomId::SFG::IsXZFiber(i)) return ND::GeomId::Def::SFG::Cube::kXZ;
    if (ND::GeomId::SFG::IsYZFiber(i)) return ND::GeomId::Def::SFG::Cube::kYZ;
    if (ND::GeomId::SFG::IsXYFiber(i)) return ND::GeomId::Def::SFG::Cube::kXY;
    return -1;
}

int ND::GeomId::SFG::GetFiberU(ND::TGeometryId i) {
    if (!ND::GeomId::SFG::IsFiber(i)) return -1;
    TSettableGeometryId id(i);
    if (ND::GeomId::SFG::IsXZFiber(i) || ND::GeomId::SFG::IsXYFiber(i)) {
        return id.GetField(ND::GeomId::Def::SFG::Cube::kCubeXMSB,
                           ND::GeomId::Def::SFG::Cube::kCubeXLSB);
    }
    if (ND::GeomId::SFG::IsYZFiber(i)) {
        return id.GetField(ND::GeomId::Def::SFG::Cube::kCubeYMSB,
                           ND::GeomId::Def::SFG::Cube::kCubeYLSB);
    }
    return -1;
}

int ND::GeomId::SFG::GetFiberV(ND::TGeometryId i) {
    if (!ND::GeomId::SFG::IsFiber(i)) return -1;
    TSettableGeometryId id(i);
    if (ND::GeomId::SFG::IsXZFiber(i) || ND::GeomId::SFG::IsYZFiber(i)) {
        return id.GetField(ND::GeomId::Def::SFG::Cube::kCubeZMSB,
                           ND::GeomId::Def::SFG::Cube::kCubeZLSB);
    }
    if (ND::GeomId::SFG::IsXYFiber(i)) {
        return id.GetField(ND::GeomId::Def::SFG::Cube::kCubeYMSB,
                           ND::GeomId::Def::SFG::Cube::kCubeYLSB);
    }
    return -1;
}

////////////////////////////////////////////////////////////
// HAT Geometry Identifiers
////////////////////////////////////////////////////////////

bool ND::GeomId::HAT::IsHAT(TGeometryId i) {
    TSettableGeometryId id(i);
    return (ND::GeomId::Def::kHAT
            == id.GetField(ND::GeomId::Def::kDetectorIdMSB,
                           ND::GeomId::Def::kDetectorIdLSB));
}

ND::TGeometryId ND::GeomId::HAT::Module(int hat) {
    TSettableGeometryId id;
    if (hat<0 || hat>1) {
        ND280Warn("HAT module out of range [0,1]: " << hat);
        return id;
    }
    id.SetField(ND::GeomId::Def::kHAT,
                ND::GeomId::Def::kDetectorIdMSB,
                ND::GeomId::Def::kDetectorIdLSB);
    id.SetField(ND::GeomId::Def::HAT::kGlobal,
                ND::GeomId::Def::HAT::kSeqIdMSB,
                ND::GeomId::Def::HAT::kSeqIdLSB);
    id.SetField(ND::GeomId::Def::HAT::Global::kHAT,
                ND::GeomId::Def::HAT::Global::kSeqIdMSB,
                ND::GeomId::Def::HAT::Global::kSeqIdLSB);
    id.SetField(hat,
                ND::GeomId::Def::HAT::Global::kFieldMSB,
                ND::GeomId::Def::HAT::Global::kFieldLSB);
    return id;
}

int ND::GeomId::HAT::GetModule(TGeometryId i) {
    TSettableGeometryId id(i);
    if (ND::GeomId::Def::kHAT
        != id.GetField(ND::GeomId::Def::kDetectorIdMSB,
                       ND::GeomId::Def::kDetectorIdLSB)) return -1;
    if (ND::GeomId::Def::HAT::kGlobal
        != id.GetField(ND::GeomId::Def::HAT::kSeqIdMSB,
                       ND::GeomId::Def::HAT::kSeqIdLSB)) return -1;
    if (ND::GeomId::Def::HAT::Global::kHAT
        != id.GetField(ND::GeomId::Def::HAT::Global::kSeqIdMSB,
                       ND::GeomId::Def::HAT::Global::kSeqIdLSB)) return -1;
    return id.GetField(ND::GeomId::Def::HAT::Global::kFieldMSB,
                       ND::GeomId::Def::HAT::Global::kFieldLSB);
}

ND::TGeometryId ND::GeomId::HAT::Bottom() {return Module(0);}
ND::TGeometryId ND::GeomId::HAT::Top() {return Module(1);}

bool ND::GeomId::HAT::IsBottom(TGeometryId i) {
    return (ND::GeomId::HAT::GetModule(i) == 0);
}

bool ND::GeomId::HAT::IsTop(TGeometryId i) {
    return (ND::GeomId::HAT::GetModule(i) == 1);
}

ND::TGeometryId ND::GeomId::HAT::MicroMega(int module, int half, int mm) {
    TSettableGeometryId id;
    if (module<0 || module>1) {
        ND280Warn("HAT module out of range [0,1]: " << module);
        return id;
    }
    if (half<0 || half>1) {
        ND280Warn("HAT half out of range [0,1]: " << half);
        return id;
    }
    if (mm<0 || mm>7) {
        ND280Warn("HAT micromega out of range [0,7]: " << mm);
        return id;
    }
    id.SetField(ND::GeomId::Def::kHAT,
                ND::GeomId::Def::kDetectorIdMSB,
                ND::GeomId::Def::kDetectorIdLSB);
    id.SetField(ND::GeomId::Def::HAT::kPad,
                ND::GeomId::Def::HAT::kSeqIdMSB,
                ND::GeomId::Def::HAT::kSeqIdLSB);
    id.SetField(module,
                ND::GeomId::Def::HAT::Pad::kModuleMSB,
                ND::GeomId::Def::HAT::Pad::kModuleLSB);
    id.SetField(half,
                ND::GeomId::Def::HAT::Pad::kHalfMSB,
                ND::GeomId::Def::HAT::Pad::kHalfLSB);
    id.SetField(mm,
                ND::GeomId::Def::HAT::Pad::kMMegaMSB,
                ND::GeomId::Def::HAT::Pad::kMMegaLSB);
    id.SetField(0,
                ND::GeomId::Def::HAT::Pad::kPadFlagMSB,
                ND::GeomId::Def::HAT::Pad::kPadFlagLSB);
    id.SetField(0,
                ND::GeomId::Def::HAT::Pad::kPadMSB,
                ND::GeomId::Def::HAT::Pad::kPadLSB);
    return id;
}

bool ND::GeomId::HAT::IsMicroMega(ND::TGeometryId i) {
    TSettableGeometryId id(i);
    int detector = id.GetField(ND::GeomId::Def::kDetectorIdMSB,
                               ND::GeomId::Def::kDetectorIdLSB);
    if (detector != ND::GeomId::Def::kHAT) return false;
    /// This may be a pad.
    int seqId = id.GetField(ND::GeomId::Def::HAT::kSeqIdMSB,
                            ND::GeomId::Def::HAT::kSeqIdLSB);
    if (seqId != ND::GeomId::Def::HAT::kPad) return false;
    int padFlag = id.GetField(ND::GeomId::Def::HAT::Pad::kPadFlagMSB,
                              ND::GeomId::Def::HAT::Pad::kPadFlagLSB);
    if (padFlag) return false;
    return true;
}

ND::TGeometryId ND::GeomId::HAT::Pad(int hat, int half, int mm, int pad) {
    TSettableGeometryId id;
    if (hat<0 || hat>1) {
        ND280Warn("HAT module out of range [0,1]: " << hat);
        return id;
    }
    if (half<0 || half>1) {
        ND280Warn("HAT half out of range [0,1]: " << half);
        return id;
    }
    if (mm<0 || mm>7) {
        ND280Warn("HAT micromega out of range [0,7]: " << mm);
        return id;
    }
    if (pad<0 || pad>1151) {
        ND280Warn("HAT pad out of range [0,1151]: " << pad);
        return id;
    }
    id.SetField(ND::GeomId::Def::kHAT,
                ND::GeomId::Def::kDetectorIdMSB,
                ND::GeomId::Def::kDetectorIdLSB);
    id.SetField(ND::GeomId::Def::HAT::kPad,
                ND::GeomId::Def::HAT::kSeqIdMSB,
                ND::GeomId::Def::HAT::kSeqIdLSB);
    id.SetField(hat,
                ND::GeomId::Def::HAT::Pad::kModuleMSB,
                ND::GeomId::Def::HAT::Pad::kModuleLSB);
    id.SetField(half,
                ND::GeomId::Def::HAT::Pad::kHalfMSB,
                ND::GeomId::Def::HAT::Pad::kHalfLSB);
    id.SetField(mm,
                ND::GeomId::Def::HAT::Pad::kMMegaMSB,
                ND::GeomId::Def::HAT::Pad::kMMegaLSB);
    id.SetField(1,
                ND::GeomId::Def::HAT::Pad::kPadFlagMSB,
                ND::GeomId::Def::HAT::Pad::kPadFlagLSB);
    id.SetField(pad,
                ND::GeomId::Def::HAT::Pad::kPadMSB,
                ND::GeomId::Def::HAT::Pad::kPadLSB);
    return id;
}

int ND::GeomId::HAT::GetMicroMegaModule(TGeometryId i) {
    return ND::GeomId::HAT::GetPadModule(i);
}

int ND::GeomId::HAT::GetPadModule(TGeometryId i) {
    TSettableGeometryId id(i);
    if (ND::GeomId::Def::kHAT
        != id.GetField(ND::GeomId::Def::kDetectorIdMSB,
                       ND::GeomId::Def::kDetectorIdLSB)) return -1;
    if (ND::GeomId::Def::HAT::kPad
        != id.GetField(ND::GeomId::Def::HAT::kSeqIdMSB,
                       ND::GeomId::Def::HAT::kSeqIdLSB)) return -1;
    return id.GetField(ND::GeomId::Def::HAT::Pad::kModuleMSB,
                       ND::GeomId::Def::HAT::Pad::kModuleLSB);
}

int ND::GeomId::HAT::GetMicroMegaHalf(TGeometryId i) {
    return ND::GeomId::HAT::GetPadHalf(i);
}

int ND::GeomId::HAT::GetPadHalf(TGeometryId i) {
    TSettableGeometryId id(i);
    if (ND::GeomId::Def::kHAT
        != id.GetField(ND::GeomId::Def::kDetectorIdMSB,
                       ND::GeomId::Def::kDetectorIdLSB)) return -1;
    if (ND::GeomId::Def::HAT::kPad
        != id.GetField(ND::GeomId::Def::HAT::kSeqIdMSB,
                       ND::GeomId::Def::HAT::kSeqIdLSB)) return -1;
    return id.GetField(ND::GeomId::Def::HAT::Pad::kHalfMSB,
                       ND::GeomId::Def::HAT::Pad::kHalfLSB);
}

int ND::GeomId::HAT::GetMicroMegaNumber(TGeometryId i) {
    return ND::GeomId::HAT::GetPadMicroMega(i);
}

int ND::GeomId::HAT::GetPadMicroMega(TGeometryId i) {
    TSettableGeometryId id(i);
    if (ND::GeomId::Def::kHAT
        != id.GetField(ND::GeomId::Def::kDetectorIdMSB,
                       ND::GeomId::Def::kDetectorIdLSB)) return -1;
    if (ND::GeomId::Def::HAT::kPad
        != id.GetField(ND::GeomId::Def::HAT::kSeqIdMSB,
                       ND::GeomId::Def::HAT::kSeqIdLSB)) return -1;
    return id.GetField(ND::GeomId::Def::HAT::Pad::kMMegaMSB,
                       ND::GeomId::Def::HAT::Pad::kMMegaLSB);
}

int ND::GeomId::HAT::GetPadNumber(TGeometryId i) {
    TSettableGeometryId id(i);
    if (ND::GeomId::Def::kHAT
        != id.GetField(ND::GeomId::Def::kDetectorIdMSB,
                       ND::GeomId::Def::kDetectorIdLSB)) return -1;
    if (ND::GeomId::Def::HAT::kPad
        != id.GetField(ND::GeomId::Def::HAT::kSeqIdMSB,
                       ND::GeomId::Def::HAT::kSeqIdLSB)) return -1;
    if (1 != id.GetField(ND::GeomId::Def::HAT::Pad::kPadFlagMSB,
                         ND::GeomId::Def::HAT::Pad::kPadFlagLSB)) return -1;
    return id.GetField(ND::GeomId::Def::HAT::Pad::kPadMSB,
                       ND::GeomId::Def::HAT::Pad::kPadLSB);
}

bool ND::GeomId::HAT::IsPad(ND::TGeometryId i) {
    TSettableGeometryId id(i);
    int detector = id.GetField(ND::GeomId::Def::kDetectorIdMSB,
                               ND::GeomId::Def::kDetectorIdLSB);
    if (detector != ND::GeomId::Def::kHAT) return false;
    /// This may be a pad.
    int seqId = id.GetField(ND::GeomId::Def::HAT::kSeqIdMSB,
                            ND::GeomId::Def::HAT::kSeqIdLSB);
    if (seqId != ND::GeomId::Def::HAT::kPad) return false;
    int padFlag = id.GetField(ND::GeomId::Def::HAT::Pad::kPadFlagMSB,
                              ND::GeomId::Def::HAT::Pad::kPadFlagLSB);
    if (!padFlag) return false;
    return true;
}

////////////////////////////////////////////////////////////
// TOF Geometry Identifiers
////////////////////////////////////////////////////////////

bool ND::GeomId::TOF::IsTOF(ND::TGeometryId i) {
    TSettableGeometryId id(i);
    return (ND::GeomId::Def::kTOF
            == id.GetField(ND::GeomId::Def::kDetectorIdMSB,
                           ND::GeomId::Def::kDetectorIdLSB));
}

ND::TGeometryId ND::GeomId::TOF::Module(int module) {
    TSettableGeometryId id;
    id.SetField(ND::GeomId::Def::kTOF,
                ND::GeomId::Def::kDetectorIdMSB,
                ND::GeomId::Def::kDetectorIdLSB);
    id.SetField(ND::GeomId::Def::TOF::kGlobal,
                ND::GeomId::Def::TOF::kSeqIdMSB,
                ND::GeomId::Def::TOF::kSeqIdLSB);
    id.SetField(module,
                ND::GeomId::Def::TOF::kModuleMSB,
                ND::GeomId::Def::TOF::kModuleLSB);
    id.SetField(ND::GeomId::Def::TOF::Global::kModule,
                ND::GeomId::Def::TOF::Global::kSeqIdMSB,
                ND::GeomId::Def::TOF::Global::kSeqIdLSB);
    id.SetField(module,
                ND::GeomId::Def::TOF::Global::kValueMSB,
                ND::GeomId::Def::TOF::Global::kValueLSB);
    return id;
}

bool ND::GeomId::TOF::IsModule(ND::TGeometryId i) {
    TSettableGeometryId id(i);
    if (!IsTOF(id)) return false;
    if (ND::GeomId::Def::TOF::kGlobal
        != id.GetField(ND::GeomId::Def::TOF::kSeqIdMSB,
                       ND::GeomId::Def::TOF::kSeqIdLSB)) return false;
    return (ND::GeomId::Def::TOF::Global::kModule
            == id.GetField(ND::GeomId::Def::TOF::Global::kSeqIdMSB,
                           ND::GeomId::Def::TOF::Global::kSeqIdLSB));
}

int ND::GeomId::TOF::GetModule(ND::TGeometryId i) {
    TSettableGeometryId id(i);
    if (!ND::GeomId::TOF::IsTOF(id)) return -1;
    return id.GetField(ND::GeomId::Def::TOF::kModuleMSB,
                       ND::GeomId::Def::TOF::kModuleLSB);
}

ND::TGeometryId ND::GeomId::TOF::North() {
    return ND::GeomId::TOF::Module(ND::GeomId::Def::TOF::kNorthModule);
}

ND::TGeometryId ND::GeomId::TOF::South() {
    return ND::GeomId::TOF::Module(ND::GeomId::Def::TOF::kSouthModule);
}

ND::TGeometryId ND::GeomId::TOF::Bottom() {
    return ND::GeomId::TOF::Module(ND::GeomId::Def::TOF::kBottomModule);
}

ND::TGeometryId ND::GeomId::TOF::Top() {
    return ND::GeomId::TOF::Module(ND::GeomId::Def::TOF::kTopModule);
}

ND::TGeometryId ND::GeomId::TOF::Upstream() {
    return ND::GeomId::TOF::Module(ND::GeomId::Def::TOF::kUpstreamModule);
}

ND::TGeometryId ND::GeomId::TOF::Downstream() {
    return ND::GeomId::TOF::Module(ND::GeomId::Def::TOF::kDownstreamModule);
}

bool ND::GeomId::TOF::IsNorth(ND::TGeometryId id) {
    return (ND::GeomId::TOF::GetModule(id)
            == ND::GeomId::Def::TOF::kNorthModule);
}

bool ND::GeomId::TOF::IsSouth(ND::TGeometryId id) {
    return (ND::GeomId::TOF::GetModule(id)
            == ND::GeomId::Def::TOF::kSouthModule);
}

bool ND::GeomId::TOF::IsBottom(ND::TGeometryId id) {
    return (ND::GeomId::TOF::GetModule(id)
            == ND::GeomId::Def::TOF::kBottomModule);
}

bool ND::GeomId::TOF::IsTop(ND::TGeometryId id) {
    return (ND::GeomId::TOF::GetModule(id)
            == ND::GeomId::Def::TOF::kTopModule);
}

bool ND::GeomId::TOF::IsUpstream(ND::TGeometryId id) {
    return (ND::GeomId::TOF::GetModule(id)
            == ND::GeomId::Def::TOF::kUpstreamModule);
}

bool ND::GeomId::TOF::IsDownstream(ND::TGeometryId id) {
    return (ND::GeomId::TOF::GetModule(id)
            == ND::GeomId::Def::TOF::kDownstreamModule);
}

ND::TGeometryId ND::GeomId::TOF::Bar(int module, int bar) {
    TSettableGeometryId id;
    id.SetField(ND::GeomId::Def::kTOF,
                ND::GeomId::Def::kDetectorIdMSB,
                ND::GeomId::Def::kDetectorIdLSB);
    id.SetField(ND::GeomId::Def::TOF::kScintillator,
                ND::GeomId::Def::TOF::kSeqIdMSB,
                ND::GeomId::Def::TOF::kSeqIdLSB);
    id.SetField(module,
                ND::GeomId::Def::TOF::kModuleMSB,
                ND::GeomId::Def::TOF::kModuleLSB);
    id.SetField(0,
                ND::GeomId::Def::TOF::Bar::kLayerMSB,
                ND::GeomId::Def::TOF::Bar::kLayerLSB);
    id.SetField(bar,
                ND::GeomId::Def::TOF::Bar::kBarMSB,
                ND::GeomId::Def::TOF::Bar::kBarLSB);
    return id;
}

ND::TGeometryId ND::GeomId::TOF::Bar(TGeometryId id) {
    if (!ND::GeomId::TOF::IsBar(id)) return TSettableGeometryId();
    return ND::GeomId::TOF::Bar(ND::GeomId::TOF::GetModule(id),
                                ND::GeomId::TOF::GetBar(id));
}

ND::TGeometryId ND::GeomId::TOF::Sensor(int module, int bar, int end) {
    TSettableGeometryId id;
    id.SetField(ND::GeomId::Def::kTOF,
                ND::GeomId::Def::kDetectorIdMSB,
                ND::GeomId::Def::kDetectorIdLSB);
    if (end == -1) {
        id.SetField(ND::GeomId::Def::TOF::kSensorMinus,
                    ND::GeomId::Def::TOF::kSeqIdMSB,
                    ND::GeomId::Def::TOF::kSeqIdLSB);
    }
    else if (end == 1) {
        id.SetField(ND::GeomId::Def::TOF::kSensorPlus,
                    ND::GeomId::Def::TOF::kSeqIdMSB,
                    ND::GeomId::Def::TOF::kSeqIdLSB);
    }
    else {
        ND280Error("Invalid TOF sensor: "
                   << " module: " << module
                   << " bar: " << bar
                   << " end: " << end);
        id.SetField(ND::GeomId::Def::TOF::kScintillator,
                    ND::GeomId::Def::TOF::kSeqIdMSB,
                    ND::GeomId::Def::TOF::kSeqIdLSB);
    }
    id.SetField(module,
                ND::GeomId::Def::TOF::kModuleMSB,
                ND::GeomId::Def::TOF::kModuleLSB);
    id.SetField(0, // we don't have this
                ND::GeomId::Def::TOF::Bar::kLayerMSB,
                ND::GeomId::Def::TOF::Bar::kLayerLSB);
    id.SetField(bar,
                ND::GeomId::Def::TOF::Bar::kBarMSB,
                ND::GeomId::Def::TOF::Bar::kBarLSB);
    return id;
}

ND::TGeometryId ND::GeomId::TOF::Sensor(TGeometryId id, int end) {
    if (!ND::GeomId::TOF::IsBar(id)) return TSettableGeometryId();
    return ND::GeomId::TOF::Sensor(ND::GeomId::TOF::GetModule(id),
                                   ND::GeomId::TOF::GetBar(id),
                                   end);
}

bool ND::GeomId::TOF::IsBar(ND::TGeometryId i) {\
    TSettableGeometryId id(i);
    if (!ND::GeomId::TOF::IsTOF(id)) return false;
    if (ND::GeomId::TOF::IsSensor(id)) return true;
    return (ND::GeomId::Def::TOF::kScintillator
            == id.GetField(ND::GeomId::Def::TOF::kSeqIdMSB,
                           ND::GeomId::Def::TOF::kSeqIdLSB));
}

bool ND::GeomId::TOF::IsSensorPlus(ND::TGeometryId i) {
    TSettableGeometryId id(i);
    if (!ND::GeomId::TOF::IsTOF(id)) return false;
    return (ND::GeomId::Def::TOF::kSensorPlus
            == id.GetField(ND::GeomId::Def::TOF::kSeqIdMSB,
                           ND::GeomId::Def::TOF::kSeqIdLSB));
}

bool ND::GeomId::TOF::IsSensorMinus(ND::TGeometryId i) {
    TSettableGeometryId id(i);
    if (!ND::GeomId::TOF::IsTOF(id)) return false;
    return (ND::GeomId::Def::TOF::kSensorMinus
            == id.GetField(ND::GeomId::Def::TOF::kSeqIdMSB,
                           ND::GeomId::Def::TOF::kSeqIdLSB));
}

bool ND::GeomId::TOF::IsSensor(ND::TGeometryId i) {
    if (IsSensorPlus(i)) return true;
    if (IsSensorMinus(i)) return true;
    return false;
}


int ND::GeomId::TOF::GetBar(ND::TGeometryId i) {
    TSettableGeometryId id(i);
    if (IsBar(id) || IsSensor(id)) {
        return id.GetField(ND::GeomId::Def::TOF::Bar::kBarMSB,
                           ND::GeomId::Def::TOF::Bar::kBarLSB);
    }
    return -1;
}

int ND::GeomId::TOF::GetSensor(ND::TGeometryId i) {
    if (IsSensorMinus(i)) return -1;
    if (IsSensorPlus(i)) return 1;
    return 0;
}
