
#include "TDataVector.hxx"
#include "TMidasBank.hxx"
#include "TMidasEventParser.hxx"
#include "TND280Log.hxx"

#include <cctype>
#include <cstring>
#include <cassert>
#include <sstream>

ClassImp(ND::TMidasEventParser);

/// Midas Bank header

struct BankHeader_t {
  UInt_t fDataSize;
  UInt_t fFlags;
};

/// 16-bit data bank

struct Bank_t {
  char fName[4];      ///< bank name
  UShort_t fType;     ///< type of data (see midas.h TID_xxx)
  UShort_t fDataSize;
};

/// 32-bit data bank

struct Bank32_t {
  char fName[4];      ///< bank name
  UInt_t fType;     ///< type of data (see midas.h TID_xxx)
  UInt_t fDataSize;
};


ND::TMidasEventParser::TMidasEventParser()
{
  fData = NULL;
  this->Clear();
}


ND::TMidasEventParser::~TMidasEventParser()
{
  this->Clear();
}

void ND::TMidasEventParser::AllocateData()
{
  if ( ! fData ) {
      fData          = new char[fEventHeader.fDataSize];
      fAllocatedByUs = true;
  }
}


Bool_t ND::TMidasEventParser::BankHeaderSanityCheck(char* bank_header,
						    UInt_t bytes_left,
						    Bool_t print_error /* = false */) const
{

    // Perform the bank header sanity check

    // 1)  The bank name consists of 4 alphanumeric characters
    bool name_valid = true;
    for (int ichar = 0; ichar < 4; ++ichar )
	if ( ! isalnum(bank_header[ichar]) || islower(bank_header[ichar]) ) name_valid = false;

    // 2)  The bank type is a small interger (< 100)
    UInt_t type = *reinterpret_cast<UInt_t*>(bank_header+4);
    bool type_valid = type < 100;

    // 3)  The bank size is less that the space left allowing for bank header
    UInt_t size = *reinterpret_cast<UInt_t*>(bank_header+8);
    bool size_valid = (size +  sizeof(Bank32_t)) <= bytes_left;

    if ( name_valid && type_valid && size_valid ) return true;

    if ( print_error ) {
	std::ostringstream oss;
	oss << "\n\nND::TMidasEventParser::BankHeaderSanityCheck: Found bad bank header @"
	    << std::hex << (void *) bank_header;
	oss << " Name: '";
	for (int ichar = 0; ichar < 4; ++ichar) {
	    if (  isalnum(bank_header[ichar]) ) oss << bank_header[ichar];
	    else oss << ".";
	}
	oss << "' ";
	if ( ! name_valid ) oss << " (bad) ";
	oss << " type: ";
	if ( bytes_left > 8) {
            oss << std::dec << type;
	    if ( ! type_valid )  oss << " (bad) ";
	}
	else                 oss <<  "n/a (bad) ";
	oss << " size: ";
	if ( bytes_left > 12) oss <<  std::dec << size;
	else                  oss <<  "n/a";
	if ( ! size_valid ) oss << " (bad - remaining space is " << bytes_left << ") ";
	ND280Warn("\n\n" << oss.str() );
    }

    return false;

}

void ND::TMidasEventParser::BuildMidasBanks(TDataVector& dv) const {

    // Program Notes:
    //
    // This method is used by TND280RawEvent whose overloaded AddDatum is blocked
    // in order to prevent it accepting objects that are not TMidasBanks
    // so we have to sneak an end run round it and use TDataVector::AddDatum(...)


    // Run begin and run end have XML dump of ODB, so dump contents
    // into a single XMLD bank.
    if ( this->IsRunBegin() || this->IsRunEnd() ) {

	      // Crowbar in a T2KHeader immediately before the data and repair the damage
	      // afterwards.  Valgrind will complain about invalid reads up to 3 bytes before 
        // valid block. Squeamish people may want to look away now.
	      char* buffer_char     = fData - sizeof(UInt_t);
	      ND280Trace("ND::MidasEventParser::BuildMidasBanks: Forming bank at " << std::hex << (void*) buffer_char
		          << " (Offset : " << fData - buffer_char
		          << ") Name: XMLD"
		          << " Type 3"
		          << " Size " << std::dec << this->GetDataSize() + sizeof(UInt_t) );
	      TMidasBank* mb = new TMidasBank("XMLD",
					  3,
					  this->GetDataSize() + sizeof(UInt_t),
					  buffer_char);
	      // Repatch T2KHeader
	      mb->SetT2KHeader(0);

	      dv.TDataVector::AddDatum(mb);
	      return;
    }

    // Quit if event has no banks or only 16-bit ones (which aren't present in ND280 data.)
    if ( ! this->HasBanks() || ! this->IsBank32() ) return;

    const BankHeader_t* event_header = (const BankHeader_t*) fData;
    const char*         event_end    = fData + sizeof(BankHeader_t) + event_header->fDataSize;

    char* bank_header = fData + sizeof(BankHeader_t);
    char* bank_data   = bank_header + sizeof(Bank32_t);

    // Some diagnostic variables
    char* previous_bank_start = 0;
    char* previous_bank_end   = 0;

    while ( bank_header < event_end ) {

        Bank32_t* bh = (Bank32_t*) bank_header;

        // Do some sanity checks and attempt to resynchronise if there are problems.

        if ( this->BankHeaderSanityCheck(bank_header,event_end - bank_header, false ) ) {
            // We are in good shape...   keep going
        } else if (event_end - bank_header == 4) {
            GetErrorLogger().Report4AtEnd();        // Extra 4 bytes at end, ignore
            break;
        } else if ( this->BankHeaderSanityCheck(bank_header+4, event_end - bank_header - 4, false )) {
            GetErrorLogger().Report4InMiddle();     // Extra 4 bytes at in middle, ignore
            bank_header += 4;
            bank_data += 4;
            bh = (Bank32_t*) bank_header;
        } else {
            // Do check again, this time with printout
            this->BankHeaderSanityCheck(bank_header,event_end - bank_header, true );


            ND280Warn( "MIDAS Record limits:  " << std::hex << "@" << (void*)fData
                    << " to: " << "@" << (void*)(event_end -1)
                    << "\nPrevious bank limits: "  << "@" << (void*)previous_bank_start
                    << " to: " << "@" << (void*)previous_bank_end
                    << "\nCurrent bank limits:  "  << "@" << (void*) bank_header
                    << " to: "  << "@" << (void*)(bank_header + sizeof(Bank32_t) + bh->fDataSize -1));
            UInt_t resync_shift   = 0;
            UInt_t num_bytes_left = event_end - bank_header;
            UInt_t max_attempt(num_bytes_left > 100 ? 100 : num_bytes_left);
            for (UInt_t attempt = 1; attempt < max_attempt; ++attempt ) {
                ++bank_header;
                ++bank_data;
                if ( this->BankHeaderSanityCheck(bank_header,event_end - bank_header) ) {
                    resync_shift = attempt;
                    GetErrorLogger().ReportRecovered(resync_shift);
                    break;
                }
            }
            if ( ! resync_shift ) {
                ND280Warn("Failed to resynchronise on next bank header, quiting event parsing.");
                GetErrorLogger().ReportFailed(num_bytes_left);
                break;
            }
            bh = (Bank32_t*) bank_header;
            ND280Warn("Resynchronised onto  next bank header (Name: " << bh->fName
                  << " Type " << bh->fType
                  << " Size " << std::dec << bh->fDataSize  
                  << ") after " << resync_shift << " bytes");
        }


        ND280Trace("ND::MidasEventParser::BuildMidasBanks: Forming bank at " << std::hex << (void*) bank_data
              << " (Offset : " << bank_data - fData 
              << ") Name:" << bh->fName
              << " Type " << bh->fType
              << " Size " << std::dec << bh->fDataSize); 
        TMidasBank* mb = new TMidasBank(bh->fName,
              bh->fType,
              bh->fDataSize,
              bank_data);
        dv.TDataVector::AddDatum(mb);
        previous_bank_start = bank_header;
        previous_bank_end   = bank_header + sizeof(Bank32_t) + bh->fDataSize -1;
        bank_header = bank_header + sizeof(Bank32_t) + ( ( bh->fDataSize + 7 ) & ~7 ); // Round up bank size to 8 byte boundary
        bank_data   = bank_header + sizeof(Bank32_t);
    }
    
}

void ND::TMidasEventParser::Clear()
{
  if ( fData && fAllocatedByUs ) delete [] fData;

  fData                      = NULL;
  fAllocatedByUs             = false;
  fEventHeader.fEventId      = 0;
  fEventHeader.fTriggerMask  = 0;
  fEventHeader.fSerialNumber = 0;
  fEventHeader.fTimeStamp    = 0;
  fEventHeader.fDataSize     = 0;
}

char* ND::TMidasEventParser::GetData()
{
  if ( ! fData ) this->AllocateData();
  return fData;
}

UInt_t ND::TMidasEventParser::GetDataSize() const
{

  return fEventHeader.fDataSize;
}

char* ND::TMidasEventParser::GetEventHeader()
{
  return reinterpret_cast<char*>(&fEventHeader);
}

UInt_t ND::TMidasEventParser::GetEventHeaderSize() const
{
  return sizeof(fEventHeader);
}

UInt_t ND::TMidasEventParser::GetEventId() const
{
  return fEventHeader.fEventId;
}

UInt_t ND::TMidasEventParser::GetTriggerMask() const
{
  return fEventHeader.fTriggerMask;
}

UInt_t ND::TMidasEventParser::GetSerialNumber() const
{
  return fEventHeader.fSerialNumber;
}

UInt_t ND::TMidasEventParser::GetTimeStamp() const
{
  return fEventHeader.fTimeStamp;
}

Bool_t ND::TMidasEventParser::HasBanks() const
{
  return fData && ! this->IsMessage() && ! this->IsRunBegin() && ! this->IsRunEnd();
}

Bool_t ND::TMidasEventParser::IsBank32() const
{
  return ((BankHeader_t *)fData)->fFlags & (1<<4);
}

Bool_t ND::TMidasEventParser::IsGoodSize() const
{
  return fEventHeader.fDataSize > 0 && fEventHeader.fDataSize <= 500 * 1024 * 1024;
}

Bool_t ND::TMidasEventParser::IsMessage() const
{
  return (fEventHeader.fEventId & 0xffff) == 0x8002;
}

Bool_t ND::TMidasEventParser::IsRunBegin() const
{
  return (fEventHeader.fEventId & 0xffff) == 0x8000;
}


Bool_t ND::TMidasEventParser::IsRunEnd() const
{
  return (fEventHeader.fEventId & 0xffff) == 0x8001;
}

void  ND::TMidasEventParser::SetData(UInt_t dataSize, char* dataBuffer) {
  fEventHeader.fDataSize = dataSize;
  assert(!fAllocatedByUs);
  assert(IsGoodSize());
  fData          = dataBuffer;
  fAllocatedByUs = false;
  SwapBytes(false);
}

/// Byte swapping routine.

typedef UChar_t BYTE;

/// 16-bit byte swapping routine.
///
#define WORD_SWAP(x) { BYTE _tmp;        \
_tmp= *((BYTE *)(x));                    \
*((BYTE *)(x)) = *(((BYTE *)(x))+1);     \
*(((BYTE *)(x))+1) = _tmp; }

/// 32-bit byte swapping routine.
///
#define DWORD_SWAP(x) { BYTE _tmp;       \
_tmp= *((BYTE *)(x));                    \
*((BYTE *)(x)) = *(((BYTE *)(x))+3);     \
*(((BYTE *)(x))+3) = _tmp;               \
_tmp= *(((BYTE *)(x))+1);                \
*(((BYTE *)(x))+1) = *(((BYTE *)(x))+2); \
*(((BYTE *)(x))+2) = _tmp; }

/// 64-bit byte swapping routine.
///
#define QWORD_SWAP(x) { BYTE _tmp;       \
_tmp= *((BYTE *)(x));                    \
*((BYTE *)(x)) = *(((BYTE *)(x))+7);     \
*(((BYTE *)(x))+7) = _tmp;               \
_tmp= *(((BYTE *)(x))+1);                \
*(((BYTE *)(x))+1) = *(((BYTE *)(x))+6); \
*(((BYTE *)(x))+6) = _tmp;               \
_tmp= *(((BYTE *)(x))+2);                \
*(((BYTE *)(x))+2) = *(((BYTE *)(x))+5); \
*(((BYTE *)(x))+5) = _tmp;               \
_tmp= *(((BYTE *)(x))+3);                \
*(((BYTE *)(x))+3) = *(((BYTE *)(x))+4); \
*(((BYTE *)(x))+4) = _tmp; }

int ND::TMidasEventParser::SwapBytes(bool force)
{
  BankHeader_t *pbh;
  Bank_t *pbk;
  Bank32_t *pbk32;
  void *pdata;
  UShort_t type;

  pbh = (BankHeader_t *) fData;
  //
  // only swap if flags in high 16-bit
  //
  if (pbh->fFlags < 0x10000 && ! force)
    return 0;

  // Trap if this is an ODB dump, and skip it.
  if (pbh->fDataSize == 0x6d783f3c) // string "<xml..." in wrong-endian format
    return 1;

  if (pbh->fDataSize == 0x3c3f786d) // string "<xml..."
    return 1;

  // New MIDAS (after about 2016) no longer dumps the ODB in a standardized
  // format by default.  It's using an unversioned implementation of JSON that
  // is much harder to check for validity, and isn't guarranteed to keep the
  // same byte sequence.  Check to see if the "typical" magic string exists
  // (i.e. "  \n{").  If the magic string is found, print a message that the
  // file is badly formed (the bank length is unreasonable anyway).  Change
  // the undef for debugging with new files, otherwise throw an error.
  if (pbh->fDataSize == 0x20200a7b
      || pbh->fDataSize == 0x7b0a2020) {
      ND280Error("MIDAS data is invalid and DAQ was probably misconfigured");
#undef MIDAS_EVENT_PARSER_ALLOW_INVALID_FILES
#ifdef MIDAS_EVENT_PARSER_ALLOW_INVALID_FILES
      return 1;
#endif
      throw std::runtime_error("Invalid MIDAS data");
  }

  //
  // swap bank header
  //
  DWORD_SWAP(&pbh->fDataSize);
  DWORD_SWAP(&pbh->fFlags);
  //
  // check for 32-bit banks
  //
  bool b32 = IsBank32();

  pbk = (Bank_t *) (pbh + 1);
  pbk32 = (Bank32_t *) pbk;
  //
  // scan event
  //
  while ((char*) pbk < (char*) pbh + pbh->fDataSize + sizeof(BankHeader_t)) {
    //
    // swap bank header
    //
    if (b32) {
      DWORD_SWAP(&pbk32->fType);
      DWORD_SWAP(&pbk32->fDataSize);
      pdata = pbk32 + 1;
      type = (UShort_t) pbk32->fType;
    } else {
      WORD_SWAP(&pbk->fType);
      WORD_SWAP(&pbk->fDataSize);
      pdata = pbk + 1;
      type = pbk->fType;
    }
    //
    // pbk points to next bank
    //
    if (b32) {
      pbk32 = (Bank32_t *) ((char*) (pbk32 + 1) +
                          (((pbk32->fDataSize)+7) & ~7));
      pbk = (Bank_t *) pbk32;
    } else {
      pbk = (Bank_t *) ((char*) (pbk + 1) +  (((pbk->fDataSize)+7) & ~7));
      pbk32 = (Bank32_t *) pbk;
    }

    switch (type) {
    case 4:
    case 5:
      while (pdata < pbk) {
        WORD_SWAP(pdata);
        pdata = ((char*)pdata) + 2;
      }
      break;
    case 6:
    case 7:
    case 8:
    case 9:
      while (pdata < pbk) {
        DWORD_SWAP(pdata);
        pdata = ((char*)pdata) + 4;
      }
      break;
    case 10:
      while (pdata < pbk) {
        QWORD_SWAP(pdata);
        pdata = ((char*)pdata) + 8;
      }
      break;
    }
  }
  return 1;
}


void ND::TMidasEventParser::SwapBytesEventHeader()
{
  WORD_SWAP(&fEventHeader.fEventId);
  WORD_SWAP(&fEventHeader.fTriggerMask);
  DWORD_SWAP(&fEventHeader.fSerialNumber);
  DWORD_SWAP(&fEventHeader.fTimeStamp);
  DWORD_SWAP(&fEventHeader.fDataSize);
}


//----------------------  Error Logger  ------------------------------

ND::TMidasEventParser::ErrorLogger* ND::TMidasEventParser::fgErrorLogger = 0;

ND::TMidasEventParser::ErrorLogger::ErrorLogger() {
  this->Clear();
}

void ND::TMidasEventParser::ErrorLogger::Clear() {
  fNum4AtEnd     = 0;
  fNum4InMiddle  = 0;
  fNumRecovered         = 0;
  fNumRecoveredMinBytes = 999999999;
  fNumRecoveredMaxBytes = 0;
  fNumFailed             = 0;
  fNumFailedMinBytes     = 999999999;
  fNumFailedMaxBytes     = 0;
}

void ND::TMidasEventParser::ErrorLogger::Print() const{
  if ( ! fNumRecovered && ! fNumFailed && !fNum4AtEnd && !fNum4InMiddle) {
    ND280Log("No bank parsing errors or warnings detected");
    return;
  }

  ND280Log("The following bank parsing errors or warnings were detected:-");
  if ( fNumRecovered )
    ND280Log("  " << std::dec << fNumRecovered << " recoverable errors (found next bank header after shifting)."
	     << " Minimum shift: " << fNumRecoveredMinBytes << " Maximum shift " << fNumRecoveredMaxBytes);
  if ( fNumFailed )
    ND280Log("  " << std::dec << fNumFailed << " unrecoverable errors (found no bank in remainder of record)."
	     << " Minimum record left: " << fNumFailedMinBytes << " Maximum record left: " << fNumFailedMaxBytes);
  if ( fNum4AtEnd )
    ND280Log("  " << std::dec << fNum4AtEnd << " warning: 4 spare bytes at end of record.");
  if ( fNum4InMiddle )
    ND280Log("  " << std::dec << fNum4InMiddle << " warning: resynchronised by 4 bytes in middle of record.");

}

void ND::TMidasEventParser::ErrorLogger::Report4AtEnd() {
  ++fNum4AtEnd;        // Extra 4 bytes at end, ignore
}

void ND::TMidasEventParser::ErrorLogger::Report4InMiddle() {
  ++fNum4InMiddle;     // Extra 4 bytes at in middle, ignore
}

void ND::TMidasEventParser::ErrorLogger::ReportRecovered(UInt_t numBytesShift) {
  ++fNumRecovered;
  if ( fNumRecoveredMinBytes > numBytesShift ) fNumRecoveredMinBytes = numBytesShift;
  if ( fNumRecoveredMaxBytes < numBytesShift ) fNumRecoveredMaxBytes = numBytesShift;
}

void ND::TMidasEventParser::ErrorLogger::ReportFailed(UInt_t numBytesLost) {
  ++fNumFailed;
  if ( fNumFailedMinBytes > numBytesLost ) fNumFailedMinBytes = numBytesLost;
  if ( fNumFailedMaxBytes < numBytesLost ) fNumFailedMaxBytes = numBytesLost;
}
