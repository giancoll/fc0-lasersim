#ifndef TAlignmentId_hxx
#define TAlignmentId_hxx

#include "TSHAHashValue.hxx"

namespace ND {
    class TAlignmentId;
}

/// Identify a set of alignment constants so we can keep track of which
/// constants are applied to a particular geometry.
class ND::TAlignmentId : public ND::TSHAHashValue {
public:
    TAlignmentId();
    TAlignmentId(const TAlignmentId& hc);
    TAlignmentId(const TSHAHashValue& hc);
    TAlignmentId(unsigned int hc[5]);
    TAlignmentId(unsigned int hc0, 
                 unsigned int hc1, 
                 unsigned int hc2, 
                 unsigned int hc3, 
                 unsigned int hc4);
    virtual ~TAlignmentId();

    /// Get the documentation string.
    const std::string& GetDocumentString() const {return fDocString;}

    /// Set the documentation string.
    void SetDocumentString(const char* s) {fDocString = std::string(s);}

private:
    /// A documentation string to go with this alignment.
    std::string fDocString;
    
    ClassDef(TAlignmentId,1);
};
#endif
