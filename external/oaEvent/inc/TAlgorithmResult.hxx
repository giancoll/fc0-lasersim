#ifndef TAlgorithmResult_hxx_seen
#define TAlgorithmResult_hxx_seen

#include "TDataVector.hxx"
#include "THandle.hxx"
#include "TReconBase.hxx"
#include "TAlgorithmTag.hxx"
#include "EoaCore.hxx"

namespace ND {
    class TAlgorithmResult;
    class TAlgorithm;

    /// A base exception for any exception associated with an algorithm
    /// result.
    OA_EXCEPTION(EAlgorithmResult,EoaCore);

    /// An exception thrown when the user tries to add a track container with an
    /// illegal name.
    OA_EXCEPTION(EIllegalTrackContainerName,EAlgorithmResult);

    /// An exception thrown when the user tries to add a reconstruction object
    /// container with an illegal name.
    OA_EXCEPTION(EIllegalRecObjContainerName,EAlgorithmResult);

    /// An exception thrown when a user tries to add a hit selection with an
    /// illegal name.
    OA_EXCEPTION(EIllegalHitSelectionName,EAlgorithmResult);
}

/// A class to save results from a TAlgorithm object (this is the base class
/// to save the reconstruction results).  This contains information about the
/// TAlgorithm object that generated the results, as well as any user defined
/// results from running the algorithm.  The status of the algorithm is
/// returned by the TAlgorithmResult::GetStatus() method.  The default status
/// is empty.  One significant use of the status field is to specify if an
/// algorithm has flagged an event to be saved or rejected.  If the status
/// contains "rej", then the event should be rejected.  The status can also be
/// used to specify which cuts an event has passed.  Typically, an algorithm
/// should create a status string, and added information using the "+="
/// operator. 
class ND::TAlgorithmResult : public TDataVector {
public:
    TAlgorithmResult();
    explicit TAlgorithmResult(const TAlgorithm& algo);
    TAlgorithmResult(const char* name, const char* title);
    virtual ~TAlgorithmResult();
    
    /// A copy constructor to turn a THitSelection object into a
    /// TAlgorithmResult.  Through the magic of C++, this acts as a conversion
    /// operator from a THitSelection object and TAlgorithmResult, so any
    /// routine that would accept a TAlgorithmResult will also accept a
    /// THitSelection.  
    TAlgorithmResult(const THitSelection& hits);
    
    //@{
    /// Add a status string to the existing status.  The status string should
    /// be a single string without spaces, and it will be saved in the status
    /// string using a canonical format.  The format is best demonstrated by
    /// example:
    ///
    /// \code
    /// result->AddStatus("tpc");         // Save a flag
    /// result->AddStatus("good-track");  // Save another flag
    /// result->AddStatus("keep:true");   // Save a boolean value
    /// std::cout << result->GetStatus() << std::endl;
    /// \endcode
    ///
    /// Will print "(tpc) (good-track) (keep:true)".  Fields that are saved in
    /// the status can be verified with the following code idiom:
    ///
    /// \code
    /// if (result->GetStatus().find("(tpc)") != std::string::npos) {
    ///     // The TPC status field was found so do something...
    /// }
    /// \endcode
    /// 
    void AddStatus(const char* status);
    void AddStatus(const std::string& status);
    //@}
    
    //@{
    /// Set a status summary of the algorithm result by over-writing the
    /// existing status.
    void SetStatus(const char* status);
    void SetStatus(const std::string& status);
    //@}
    
    /// Get the status summary of the result.  This can be used to summarize
    /// any error conditions that the algorithm found, cuts that were applied,
    /// or any other general status information.  The status summary is
    /// returned as a std::string and the default value is empty.  One
    /// typical use of this is to determine if an event should be saved or
    /// rejected by an analysis.
    std::string GetStatus(void) const;
    
    /// Add a new reconstruction object container to the TAlgorithmResult.
    /// Note that the container CANNOT be named "results".  Also, the
    /// container will be owned by the TAlgorithmResult.  The most recently
    /// added TReconObjectContainer object will become the default set of
    /// results.
    void AddResultsContainer(TReconObjectContainer* container); 
    
    /// Get a reconstruction object container out of this TAlgorithmResult.
    THandle<ND::TReconObjectContainer> 
    GetResultsContainer(const char* track="results") const;
    
    /// Set the default reconstruction object container.
    void SetDefaultResultsContainer(const char* tracks);
    
    /// Add a new hit selection into the TAlgorithmResult.  Note that the
    /// hit selection CANNOT be named "hits".  Also, the hit selection
    /// will be owned by the TAlgorithmResult.  The most recently added
    /// THitSelection object will become the default set of hits.
    void AddHitSelection(THitSelection* hitselection); 
    
    /// Get a hit selection out of this TAlgorithmResult.
    ND::THandle<ND::THitSelection> 
    GetHitSelection(const char* hit="hits") const;
    
    /// Set the default set of hits.  
    void SetDefaultHitSelection(const char* hits);
    
    /// Return the algorithm tag associated with this result.  The tag will
    /// uniquely identify the algorithm, as well as provide information about
    /// how the algorithm was run, when it was run, &c.
    const TAlgorithmTag& GetAlgorithmTag() const {return fTag;}
    
    /// Print the result information.
    virtual void ls(Option_t *opt = "") const;
    
private:   
    friend class TAlgorithm;
    
    /// Set the algorithm tag for this result.
    void SetAlgorithmTag(const TAlgorithmTag& tag) {fTag = tag;}
    
    /// The algorithm tag associated with this result.
    TAlgorithmTag     fTag;
    
    /// A summary of the algorithm status.  This is typical used to save the
    /// "event disposition" about whether an event is selected by a set of
    /// cuts or not, but can also be used to flag error conditions from the
    /// algorithm
    std::string fStatusSummary;
    
    ClassDef(TAlgorithmResult,3);
};
#endif
