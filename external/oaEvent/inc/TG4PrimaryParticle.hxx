#ifndef TG4PrimaryParticle_hxx_seen
#define TG4PrimaryParticle_hxx_seen

#include <vector>

#include <TObject.h>
#include <TLorentzVector.h>
#include <TParticlePDG.h>

#include "TDatum.hxx"

namespace ND {
    class TG4PrimaryParticle;
    typedef std::vector<TG4PrimaryParticle> TG4PrimaryParticleContainer;
}

/// A class to save a G4 primary particle into a root output file without
/// linking to geant.
class ND::TG4PrimaryParticle: public TDatum {
public:
    TG4PrimaryParticle(void);
    virtual ~TG4PrimaryParticle();

    /// Get a reference to the 4-vector momentum for this particle.
    const TLorentzVector& GetMomentum() const {return fMomentum;}

    /// Get a reference to the 4-vector momentum for this particle.
    TLorentzVector& GetMomentum() {return fMomentum;}

    /// Set the track Id for this particle.
    void SetTrackId(int i) {fTrackId=i;}

    /// Get the track Id of the trajectory associated with this particle.
    int GetTrackId(void) const {return fTrackId;}

    /// Set the "parent" particle.  This is useful when we have a bunch of
    /// informational particles and want to decide which primary came from
    /// which informational.
    void SetParentId(int i) {fParentId=i;}

    /// Get the "parent" particle.  This is useful when we have a bunch of
    /// informational particles and want to decide which primary came from
    /// which informational.
    int GetParentId(void) const {return fParentId;}

    /// Set the PDG code for this particle.
    void SetPDGCode(int c) {fPDGCode=c;}

    /// Get the PDG code for this particle.
    int GetPDGCode(void) const {return fPDGCode;}

    /// Return a pointer to the PDG information for this particle.
    const TParticlePDG* GetParticle(void) const;

    /// Print the hit information.
    virtual void ls(Option_t * ="") const;

private:

    /// The Track Id of the matching trajectory.  Particles that are not
    /// tracked will have negative track id values.
    int fTrackId;

    /// The index into the informational particle array for the particle that
    /// created this primary.
    int fParentId;

    /// The PDG code of the particle.
    int fPDGCode;

    /// The initial momentum of the particle
    TLorentzVector fMomentum;

    ClassDef(TG4PrimaryParticle,3);
};
#endif
