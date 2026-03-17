#ifndef TPIDState_hxx_seen
#define TPIDState_hxx_seen

#include "TReconState.hxx"
#include "TTrackState.hxx"
#include "TShowerState.hxx"
#include "TReconNode.hxx"

namespace ND {
    class TPIDState;
}
    
/// A state holding parameters associated with a TReconPID.  
class ND::TPIDState:
    public TReconState
{
public:
    TPIDState();
    TPIDState(const ND::TPIDState& init);
    TPIDState(const ND::TTrackState& tstate);
    TPIDState(const ND::TShowerState& tstate);

    virtual ~TPIDState();
    virtual TPIDState& operator=(const ND::TPIDState& rhs);

   static TVector3 GetStateDirection(ND::THandle<ND::TReconState> state);
   static TVector3 GetStateDirectionVariance(ND::THandle<ND::TReconState> state);
   static float GetStateEDeposit(ND::THandle<ND::TReconState> state);
   
    POSITION_STATE_DECLARATION;
    DIRECTION_STATE_DECLARATION;
    MOMENTUM_STATE_DECLARATION;
    CHARGE_STATE_DECLARATION;

    PROJECT_STATE_DECLARATION(ProjectPosition,TPIDState);
    PROJECT_STATE_DECLARATION(ProjectDirection,TPIDState);
    PROJECT_STATE_DECLARATION(ProjectMomentum,TPIDState);
    PROJECT_STATE_DECLARATION(ProjectCharge,TPIDState);

 private: 
   // unsigned char fPositionIndex; //! Do Not Save
  //  unsigned char fDirectionIndex; //!Do Not Save
  //  unsigned char fMomentumIndex; //!Do Not Save
  //  unsigned char fChargeIndex; //!Do Not Save




    ClassDef(TPIDState,2);
};
#endif
