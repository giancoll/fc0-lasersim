#ifndef TClusterState_hxx_seen
#define TClusterState_hxx_seen

#include "TReconState.hxx"
#include "TReconNode.hxx"

namespace ND {
    class TClusterState;
}

/// A state holding the parameters associated with a TReconCluster.  
class ND::TClusterState:
    public TReconState
{
public:
    TClusterState();
    virtual ~TClusterState();
    TClusterState(const TClusterState& init);
    virtual TClusterState& operator=(const TClusterState& rhs);

    ENERGY_DEPOSIT_STATE_DECLARATION;
    POSITION_STATE_DECLARATION

    PROJECT_STATE_DECLARATION(ProjectEDeposit,TClusterState);
    PROJECT_STATE_DECLARATION(ProjectPosition,TClusterState);
    
    private: 
    //    unsigned char fEDepositIndex; //! Do Not Save 
    //    unsigned char fPositionIndex; //! Do Not Save

    ClassDef(TClusterState,2);
};
#endif
