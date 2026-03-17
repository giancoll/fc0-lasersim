#ifndef TReconTrack_hxx_seen
#define TReconTrack_hxx_seen

#include <TLorentzVector.h>
#include <TVector3.h>

#include "THandle.hxx"
#include "TReconBase.hxx"
#include "TTrackState.hxx"

namespace ND {
    class TReconTrack;
}

/// A representation of a curvilinear energy deposit starting a position, and
/// following a path.  This is described by the amount of energy (dEdX), the
/// initial position , the initial time, the initial direction, the initial
/// curvature, and the initial path width.  There must be a way to represent
/// the path of the energy deposition between the initial and final ends of
/// the deposition.  At each intermediate point, we require a representation
/// of the energy deposit (dEdX), position, time, direction, curvature, and
/// width.  The detector hits are associated with each node along the track.
///
/// The ND::TReconTrack class is intended to describe the geometry of the
/// energy deposition in a detector, and not make the association with a
/// particular particle identification.  Assignment of particle types to the
/// detector measurements is done in the ND::TReconPID class.
class ND::TReconTrack: public ND::TReconBase {
public:
    TReconTrack();

    /// copy constructor
    TReconTrack(const ND::TReconTrack& track);

    virtual ~TReconTrack();

    /// Get the energy deposited in the track.
    double GetEDeposit() const;

    /// Get the track starting position.
    TLorentzVector GetPosition() const;

    /// Get the track starting position uncertainty.
    TLorentzVector GetPositionVariance() const;

    /// Get the number of (non-free) spacial dimensions
    int GetDimensions() const;

    /// Check if this track has X information.
    bool IsXTrack() const;

    /// Check if this track has Y information.
    bool IsYTrack() const;

    /// Check if this track has Z information.
    bool IsZTrack() const;

    /// Get the track direction.
    TVector3 GetDirection() const;

    /// Get the track curvature around the X axis.
    double GetCurvature() const;

    /// Get the width of the track.
    TVector3 GetWidth() const;

    /// Get the state at the front of the track
    ND::THandle<ND::TTrackState> GetFront() const;

    /// Get the state at the back of the track
    ND::THandle<ND::TTrackState> GetBack() const;

    ClassDef(TReconTrack,1);
};
#endif
