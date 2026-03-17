#include "TTrackState.hxx"

///////////////////////////////////////////////////////
ClassImp(ND::TTrackState);

ND::TTrackState::TTrackState() {

    ENERGY_DEPOSIT_STATE_DEFINITION;
    POSITION_STATE_DEFINITION;
    DIRECTION_STATE_DEFINITION;
    CURVATURE_STATE_DEFINITION;
    WIDTH_STATE_DEFINITION;

    Init();
}

ND::TTrackState::~TTrackState() {}

ND::TTrackState::TTrackState(const ND::TTrackState& init) {

    ENERGY_DEPOSIT_STATE_DEFINITION;
    POSITION_STATE_DEFINITION;
    DIRECTION_STATE_DEFINITION;
    CURVATURE_STATE_DEFINITION;
    WIDTH_STATE_DEFINITION;

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

ND::TTrackState& ND::TTrackState::operator=(const ND::TTrackState& rhs) {
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
PROJECT_STATE_DEFINITION(TTrackState,ProjectPosition,GetPositionIndex,kPositionStateSize,X Y Z T)
PROJECT_STATE_DEFINITION(TTrackState,ProjectEDeposit,GetEDepositIndex,kEDepositStateSize,EDeposit)
PROJECT_STATE_DEFINITION(TTrackState,ProjectDirection,GetDirectionIndex,kDirectionStateSize,DX DY DZ)
PROJECT_STATE_DEFINITION(TTrackState,ProjectWidth,GetWidthIndex,kWidthStateSize,W1 W2)
PROJECT_STATE_DEFINITION(TTrackState,ProjectCurvature,GetCurvatureIndex,kCurvatureStateSize,Curvature)
