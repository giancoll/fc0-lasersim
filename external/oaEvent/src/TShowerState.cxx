#include "TShowerState.hxx"

///////////////////////////////////////////////////////
ClassImp(ND::TShowerState);

ND::TShowerState::TShowerState() {
    ENERGY_DEPOSIT_STATE_DEFINITION;
    POSITION_STATE_DEFINITION;
    DIRECTION_STATE_DEFINITION;
    CONE_STATE_DEFINITION;
    Init();
}

ND::TShowerState::~TShowerState() {}

ND::TShowerState::TShowerState(const ND::TShowerState& init) {
    ENERGY_DEPOSIT_STATE_DEFINITION;
    POSITION_STATE_DEFINITION;
    DIRECTION_STATE_DEFINITION;
    CONE_STATE_DEFINITION;
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

ND::TShowerState& ND::TShowerState::operator=(const ND::TShowerState& rhs) {
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
PROJECT_STATE_DEFINITION(TShowerState,ProjectPosition,GetPositionIndex,kPositionStateSize,X Y Z T)
PROJECT_STATE_DEFINITION(TShowerState,ProjectEDeposit,GetEDepositIndex,kEDepositStateSize,EDeposit)
PROJECT_STATE_DEFINITION(TShowerState,ProjectDirection,GetDirectionIndex,kDirectionStateSize,DX DY DZ)
PROJECT_STATE_DEFINITION(TShowerState,ProjectCone,GetConeIndex,kConeStateSize,C1 C2)
