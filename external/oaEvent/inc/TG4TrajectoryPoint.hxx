#ifndef TG4TrajectoryPoint_hxx_seen
#define TG4TrajectoryPoint_hxx_seen

#include <string>
#include <vector>

#include <TObject.h>
#include <TLorentzVector.h>
#include <TVector3.h>

#include "TDatum.hxx"

namespace ND {
    class TG4TrajectoryPoint;
}

/// A class to save a G4 trajectory point into a root output file without
/// linking to geant.  The trajectory point is saved in a TG4Trajectory as a
/// way to record the path of a particle through the detector.  This is the
/// truth information about the particles which were tracked, but is not a
/// good record of the energy deposition.  Use the TG4HitSegment objects for a
/// record of the energy deposition.
class ND::TG4TrajectoryPoint: public TObject {
public:
    TG4TrajectoryPoint();
    virtual ~TG4TrajectoryPoint();

    /// Get the position of the trajectory point.
    const TLorentzVector GetPosition(void) const {
        return TLorentzVector(fPositionX, fPositionY, fPositionZ, fPositionT);
    }

    /// Set the position of the trajectory point.
    void SetPosition(double x, double y, double z, double t) {
        fPositionX = x;
        fPositionY = y;
        fPositionZ = z;
        fPositionT = t;
    }

    /// Get the momentum of the particle at the trajectory point.
    const TVector3 GetMomentum(void) const {
        return TVector3(fMomentumX, fMomentumY, fMomentumZ);
    }

    /// Set the momentum of the particle at this trajectory point.
    void SetMomentum(double px, double py, double pz) {
        fMomentumX = px;
        fMomentumY = py;
        fMomentumZ = pz;
    }

    /// Get the name of the physical volume for the trajectory point.
    std::string GetVolumeName(void) const;

    /// Set the ROOT node id for this trajectory point
    void SetVolumeNode(int id) {fVolumeNode = id;}

    /// Get the ROOT geometry node id for this track point.  This can be used
    /// to identify which volume is being hit using the
    /// TGeomInfo::Get().VolumeName() method.
    int GetVolumeNode(void) const {
        return fVolumeNode;
    }

    /// Process types copied from the G4 definitions so that this can be
    /// compiled without having geant4 installed.  Check the exact definitions
    /// are in the geant4 documentation, but the (important) names are pretty
    /// self explanatory.  The definitions can be found in the geant4 include
    /// file named G4ProcessType.hh.
    enum G4ProcessType {
        kProcessNotDefined = 0,
        kProcessTransportation = 1,
        kProcessElectromagetic = 2 ,
        kProcessOptical = 3,
        kProcessHadronic = 4,
        kProcessPhotoLeptonHadron = 5,
        kProcessDecay = 6,
        kProcessGeneral = 7,
        kProcessParameterization = 8,
        kProcessUserDefined = 9
    };

    /// Get the G4ProcessType (as an integer).
    int GetProcessType() const {return fProcessType % 1000;}

    /// Several important process sub-types as defined by geant4.  These are
    /// copied so that reading the files does not directly depend on having
    /// geant4 installed.  Check the geant4 documentation for the
    /// documentation, but most of the names are fairly self explanatory.  The
    /// definitions are mostly found in the geant4 include files named
    /// G4HadronicProcessType.hh and G4EmProcessSubType.hh
    enum G4ProcessSubtype {
        // EM subtypes for charged particles.
        kSubtypeEMCoulombScattering = 1,
        kSubtypeEMIonization = 2,
        kSubtypeEMBremsstrahlung = 3,
        kSubtypeEMPairProdByCharged = 4,
        kSubtypeEMNuclearStopping = 8,

        // EM subtypes for photons
        kSubtypeEMMultipleScattering = 10,
        kSubtypeEMPhotoelectric = 12,
        kSubtypeEMComptonScattering = 13,
        kSubtypeEMGammaConversion = 14,

        // Transportation subtypes
        kSubTypeTransportation = 91,
        kSubtypeTransportationCoupled = 92,

        // Hadronic subtypes
        kSubtypeHadronElastic = 111,
        kSubtypeHadronInelastic = 121,
        kSubtypeHadronCapture = 131,
        kSubtypeHadronChargeExchange = 161,

        // Decay subtypes
        kSubtypeDecay = 201,
        kSubtypeDecayWithSpin = 202,
        kSubtypeDecayPionMakeSpin = 203,
        kSubtypeDecayRadioactive = 210,
        kSubtypeDecayUnknown = 211,
        kSubtypeDecayExternal = 231,

        // General subtypes
        kSubtypeGeneralStepLimit = 401
    };

    /// Get the G4 process subtype (as an integer).
    int GetProcessSubtype() const {return fProcessType / 1000;}

    /// Set the G4 process type and and subtype (this is low level "developer"
    /// information).  The type and sub-type can be set directly from the
    /// value of
    /// aStep->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessType()
    /// and
    /// aStep->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessSubType()
    /// which made available in ND280GEANT4SIM by
    /// ND280TrajectoryPoint::GetProcessType() and
    /// ND280TrajectoryPoint::GetProcessSubType().
    void SetProcessType(int type, int subtype) {
        fProcessType = type + 1000*subtype;
    }

    /// Print the hit information.
    virtual void ls(Option_t * = "") const;

private:
    /// @{ The position of this trajectory point.
    float fPositionX;
    float fPositionY;
    float fPositionZ;
    float fPositionT;
    /// @}

    /// @{ The momentum of the particle at this trajectory point.
    float fMomentumX;
    float fMomentumY;
    float fMomentumZ;
    /// @}

    /// The ROOT geometry node id for this track step.  This can be used to
    /// identify which volume is being hit using the
    /// TGeomInfo::Get().VolumeName() method from the oaUtility library.
    Int_t fVolumeNode;

    /// The interaction process type associated with this trajectory point.
    /// The value is a combination of the process type and the process
    /// subtype [the type is (fProcessType%1000), the subtype is
    /// (fProcessType/1000)].  It's filled with G4ProcessType +
    /// 1000*G4ProcessSubType.
    UInt_t fProcessType;

    ClassDef(TG4TrajectoryPoint,6);
};
#endif
