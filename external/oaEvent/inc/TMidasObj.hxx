#ifndef TMidasObj_hxx_seen
#define TMidasObj_hxx_seen

#include <Rtypes.h>


namespace ND {
    class TMidasBank;
    class TMidasObj;
};

/// This is the base class for all objects created by the
/// <b><em>Access Classes</em></b> that inherit from TMidasBank.  To
/// keep all such objects lightweight so that they can be passed by
/// value, it is sometimes necessary to link back to the parent
/// TMidasBank and access its data. Constructors of TMidasObj that do
/// this inform the parent using ConnectChildObj(...) and destructors
/// call DisconnectChildObj(...).  The parent TMidasBank keeps a list
/// of all such child objects and informs any that exist using BankDeleted()
/// method when it is destroyed which makes them invalid and prevents
/// illegal access of the destroyed bank.


class ND::TMidasObj  {


public:

    explicit TMidasObj(UShort_t rawDataVer = 0,
		       UShort_t invalid_code = 0,
		       const TMidasBank* parent = 0);
    TMidasObj(const ND::TMidasObj& from);
    virtual ~TMidasObj();

    const TMidasBank* GetParent() const     { return fParent; }
    UInt_t            GetRawDataVer() const { return fRawDataVer; }

    /// Print contents to ND280Log
    ///
    /// The recommended values for opt are:-
    ///   l  - log:           None or minimal
    ///   i  - info:          Brief
    ///   v  - verbose:       Full but sample if excessive
    ///   vv - very verbose:  Every gory detail
    /// Subclasses may implement only part of the above and/or
    /// extend it e.g. different formats, but should not
    /// introduce a contradictory scheme nor flag any of the above
    /// values as an error.
    virtual void Print(const Option_t* opt = "") const = 0; // Subclass must implement

    // Validity methods

    typedef enum {NotInvalid = 0,
		  DeletedParent,
		  IteratorOutOfRange,
		  UnknownInvalid} InvalidCodes;


    /// Return true if valid
    Bool_t IsValid() const {return fInvalidCode == NotInvalid;}

    /// Return true if invalid and also report invalid access.
    /// All methods except IsValid() of any object that could be
    /// invalid should begin by using this method and return null
    /// value if it returns true.
    Bool_t NotValid(const char* calledMethod = 0) const;

    /// Return invalidity code
    UShort_t GetInvalidCode() const { return fInvalidCode; }

    /// Return reason why object is invalid
    const char* GetInvalidReason() const;

    /// Set invalidity code
    void SetInvalidCode(UShort_t invalid_code);


    // Methods dealing with relationship to parent (if any)

    /// Copy constructor and assigment operator have to be aware of parent.
    void operator = (const ND::TMidasObj& from) { this->Clone(from); }

    /// Parent bank deleted
    void BankDeleted();

private:

    /// Replace contents with a copy of another TMidasObj.
    void Clone(const ND::TMidasObj& from);

    /// Connect to parent
    void Connect();

    /// Disconnect to parent
    void Disconnect();

    /// Pointer to parent TMidasBank or NULL if not reliant on parent
    const TMidasBank* fParent;

    /// The version number of raw data format (from bank's T2K specific header)
    UShort_t fRawDataVer;

    /// Invalidity code (=0 if valid)
    UShort_t fInvalidCode; 

    ClassDef(TMidasObj,1)

};

#endif
