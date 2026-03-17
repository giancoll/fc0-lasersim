#ifndef TComboHit_hxx_seen
#define TComboHit_hxx_seen

#include <iostream>
#include <vector>

#include <TGeoNode.h>

#include "THit.hxx"
#include "THitSelection.hxx"

namespace ND {
    class TComboHit;
    
    /// An exception flagging that hit information was accessed while the hit
    /// was open for modification.
    OA_EXCEPTION(EComboHitOpen,EoaCore);

    /// An exception flagging that an attempt to modify the hit information
    /// was made after the hit was closed.
    OA_EXCEPTION(EComboHitClosed,EoaCore);
}

/// A single calibrated hit detector element.  This corresponds to a real
/// measurement inside the detector, but after it has been fully calibrated.
class ND::TComboHit : public THit {
public:
    TComboHit();
    TComboHit(const TComboHit& val);
    virtual ~TComboHit();

    /// Get the geometry id for the volume that contains this hit.  This
    /// should be handled carefully with a TComboHit since the volume
    /// containing this hit might not be enumerated.
    virtual ND::TGeometryId GetGeomId(void) const;

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

    /// Get the parent geometry object.
    virtual TGeoNode* GetParentNode(int i) const;

    /// Get a constant version of hits associated with this combination hit.
    virtual const THitSelection& GetHits(void) const;

    /// Add a hit to the combo hits.
    virtual void AddHit(ND::THandle<ND::THit>& hit);

    /// Copy a hit selection into the combo hit.
    virtual void AddHitSelection(THitSelection& hits);

    /// Allow access to the non-constant version of the hits.
    virtual void OpenHits();

    /// Recalculate internal state variables in this hit list of
    /// associated hits.
    virtual void CloseHits() const;

    virtual void ls(Option_t *opt="") const;

private:
    /// Mark the cache as invalid.
    mutable bool fHitsOpen; //! Don't save in ROOT.

    /// The hits that contribute to this combined hit.
    THitSelection fHits;

    /// The current volume and parents that contain this cluster.  This has
    /// the constraint that it must contain at least two nodes, the volume
    /// itself, and the parent volume.  If the size is less than 2, the vector
    /// is considered empty.
    mutable std::vector<TGeoNode*> fParentNodes; //! Don't save in ROOT.

    /// The geometry id of the vlume that contains the hit.  This is the
    /// volume of the averaged position, and may not contain any of the
    /// constituient hits.
    mutable TGeometryId fGeomId; //! Don't save in ROOT.

    /// The measured "charge" for this hit.
    mutable double fCharge; //! Don't save in ROOT.

    /// The measured "time" for this hit.
    mutable double fTime; //! Don't save in ROOT.

    /// The position of the hit.
    mutable TVector3 fPosition; //! Don't save in ROOT.

    /// The spread of the hit position.
    mutable TVector3 fSpread; //! Don't save in ROOT.

    /// The uncertainty of the hit position.
    mutable TVector3 fUncertainty; //! Don't save in ROOT.

    /// The uncertainty in the hit time.
    mutable double fTimeUncertainty; //! Don't save in ROOT.

    /// Flags for what type of position information is available.
    mutable bool fIsXHit; //! Don't save in ROOT.
    mutable bool fIsYHit; //! Don't save in ROOT.
    mutable bool fIsZHit; //! Don't save in ROOT.

    ClassDef(TComboHit,3);
};
#endif
