#ifndef TMCHit_hxx_seen
#define TMCHit_hxx_seen

#include <iostream>
#include <vector>

#include "TSingleHit.hxx"

namespace ND {
    class TG4VHit;
    class TMCHit;
    class TWritableMCHit;
}

/// A single calibrated hit detector element from the MC.  This corresponds to
/// a real measurement inside the detector, but after it has been fully
/// calibrated.
class ND::TMCHit : public TSingleHit {
public:
    typedef std::vector<TG4VHit*> ContributorContainer;

    TMCHit();
    TMCHit(const TWritableMCHit& val);
    virtual ~TMCHit();

    /// Return the MC information for the raw MC hits that contributed to this
    /// hit.
    virtual const ContributorContainer& GetContributors() const {
        return fContributors;
    }

    /// Print the hit information.
    virtual void ls(Option_t *opt = "") const;

protected:
    /// The G4HitBase objects that contributed to this hit.
    ContributorContainer fContributors;

    ClassDef(TMCHit,5);
};

/// Provide a writable derived class used to create a TMCHit object.
class ND::TWritableMCHit : public TMCHit {
public:
    TWritableMCHit();
    TWritableMCHit(const TWritableMCHit& h);
    virtual ~TWritableMCHit();

    void SetGeomId(TGeometryId id);

    void SetCharge(double q);

    void SetTime(double t);

    void SetChannelId(ND::TChannelId id);

    void SetChargeValidity(bool valid);

    void SetTimeValidity(bool valid);

    void SetAlternateCharge(int flag);

    /// Return non constant version of the MC information for the raw MC hits
    /// that contributed to this hit.
    virtual ContributorContainer& GetContributors() {
        return fContributors;
    }

    /// Return the MC information for the raw MC hits that contributed to this
    /// hit.
    virtual const ContributorContainer& GetContributors() const {
        return fContributors;
    }

    ClassDef(TWritableMCHit,5);
};
#endif
