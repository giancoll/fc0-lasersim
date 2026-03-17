#ifndef TDigitContainer_hxx_seen
#define TDigitContainer_hxx_seen

#include <vector>
#include <TROOT.h>

#include "TDatum.hxx"
#include "TDigit.hxx"

namespace ND {
    class TDigit;
    class TDigitHeader;
    class TDigitContainer;
}

/// A container for TDigit and TDigitHeader objects.  The TDigitContainer
/// class holds a representation of the uncalibrated electronics data and is
/// generated from two sources.  For data, the TDigitContainer is dynamically
/// generated from the MIDAS information saved in TND280RawEvent.  For Monte
/// Carlo, the TDigitContainer is saved with the event (instead of saving a
/// TND280RawEvent).  The information inside of the TDigitContainer is usually
/// only accessed by the calibration routines, but can be accessed from the
/// final THitObjects using the TDigitProxy which references the contributing
/// TDigit.
class ND::TDigitContainer 
    : public ND::TDatum, public std::vector<ND::TDigit*> {
public:
    TDigitContainer(const char* name = "digits", 
                    const char* title = "Digit Pointers");
    ~TDigitContainer();

    /// Get a header out of the container.  These headers provide information
    /// needed to calibrate the digits in this container.
    ND::TDigitHeader * GetHeader(int i) const;

    /// Get the total number of headers in the container.
    unsigned int GetHeaderCount() const {return fHeaders.size();}

    /// Add a header to the container.
    void AddHeader(ND::TDigitHeader* header);

    /// Return the signature of this digit container.  The signature is used
    /// by the proxy to make sure that the correct TDigitContainer is being
    /// referenced.
    unsigned int GetSignature() const;

    /// Print the datum information.
    virtual void ls(Option_t* opt = "") const;

private:
    /// A vector of headers.
    std::vector<ND::TDigitHeader*> fHeaders;

    /// The signature of this container.
    mutable unsigned int fSignature; //! Do not save.

    ClassDef(TDigitContainer,1);
};
#endif
