#include <cmath>

#include "TPIDState.hxx"
#include "HEPUnits.hxx"
#include "TClusterState.hxx"

///////////////////////////////////////////////////////
ClassImp(ND::TPIDState);

ND::TPIDState::TPIDState() {

    POSITION_STATE_DEFINITION;
    DIRECTION_STATE_DEFINITION;
    MOMENTUM_STATE_DEFINITION;
    CHARGE_STATE_DEFINITION;

    Init();
}

ND::TPIDState::TPIDState(const ND::TTrackState& tstate) {

    POSITION_STATE_DEFINITION;
    DIRECTION_STATE_DEFINITION;
    MOMENTUM_STATE_DEFINITION;
    CHARGE_STATE_DEFINITION;

    Init();

    // retrieve the position and it's covariance.
    for(int i = 0;i < ND::TReconState::kPositionStateSize; ++i) {
        SetValue(i+GetPositionIndex(),
                 tstate.GetValue(i+tstate.GetPositionIndex()));
        for(int j = 0;j < ND::TReconState::kPositionStateSize; ++j) {
            SetCovarianceValue(i+GetPositionIndex(),
                               j+GetPositionIndex(),
                               tstate.GetCovarianceValue(
                                   i+tstate.GetPositionIndex(),
                                   j+tstate.GetPositionIndex()));
        }
    }

    // retrieve the direction and it's covariance.
    for(int i = 0;i < ND::TReconState::kDirectionStateSize;++i){
        SetValue(i+GetDirectionIndex(),
                 tstate.GetValue(i+tstate.GetDirectionIndex()));
        for(int j = 0;j < ND::TReconState::kDirectionStateSize;++j){
            SetCovarianceValue(i+GetDirectionIndex(),
                               j+GetDirectionIndex(),
                               tstate.GetCovarianceValue(
                                   i+tstate.GetDirectionIndex(),
                                   j+tstate.GetDirectionIndex()));
            SetCovarianceValue(i+GetPositionIndex(),
                               j+GetDirectionIndex(),
                               tstate.GetCovarianceValue(
                                   i+tstate.GetPositionIndex(),
                                   j+tstate.GetDirectionIndex()));
            SetCovarianceValue(i+GetDirectionIndex(),
                               j+GetPositionIndex(),
                               tstate.GetCovarianceValue(
                                   i+tstate.GetDirectionIndex(),
                                   j+tstate.GetPositionIndex()));
        }
    }

#ifdef USE_MAGNETIC_FIELD
    // If curvature is available, then convert to momentum.  This is bogus
    // since it depends on a magnetic field, so the momentum is initialized
    // assuming a constant 0.2T field, but is set as a free parameter.  be
    // free (by default).
    double B=0.2;
    double factor = -0.3*B;
    double p = 0;
    if (!tstate.IsFree(tstate.GetCurvatureIndex())) {
        p=std::fabs(factor*1/tstate.GetValue(tstate.GetCurvatureIndex()));
    }

    SetValue(GetMomentumIndex(),p);
    SetFree(GetMomentumIndex());
#endif

    // The track charge can't be determined, so set it to free with a charge
    // of zero.
    SetValue(GetChargeIndex(),0.0);
    SetFree(GetChargeIndex());
}


ND::TPIDState::TPIDState(const ND::TShowerState& tstate) {

    POSITION_STATE_DEFINITION;
    DIRECTION_STATE_DEFINITION;
    MOMENTUM_STATE_DEFINITION;
    CHARGE_STATE_DEFINITION;

    Init();

    // retrieve the position and it's covariance.
    for(int i = 0;i < ND::TReconState::kPositionStateSize; ++i) {
        SetValue(i+GetPositionIndex(),
                 tstate.GetValue(i+tstate.GetPositionIndex()));
        for(int j = 0;j < ND::TReconState::kPositionStateSize; ++j) {
            SetCovarianceValue(i+GetPositionIndex(),
                               j+GetPositionIndex(),
                               tstate.GetCovarianceValue(
                                   i+tstate.GetPositionIndex(),
                                   j+tstate.GetPositionIndex()));
        }
    }

    // retrieve the direction and it's covariance.
    for(int i = 0;i < ND::TReconState::kDirectionStateSize; ++i){
        SetValue(i+GetDirectionIndex(),
                 tstate.GetValue(i+tstate.GetDirectionIndex()));
        for(int j = 0;j < ND::TReconState::kDirectionStateSize; ++j){
            SetCovarianceValue(i+GetDirectionIndex(),
                               j+GetDirectionIndex(),
                               tstate.GetCovarianceValue(
                                   i+tstate.GetDirectionIndex(),
                                   j+tstate.GetDirectionIndex()));
            SetCovarianceValue(i+GetPositionIndex(),
                               j+GetDirectionIndex(),
                               tstate.GetCovarianceValue(
                                   i+tstate.GetPositionIndex(),
                                   j+tstate.GetDirectionIndex()));
            SetCovarianceValue(i+GetDirectionIndex(),
                               j+GetPositionIndex(),
                               tstate.GetCovarianceValue(
                                   i+tstate.GetDirectionIndex(),
                                   j+tstate.GetPositionIndex()));
        }
    }

    // Set momentum and charge
    double p = tstate.GetEDeposit(); // Use the deposited energy (bogus)
    double q = 0;                    // Don't have any curvature.

    SetValue(GetMomentumIndex(),p);
    SetFree(GetMomentumIndex());

    SetValue(GetChargeIndex(),q);
    SetFree(GetChargeIndex());
}

ND::TPIDState::TPIDState(const ND::TPIDState& init) {

    POSITION_STATE_DEFINITION;
    DIRECTION_STATE_DEFINITION;
    MOMENTUM_STATE_DEFINITION;
    CHARGE_STATE_DEFINITION;

    Init();

    for (int i=0; i<GetDimensions(); ++i) {
        SetValue(i,init.GetValue(i));
    }

    for (int i=0; i<GetDimensions(); ++i) {
        for (int j=0; j<GetDimensions(); ++j) {
            SetCovarianceValue(i,j,init.GetCovarianceValue(i,j));
        }
    }

}

ND::TPIDState& ND::TPIDState::operator=(const ND::TPIDState& rhs) {
    if (this == &rhs) return *this;

    for (int i=0; i<GetDimensions(); ++i) {
        SetValue(i,rhs.GetValue(i));
    }

    for (int i=0; i<GetDimensions(); ++i) {
        for (int j=0; j<GetDimensions(); ++j) {
            SetCovarianceValue(i,j,rhs.GetCovarianceValue(i,j));
        }
    }

    return *this;
}

ND::TPIDState::~TPIDState() {}

PROJECT_STATE_DEFINITION(TPIDState,ProjectPosition,GetPositionIndex,kPositionStateSize,X Y Z T)
PROJECT_STATE_DEFINITION(TPIDState,ProjectDirection,GetDirectionIndex,kDirectionStateSize,DX DY DZ)
PROJECT_STATE_DEFINITION(TPIDState,ProjectMomentum,GetMomentumIndex,kMomentumStateSize,W1 W2)
PROJECT_STATE_DEFINITION(TPIDState,ProjectCharge,GetChargeIndex,kChargeStateSize,Charge)
// This is nothing to do with PID per se, it is just a convenient class that
// knows about PID Shower an Track objects already so this utility method is
// here
//
TVector3 ND::TPIDState::GetStateDirection(ND::THandle<ND::TReconState> state) {
    ND::TPIDState* ps = dynamic_cast<ND::TPIDState*> ( ND::GetPointer(state)) ;
    if (ps) return ps->GetDirection();

    ND::TShowerState* ss
        = dynamic_cast<ND::TShowerState*> ( ND::GetPointer(state)) ;
    if (ss) return ss->GetDirection();

    ND::TTrackState* ts
        = dynamic_cast<ND::TTrackState*> ( ND::GetPointer(state)) ;
    if (ts) return ts->GetDirection();
    return TVector3();
}

float ND::TPIDState::GetStateEDeposit(ND::THandle<ND::TReconState> state) {
    ND::TClusterState* cs = dynamic_cast<ND::TClusterState*> (
        ND::GetPointer(state));
    if (cs) return cs->GetEDeposit() ;

    ND::TShowerState*  ss = dynamic_cast<ND::TShowerState*>  (
        ND::GetPointer(state));
    if (ss) return ss->GetEDeposit();

    ND::TTrackState*   ts = dynamic_cast<ND::TTrackState*>   (
        ND::GetPointer(state));
    if (ts) return ts->GetEDeposit();

    return 0.0;
};

TVector3 ND::TPIDState::GetStateDirectionVariance(
    ND::THandle<ND::TReconState> state) {
    ND::TPIDState* ps = dynamic_cast<ND::TPIDState*> (
        ND::GetPointer(state));
    if (ps) return ps->GetDirectionVariance();

    ND::TShowerState* ss = dynamic_cast<ND::TShowerState*> (
        ND::GetPointer(state));
    if (ss) return ss->GetDirectionVariance();

    ND::TTrackState* ts = dynamic_cast<ND::TTrackState*> (
        ND::GetPointer(state));
    if (ts) return ts->GetDirectionVariance();

    return TVector3();
}
