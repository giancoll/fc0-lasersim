#ifndef TTrackState_hxx_seen
#define TTrackState_hxx_seen

#include "TReconState.hxx"
#include "TReconNode.hxx"

namespace ND {
    class TTrackState;
}

/// A state holding parameters associated with a TReconTrack, and the
/// intermediate states.
class ND::TTrackState:    public TReconState {
public:
    TTrackState();
    TTrackState(const TTrackState& init);
    virtual ~TTrackState();
    virtual TTrackState& operator=(const TTrackState& rhs);

    ENERGY_DEPOSIT_STATE_DECLARATION;
    POSITION_STATE_DECLARATION;
    DIRECTION_STATE_DECLARATION;
    CURVATURE_STATE_DECLARATION;
    WIDTH_STATE_DECLARATION;

    PROJECT_STATE_DECLARATION(ProjectEDeposit,TTrackState);
    PROJECT_STATE_DECLARATION(ProjectPosition,TTrackState);
    PROJECT_STATE_DECLARATION(ProjectDirection,TTrackState);
    PROJECT_STATE_DECLARATION(ProjectCurvature,TTrackState);
    PROJECT_STATE_DECLARATION(ProjectWidth,TTrackState);

private:
    ClassDef(TTrackState,3);
};

#endif
