#ifndef TSingleHit_hxx_seen
#define TSingleHit_hxx_seen

#include "THit.hxx"

class TGeoManager;

namespace ND {
    class TSingleHit;
    class TMCHit;
    class TDataHit;
    class TWritableMCHit;
    class TWritableDataHit;
}

/// A single hit.  This cannot be directly instantiated.
class ND::TSingleHit: public THit {
private:
    friend class TMCHit;
    friend class TDataHit;
    friend class TWritableMCHit;
    friend class TWritableDataHit;

    // The TSingleHit class can't be directly instantiated.  It is created
    // using the TWritableMCHit, or TWritableDataHit classes and is accessed
    // as a THit class.
    TSingleHit();
    TSingleHit(const TSingleHit&);

public:
    virtual ~TSingleHit();

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

    /// Return the calibrated "charge" for the hit.
    virtual double GetCharge(void) const;

    /// Return the calibrated "time" for the hit.
    virtual double GetTime(void) const;

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

private:
    /// Fill all of the geometry related fields from the geometry data base.
    void Initialize();

    /// A helper routine to handle the specific TPC geometry data base.
    bool InitializeTPC();

    /// A helper routine to handle the specific HAT geometry data base.
    bool InitializeHAT();

    /// A helper routine to handle the specific SFG details.
    bool InitializeSFG();

    /// A helper routine to handle the generic geometry data base (for now,
    /// this is everything except the TPCs and SFGD).
    bool InitializeGeneric();

protected:
    /// The geometry node that was hit
    Int_t fGeomId;

    /// The measured "charge" for this hit.
    Float_t fCharge;

    /// The measured "time" for this hit.
    Float_t fTime;

    /// The channel id associated with this hit.
    ND::TChannelId fChannelId;

    /// This is set to true if the fast access fields below have been
    /// initialized.
    bool fInitialized; //! Don't Save

    /// The position of the hit.
#ifdef TSINGLE_HIT_SAVE_POSITION
    // Define TSINGLE_HIT_SAVE_POSITION to save the hit position in the output
    // file.  Otherwise, the hit position is found from the TGeoManager.  This
    // flag should not be defined unless you are debugging the geometry code
    // since it prevents the calibration system from applying corrections to
    // the geometry.
#warning This object is being compiled with the hit position saved in the \
output because the TSINGLE_HIT_SAVE_POSITION cflag was defined.  The position \
should only be saved if your a debugging the oaEvent hit geometry interface, \
and MUST NOT be saved for normal data.  Saving the position with the event \
by defining the TSINGLE_HIT_SAVE_POSITION flag may prevent the calibration \
system from applying corrections to the geometry.
    TVector3 fPosition;
#else
    TVector3 fPosition; //! Don't Save
#endif

    /// Flags for what type of position information is available.
    bool fIsXHit;  //! Don't Save
    bool fIsYHit;  //! Don't Save
    bool fIsZHit;  //! Don't Save

    /// The spread of the hit position.
    TVector3 fSpread; //! Don't Save

    /// The uncertainty of the hit position.
    TVector3 fUncertainty; //! Don't Save

    /// The uncertainty for the timing.
    double fTimeUncertainty; //! Don't Save.

    ClassDef(TSingleHit,3);
};
#endif
