#ifndef TReconHit_hxx_seen
#define TReconHit_hxx_seen

#include <iostream>
#include <vector>

#include "THit.hxx"
#include "TGeometryId.hxx"

namespace ND {
    class TReconHit;
    class TWritableReconHit;

    OA_EXCEPTION(EReconHitIllegal,EoaCore);
}

/// A calibrated hit detector element where the hit information has been
/// reconstructed from the raw hits.  A TReconHit object represents a hit that
/// has been generated from one or more hits associated with a geometry
/// object.  A common usage is to represent a hit that has been constructed
/// from two hits on a bar that is read out at both ends.  The Geometry id of
/// the TReconHit is defined by the first added hit, and any other added hits
/// are not directly used in TReconHit.  It is created using the
/// TWritableReconHit class and is accessed as a THit class.  The
/// TWritableReconHit class imposes the constraint that the constituent hits
/// are from a single geometry object.
class ND::TReconHit : public THit {
public:
    TReconHit();
    TReconHit(const TWritableReconHit& val);
    virtual ~TReconHit();

    /// Get the geometry node that contains this hit.
    virtual TGeometryId GetGeomId(void) const;

    /// Return the calibrated "charge" for the hit.
    virtual double GetCharge(void) const;

    /// Return the calibrated "time" for the hit.
    virtual double GetTime(void) const;

    /// Return the reconstructed time uncertainty of the hit.
    virtual double GetTimeUncertainty(void) const;

    /// The reconstructed position of the hit
    virtual const TVector3& GetPosition(void) const;

    /// Return the "uncertainty" of the reconstructed  hit position.
    virtual const TVector3& GetUncertainty(void) const;

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

    /// Return a contributing hit.  If the index is out of range, this will
    /// throw an EHitOutOfRange exception.
    ND::THandle<ND::THit> GetContributor(int i) const;

    /// Return the number of hits that contribute to this hit.
    int GetContributorCount() const;

    /// Print the hit information.
    virtual void ls(Option_t *opt = "") const;

private:

    /// Fill all of the geometry related fields from the single hits and
    /// apply corrections.
    void Initialize();

protected:

    /// The geometry node that was hit
    TGeometryId fGeomId;  //!Don't Save

    /// The measured "charge" for this hit.
    Float_t fCharge;

    /// The measured "time" for this hit.
    Float_t fTime;

    /// The reconstructed time uncertainty
    Float_t fTimeUncertainty;

    /// The reconstructed position of the hit in *** LOCAL *** coordinates.
    /// Saving the position in local coordinates allows the alignment
    /// constants to be correctly applied.
    TVector3 fReconPosition;

    /// The uncertainty of the hit position *** LOCAL *** coordinates.  Saving
    /// the uncertainty in local coordinates allows the alignment constants to
    /// be correctly applied.
    TVector3 fReconUncertainty;

    /// The THits that make up this reconstructed hit.
    std::vector< ND::THandle < ND::THit > > fConstituents;

    /// This is set to true if the fast access fields below have been
    /// initialized.
    bool fInitialized; //! Don't Save

    // The position of the hit after applying alignment corrections.  This is
    // calculated from the fReconPosition field which is saved in local
    // coordinates.  The alignment is applied by querying the geometry to find
    // the current geometry object position.
    TVector3 fPosition; //! Don't Save

    /// Flags for what type of position information is available.
    bool fIsXHit;  //! Don't Save
    bool fIsYHit;  //! Don't Save
    bool fIsZHit;  //! Don't Save

    /// The spread of the hit position.
    TVector3 fSpread; //! Don't Save

    /// The uncertainty of the hit position.  See fPosition for more
    /// information.
    TVector3 fUncertainty; //! Don't Save

    ClassDef(TReconHit,4);
};



/// Provide a writable interface to a TReconHit that can be used to fill the
/// object. The geometry information (i.e. the geometry id) is taken from the
/// first hit, but the other fields are set to default values (zero for the
/// vectors, and negative for the charge, time and uncertainty).  All of the
/// THit objects added to a single TWritableReconHit object should be in the
/// same geometry object (e.g. all in the same bar), but it is not checked by
/// the class..  The other hits added to the TWritableReconHit are not used by
/// this class, but are available for information purposes.  If hits from
/// different volumes are added using the convenience constructors, an
/// EReconHitIllegal exception will be thrown.
class ND::TWritableReconHit : public TReconHit {
public:
    TWritableReconHit();
    TWritableReconHit(const TWritableReconHit& h);

    /// A "convenience" constructor for the case where there is one
    /// constituent.  This sets the constituent fields, but the position,
    /// time, &c are not initialized.  These fields and their uncertainties
    /// must be set using SetCharge, SetTime, &c.
    explicit TWritableReconHit(ND::THandle<ND::THit> hit);

    /// A "convenience" constructor for the case where there are two
    /// constituents.  This sets the constituent fields, but the position,
    /// time, &c are not initialized.  These fields and their uncertainties
    /// must be set using SetCharge, SetTime, &c.
    explicit TWritableReconHit(ND::THandle<ND::THit> hit1,
                               ND::THandle<ND::THit> hit2);

    virtual ~TWritableReconHit();

    /// Add a hit to an existing TWritableReconHit.  The hits must be in the
    /// same volume, except when sameVolume is false.  If sameVolume is false,
    /// this removes the consistency check.  The geometry id comes from the
    /// first constituent hit.
    void AddHit(ND::THandle<ND::THit> hit, bool sameVolume=true);

    /// Set the charge for the hit.  This will mark the charge as valid
    /// (invalid before it is set).
    void SetCharge(double q);

    /// Set the time for the hit.  This will mark the time as valid (invalid
    /// before it is set).
    void SetTime(double t);

    /// Set the time uncertainty for the hit.
    void SetTimeUncertainty(double tunc);

    /// Set the position for the hit in *** GLOBAL *** coordinates.
    /// Internally, this is translated to local coordinates.
    void SetPosition(const TVector3& pos);

    /// Set the position uncertainty for the hit in *** GLOBAL ***
    /// coordinates.  Internally, this is translated to local coordinates.
    void SetUncertainty(const TVector3& unc);

    /// Override the hit charge validity flag.
    void SetChargeValidity(bool valid);
#define Has_ND_TReconHit_SetChargeValidity /* it is available */

    /// Override the time validity flag.
    void SetTimeValidity(bool valid);
#define Has_ND_TReconHit_SetTimeValidity /* it is available */

    /// Set the alternate charge flag.
    void SetAlternateCharge(int flag);
#define Has_ND_TReconHit_SetAlternateCharge /* it is available */

    ClassDef(TWritableReconHit,4);
};
#endif
