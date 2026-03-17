#ifndef THit_hxx_seen
#define THit_hxx_seen

#include <iostream>
#include <vector>

#include <TObject.h>
#include <TVector3.h>

#include "TDatum.hxx"
#include "THandle.hxx"
#include "TGeometryId.hxx"
#include "TChannelId.hxx"
#include "TDigitProxy.hxx"

#include "method_deprecated.hxx"

namespace ND {
    class THit;

    /// Base class for all exceptions associated with the THit classes.
    OA_EXCEPTION(EHit,EoaCore);

    /// The index of the requested contributor, digit, or channel was out of
    /// range.
    OA_EXCEPTION(EHitOutOfRange,EoaCore);

#ifdef USE_EDigitOutOfRange
    /// The index of the requested digit was out of range.  This isn't used
    /// since all the OutOfRange exceptions use EHitOutOfRange to make traps
    /// less complex.
    OA_EXCEPTION(EDigitOutOfRange,EoaCore);
#endif
}

/// The base class for a hit detector element.  This corresponds to a real
/// measurement inside the detector after it has been fully calibrated.  Hits
/// correspond to a particular geometric element inside of the detector.  In
/// many parts of the detector, a geometric element also corresponds to a
/// particular electronics channel, however, the match to the detector
/// geometry, and not the electroncis channel.
class ND::THit : public TObject {
public:
    THit();
    virtual ~THit();

    /// Return the geometry identifier of the volume associated with this hit.
    /// This uniquely identifies the volume containing the hit (e.g. the
    /// scintillator bar, or the TPC micromega pad).  Notice that it is
    /// possible for there to be more than one hit in a single volume.  For
    /// instance, if a scintillator has double ended readout, it will have two
    /// associated channels that will both generate a hit for the same volume.
    /// This sort of double hit will generally be handled using a TReconHit.
    ///
    /// For most users, the geometry identifiers replace the functionality of
    /// the TGeoManager node identifiers.  The most common usage of the node
    /// identifier is to set the current node using TGeoManager::CdNode().
    /// The same operation can be done with a TGeometryId using
    /// TGeomIdManager::CdId(TGeometryId).  The CdId method makes the volume
    /// identified by the TGeometryId into the current TGeoManager volume, and
    /// then the user can use the various TGeoManager methods to query
    /// information.  For instance, the node identifier associated with a
    /// TGeometryId can be found using
    ///
    /// \code
    /// THandle<THit> myHit = ... Must have a value ...
    /// TOADatabase::Get().GeomId().CdId(myHit->GetGeomId());
    /// int node = gGeoManager->GetCurrentNodeId();
    /// \endcode
    ///
    /// Notice that using TGeomIdManager::CdId() changes the state of the
    /// TGeoManager in exactly the same way as TGeoManager::CdNode().
    ///
    /// The name of the volume is accessible using TGeometryId::GetName(), and
    /// the position is available either through THit::GetPosition(), or
    /// TGeometryId::GetPosition().
    virtual TGeometryId GetGeomId(void) const;

#ifdef USE_DEPRECATED_GetGeoNodeId
#warning Useing deprecated GetGeoNodeId.
    /// \deprecated Deprecated method.
    virtual int GetGeoNodeId(void) const METHOD_DEPRECATED;
#endif

    /// Return the calibrated "charge" for the hit.
    virtual double GetCharge(void) const;

    /// Return true if the calibrated charge is valid.
    virtual bool HasValidCharge(void) const;

    /// Return true if this is an alternate charge type.  This is used when a
    /// channel has multiple different ways to determine the charge.  For
    /// instance, a channel might be instrumented with an ADC and TOT charge
    /// circuit, and only one is available for each hit.  It is only needed
    /// when the charge circuits can have different behavior.
    virtual int HasAlternateCharge(void) const;

    /// Return the calibrated "time" for the hit.
    virtual double GetTime(void) const;

    /// Return true if the calibrated time is valid.
    virtual bool HasValidTime(void) const;

    /// The center of the volume associated with this hit.
    virtual const TVector3& GetPosition(void) const;

    /// Return true if this hit has useful X information.
    virtual bool IsXHit(void) const;

    /// Return true if this hit has useful Y information.
    virtual bool IsYHit(void) const;

    /// Return true if this hit has useful Z information.
    virtual bool IsZHit(void) const;

    /// Return the "spread" of the hit position.  This is the extent in the X,
    /// Y, and Z directions.  For instance, a P0D bar is about
    /// (3cm)x(2m)x(1.5cm), so the spread is (1.3cm)x(1m)x(0.75cm)
    virtual const TVector3& GetSpread(void) const;

    /// Return the "uncertainty" of the hit position.  This is the position
    /// resolution of the hit.
    virtual const TVector3& GetUncertainty(void) const;

    /// Return the "uncertainty" for the time measurement.  This is the timing
    /// resolution of the sensor.
    virtual double GetTimeUncertainty(void) const;

    /// Return a contributing hit.  If the index is out of range, this will
    /// throw an EHitOutOfRange exception.  By default this will throw an
    /// EHitOutOfRange, but it may be over-ridden in a derived class.
    virtual ND::THandle<ND::THit> GetContributor(int i=0) const;

    /// Return the number of hits that contribute to this hit.
    virtual int GetContributorCount() const;

    /// Return a proxy to the digit that generated this hit.  The TDigitProxy
    /// will be valid if a TDigitContainer is available, and if the digit
    /// being accessed is in the first 131,000 hits (2^17).  Accessing hits
    /// with an invalid index, missing TDigitContainer, or outside of that
    /// range will cause a EHitOutOfRange exception.  For specialized studies,
    /// digits after the first 131,000 can be accessed by using GetChannelId()
    /// and then searching for the appropriate hit in the TDigitContainer.
    /// This search requires knowledge of how the digits are translated into
    /// hits.
    virtual const ND::TDigitProxy& GetDigit(int i=0) const;

    /// Return the number of digits that contribute to this hit.
    virtual int GetDigitCount() const;

    /// Return the channel identifier associated with this hit.  If the hit
    /// has an associated channel identifier, then this will return a valid
    /// TChannelId.  If there isn't an associated hit, this will return an
    /// invalid value (check with TChannelId::IsValid).  If the index is out
    /// of range, then this will throw an EHitOutOfRange exception.  This
    /// information is also available through the digit, and it is an error
    /// condition if the two values disagree.
    virtual TChannelId GetChannelId(int i=0) const;

    /// Return the number of channel identifiers associated with the hit.
    virtual int GetChannelIdCount() const;

    /// Print the hit information.
    virtual void ls(Option_t *opt = "") const;

    /// Define the status bits used by the THit object.  These can't collide
    /// with any status bits defined in TObject (the parent class for THit),
    /// and none of the THit children can define a status bit that collides
    /// with these definitions.  Bits 14 to 23 are available for use.
    enum EStatusBits {
      kAltCharge1         = BIT(17),   // A flag for the calibration.
      kInvalidTime        = BIT(18),   // if time is not valid.
      kInvalidCharge      = BIT(19)    // if charge is not valid.
    };

protected:

    /// Set the validity of the calibrated time, BIT(kInvalidTime).
    virtual void SetTimeValidity(bool valid);

    /// Set the validity of the calibrated charge, BIT(kInvalidCharge).
    virtual void SetChargeValidity(bool valid);

    /// Set the alternate charge hit flag, BIT(kAltCharge1).
    virtual void SetAlternateCharge(int flag);

private:
    ClassDef(THit,4);
};
#endif
