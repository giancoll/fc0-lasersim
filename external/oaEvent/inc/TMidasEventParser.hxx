#ifndef TMidasEventParser_hxx_seen
#define TMidasEventParser_hxx_seen

#include <string>

#include "Rtypes.h"

namespace ND {
    class TDataVector;
    class TMidasEventParser;
};

/// Acknowledgements: This class derives from the header and bank parsing of
/// the class TMidasEvent developed by 
///
///   Stefan Ritt, Pierre-Andre Amaudruz and Konstantin Olchanski 
///
/// in the rootana package which is part of the Midas Data
/// Acquisition:-
/// 
///     https://midas.psi.ch/
///
/// The code was extracted from revision 55 of Subversion repository:-
///      svn://ladd00.triumf.ca/rootana/trunk rootana
/// The T2K group should monitor changes to that code looking for enhancements
/// and should report back any code improvements we make to Konstantin. 
/// 
/// Last checked rootana revision: 66.
/// 
/// TMidasEventParser is a class that can be filled for the current
/// event held by TMidasFile using the
/// 
///   bool Read(TMidasEventParser *event);
/// 
/// method. The TND280RawEvent(TMidasEventParser&) constructor uses 
/// a TMidasEventParser to unpack the event header and bank data.

class  ND::TMidasEventParser
{

public:

  TMidasEventParser();                  ///< default constructor
  virtual ~TMidasEventParser();         ///< destructor

  // Helpers for internal state initialisation.

  void   Clear();                       ///< Release any storage and clear.
  char*  GetData();                     ///< return pointer to the data buffer
  UInt_t GetDataSize() const;           ///< return size in bytes of the data buffer
  char*  GetEventHeader();              ///< return pointer to the  Midas event header
  UInt_t GetEventHeaderSize() const;    ///< return size of Midas event header
  void   SetData(UInt_t dataSize, 
                 char* dataBuffer);     ///< set an externally allocated data buffer

  Int_t  SwapBytes(bool);               ///< Convert event data between little-endian (Linux-x86) and big endian (MacOS-PPC) 
  void   SwapBytesEventHeader();        ///< Convert event header between little-endian (Linux-x86) and big endian (MacOS-PPC) 

  // General state getters

  UInt_t GetEventId() const;            ///< return the event id
  UInt_t GetTriggerMask() const;        ///< return the trigger mask
  UInt_t GetSerialNumber() const;       ///< return the serial number
  UInt_t GetTimeStamp() const;          ///< return the time stamp (unix time in seconds)
  Bool_t HasBanks() const;              ///< True if event has banks. 
  Bool_t IsBank32() const;              ///< returns "true" if event uses 32-bit banks
  Bool_t IsGoodSize() const;            ///< validate the event length
  Bool_t IsMessage() const;             ///< True if message event
  Bool_t IsRunBegin() const;            ///< True if begin run.
  Bool_t IsRunEnd() const;              ///< True if end run.

  // Helper for TND280RawEvent initialisation.

  void   BuildMidasBanks(ND::TDataVector& dv) const; ///< Add set of TMidasBanks to client

  // Class to log errors

  class ErrorLogger {
  public:
    ErrorLogger();
    ~ErrorLogger() {};
    void Clear();
    void Print() const;
    void ReportRecovered(UInt_t numBytesShift);
    void ReportFailed(UInt_t numBytesLost);
    void Report4AtEnd();
    void Report4InMiddle();
  private:
    UInt_t fNum4AtEnd;
    UInt_t fNum4InMiddle;
    UInt_t fNumRecovered;
    UInt_t fNumRecoveredMinBytes;
    UInt_t fNumRecoveredMaxBytes;
    UInt_t fNumFailed;
    UInt_t fNumFailedMinBytes;
    UInt_t fNumFailedMaxBytes;
  };
  // Singleton Error Logger.
  static ErrorLogger* fgErrorLogger;
  static ErrorLogger& GetErrorLogger() {
    if ( ! fgErrorLogger ) fgErrorLogger = new ErrorLogger;
    return *fgErrorLogger;
  }

private:

  /// Allocate data buffer using the existing event header
  void   AllocateData(); 
  ///  Perform sanity check on bank header  
  Bool_t BankHeaderSanityCheck(char* bank_header, UInt_t bytes_left, Bool_t print_error = false) const;

  struct {
    UShort_t fEventId;      ///< event id
    UShort_t fTriggerMask;  ///< event trigger mask
    UInt_t   fSerialNumber; ///< event serial number
    UInt_t   fTimeStamp;    ///< event timestamp in seconds
    UInt_t   fDataSize;     ///< event size in bytes
  } fEventHeader;           ///< event header
  char* fData;              ///< event data buffer
  Bool_t fAllocatedByUs;    ///< "true" if we own the data buffer fData

  ClassDef(TMidasEventParser,0)
};

#endif // TMidasEventParser_hxx_seen
