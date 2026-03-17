#include "TVertexState.hxx"

///////////////////////////////////////////////////////
ClassImp(ND::TVertexState);

ND::TVertexState::TVertexState() {
    POSITION_STATE_DEFINITION;
    Init();
}

ND::TVertexState::~TVertexState() {}

ND::TVertexState::TVertexState(const ND::TVertexState& init) {

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

ND::TVertexState& ND::TVertexState::operator=(const ND::TVertexState& rhs) {
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

PROJECT_STATE_DEFINITION(TVertexState,ProjectPosition,GetPositionIndex,kPositionStateSize,X Y Z T)
