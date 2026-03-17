#ifndef TMidasBankProxyRegistry_hxx_seen
#define TMidasBankProxyRegistry_hxx_seen

#include <list>

#include <Rtypes.h>

#include "TMidasBankProxy.hxx"

namespace ND {
    class TMidasBankProxyRegistry;
};


/// ACKNOWLEDGEMENTS: The TMidasBankProxyRegistry and the
///                   TMidasBankProxy objects it
///                   registers are based on the MINOS classes 
///                   RawBlockRegistry and RawBlockProxy.
/// 

/// This singleton class holds a registry of proxies that are
/// subclassed from TMidasBankProxy and that between them are capable
/// of creating all currently active raw (DAQ) TXxxBank classes. All
/// MIDAS banks have 4 character names and for every MIDAS bank name
/// there is a corresponding TXxxBank but the mapping is not 1:1.
/// Instead a family of bank names that match the bank mask map to a
/// single class.  A bank mask is constructed by specifying the
/// characters in at least 2 locations in the name and leaving the
/// others as '_' which denotes a wildcard.

/// 
///   So the bank mask E_D_ would match the bank name e.g. E3D0
/// 
/// The system for creating the TXxxBankProxy class code and
/// registering objects of each class with this registry requires that the
/// the implementation file of every TXxxBank class invokes the macro
/// REGISTER_MIDAS_BANK and supplies the raw data class and the family mask
/// for example
/// 
///   REGISTER_MIDAS_BANK(TRawEcalBlock,E_D_)
/// 
/// This macro:-
///<OL> 
///<LI> Generates the code for class TXxxBankProxy.  
///     On instantiation objects of the class register themselves with
///     this registry.
/// 
///<LI> Generates a single file static object of the TXxxBankProxy
///     class.
///</OL> 
/// Creation of a static objects exploits the fact that all such static
/// objects are created by the loader before running the main program. The
/// net effect is that by simply including the macro in every TXxxBank
/// implementation will define the associated proxy class, create a single
/// instance of that class and register it with the central
/// TMidasBankRegistry where it can then be used to create appropriate
/// TXxxBank as required.


class ND::TMidasBankProxyRegistry {


public:

    virtual ~TMidasBankProxyRegistry();

    /// Access to the singleton registry.
    static ND::TMidasBankProxyRegistry& Instance();

    /// Register a new proxy
    void Register(TMidasBankProxy *midasBankProxy);

    /// Return the number of proxies currently being managed
    Int_t GetNumProxies() const { return fMidasBankProxyTable.size(); }

    /// Return the proxy (or NULL if none) that matches the supplied MIDAS bank name
    ND::TMidasBankProxy *LookUp(const std::string midasBankName) const;

    /// Display the contents of the registry
    void Print();

private:

    /// Singleton: Private ctor.
    TMidasBankProxyRegistry();
    TMidasBankProxyRegistry(const TMidasBankProxyRegistry&);
    TMidasBankProxyRegistry& operator=(const TMidasBankProxyRegistry&);

    /// Singleton: pointer to instance.
    static TMidasBankProxyRegistry  *fInstance;

    /// The registry of proxies
    std::list<TMidasBankProxy*> fMidasBankProxyTable;


    ClassDef(TMidasBankProxyRegistry,1)

};

////////////////////////////////////////////////////////////////////////
//
// Define a macro to register banks
//
////////////////////////////////////////////////////////////////////////
#define REGISTER_MIDAS_BANK(CLASS, MASK)                                         \
static class CLASS##Proxy : public ND::TMidasBankProxy {                         \
public:                                                                          \
    CLASS##Proxy() :                                                             \
        ND::TMidasBankProxy(#CLASS,#MASK) {				         \
	ND::TMidasBankProxyRegistry::Instance().Register(this);	                 \
    }								    	         \
    ND::TMidasBank *CreateMidasBank(const ULong64_t *bank = NULL,                \
                                          const char* title = #CLASS) const {    \
	ND::TMidasBank *rdb = new ND::CLASS(bank,title);		         \
        return rdb;                                                              \
    }									         \
} gs_##CLASS##Proxy_obj;


#endif
