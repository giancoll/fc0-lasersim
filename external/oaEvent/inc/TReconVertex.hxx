#ifndef TReconVertex_hxx_seen
#define TReconVertex_hxx_seen

#include "THandle.hxx"
#include "TReconBase.hxx"
#include "TVertexState.hxx"

namespace ND {
    class TReconVertex;
}

/// Define a vertex location within the detector.
class ND::TReconVertex: public ND::TReconBase {
public: 
    TReconVertex();
    virtual ~TReconVertex();

    /// Get the vertex position.
    TLorentzVector GetPosition() const;

    /// Get the track starting position uncertainty.
    TLorentzVector GetPositionVariance() const;

    /// Get the number of (non-free) spacial dimensions 
    int GetDimensions() const;

    /// Check if this vertex has X information.
    bool IsXVertex() const;

    /// Check if this vertex has Y information.
    bool IsYVertex() const;

    /// Check if this vertex has Z information.
    bool IsZVertex() const;

    ClassDef(TReconVertex,1);
};
#endif
