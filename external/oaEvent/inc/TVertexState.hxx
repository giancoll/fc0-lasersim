#ifndef TVertexState_hxx_seen
#define TVertexState_hxx_seen

#include "TReconState.hxx"
#include "TReconNode.hxx"

namespace ND {
    class TVertexState;
}

/// A state holding parameters associated with a TReconVertex.
class ND::TVertexState: public TReconState
{
public:
    TVertexState();
    virtual ~TVertexState();
    TVertexState(const TVertexState& init);
    virtual TVertexState& operator=(const TVertexState& rhs);

    POSITION_STATE_DECLARATION
    
    PROJECT_STATE_DECLARATION(ProjectPosition,TVertexState);

private:
    
    ClassDef(TVertexState,3);
};
#endif
