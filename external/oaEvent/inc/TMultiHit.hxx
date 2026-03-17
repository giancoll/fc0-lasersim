#ifndef TMultiHit_hxx_seen
#define TMultiHit_hxx_seen

#include <iostream>
#include <vector>

#include "EoaCore.hxx"
#include "THit.hxx"
#include "TSingleHit.hxx"
#include "THandle.hxx"

namespace ND {
    class TMultiHit;

    OA_EXCEPTION(EMultiHitIllegal,EoaCore);
}

/// Contain a calibrated wave form (collection of time ordered hits) for a
/// single calibrated hit detector element (the entire wave form must come
/// from the same channel).  This corresponds to a real measurement inside the
/// detector, but after it has been fully calibrated.  The TMultiHit class is
/// designed to allow waveforms to be easily split into separate zero
/// suppressed TMultiHit objects.  A new TMultiHit can be created from a
/// sequence of THandle<TSingleHit> objects between a "first" and "last"
/// iterator
///
/// \code
/// std::vector< THandle<TSingleHit> > sVect;  -- must be filled!
/// THandle<TMultiHit> mHit(new TMultiHit(sVect.begin(),sVect.end()));
/// THandle<TMultiHit> aHit(new TMultiHit(mHit.begin(),mHit.end()));
/// \endcode
///
/// The access to individual measurements in a TMultiHit object is done
/// through an interface similar to a std:vector< THandle<TSingleHit> >.  The
/// measurements can be accessed using iterators (the TMulitHit::begin() and
/// TMultiHit::end() methods), or by indexing (the TMultiHit::operator[] and
/// TMultiHit::size() methods).  As with all (non-TComboHit) hit objects, a
/// TMultiHit object is immutable after it has been created.
///
/// Since TMultiHit inherits from THit, the standard time, charge, position,
/// &c information are provided.  However, this summary information is only
/// based on a crude summary of the wave form (e.g. time of the first hit,
/// total sum of the charge, &c).
class ND::TMultiHit : public THit {
public:
    typedef std::vector< THandle< TSingleHit > > Hits;
    typedef Hits::const_iterator iterator;

    TMultiHit();
    TMultiHit(const TMultiHit& val);
    TMultiHit(iterator first, iterator last);
    virtual ~TMultiHit();

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

    /// Return the "uncertainty" of the hit position.  
    virtual const TVector3& GetUncertainty(void) const;

    /// Return the "uncertainty" for the time measurement.
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

    /// Return an iterator to the first hit.
    virtual iterator begin() const;

    /// Return an iterator to the final hit.
    virtual iterator end() const;

    /// Return a reference to the i'th THandle<THit> in the multi hit.
    const THandle<ND::TSingleHit>& operator [](int index) const;

    /// Return the number of THandle<THit> in the multi hit
    unsigned int size() const;

    virtual void ls(Option_t *opt="") const;

private:

    /// Fill all of the geometry related fields from the fHits and the
    /// geometry data base.
    void Initialize();

    /// The hits that contribute to this combined hit.
    Hits fHits;

protected:
    /// This is set to true if the fast access fields below have been
    /// initialized.
    bool fInitialized; //! Don't Save

    /// The geometry node that was hit
    TGeometryId fGeomId; //! Don't Save

    /// The key of the voxel for this hit.
    Int_t fGeoKey; //! Don't Save

    /// The measured "charge" for this hit.
    Float_t fCharge; //! Don't Save

    /// The measured "time" for this hit.
    Float_t fTime; //! Don't Save

    /// The position of the hit.
    TVector3 fPosition; //! Don't Save

    /// Flags for what type of position information is available.
    bool fIsXHit;  //! Don't Save
    bool fIsYHit;  //! Don't Save
    bool fIsZHit;  //! Don't Save

    /// The spread of the hit position.
    TVector3 fSpread; //! Don't Save

    /// The uncertainty of the hit position.
    TVector3 fUncertainty; //! Don't Save

    /// The uncertainty of the time.
    double fTimeUncertainty; //! Don't Save

    ClassDef(TMultiHit,3);
};
#endif
