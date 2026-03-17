#ifndef TDigitHeader_hxx_seen
#define TDigitHeader_hxx_seen

#include <string>

#include <TROOT.h>
#include <THandle.hxx>
#include <TDatum.hxx>
#include <TDataVector.hxx>

namespace ND {
    class TDigitHeader;
};

/// A base class for header information that is saved with the digits in a
/// TDigitContainer.  The TDigitHeader only has validity within the
/// TDigitContainer that owns it.  This class is for saving header information
/// required to calibrate the TDigit objects in the TDigitContainer.
class ND::TDigitHeader {
public:
    TDigitHeader();
    virtual ~TDigitHeader();

    /// Create a TDigitHeader with an explicit name.
    explicit TDigitHeader(const std::string& name);

    /// Get the name of the digit header.
    const std::string& GetName() const {return fName;}

    /// Set the name of the digit header.
    void SetName(const std::string& name) {fName = name;}

    /// Get the 
    /// Get the first digit for which this header information is valid.  This
    /// can be used to loop over the digits corresponding to this header
    /// information.
    ///
    /// \code
    /// TDigitContainer* container = ...
    /// TDigitHeader* hdr = ...
    /// for (unsigned d = hdr->GetBeginValid(); 
    ///      d < hdr->GetEndValid(); 
    ///      ++d) {
    ///    TDigit* digit = container->at(d);
    ///    ... do stuff ...
    /// }
    /// \endcode
    unsigned int GetBeginValid() const {return fBeginValid;}

    /// Set the beginning of the valid range.
    void SetBeginValid(unsigned int i) {fBeginValid = i;}
    
    /// Get the end of the range for which this header information is valid.
    /// See the documentation for GetBeginValid().
    unsigned int GetEndValid() const {return fEndValid;}

    /// Set the end of the valid range.  This should be one past the last
    /// valid digit.
    void SetEndValid(unsigned int i) {fEndValid = i;}

    /// A default way to add a TDatum.  Derived classes will usually
    /// provide a specialized insertion method that is optimized for
    /// the class, but this provides a default interface that can be
    /// used in most circumstances.
    virtual void AddDatum(ND::TDatum* val, const char* name=NULL);

    /// A default way to add a THandle<TDatum>.  This takes over memory
    /// management of the handle and will throw the EBadInsertion
    /// exception if the insertion fails.
    virtual void AddDatum(ND::THandle<ND::TDatum> handle,
                          const char* name=NULL);
    
    template <class T> ND::THandle<T> Get(const char* name=".") const {
        if (!fDataVector) return ND::THandle<T>(NULL);
        return fDataVector->Get<T>(name);
    }

    /// Print the headera information.
    virtual void ls(Option_t* opt = "") const;

private:
    /// The name of the digit header.
    std::string fName;

    /// The index of the first digit for which this header information is valid.
    unsigned int fBeginValid;

    /// The index one past the last digit for which this header information is
    /// valid.
    unsigned fEndValid;

    ND::TDataVector* fDataVector;

    ClassDef(TDigitHeader, 1);
};
#endif
