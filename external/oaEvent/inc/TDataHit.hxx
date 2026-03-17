#ifndef TDataHit_hxx_seen
#define TDataHit_hxx_seen

#include <iostream>
#include <vector>

#include "TSingleHit.hxx"
#include "TDigitProxy.hxx"

namespace ND {
    class TDataHit;
    class TWritableDataHit;
}

/// A single calibrated hit detector element.  This corresponds to a real
/// measurement inside the detector, after it has been fully calibrated.  The
/// hit is associated with a particular geometric element inside of the
/// detector.
///
/// If the MC has written digitized data (derived from TMCDigit), then a
/// TDataHit will be used to hold the calibrated MC THit objects.  You can get
/// associated MC information using the TDigitProxy saved in the hit.
class ND::TDataHit : public TSingleHit {
public:
    TDataHit();
    TDataHit(const TWritableDataHit& val);
    virtual ~TDataHit();

    /// Return a proxy to the digit that generated this hit.  If the index is
    /// out of range, this will throw an EHitOutOfRange exception.
    virtual const ND::TDigitProxy& GetDigit(int i=0) const;

    /// Return the number of digits that contribute to this hit.
    virtual int GetDigitCount() const;

protected:
    /// The digit that generated this hit.
    TDigitProxy fProxy;

    ClassDef(TDataHit,5);
};

/// Provide a writable interface to a TDataHit that can be used to fill the
/// object.
class ND::TWritableDataHit : public TDataHit {
public:
    TWritableDataHit();
    TWritableDataHit(const TWritableDataHit& h);
    virtual ~TWritableDataHit();

    void SetGeomId(TGeometryId id);

    void SetCharge(double q);

    void SetTime(double t);

    void SetChargeValidity(bool valid);

    void SetTimeValidity(bool valid);

    void SetAlternateCharge(int flag);

    /// Set the digit and channel id for this hit.  If the proxy can access a
    /// valid digit, then this also sets the channel id.
    void SetDigit(ND::TDigitProxy proxy);

    /// Explicitly set the channel id (shouldn't be required since also done
    /// by SetDigit).
    void SetChannelId(ND::TChannelId id);

    ClassDef(TWritableDataHit,4);
};
#endif
