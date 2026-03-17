#ifndef TG4HitSegment_hxx_seen
#define TG4HitSegment_hxx_seen

#include <vector>

#include "TG4VHit.hxx"

namespace ND {
    class TG4HitSegment;
}

/// DO NOT USE THIS CLASS IN ANALYSIS CODE.
///
/// This is a base class used to save G4Hit objects into a root output file.
/// It contains the global position of the starting point and stopping point
/// of the track segment that created the hit, plus the fields provided by
/// TG4VHit.  The ND280GEANT4SIM expects the electronics response to be
/// simulated off-line, and to assign the sensor identifiers to each hit.
///
/// In the ND280GEANT4SIM, this class is multiply inherited with
/// ND280HitSegment to implement a G4VHit derived class that can be used in a
/// G4HitCollection.  The behavior is designed so that TG4HitSegment contains
/// all of the payload for the hit, and ND280HitSegment contains all of the
/// interface related to the G4 infrastructure.
class ND::TG4HitSegment: public TG4VHit {
public:
    TG4HitSegment();
    TG4HitSegment(const TG4HitSegment& rhs);
    virtual ~TG4HitSegment();

    /// Return a list of track identifiers that contributed to this hit.
    /// These track ids can be used as indicies to find trajectories in the
    /// TG4TrajectoryContainer object associated with an MC event.
    const std::vector<Int_t>& GetContributors(void) const {
        return fContributors;
    }

    /// Get the TrackId of the "primary" particle that is associated with this
    /// hit.  This is slightly complicated since the "interesting" primary
    /// particle is saved.  For instance, if the primary particle is a pizero,
    /// The TrackId of the gamma-rays from the decay is saved.  Likewise, the
    /// electron from a muon decay is used as the primary particle.  You can
    /// find the "really truly" primary particle by finding the trajectory
    /// associated with this PrimaryId, and then working backwards to the
    /// associated G4PrimaryParticle (You can tell that a trajectory comes
    /// from a primary particle by checking if it's ParentID is zero.  If it
    /// is zero, the trajectory came from a primary).
    int GetPrimaryId(void) const {return fPrimaryId;}

    /// Get the total energy deposited in this hit.
    double GetEnergyDeposit(void) const {return (double) fEnergyDeposit;}

    /// Get the secondary energy deposited in this hit.  This is used to help
    /// simulate scintillation and the recombination of electrons.  The
    /// secondary energy deposit is included in the total fEnergyDeposit.
    ///
    /// The details of how this field is used are determined by the detector
    /// simulation, and the material where the energy deposition occurs.  The
    /// usual case is that this field is filled by the scintillation process
    /// (i.e. a deriviative of the G4Scintillator process).  In this case, the
    /// secondary deposit represents the energy deposited as optical photons,
    /// and will include a details simulations of the Birks's law "quenching".
    /// In a detector, (i.e. if we had liquid argon), the remaining energy
    /// will be deposited as ionization.
    ///
    /// Example: For scintillator, the number of photons generated will be
    /// &lt;N_ph&gt; = (fSecondaryDeposit)/(~100 eV)
    ///
    /// Example: For an argon TPC, the mean number of quanta generated will be
    /// &lt;N_q&gt; = (fEnergyDeposit)/(19.5*eV).  Of the quanta that are generated,
    /// the mean number of photons will be &lt;N_ph&gt; =
    /// (fSecondaryDeposit)/(19.5*eV), and the free ionized electrons can be
    /// calculated from &lt;N_q&gt; = &lt;N_ph&gt; + &lt;N_e&gt;.  Note: Depending on the G4
    /// model for argon, the fSecondaryDeposit value (often) already includes
    /// binomial fluctuations, so take care to not double fluctuate N_ph or
    /// N_e.
    ///
    /// A preprocessor macro (TG4HitSegment_HasSecondaryDeposit) is defined so
    /// that code using this class can check if the feature is available.
    double GetSecondaryDeposit(void) const {return (double) fSecondaryDeposit;}
#define TG4HitSegment_HasSecondaryDeposit true

    /// Get the total charged track length in this hit.  This includes all of
    /// the contributions from secondary particles that got lumped into this
    /// hit (e.g. the contributions from delta-rays).
    double GetTrackLength(void) const {return (double) fTrackLength;}

    /// The X position of the hit starting point.  Note that a hit by
    /// definition is in a single volume.  If the hit is spread over two
    /// volumes, it's a result of round-off error (and is almost a bug).  The
    /// GeoNodeId should be defined by the average position of the hit.
    double GetStartX(void) const {return fStartX;}

    /// The Y position of the hit starting point.  Note that a hit by
    /// definition is in a single volume.  If the hit is spread over two
    /// volumes, it's a result of round-off error (and is almost a bug).  The
    /// GeoNodeId should be defined by the average position of the hit.
    double GetStartY(void) const {return fStartY;}

    /// The Z position of the hit starting point.  Note that a hit by
    /// definition is in a single volume.  If the hit is spread over two
    /// volumes, it's a result of round-off error (and is almost a bug).  The
    /// GeoNodeId should be defined by the average position of the hit.
    double GetStartZ(void) const {return fStartZ;}

    /// The time of the hit starting point.  Note that a hit by
    /// definition is in a single volume.  If the hit is spread over two
    /// volumes, it's a result of round-off error (and is almost a bug).  The
    /// GeoNodeId should be defined by the average position of the hit.
    double GetStartT(void) const {return fStartT;}

    /// The X position of the hit stoping point.  Note that a hit by
    /// definition is in a single volume.  If the hit is spread over two
    /// volumes, it's a result of round-off error (and is almost a bug).  The
    /// GeoNodeId should be defined by the average position of the hit.
    double GetStopX(void) const {return fStopX;}

    /// The Y position of the hit stoping point.  Note that a hit by
    /// definition is in a single volume.  If the hit is spread over two
    /// volumes, it's a result of round-off error (and is almost a bug).  The
    /// GeoNodeId should be defined by the average position of the hit.
    double GetStopY(void) const {return fStopY;}

    /// The Z position of the hit stoping point.  Note that a hit by
    /// definition is in a single volume.  If the hit is spread over two
    /// volumes, it's a result of round-off error (and is almost a bug).  The
    /// GeoNodeId should be defined by the average position of the hit.
    double GetStopZ(void) const {return fStopZ;}

    /// The time of the hit stoping point.  Note that a hit by
    /// definition is in a single volume.  If the hit is spread over two
    /// volumes, it's a result of round-off error (and is almost a bug).  The
    /// GeoNodeId should be defined by the average position of the hit.
    double GetStopT(void) const {return fStopT;}

    /// Print the hit information.
    virtual void ls(Option_t * = "") const;

protected:
    /// The TrackID for each trajectory that contributed to this hit.  This
    /// could contain the TrackID of the primary particle, but not
    /// necessarily.
    std::vector<Int_t> fContributors;

    /// The track id of the primary particle.
    Int_t fPrimaryId;

    /// The total energy deposit in this hit.
    Float_t fEnergyDeposit;

    /// The "secondary" energy deposit in this hit.  This is used to help
    /// simulate scintillation and the recombination of electrons.  The
    /// secondary energy deposit is included in the total fEnergyDeposit.  See
    /// the GetSecondaryDeposit() above for complete documentation.
    Float_t fSecondaryDeposit;

    /// The total charged track length in this hit.  This includes the
    /// contribution from all of the secondary particles (e.g. delta-rays)
    /// that are included in this hit.
    Float_t fTrackLength;

    /// The X position of the hit starting point.
    Float_t fStartX;

    /// The Y position of the hit starting point.
    Float_t fStartY;

    /// The Z position of the hit starting point.
    Float_t fStartZ;

    /// The time of the hit starting point.
    Float_t fStartT;

    /// The X position of the hit stopping point.
    Float_t fStopX;

    /// The Y position of the hit stopping point.
    Float_t fStopY;

    /// The Z position of the hit stopping point.
    Float_t fStopZ;

    /// The time of the hit stopping point.
    Float_t fStopT;

    ClassDef(TG4HitSegment,2);
};
#endif
