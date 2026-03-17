#ifndef TReconShower_hxx_seen
#define TReconShower_hxx_seen

#include "THandle.hxx"
#include "TReconBase.hxx"
#include "TShowerState.hxx"

namespace ND {
    class TReconShower;
}

/// A representation of an energy deposition starting at a position and
/// falling within a cone.  This is described by the total amount of energy
/// deposited, the starting position of the cone, the time of the deposit, the
/// direction of the cone axis, and the opening angle of the cone (deposit,
/// position, time, direction, opening angle).
///
/// The ND::TReconShower class is intended to describe the geometry of the
/// energy deposition in a detector, and not make the association with a
/// particular particle identification.  Assignment of particle types to the
/// detector measurements is done in the ND::TReconPID class.
class ND::TReconShower: public ND::TReconBase {
public: 
    TReconShower();

    /// copy constructor
    TReconShower(const ND::TReconShower& shower);

    virtual ~TReconShower();

    /// Get the energy deposited in the shower.
    double GetEDeposit() const;

    /// Get the shower starting position.
    TLorentzVector GetPosition() const;

    /// Get the shower starting position uncertainty.
    TLorentzVector GetPositionVariance() const;

    /// Get the number of (non-free) spacial dimensions 
    int GetDimensions() const;

    /// Check if this shower has X information.
    bool IsXShower() const;

    /// Check if this shower has Y information.
    bool IsYShower() const;

    /// Check if this shower has Z information.
    bool IsZShower() const;

    /// Get the shower direction.
    TVector3 GetDirection() const;

    /// Get the shower opening angle
    TVector3 GetConeAngle() const;

    ClassDef(TReconShower,1);
};
#endif
