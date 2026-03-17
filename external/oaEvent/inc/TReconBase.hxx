#ifndef TReconBase_hxx_seen
#define TReconBase_hxx_seen

#include <TObject.h>

#include "EoaCore.hxx"
#include "TND280Log.hxx"
#include "THandle.hxx"
#include "TDatum.hxx"
#include "TDataVector.hxx"
#include "THitSelection.hxx"
#include "TReconState.hxx"

namespace ND {
    class TReconBase;
    class TReconObjectContainer;
    class TReconNodeContainer;

    /// An exception associated with a reconstruction object.
    OA_EXCEPTION(EReconObject, EoaCore);

    /// An exception thrown when a requested field is not supported by the
    /// current object state.
    OA_EXCEPTION(EMissingField, EReconObject);
}

/// Define a base class for all of the reconstruction objects.  This contains
/// the shared methods required to implement the basic reconstruction object
/// functionality.  Each TReconBase object has it's TObject::fUniqueID value
/// set.  These values can be used to associate global reconstruction objects
/// to sub-detector reconstruction objects.
class ND::TReconBase : public TDataVector {
public:
    /// The bits defining the state of the reconstruction that created this
    /// object.  The state bits come in two flavors: Status bits are used to
    /// summarize the completion status of the algorithm.  Detector bits are
    /// used to record which detectors provided information to the algorithm.
    /// The status bits are accessed using SetStatus() and GetStatus().  The
    /// detector bits are accessed using AddDetector() and UsesDetector().
    /// The full status field can be accessed using the GetStatus() method.
    typedef enum {
        /// The algorithm ran to completion.  This bit is set using
        /// SetStatus() and checked using CheckStatus()
        kRan            = BIT(0),

        /// The algorithm succeeded and the result is reliable.  This bit is
        /// set using SetStatus and checked using CheckStatus().
        kSuccess        = BIT(1),

        /// A chi2 fitting method was used and the quality corresponds to a
        /// chi2.  This bit is set using SetStatus and checked using
        /// CheckStatus().
        kChi2Fit        = BIT(2),

        /// A maximum likelihood fitting methods was used.  This bit is set
        /// using SetStatus and checked using CheckStatus().
        kLikelihoodFit  = BIT(3),

        /// A kalman fitting algorithm was used.  This bit is set using
        /// SetStatus and checked using CheckStatus().
        kKalmanFit      = BIT(4),

        /// A mask of all of the status bits.
        kStatusMask     = kRan|kSuccess|kChi2Fit|kLikelihoodFit|kKalmanFit,

        /////////////////////////////////////////////////////////////////
        // The remaining bits define which detectors contribute information
        // to the reconstruction object.
        /////////////////////////////////////////////////////////////////

        /// The result uses data from the Time-Of-Flight detector.  This bit
        /// is set using AddDetector() and checked using UsesDetector().
        kTOF            = BIT(8),

        /// The result uses data from the Bottom HAT detector.  This bit is set
        /// using AddDetector() and checked using UsesDetector().
        kBottomHAT      = BIT(9),

        /// The result uses data from the Top HAT detector.  This bit is set
        /// using AddDetector() and checked using UsesDetector().
        kTopHAT         = BIT(10),

        /// A mask to check if the fit used data from the HAT.  This is
        /// checked using UsesDetector().
        kHAT            = kBottomHAT|kTopHAT,

        /// The result uses data from the SuperFGD.  This bit is set
        /// using AddDetector() and checked using UsesDetector().
        kSFG            = BIT(11),

        /// The result uses data from the bottom of the SMRD.  This bit is set
        /// using AddDetector() and checked using UsesDetector().
        kBottomSMRD     = BIT(12),

        /// The result uses data from the left side of the SMRD.  This bit is
        /// set using AddDetector() and checked using UsesDetector().
        kLeftSMRD       = BIT(13),

        /// The result uses data from the top of the SMRD.  This bit is set
        /// using AddDetector() and checked using UsesDetector().
        kTopSMRD        = BIT(14),

        /// The result uses data from the right side of the SMRD.  This bit is
        /// set using AddDetector() and checked using UsesDetector().
        kRightSMRD      = BIT(15),

        /// A mask to check if the fit used data from the SMRD.  This is
        /// checked using UsesDetector().
	    kSMRD           = kRightSMRD|kTopSMRD|kLeftSMRD|kBottomSMRD,

        /// The result uses data from the bottom of the P0D-ECal.  This bit is
        /// set using AddDetector() and checked using UsesDetector().
        kBottomPECal    = BIT(16),

        /// The result uses data from the left side of the P0D-ECal.  This bit
        /// is set using AddDetector() and checked using UsesDetector().
        kLeftPECal      = BIT(17),

        /// The result uses data from the top of the P0D-ECal.  This bit is
        /// set using AddDetector() and checked using UsesDetector().
        kTopPECal       = BIT(18),

        /// The result uses data from the right side of the P0D-ECal.  This
        /// bit is set using AddDetector() and checked using UsesDetector().
        kRightPECal     = BIT(19),

        /// A mask to check if the result used data from the P0D-ECal.  This
        /// mask is checked using UsesDetector().
	    kPECal          = kRightPECal|kTopPECal|kLeftPECal|kBottomPECal,

        /// The result uses data from the bottom of the Tracker ECal.  This
        /// bit is set using AddDetector() and checked using UsesDetector().
        kBottomTECal    = BIT(20),

        /// The result uses data from the left side of the Tracker ECal.  This
        /// bit is set using AddDetector() and checked using UsesDetector().
        kLeftTECal      = BIT(21),

        /// The result uses data from the top of the Tracker ECal.  This bit
        /// is set using AddDetector() and checked using UsesDetector().
        kTopTECal       = BIT(23),

        /// The result uses data from the right side of the Tracker ECal.
        /// This bit is set using AddDetector() and checked using
        /// UsesDetector().
        kRightTECal     = BIT(24),

        /// A mask to check if the result used data from the Tracker ECal.
        /// This mask is checked using UsesDetector().
	    kTECal          = kRightTECal|kTopTECal|kLeftTECal|kBottomTECal,

        /// A mask to check if the result used data from the ECal.  This mask
        /// is checked using UsesDetector.
	    kECal          = kPECal|kTECal,

        /// The result uses data from the downstream ECal.  This bit is set
        /// using AddDetector() and checked using UsesDetector().
        kDSECal         = BIT(25),

        /// The result uses data from the downstream FGD w/ water target.
        /// This bit is set using AddDetector() and checked using
        /// UsesDetector().
        kFGD2           = BIT(26),

        /// The result uses data from the upstream FGD.  This bit is set using
        /// AddDetector() and checked using UsesDetector().
        kFGD1           = BIT(27),

        /// A mask to check if the result includes data from the FGD.  This
        /// mask is checked using UsesDetector().
	    kFGD            = kFGD1|kFGD2,

        /// The result uses data from the downstream TPC.  This bit is set
        /// using AddDetector() and checked using UsesDetector().
        kTPC3           = BIT(28),

        /// The result uses data from the central TPC.  This bit is set using
        /// AddDetector() and checked using UsesDetector().
        kTPC2           = BIT(29),

        /// The result uses data from the upstream TPC.  This bit is set using
        /// AddDetector() and checked using UsesDetector().
        kTPC1           = BIT(30),

        /// A mask to check if the result includes data from the TPC.  This
        /// mask is checked using UsesDetector().
	    kTPC            = kTPC1|kTPC2|kTPC3,

        /// A mask to check if the result includes data from the Tracker.
        /// This mask is checked using UsesDetector().
        kTracker        = kFGD|kTPC|kDSECal|kTECal,

        /// The result uses data from the P0D.  This bit is set using
        /// AddDetector() and checked using UsesDetector().
        kP0D            = BIT(31),

        /// A mask for all of the detector bits.
        kDetectorMask = kTOF|kHAT|kSFG|kSMRD|kPECal|kTracker|kP0D
    } StateBits;

    /// A status value with one or more bits sets
    typedef unsigned long Status;

public:
    virtual ~TReconBase();

    /// Get the name of the algorithm that created this reconstruction object.
    std::string GetAlgorithmName() const {return fAlgorithm;}

    /// Set the name of the algorithm.
    void SetAlgorithmName(const char* name) {fAlgorithm = name;}

    /// Check the status of the reconstruction that created this
    /// reconstruction result.  This method checks if any of the bit
    bool CheckStatus(ND::TReconBase::Status status) const;

    /// Set the status of the algorithm that generated this object.
    void SetStatus(ND::TReconBase::Status status);

    /// Clear the status of the algorithm that generated this object.
    void ClearStatus(ND::TReconBase::Status status);

    /// Get the status field for this object.
    ND::TReconBase::Status GetStatus() const;

    /// Get the detector bits that are set for this object.
    ND::TReconBase::Status GetDetectors() const;

    /// Return true if the object uses a particular detector
    bool UsesDetector(ND::TReconBase::Status detector) const;

    /// Add one or more detector to the object.
    void AddDetector(ND::TReconBase::Status detector);

    /// Remove a detector from the object.
    void RemoveDetector(ND::TReconBase::Status detector);

    /// Return the goodness of fit for the reconstruction.
    double GetQuality() const {return fQuality;}

    /// Set the goodness of the fit from the reconstruction.
    void SetQuality(double quality) {fQuality = quality;}

    /// Get the number of degrees of freedom in the reconstruction.
    int GetNDOF() const {return fNDOF;}

    /// Set the number of degrees of freedom.
    void SetNDOF(int n) {fNDOF = n;}

    /// Get the state associated with this reconstruction object.  The state
    /// is created by the derived class.  This should generally be used in
    /// user code with a specific ND::THandle type.  For instance, if you want
    /// to access a ND::TReconCluster state, you should write code like:
    ///
    /// \code
    /// ND::THandle<ND::TClusterState> state = recObj->GetState();
    /// if (state) {
    ///    // the state
    /// }
    /// \endcode
    ///
    /// Which hides the dynamic cast and makes sure that the local variable
    /// has the right type.
    ///
    /// The state holds (most of) the reconstruction object parameter values.
    /// And is used to get and set the values and covariances.  As an example,
    /// to set the direction of the TReconTrack object:
    /// \code
    /// ND::THandle<ND::TTrackState> trackState = reconTrack->GetState();
    /// TVector3 theDir(0,0,1);
    /// trackState->SetDirection(theDir); // This works
    /// trackState->SetDirection(0,0,1);  // This also works.
    /// // Assume dirCov is a TMatrixD holding the covariance.
    /// for (int i=0; i< TMDirectionState::GetSize(); ++i) {
    ///     for (int j=0; j< TMDirectionState::GetSize(); ++j) {
    ///         trackState->SetCovariance(i+trackState->GetDirectionIndex(),
    ///                                   j+trackState->GetDirectionIndex(),
    ///                                   dirCov(i,j));
    ///     }
    /// }
    /// \endcode
    ND::THandle<ND::TReconState> GetState() const {
        if (!fState) {
            ND280Error("TReconBase with NULL State: "
                       << "State must be created in the derived class.");
            throw EReconObject();
        }
        fState->check();
        return ND::THandle<ND::TReconState>(fState,false);
    }

    /// Get the parameter values at each stage of the reconstruction.
    /// - For the track and particle objects (see the \ref reconTrack and \ref
    ///   reconPID sections), the parameter values will be the associated
    ///   with each intermediate position.
    /// - For an incremental fit, the parameter values are associated with each
    ///   stage of the fit.
    /// - For other objects, the parameter values will be associated with the
    ///   reconstruction objects that have been added to the fit.
    const ND::TReconNodeContainer& GetNodes() const {
        if (!fNodes) {
            ND280Error("TReconBase without a TReconNodeContainer:"
                       << "Must be created in derived class constructor");
            throw EReconObject();
        }
        return *fNodes;
    }

    /// Provide a non-constant version of the node container so that nodes can
    /// be added.
    ND::TReconNodeContainer& GetNodes() {
        if (!fNodes) {
            ND280Error("TReconBase without a TReconNodeContainer:"
                       << "Must be created in derived class constructor");
            throw EReconObject();
        }
        return *fNodes;
    }

    /// Provide the hits associated with this recon object.  The hits are
    /// saved as an entry in the data vector (with name "hits"), and may not
    /// be present for all objects.
    ND::THandle<ND::THitSelection> GetHits() const;

    /// Add a hit selection to the recon object.  The recon object takes
    /// ownership of the hit (as per the ND280 pointer ownership policy), so
    /// you must not add a hit selection currently owned by another object.
    /// The name of the hit selection will be changed to "hits".
    void AddHits(ND::THitSelection* hits);

    /// Provide the reconstruction objects used in the algorithm that created
    /// this object.  The consituients are used to help understand how the
    /// object was fit, and are different from the nodes which represent
    /// actual intermediate steps of the fitting process.
    ND::THandle<ND::TReconObjectContainer> GetConstituents() const;

    /// Add a new constituent to the TReconObject.  This stores the
    /// constituents in the order in which they are added.  The constituents
    /// are saved in a TReconObjectContainer, and are available using the
    /// GetConstituents method, or using the normal TDatum::Get<> method.
    /// This will create a constituents sub-object if one doesn't exist.
    void AddConstituent(ND::THandle<ND::TReconBase> obj);

    /// Add several constituents in one go by adding all objects in a
    /// TReconObjectContainer
    void AddConstituents(ND::THandle<ND::TReconObjectContainer> objs);

    /// @{Override methods in the base TObject class.
    virtual void ls(Option_t* opt = "") const;
    virtual void Browse(TBrowser* b);
    /// @}

    /// Turn the object status into a string.
    std::string ConvertStatus() const;

    /// Turn the contributing detectors into a string.
    std::string ConvertDetector() const;

    /// Return true if this is a composite object.  The definition of a
    /// composite object means that different parts of the object might be
    /// considered as the "root".  This allows the implementation of
    /// "junctions" and other more complex concepts.
    bool IsComposite() const;

    /// Mark this as a composite object.
    void SetComposite(bool comp = true);

protected:
    /// Default constructor.
    TReconBase();

    /// Construct a named reconstruction object.  This constructs a new empty
    /// TReconBase object and is defined as explicit so that it will not
    /// function as a type conversion between a const char* and the TReconBase
    /// class.  This can only be used in one of the derived classes.
    explicit TReconBase(const char* name,
                        const char* title = "Reconstruction Object");

    /// Copy the object (this is not a deep copy).
    TReconBase(const ND::TReconBase& object);

    /// Implement the listing of the base class fields.  This is used by ls()
    void ls_base(Option_t* opt) const;

    /// The quality of reconstruction;
    float fQuality;

    /// The state for this object.
    ND::TReconState* fState;

    /// The nodes for the object.
    ND::TReconNodeContainer* fNodes;

    /// The status of the reconstruction
    ND::TReconBase::Status fStatus;

    /// The number of degrees of freedom.
    int fNDOF;

    /// The name of the reconstruction algorithm.
    std::string fAlgorithm;

    /// Define the flag bits used by the TReconBase object.  These can't
    /// collide with any state bits defined in TObject, TDatum, or TDataVector
    /// (the parent classes), and none of the children can define a state bit
    /// that collides with these definitions.  Bits 14 to 23 are available for
    /// use.
    enum EReconBaseBits {
        kCompositeObject = BIT(18)  // The object is composite.
    };

    ClassDef(TReconBase,3);
};

/// A container class for reconstruction objects inheriting from a std::vector
/// of THandle<TReconBase>.
class ND::TReconObjectContainer
    : public TDatum, public std::vector< ND::THandle<ND::TReconBase> > {
public:
    TReconObjectContainer();
    TReconObjectContainer(const char* name,
                          const char* title = "Recon Object Container");
    virtual ~TReconObjectContainer();

    virtual void push_back(ND::THandle<ND::TReconBase> data);

    /// @{Override methods in the base TObject class.
    virtual void ls(Option_t* opt = "") const;
    virtual Bool_t IsFolder(void) const {return kTRUE;};
    virtual void Browse(TBrowser* b);
    /// @}

private:
    ClassDef(TReconObjectContainer,1);
};
#endif
