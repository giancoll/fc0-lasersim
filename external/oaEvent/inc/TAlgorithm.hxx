#ifndef TAlgorithm_hxx_seen
#define TAlgorithm_hxx_seen

#include <string>
#include <TTask.h>
#include <TGeoManager.h>

#include "TND280Event.hxx"
#include "THitSelection.hxx"
#include "TAlgorithmTag.hxx"
#include "TAlgorithmResult.hxx"
#include "TEventFolder.hxx"
#include "EoaCore.hxx"

namespace ND {
    class TAlgorithm;

    /// An exception thrown if an algorithm expecting an event is run without
    /// a current event available.
    OA_EXCEPTION(EAlgorithmWithoutEvent,EoaCore);
}

/// Base class for reconstruction algorithms.  It contains the basic
/// information about the "flavour" and version.  Specific algorithms should
/// inherit the class and returned results using a TAlgorithmResult object.  
/// Inherited classes must implement the
///
/// \code
/// THandle<TAlgorithmResult> Process(const TAlgorithmResult& input)
/// \endcode
///
/// which is a pure virtual member of this class.  The user should use
/// TAlgorithm::CreateResult() to construct an empty result that is returned
/// by the Process method.  This convenience function will set the algorithm
/// result name, and fill the book keeping information.
class ND::TAlgorithm : public TNamed {
public:
    TAlgorithm(const char* name, const char* title="T2K Algorithm");
    virtual ~TAlgorithm();
    
    /// This method returns information about the compiled version 
    virtual const std::string& GetVersion() const {return fVersion;}
    
    typedef enum {Quiet, Brief, Summary, Detailed} Verbosity;
    
    /// This string contains a description of the algorithm depending on the 
    /// "verbosity" fVerbose :
    ///   - 0 (Quiet)   : quiet
    ///   - 1 (Brief)   : one quick line to describe the algorithm
    ///   - 2 (Summary) : a summary of the algorithm features
    ///   - 3 (Detailed): a more detailed description (encouraged for
    ///                   non-trivial algorithms) 
    virtual std::string GetDescription() const;
    
    /// Set the brief description of this algorithm.
    void SetBriefDescription(const char* d) {
        fBriefDescription = d;
    }
    
    /// Set the summary description of this algorithm.
    void SetSummaryDescription(const char* d) {
        fSummaryDescription = d;
    }

    /// Set the detailed description of this algorithm.
    void SetDetailedDescription(const char* d) {
        fDetailedDescription = d;
    }

    /// Set verbosity level 
    virtual void SetVerbose(Verbosity verb) {fVerbose = verb;}

    /// Set the output state of the algorithm.
    /// 
    /// \todo This function should be transfered into a more general logging
    /// class.  I wish some experiment someplace would make one generally
    /// available.
    void SetQuiet(Bool_t state) {fQuiet = state;}

    /// A convenience method that doesn't require input and which MUST NOT be
    /// implemented by the derived class.
    THandle<ND::TAlgorithmResult> Execute(void);

    /// The routine that does the actual work.  This must be implemented by
    /// any derived class.
    virtual THandle<ND::TAlgorithmResult> 
    Process(const ND::TAlgorithmResult& input) = 0;

    /// A convenience routine that will try to find a result from a previously
    /// run version of this algorithm.  If the result is not found, then the
    /// standard process methods is run, and the result is saved into the
    /// event.
    THandle<ND::TAlgorithmResult> MaybeProcess(const TAlgorithmResult& input);

    /// Get the output state of the algorithm.
    bool GetQuiet(void) const {return fQuiet;}

    /// Get the algorithm tag.
    TAlgorithmTag GetTag() const;

    /// Reset algorithm containers, but keep do not change the
    /// parameters by default
    virtual void Clear(Option_t* option);

    /// Don't explicitly override the derived Execute method.
    void Execute(const char *method,  const char *params, Int_t *error=0) {
        TNamed::Execute(method,params,error);
    }

    /// Don't explicitly override the derived Execute method.
    void Execute(TMethod *method, TObjArray *params, Int_t *error=0) {
        TNamed::Execute(method,params,error);
    }

protected:
    /// Set the version of the algorithm.  This should be set in the derived
    /// classes constructor.
    void SetVersion(const char* v);

    /// A convenient way to get the event being processed by the algorithm.
    /// The returned event is only valid during a single call to Process so
    /// the pointer can not be saved in a field.
    ND::TND280Event& GetEvent() const;
    
    /// A convenient way to get the correct geometry for the current event.
    /// If there isn't a valid geometry available, this will return NULL.  As
    /// a side effect, the gGeoManager global variable is set.
    TGeoManager* GetGeom() const;
  
    /// A convenient way to create an empty algorithm result to be returned by
    /// a derived class.
    THandle<ND::TAlgorithmResult> CreateResult() {
        return THandle<TAlgorithmResult>(new TAlgorithmResult(*this));
    }

private:
    /// A version string that must be set in the constructor with SetVersion().
    std::string fVersion;

    /// The verbosity of the description and output. 
    Verbosity fVerbose;

    /// A flag to switch on/off output
    Bool_t fQuiet;

    /// One quick line to describe the algorithm returned when the verbosity
    /// is TAlgorithm::Brief.
    std::string fBriefDescription;
    
    /// A short summary of the algorithm returned when the verbosity
    /// is TAlgorithm::Summary.
    std::string fSummaryDescription;
    
    /// A detailed description of the algorithm returned when the verbosity is
    /// TAlgorithm::Detailed.
    std::string fDetailedDescription;
    
    ClassDef(TAlgorithm, 4);
};
#endif
