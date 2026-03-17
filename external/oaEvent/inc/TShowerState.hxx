#ifndef TShowerState_hxx_seen
#define TShowerState_hxx_seen

#include "TReconState.hxx"
#include "TReconNode.hxx"

namespace ND {
    class TShowerState;
}

/// A state holding the parameters associated with a TReconShower.
class ND::TShowerState: public TReconState {
public:
    TShowerState();
    TShowerState(const TShowerState& init);
    virtual ~TShowerState();
    virtual TShowerState& operator=(const TShowerState& rhs);

    ENERGY_DEPOSIT_STATE_DECLARATION;
    POSITION_STATE_DECLARATION;
    DIRECTION_STATE_DECLARATION;
    CONE_STATE_DECLARATION;

    PROJECT_STATE_DECLARATION(ProjectEDeposit,TShowerState);
    PROJECT_STATE_DECLARATION(ProjectPosition,TShowerState);
    PROJECT_STATE_DECLARATION(ProjectDirection,TShowerState);
    PROJECT_STATE_DECLARATION(ProjectCone,TShowerState);

private:
    ClassDef(TShowerState,3);
};
#endif
