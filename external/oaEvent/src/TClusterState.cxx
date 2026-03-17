#include "TClusterState.hxx"
#include "TND280Log.hxx"
///////////////////////////////////////////////////////
ClassImp(ND::TClusterState);

ND::TClusterState::TClusterState() {
    ENERGY_DEPOSIT_STATE_DEFINITION;
    POSITION_STATE_DEFINITION;
    Init();
}

ND::TClusterState::~TClusterState() {}

ND::TClusterState::TClusterState(const ND::TClusterState& init) {

    ENERGY_DEPOSIT_STATE_DEFINITION;
    POSITION_STATE_DEFINITION;

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

ND::TClusterState& ND::TClusterState::operator=(const ND::TClusterState& rhs) {
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

PROJECT_STATE_DEFINITION(TClusterState,ProjectPosition,GetPositionIndex,kPositionStateSize,X Y Z T)
PROJECT_STATE_DEFINITION(TClusterState,ProjectEDeposit,GetEDepositIndex,kEDepositStateSize,EDeposit)
