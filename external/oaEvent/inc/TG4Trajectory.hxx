#ifndef TG4Trajectory_hxx_seen
#define TG4Trajectory_hxx_seen

#include <string>
#include <map>

#include <TObject.h>
#include <TLorentzVector.h>
#include <TParticlePDG.h>

#include "TDatum.hxx"
#include "THandle.hxx"
#include "TG4TrajectoryPoint.hxx"

namespace ND {
    class TG4Trajectory;
    class TG4TrajectoryContainer;
}

/// A class to save a G4 trajectory into a root output file without linking to
/// geant.  A trajectory is the truth information about the path of a particle
/// through the G4 simulation. It saves the parent trajectory that generated
/// this particle, the initial momentum of the particle, and the path followed
/// by the particle in the detector.
class ND::TG4Trajectory: public TObject {
public:
    typedef std::vector<TG4TrajectoryPoint> Points;

    TG4Trajectory(void);
    virtual ~TG4Trajectory();

    /// Get the identifier (an integer) of this track.  This is used by child
    /// trajectories to refer to this trajectory and uniquely identifies this
    /// particle trajectory within this event.
    int GetTrackId() const {return fTrackId;};

    /// Set the identifier for this track.
    void SetTrackId(int t) {fTrackId = t;};

    /// Get the identifier (an integer) of the parent trajectory for this
    /// track.
    int GetParentId() const {return fParentId;}

    /// Set the identifier of the parent trajectory.
    void SetParentId(int t) {fParentId = t;}

    /// Get the name of this particle (e.g. muon, electron, proton, &c).
    std::string GetParticleName() const {return fParticleName;};

    /// Set the particle name.
    void SetParticleName(std::string n) {fParticleName = n;};

    /// Get the PDG number for the particle that made this trajectory.
    int GetPDGEncoding() const {return fPDGEncoding;};

    /// Set the PDG number for this trajectory.
    void SetPDGEncoding(int t) {fPDGEncoding = t;};

    /// Get the starting momentum for this trajectory.
    const TLorentzVector GetInitialMomentum() const {
        return TLorentzVector(fInitialMomentumX,
                              fInitialMomentumY,
                              fInitialMomentumZ,
                              fInitialMomentumT);
    }

    void SetInitialMomentum(const TLorentzVector& m) {
        fInitialMomentumX = m.X();
        fInitialMomentumY = m.Y();
        fInitialMomentumZ = m.Z();
        fInitialMomentumT = m.T();
    }

    /// A convenience function to get the initial position of the trajectory.
    /// This provides the position of the first trajectory point in the
    /// trajectory list.  If for some reason the trajectory doesn't have a
    /// position (i.e. there aren't any trajectory points), this will return a
    /// position outside of the detector hall, and before the spill.
    TLorentzVector GetInitialPosition(void) {
        if (fPositions.size()>0) return fPositions.front().GetPosition();
        return TLorentzVector(10000,10000,10000,-10000);
    }

    /// A convenience function to get the final position of the trajectory.
    /// This provides the position of the last trajectory point in the
    /// trajectory list.  If for some reason the trajectory doesn't have a
    /// position (i.e. there aren't any trajectory points), this will return a
    /// position outside of the detector hall, and before the spill.
    TLorentzVector GetFinalPosition(void) {
        if (fPositions.size()>0) return fPositions.back().GetPosition();
        return TLorentzVector(10000,10000,10000,-10000);
    }

    /// The positions that the particle passed through as it followed this
    /// trajectory.
    Points& GetTrajectoryPoints(void) {
        return fPositions;
    }

    /// The positions that the particle passed through as it followed this
    /// trajectory.
    const Points& GetTrajectoryPoints(void) const {
        return fPositions;
    }

    /// Return a pointer to the PDG record.
    const TParticlePDG* GetParticle() const;

    /// Print the hit information.
    virtual void ls(Option_t* opt = "") const;

private:

    /// The TrackId of this trajectory.
    int fTrackId;

    /// The unique Id of the parent trajectory (The TrackId of the parent).
    int fParentId;

    /// The name of the particle.
    std::string fParticleName;

    /// The PDG encoding of the particle.
    int fPDGEncoding;

    /// @{ The initial momentum of the particle
    float fInitialMomentumX;
    float fInitialMomentumY;
    float fInitialMomentumZ;
    float fInitialMomentumT;
    /// @}

    /// The trajectory points for this trajectory.
    Points fPositions;

    ClassDef(TG4Trajectory,4);
};

/// A container for all of the trajectory information.  The trajectory
/// container inherits std::map<int,TG4Trajectory> and keyed using the
/// trajectory number.  Iterating through the trajectory returns pairs with
/// the first element being the trajectory number, and the second element
/// being the TG4Trajectory object.
class ND::TG4TrajectoryContainer : public TDatum,
                                   public std::map<int, ND::TG4Trajectory> {
public:
    TG4TrajectoryContainer(): TDatum("G4Trajectories","G4 Trajectories") {}
    ~TG4TrajectoryContainer() {}

    /// Get the identifier of the primary particle that generated a
    /// trajectory.
    int GetPrimaryId(int trackId) const;

    /// Find the trajectory, or return NULL if it doesn't exist.

    THandle<ND::TG4Trajectory> GetTrajectory(int trackId) const;

    /// Print the data vector information.
    virtual void ls(Option_t* opt = "") const;

private:

    ClassDef(TG4TrajectoryContainer,3);
};
#endif
