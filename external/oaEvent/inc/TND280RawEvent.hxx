#ifndef TND280RawEvent_hxx_seen
#define TND280RawEvent_hxx_seen

#include <string>
#include <typeinfo>

#include <Rtypes.h>
#include <TClass.h>

#include "TDataVector.hxx"
#include "THandle.hxx"
#include "TMidasBank.hxx"
#include "TRawDataHeader.hxx"
#include "Demangle.hxx"

#define TND280_RAW_EVENT_TITLE "Raw Event"

namespace ND {
    class TMidasEventParser;
    class TND280RawEvent;
}


/// The TND280RawData class is the I/O container class for all DAQ
/// banks.  It can only hold objects that are, or inherit from,
/// TMidasBank.  This is ensured by the AdoptMidasBank method.
/// 
/// All banks from the DAQ are initially encapsulated within generic
/// TMidasBank objects.  In this form they only give access to generic
/// information for example the name of the MIDAS bank they came from,
/// the size of the data and raw uninterpreted data itself.  This
/// class makes use of a proxy registration system
/// TMidasBankProxyRegistry (q.v.) which allows it to "promote" these
/// generic data objects into ones that give full interpreted access
/// to the data.

class ND::TND280RawEvent : public TDataVector {

public:

    typedef TDataVector::iterator iterator;
    typedef TDataVector::const_iterator const_iterator;

    TND280RawEvent();
    explicit TND280RawEvent(const char* name, 
			    const TRawDataHeader& header = TRawDataHeader(),
                            const char* title = TND280_RAW_EVENT_TITLE);

/// This constructor is used by TMidasFile to create a TND280RawEvent from
/// file.  Typical code to create a TMidasFile, connect it to a file, and
/// then loop sequentially returning TND280RawEvents is as follows:-
/// 
/// \code
///   ND::TMidasFile mf;
///   mf.Open("/home/west/test_data/run01854.mid");
///   while ( ND::TND280RawEvent* re = mf.ReadRawEvent() ) {
///     // Process event and then discard it.
///     re->Print();
///     delete re;
///   }
/// \endcode
///
/// This is also indirectly used by nd280EventLoop (through the
/// TVInputFile::NextEvent interface) to create a TND280Event.  The NextEvent
/// method calls ReadRawEvent() to generate a TND280RawEvent object using this
/// constructor, and the raw event is then added to a new TND280Event.
    TND280RawEvent(const ND::TMidasEventParser& mep,
		   const char* name,
		   const char* title = TND280_RAW_EVENT_TITLE);
    virtual ~TND280RawEvent();

    Int_t GetEventSize() const { return fEventSize; }
    const TRawDataHeader& GetHeader() const { return fHeader; }

    /// Fill the header using the oaRawEvent context setters.  If oaRawEvent
    /// is not available, this will return false.
    bool FillHeader();

    /// Print contents to ND280Log
    void Print(const Option_t* opt = "") const;


/// This member method template handles type conversions and helps to
/// insure type safety. It is modelled on the
///
/// \code 
///   template <class T> ND::THandle<T> Get(const char* name=".") const
/// \endcode 
/// 
/// but that method can neither be used nor overridden as the semantics
/// are different in several regards:-
///
///    -# The user may wish to access a bank either by MIDAS bank name
///    or by object type, i.e. class (and recall that multiple bank names can
///    map to a single class).
/// 
///    In order to address this the second argument is
/// 
/// \code
///Bool_t byType
/// \endcode 
/// 
///   - if true, name is assumed to be a class name
///   - if false, name is assumed to be a MIDAS bank name
///  
/// 
/// - There is a degeneracy for example there can be multiple banks of
///    the same type and access has to be provided to them all.
/// 
///    In order to address this the third argument is
/// 
/// \code
///THandle<T> startAfter
/// \endcode 
/// 
///   - if NULL, pointer start fresh search
///   - if non-NULL, handle resume search after handle
///  
/// 
/// - Searching is constrained to the local container of banks,
///    hierarchical searching following a Unix style directory structure is
///    not supported or required.
/// 
/// There are some "convenience" interfaces to this function so that the
/// user never has to explicitly supply the default value for any
/// argument.
/// 
/// Use of this method may trigger internal promotion of TMidasBank
/// objects to their true class.  This should be transparent (note this
/// method claims to be constant, and logically is).  For more details see
/// the TMidasBankProxyRegistry.
/// 
/// If the name is not found in the event, then this returns a NULL
/// pointer. If name is found but cannot be converted to the request
/// class, EBadConversion is thrown 
/// 
/// A typical usage might look something like this:
/// 
/// \code
///  myFunc(TND280RawEvent& revt) {
///     // Get first TDemo1Bank object
///     THandle<TDemo1Bank> h
///           = revt.GetMidasBank<TDemo1Bank>("ND::TDemo1Bank");
///     while ( h ) {
/// 	    std::cout << "Found " << h->GetName() << std::endl;
///         // Get next TDemo1Bank object
/// 	    h = revt.GetMidasBank<TDemo1Bank>("ND::TDemo1Bank",h);
///     }
///  }
/// \endcode

    template <class T> ND::THandle<T> GetMidasBank(const char* name,
						   Bool_t byType,
						   THandle<T> startAfter) const {

        // Make sure all the data is promoted unless accessing at the generic level.
	// This should allow generic access without generating warnings when the
	// oaRawEvent library isn't present.
	if (typeid(T) != typeid(ND::TMidasBank)) this->PromoteMidasBanks();

        const_iterator itr(this->TDataVector::begin()),
    	               itrEnd(this->TDataVector::end());

        // Skip forward if requested
	if ( startAfter ) {
	    TMidasBank* start_obj = dynamic_cast<TMidasBank*>(&(*startAfter));
	    for (; itr != itrEnd; ++itr) {
		if ( start_obj == *itr ) {
		    ++itr;
		    break;
		}
	    }
	}

        // Scan the container
        for (; itr != itrEnd; ++itr) {
	    TMidasBank* mb = dynamic_cast<TMidasBank*>(*itr);
	    if ( ! mb ) continue; // This should not happen - all elements should be TMidasBank*
        std::string mbTypeName_demangled=Tools::Demangle(typeid(*mb));
        const char* mb_name = byType ? mbTypeName_demangled.c_str()  : mb->GetName();
	    if ( strlen(name) == 0 || strcmp(name,mb_name) == 0 ) {
		T* t = dynamic_cast<T*>(mb);
		// Don't warn about conversion failures if accepting every bank name.
		if ( strlen(name) == 0 && !t ) continue;
		if (!t) {
		    ND280Warn(" Bank: " << mb->GetName() 
                              << " -- Cannot convert from"
			      << " from \"TMidasBank*\""
                              << " to \"" << typeid(T).name() << "\"");
		    throw EBadConversion();
		}
		return THandle<T>(t,false);
	    }
	}
	return THandle<T>(NULL);
    }

    /// @{ Convenience functions so that default values never need to be
    /// explicitly supplied.  See full GetMidasBank documentation for
    /// definitions.
    template <class T> ND::THandle<T> 
    GetMidasBank(const char* name, Bool_t byType = false) const {
	return GetMidasBank<T>(name,byType,THandle<T>());
    }

    template <class T> ND::THandle<T> 
    GetMidasBank(const char* name, THandle<T> startAfter) const {
	return GetMidasBank<T>(name,false,startAfter);
    }
    /// @}

/// Return a reference to a TMidasBank or subclass object (see the
/// GetMidasBank() method for discussion of the call arguments).  If
/// the object is not found, then an ENoSuchElement exception will be
/// thrown and if the object cannot be dynamic cast to the requested class
/// an EBadConversion will be thrown.  A typical usage might look
/// something like this:
/// 
/// \code
///  myFunc(TND280RawEvent& revt) {
///     // Try to find a TDemo1Bank object
///     try
///         TDemo1Bank& rd1b
///           = rev.UseMidasBank<TDemo1Bank>("ND::TDemo1Bank");
/// 	    std::cout << "Found " << h->GetName() << std::endl;
///      catch (ENoSuchElement) {
///         std::cout << "object is not found" << std::endl;
///     }
///  }
/// \endcode

    template <class T> T& UseMidasBank(const char* name,
				       Bool_t byType,
				       THandle<T> startAfter ) const {
	THandle<T> object(GetMidasBank<T>(name));
        if (!object) throw ENoSuchElement();
        return *object;
    }

    /// @{ Convenience functions so that default values never need to be
    /// explicitly supplied.  See full UseMidasBank template for parameter
    /// definitions.
    template <class T> T& UseMidasBank(const char* name,
				       Bool_t byType = false) const {
	return UseMidasBank<T>(name,byType,THandle<T>());
    }

    template <class T> T& UseMidasBank(const char* name,
				       THandle<T> startAfter) const {
	return UseMidasBank<T>(name,false,startAfter);
    }
    /// @}

    /// Take ownership of a generic TMidasBank
    void AdoptMidasBank(TMidasBank* component) {
        this->TDataVector::AddDatum(component);
	fHasUnpromotedBanks = true;
    }


    /// Scan container and promote banks.  Its declared as a const method
    /// as conceptually it is even though it may sneakly replace one set of
    /// objects with another.
    void PromoteMidasBanks(Bool_t warn = true) const;

    /// @{Override methods in the base TObject class.
    virtual void ls(Option_t* opt = "") const;
    /// @}

private:

    /// Block methods inherited from TDataVector that would allow user
    /// to insert objects that are not TMidasBank or their subclasses

    void NotImplemented(const char* method) const;
    virtual void AddDatum(ND::THandle< ND::TDatum > handle, const char *name=NULL) {this->NotImplemented("AddDatum");}
    virtual void AddDatum(TDatum *val, const char *name=NULL) {this->NotImplemented("AddDatum");}
    virtual void push_back(TDatum *data) {this->NotImplemented("push-back");}
    virtual iterator insert (iterator position, TDatum *data) {this->NotImplemented("iterator"); return position;}


    /// Set name based on header
    void SetNameFromHeader();

    /// Global information for the raw event
    TRawDataHeader fHeader;

    /// Event size in bytes from the 3rd word of the MIDAS event header
    Int_t fEventSize;

    /// Global information for the run (updated when begin/end run record encountered).
    static Int_t fgRun;

    /// Optimisation: don't waste time looking for banks to promote if false
    mutable Bool_t fHasUnpromotedBanks; //! Do not save

    ClassDef(TND280RawEvent,2)

};

#endif
