#ifndef TMidasBankProxy_hxx_seen
#define TMidasBankProxy_hxx_seen

#include <list>
#include <string>

#include <Rtypes.h>

#define TMIDAS_BANK_TITLE "MIDAS Bank"

namespace ND {
    class TMidasBank;
    class TMidasBankProxy;
};


/// This is the base class for a family of TXxxBankProxy classes each
/// of which has the capability to create on demand its corresponding
/// TXxxBank, where Xxx denotes the name of a raw (DAQ) bank.  These
/// classes form the basis a system that can reinterpret a homogeneous
/// set of type neutral TMidasBank objects as heterogeneous set of
/// TXxxBank objects capable of providing access to the data the
/// banks contain.  For more detail see the TMidasBankProxyRegistry
/// class.

class ND::TMidasBankProxy {


public:

    explicit TMidasBankProxy(const std::string name = "TMidasBankProxy",
			     const std::string masks = "none");
    virtual ~TMidasBankProxy();

    /// Return, as a semi-colon separated list, the bank masks used to map
    /// bank names to the TXxxBank class that holds the banks data.
    /// If index supplied return just the mask at that index or "" if none
    std::string GetMidasBankMasks(Int_t index = -1) const;

    /// Add another mask to the proxy.
    void AddMask(const std::string mask) { fMidasBankMasks.push_back(mask); }

    /// Return the class name that this proxy creates.
    std::string GetName() const { return fName; }

    /// Return true if the supplied MIDAS bank name matches this proxy's bank
    /// mask.
    Bool_t MatchesMidasBankName(const std::string name) const;

    /// Create a TXxxBank object of the class this proxy represents.  The
    /// caller owns the returned object.
    virtual ND::TMidasBank* CreateMidasBank(const ULong64_t *bank = NULL,
					    const char* title = TMIDAS_BANK_TITLE) const;
private:

    /// The list of MIDAS bank masks for the family of bank names that
    /// map to the class the proxy creates.  A individual bank mask is
    /// similar to a bank name except that the character '*' is a
    /// wildcard and matches any character.
    std::list<std::string> fMidasBankMasks;

    /// The name of the TXxxBank
    std::string fName;

    ClassDef(TMidasBankProxy,1)

};

#endif
