#ifndef TMidasBank_hxx_seen
#define TMidasBank_hxx_seen

#include <Rtypes.h>

#include <TDatum.hxx>

#include <list>
#include <string>
#include "TRawDataHeader.hxx"

#define TMIDAS_BANK_TITLE "MIDAS Bank"

namespace ND {
    class TND280RawEvent;
    class TMidasBank;
    class TMidasEventParser;
    class TMidasObj;
    class TRawDataHeader;
}

/// This is the wrapper for a single MIDAS bank.  As such it is the base
/// class for all the banks produced by MIDAS.  It is a concrete
/// class and in this form provides generic services such as bank name
/// reporting and checksum testing.  However to access that data in
/// any more that its rawest form (an array of Long64_t) it is
/// necessary to instantiate the bank subclass that corresponds to the
/// name.  See TND280RawEvent for the access service that can perform
/// this instantiation on demand.

///
/// The layout of the data in the Long64_t is as follows:-
///<pre>
/// Word   Bits     Contents
///
///  0      0-31    Bank name - 4 printable ASCII characters
///        32-63    Bank type (char, short, long etc) as defined in midas.h
///
///  1      0-31    Bank size in bytes of all the following data including T2K specific header (=m)
///        32-63    T2K specific header consisting of
///                   Bits       Content
///           32          0      Trailer flag: Trailer present if bit set
///        33-35       1- 3      Checksum type: =0 None
///                                             =1 64-bit CRC folded into 16-bits
///                                             =4 FGD hardware CRC applied
///        36-39       4- 7      Version number of raw data format - incremented when firmware bugs are fixed.
///           40          8      Sub-block flag: Sub-blocks present if bit set (currently not used by any sub-detector).
///        41-43       9-11      Reserved for future use. Currently all zero.
///        44-47      12-15      Reserved for bank name extension (currently all zero - not used by any sub-detector).
///        48-63      16-31      CRC if Checksum type != 0. Otherwise 0.
///
///  2      0-63    First bank data word
///
///  3      0-63    Second bank data word
/// ...     ...     ...
/// ...     ...     ...
/// ...     ...     ...
///  N     0-63     Last bank data word.  In relation to bank size: m = 4 + 8*(N-1)
///                 If Trailer flag bit is set then this last word contains
///
///        0-31	    Bank size in bytes excluding header (=m)
///       32-48     'Magic word' = d1ce
///       49-63     CRC if Checksum type != 0. Otherwise 0.
///</PRE>
/// The getter methods
///
///   Int_t            GetBankSize()
///
///   const ULong64_t* GetBankData()
///
/// give access to the entire bank as a 64-bit array whilst the getters
///
///   Int_t            GetSize()
///
///   const ULong64_t* GetData()
///
/// exclude all T2K headers and trailers so GetSize() returns N-2 (or
/// N-3 if there is a trailer) and GetData() returns the address of
/// word 2.  There are also "convenience" getters that interpret the
/// data as 32-bit and 16-bit data.



class ND::TMidasBank : public TDatum {

    friend class TND280RawEvent;    //Needs access to DonateDataBuffer method
    friend class TMidasEventParser; //Needs access to SetT2KHeader
public:

    TMidasBank();
    explicit TMidasBank(const ULong64_t *bank,
			   const char* title = TMIDAS_BANK_TITLE);
    TMidasBank(const char name[4],UInt_t type,UInt_t data_size,const char* data,
	       const char* title = TMIDAS_BANK_TITLE);
    virtual ~TMidasBank();

    /// Get the real class name for this bank.  This uses RTTI to query the
    /// "this" pointer and returns a std::string with the class name.  The
    /// returned value can be used in place of the normal ROOT ClassName
    /// method which returns the name of the class for I/O purposes (all
    /// TMidasBank derived objects will return "ND::TMidasBank" from
    /// ClassName.)
    std::string GetClassName() const;

    /// Print contents to ND280Log
    void Print(const Option_t* opt = "") const;

    /// @{Override methods in the base TObject class.
    virtual void ls(Option_t* opt = "") const;
    /// @}

    // Unfettered read-only access to the unadulterated data as bank, 64-bit array, 32-bit array and 16-bit array.
    // See notes above on buffer layout.

    /// Return the number of bytes occupied by T2KHeader and trailer (if any).
    UInt_t           GetT2KOverhead() const { return this->HasT2KTrailer() ?  12 : 4; }
    /// Return number of bytes of data, as recorded in the bank, including T2KHeader and trailer (if any).
    UInt_t           GetRawSize()    const { return fBank[1] & 0xffffffff; }
    /// Return the number of ULong64_t used to hold the entire bank including MIDAS bank header
    UInt_t           GetBankSize()   const { return fSize; }
    /// Return the number of ULong64_t used to hold the data excluding any  T2KHeader and trailer (if any).
    UInt_t           GetSize()       const { return (this->GetSize32() + 1) >> 1; } // Include any part filled 8-byte
    /// Return the number of UInt_t used to hold the data excluding any  T2KHeader and trailer (if any).
    UInt_t           GetSize32()     const {
        UInt_t size     = this->GetRawSize();
        UInt_t overhead = this->GetT2KOverhead();
        return size > overhead ? (size-overhead) >> 2 : 0;
    }
    /// Return the number of UShort_t used to hold the data excluding any  T2KHeader and trailer (if any).
    UInt_t           GetSize16()     const { return this->GetSize32() << 1; }
    /// Return the number of UChar_t used to hold the data excluding any  T2KHeader and trailer (if any).
    UInt_t           GetSize8()      const { return this->GetSize32() << 2; }

    const ULong64_t* GetBank()       const { return fBank; }
    const ULong64_t* GetData()       const { return fBank+2; }
    ULong64_t        GetDataBits(UInt_t word, UInt_t bit_lo, UInt_t bit_hi) const;
    const UInt_t*    GetData32()     const { return reinterpret_cast<const UInt_t*>(this->GetData()); }
    const UShort_t*  GetData16()     const { return reinterpret_cast<const UShort_t*>(this->GetData()); }
    const UChar_t*   GetData8()      const { return reinterpret_cast<const UChar_t*>(this->GetData()); }

    UInt_t           GetType()       const { return fSize > 0 ? fBank[0] >> 32 : 0; }

    // T2K Header and trailer methods

    /// Calculate checksum from bank contents
    UInt_t           CalcChecksum() const;
    /// Return the checksum stored in the banks's T2K header or trailer
    UInt_t           GetChecksum() const;
    /// Return the checksum type
    UInt_t           GetChecksumType() const { return (this->GetT2KHeader() >>  1) & 0x7;  }
    /// Return the bank's version number
    UInt_t           GetRawDataVer() const   { return (this->GetT2KHeader() >>  4) & 0xf;  }
    /// Return the T2K header
    UInt_t           GetT2KHeader()  const { return this->GetRawSize() >= 4 ? fBank[1] >> 32 : 0; }
    /// Return the T2k trailer if it exists, or 0 otherwise.
    ULong64_t        GetT2KTrailer() const;
    /// Return a string representing the bank's validity.  Form is one of the following:-
    ///   "o.k."                     Bank passes all validity tests
    ///   "n/a"                      Bank has no validity data
    ///   "bad reason {reason ...}"  Bank fails at least some validity tests
    std::string GetTestValidity() const;
    /// Return true if bank has a checksum
    Bool_t           HasChecksum() const     { return this->GetChecksumType() != 0;        }
    /// Return true if bank has a T2K trailer
    Bool_t           HasT2KTrailer() const   { return (this->GetT2KHeader() & 0x1) == 1;   }
    /// Return true if T2K trailer is not bad (but might be missing).
    Bool_t           IsT2KTrailerOK() const;
    /// Return true if bank passes checksum and trailer tests
    Bool_t IsValid() const { return this->IsT2KTrailerOK() && this->TestChecksum(); }
    /// Return true if bank does not have a checksum or its value agrees with the calculation
    Bool_t           TestChecksum() const;

    ///  Methods dealing with child TMidasObj which need access to this object's data
    void ConnectChildObj(TMidasObj* child ) const    { if ( this->IsNotZombie() ) fChildObjs.push_back(child); }
    void DisconnectChildObj(TMidasObj* child ) const { if ( this->IsNotZombie() ) fChildObjs.remove(child); }
    UInt_t GetNumChildObjs() const { return fChildObjs.size();}
    /// Paranoia code: Attempt to identify destroyed TMidasBanks so that unregistered
    /// children don't attempt to access.
    Bool_t IsNotZombie() const { return fNotZombieFlag == 0x02468ace; }

    /// Bank name encoded channel information - see fChannelInfo
    UInt_t GetChannelInfo() const { return fChannelInfo; }

    /// "Context setting" methods
    virtual Bool_t IsContextSetter() const  {return false;}
    virtual void SetContext(ND::TRawDataHeader& header) const {if(false)header.Print();};


private:

    /// Donate data buffer to another bank (useful when promoting to actual subclass)
    void DonateDataBuffer(TMidasBank* donee);

    /// Drop data buffer
    void DropDataBuffer();

    /// Overwrite T2KHeader
    void SetT2KHeader(UInt_t header)   {
	if (this->GetRawSize() >= 4) reinterpret_cast<UInt_t*>(fBank)[3] = header;
    }

    /// Initialise bank content specific data members.  All subclasses that have such members
    /// MUST override this method and call it in their ctors.  This is because
    /// objects of these subclasses are typically created by promotion which involves
    /// first creating an object with no data buffer and then donating the buffer
    /// from the TMidasBank object it is replacing using DonateDataBuffer() which
    /// calls the donee's Init() buffer to reinitialise.
    virtual void Init() {}

    /// Number of Long64_t words stored in fBank including all headers
    Int_t fSize;

    /// The unstructured data. See notes above on buffer layout.
    ULong64_t   *fBank;   //[fSize]

    /// Child TMidasObj that are reliant on this object's data.
    /// The list is declared mutable as connection and disconnection
    /// of children don't effect the intrinsic state of the object
    mutable std::list<ND::TMidasObj*> fChildObjs; //! cache variable.

    /// Until destroyed this flag == 0x02468ace.  See IsNotZombie()
    UInt_t fNotZombieFlag;

protected:

    /// This is used to pass channel information, encoded in the bank
    /// name, to a TMidas*Digit digit via intervening TMidasItr
    /// iterators.  This base class initialises the value to zero but
    /// makes it protected so that subclass objects are free to set
    /// bits in any manner that is consistent with their eventual use.
    /// For example, for TripT banks the name encodes both detector
    /// and RMM information and TTripTDigitBank encodes this for
    /// subsequent use by its TMidasTripTDigit .
    UInt_t fChannelInfo;

    ClassDef(TMidasBank,1)

};

#endif
