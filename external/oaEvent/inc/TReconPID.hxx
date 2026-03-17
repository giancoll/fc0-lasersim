#ifndef TReconPID_hxx_seen
#define TReconPID_hxx_seen

#include "THandle.hxx"
#include "TReconBase.hxx"
#include "TReconTrack.hxx"
#include "TReconShower.hxx"
#include "TPIDState.hxx"

namespace ND {
    class TReconPID;
}

/// Represent a ND::TReconCluster, ND::TReconShower, or ND::TReconTrack after
/// it has had a particle identification attached.
class ND::TReconPID: public ND::TReconBase {
public:
    TReconPID();

    /// copy constructor
    TReconPID(const ND::TReconPID& pid);


    virtual ~TReconPID();

    /// Constructors for promotion.  This converts tracks or showers into a
    /// PID with particle type "kNotSet".
    TReconPID(ND::THandle<ND::TReconTrack> track);
    TReconPID(ND::THandle<ND::TReconShower> shower);

    /// Define the particle types.
    typedef enum {
        /// A particle that has not been handled by PID.
        kNotSet,

        /// A particle that has been handled by PID, but was not
        /// successfully identified.
        kOther,

        /// \name Showering Particle Types
        /// These are particle types that are often associated with
        /// being showers.
        /// @{

        /// A generic shower.
        kShower,

        /// An electro magnetic shower.
        kEM,

        /// An electron.  Keep in mind that in the TPC, an electron will be
        /// reconstructed as a track, and then identified based on dEdX.
        kElectron,

        /// A gamma ray.
        kGamma,

        /// A generic hadronic shower.
        kHadronic,

        /// A directly identified pizero.
        kPiZero,
        /// @}

        /// @name Minimum Ionizing, or Light Particle Types
        /// These are particles that produce tracks with small ionization, or
        /// (almost equivalently) have moderate masses.  Practically, these
        /// particles will be muons, or possibly charged pions.
        /// @{

        /// A generic track, typically a MIP.
        kLightTrack,

        /// A track that has been identified as a muon.
        kMuon,

        /// A track that has been identified as a charged pion.
        kPion,

        /// @}

        /// @name Heavy Particle Types
        /// @{

        /// A generic heavy track.
        kHeavyTrack,

        /// A track identified as a proton.
        kProton,

        /// A track identified as a kaon.
        kKaon
        /// @}
    } ParticleId;

    /// Get the assigned particle id.
    ParticleId GetParticleId() const {return fParticleId;}

    /// Set the asigned particle id.
    void SetParticleId(ParticleId type) {
        fParticleId = type;
    }

    /// Get the track starting position.
    TLorentzVector GetPosition() const;

    /// Get the PID starting position uncertainty.
    TLorentzVector GetPositionVariance() const;

    /// Get the number of (non-free) spacial dimensions
    int GetDimensions() const;

    /// Check if this PID has X information.
    bool IsXPID() const;

    /// Check if this PID has Y information.
    bool IsYPID() const;

    /// Check if this PID has Z information.
    bool IsZPID() const;

    /// Get the PID direction.
    TVector3 GetDirection() const;

    /// Get the momentum of the identified particle.
    double GetMomentum() const;

    /// Get the charge of the identified particle.
    double GetCharge() const;

    /// Get the weight of the particle particle id.  In the standard
    /// implementation, this corresponds to the probability of the PID.  Other
    /// implementations may use the probability, relative likelihood, or
    /// chi-square of the PID.
    double GetPIDWeight() const {return fParticleWeight;}

    /// Set the weight of the particle.
    void SetPIDWeight(double p) {fParticleWeight = p;}

    /// Get the state at the front of the PID
    ND::THandle<ND::TPIDState> GetFront() const;

    /// Get the state at the back of the PID
    ND::THandle<ND::TPIDState> GetBack() const;

    /// Get the alternate hypotheses for this PID.  The primary PID (i.e. top
    /// level) contains the most likely hypothesis, and other possible
    /// interpretations are here.
    const ND::TReconObjectContainer& GetAlternates() const {
        return fAlternatives;
    }

    /// Add an alternative hypothesis.
    void AddAlternate(ND::THandle<TReconPID> alter) {
        fAlternatives.push_back(alter);
    }

    /// Add an alternative hypothesis.provided a ParticleId and a weight
    /// Creates a new ReconPID and add it
    void AddAlternate( ParticleId pid, double weight );

    /// Turn the ParticleId into a string.
    std::string ConvertParticleId() const;
    static std::string ConvertParticleId(ND::TReconPID::ParticleId id);

    /// List the results of particle id.
    virtual void ls(Option_t* opt = "") const;

private:

    ///  Copy constructor that does not copy alternates (used only internally)
    TReconPID(const ND::TReconPID& pid,int);

    /// it copies all information in a TReconPID except the alternates to
    /// avoid infinite recursion in alternates. It is used by the copy
    /// constructors
    void CopyTReconPID(const ND::TReconPID& pid);


    /// The particle id.
    ParticleId fParticleId;

    /// The likelihood/probability/etc. for this hypothesis.
    double fParticleWeight;

    /// The alternative hypotheses for this PID.
    ND::TReconObjectContainer fAlternatives;

    ClassDef(TReconPID,1);
};
#endif
