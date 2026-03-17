#include <typeinfo>
#include <cstring>
#include <iomanip>
#include <sstream>
#include <cstdlib>

#include "TMidasBank.hxx"
#include "TMidasObj.hxx"
#include "TND280Log.hxx"
#include "Demangle.hxx"

ClassImp(ND::TMidasBank);

//_____________________________________________________________________________
    
ND::TMidasBank::TMidasBank()
    :  TDatum("",TMIDAS_BANK_TITLE),
       fSize(0),
       fBank(0),
       fNotZombieFlag(0x02468ace),
       fChannelInfo(0) {

   // Default constructor

    ND280Trace("TMidasBank: Default ctor at:" << (void*) this);
    
}

//_____________________________________________________________________________

ND::TMidasBank::TMidasBank(const ULong64_t *bank,
			   const char* title       /* = TMIDAS_BANK_TITLE */)
    :  TDatum("unnamed",title),
       fSize(0),
       fBank(0),
       fNotZombieFlag(0x02468ace),
       fChannelInfo(0) {

    // Normal constructor
    //
    // Build from a MIDAS Bank32 (not 16-bit version) simply by duplicating data
    //
    // Caution: If bank is not NULL it is assumed to be at least 2 elements long
    //          as the true size is stored in the second element.

    // Extract bank name and size from bank header and make a owned copy of the bank.

    static char name[5];
    strcpy(name,"????");
    unsigned int byte_size = 0;

    if ( bank ) {
	strncpy(name,reinterpret_cast<const char*>(bank),4);
	// Add in size of header (12 bytes)
	unsigned int byte_size = (bank[1] & 0xffffffff) + 12;
	// Round up if not an integral number of 64-bit words
	fSize = (byte_size + 7) >> 3;
	fBank = new ULong64_t [fSize];
	memcpy(fBank,bank,sizeof(ULong64_t)*fSize);
    }

    this->SetName(name);


    ND280Trace("ND::TMidasBank: Flat buffer ctor at:" << (void*) this 
 	       << "\n   Name:'" << name << "'; Title:" << title 
	       << "; buffer size:" << fSize << "; starts at:" << (void*) fBank);


    if ( byte_size & 0x7 )
	ND280Error("ND::TMidasBank: Bank truncation!  The header byte size: " << byte_size
		   << " is not exactly divisible by 8\n"
		   << "     - trailing bytes in last incomplete word will be lost!"); 


}

//_____________________________________________________________________________

ND::TMidasBank::TMidasBank(const char name[4],
			   UInt_t type,
			   UInt_t data_size,
			   const char* data,
			   const char* title       /* = TMIDAS_BANK_TITLE */)
    :  TDatum(std::string(name,4).c_str(),title),
       fSize(0),
       fBank(0),
       fNotZombieFlag(0x02468ace),
       fChannelInfo(0) {

    // Construct bank from component parts.

    // Note If the T2K specific header indicates that there is a trailer, this routine
    //      assumes that it is already part of the data; it does not attempt to append one.

    // Compute the bank size in bytes, by adding in the 12 byte header 
    // but round up to 8-byte boundary and create buffer for bank.
    fSize = ((data_size + 12 + 7) & ~7) >> 3;
    fBank = new ULong64_t [fSize];
    fBank[fSize-1] = 0; // Ensure last word is in well defined state even if only partially filled.

    // Populate it.
    UInt_t* data32 =  reinterpret_cast<UInt_t*>(fBank);
    memcpy(data32+0,name,sizeof(char)*4);
    data32[1] = type;
    data32[2] = data_size;
    memcpy(data32+3,data,sizeof(char)*data_size);


}

//_____________________________________________________________________________

ND::TMidasBank::~TMidasBank() {
    this->DropDataBuffer();

    /// Inform all child objects that they are on their own.
      std::list<TMidasObj*>::iterator itr(fChildObjs.begin()),
	                              itrEnd(fChildObjs.end());
      while ( itr != itrEnd ) (*(itr++))->BankDeleted();

      ND280Trace("ND::TMidasBank: dtor at:" << (void*) this);
      fNotZombieFlag = 0;

}

//_____________________________________________________________________________

UInt_t ND::TMidasBank::CalcChecksum() const {
    if ( ! this->HasChecksum() ) return 0;
    UInt_t type = this->GetChecksumType();
    if ( type != 1 ) {
	ND280Warn("ND::TMidasBank::CalcChecksum: Unable to compute checksum for unsupported type " 
		  << type << ". Returning bad checksum value instead which may well lead to checksum failure errors.");
	return 0x0bad0c00;
    }

    // Compute the checksum for the entire bank including the MIDAS
    // bank header but excluding the T2K trailer (if any) and the
    // checksum field of the T2K header.
    const UInt_t* bank =  reinterpret_cast<const UInt_t*>(this->GetBank());
    UInt_t size = this->GetRawSize()/4 + 3;   // Add in the MIDAS header
    if ( this->HasT2KTrailer() ) size -= 2; // Exclude T2K trailer
    UInt_t checksum = 0;
    for (UInt_t i = 0; i < size; ++i ) {
	UInt_t data = bank[i];
	if ( i == 3 ) data &= 0xffff; // Exclude checksum field of the T2K header
	checksum ^= data;  // XOR operation
    }
    // Fold into 16 bits
    checksum =  (checksum & 0xffff) ^ ((checksum >> 16) & 0xffff);
    return checksum;
}

//_____________________________________________________________________________

void ND::TMidasBank::DonateDataBuffer(TMidasBank* donee) {
    donee->DropDataBuffer();
    donee->fSize = fSize;
    donee->fBank = fBank;
    donee->Init();  // Reinitialise donee with the new buffer
    fSize        = 0;
    fBank        = 0;
}


//_____________________________________________________________________________

void ND::TMidasBank::DropDataBuffer() {
    if (fSize > 0) delete [] fBank;
    fBank = 0;
    fSize = 0;
}

//_____________________________________________________________________________

UInt_t ND::TMidasBank::GetChecksum() const {
    if ( ! this->HasChecksum() ) return 0;
    return this->HasT2KTrailer() ? this->GetT2KTrailer() >> 48 : this->GetT2KHeader() >> 16;
}

//_____________________________________________________________________________

ULong64_t ND::TMidasBank::GetDataBits(UInt_t word, UInt_t bit_lo, UInt_t bit_hi) const {

  if ( word >= this->GetSize() ) return 0;
  ULong64_t data = *(this->GetData() + word);
  return ( data << (63 - bit_hi) ) >> (63 - bit_hi + bit_lo);

}

//_____________________________________________________________________________

ULong64_t ND::TMidasBank::GetT2KTrailer() const { 
    // Cannot assume that the payload data is an even number of 4-byte words so cannot simply do:-
    // return this->HasT2KTrailer() ? fBank[fSize-1] : 0; }
    // Instead pick up the last 8 bytes of data
    if ( ! this->HasT2KTrailer() ) return 0;
    const UInt_t* raw_data = this->GetData32() - 1; // Include the T2K header
    UInt_t        raw_size = this->GetRawSize()/4;
    return *reinterpret_cast<const ULong64_t*>(raw_data + raw_size - 2);
}

//_____________________________________________________________________________

std::string ND::TMidasBank::GetTestValidity() const {
    std::string result("O.K");
    if ( this->IsValid() ) {
	if ( ! this->HasChecksum() && ! this->HasT2KTrailer() ) result = "n/a";
    }
    else {
	result = "bad ";
	if ( ! this->TestChecksum()   ) result += "chksm ";
	if ( ! this->IsT2KTrailerOK() ) result += "trailer ";
    }
    return result;
}

//_____________________________________________________________________________

Bool_t ND::TMidasBank::IsT2KTrailerOK() const {

    if ( ! this->HasT2KTrailer() ) return true;
    ULong64_t trail = this->GetT2KTrailer();
    return (trail & 0xffffffff) == this->GetRawSize() && ((trail >> 32) & 0xffff) == 0xd1ce;
    
}

//_____________________________________________________________________________

void ND::TMidasBank::Print(const Option_t* opt /*   = "" */) const {

  // Option settings:-
  //
  //  l  (log)          Default: Exactly one line of output
  //  i  (info)         Maximum 6 lines output
  //  v  (verbose)      Maximum 200 lines output
  //  vv (very verbose) Unlimited lines output
  //
  //  1b                Print as 1 byte words
  //  2b                Print as 2 byte words
  //  4b                Print as 4 byte words
  //  8b                Print as 8 byte words
  //                    Default: use bank type to decide.

    Int_t num_bytes_data = this->GetRawSize();

    std::string flags;
    if ( this->HasChecksum()   )  flags += "C";
    if ( this->HasT2KTrailer() )  flags += "T";

    std::ostringstream osT2KHeader;
    osT2KHeader << "0x" << std::hex << std::setfill('0') << std::setw(8) 
                << this->GetT2KHeader();

    ND280Log(this->GetClassName() << " named " << this->GetName() 
	     << std::dec << " Type " << this->GetType()
	     << " Data: " << this->GetSize() << " Long64_t words Bank: "
	     << num_bytes_data << " bytes" 
	     << " Flags: " << flags
	     << " Ver " << this->GetRawDataVer()
	     << " Validity: " << this->GetTestValidity()
             << " Full T2K header: " << osT2KHeader.str());

    TString option = opt;
    option.ToLower();
    Int_t max_half_rows = 0;
    if ( option.Contains("i") )  max_half_rows = 2; 
    if ( option.Contains("v") )  max_half_rows = 99; 
    if ( option.Contains("vv") ) max_half_rows = 999999999; 
    if ( max_half_rows == 0 ) return;

    // Do a hex dump of the data. If there is too much to dump in full
    // at current verbosity level, print the begining and end and skip
    // the centre.

    Int_t num_words_per_row = 4;
    Int_t num_rows          = (this->GetSize() + num_words_per_row -1) /num_words_per_row;

    // Determine the number of bytes to print together.
    UInt_t bank_type = this->GetType();
    UShort_t num_bytes = 1;
    if      (    bank_type == 10 )  num_bytes = 8;
    else if (    bank_type ==  6
              || bank_type ==  7
              || bank_type ==  8
              || bank_type ==  9
	      || bank_type == 11 )  num_bytes = 4;
    else if (    bank_type ==  4
	      || bank_type ==  5 )  num_bytes = 2;
    if ( option.Contains("1b") ) num_bytes = 1;
    if ( option.Contains("2b") ) num_bytes = 2;
    if ( option.Contains("4b") ) num_bytes = 4;
    if ( option.Contains("8b") ) num_bytes = 8;

    Int_t start_skip_row    = 99999999;
    Int_t end_skip_row      = 0;
    if ( num_rows > 2*max_half_rows) {
      start_skip_row = max_half_rows +1;
      end_skip_row   = num_rows - max_half_rows;
    }

    Bool_t print_dots = true;

    for (Int_t i_row = 1; i_row <= num_rows; ++i_row) {
      if ( i_row >= start_skip_row and i_row <= end_skip_row ) {
	if ( ! print_dots ) continue;
	ND280Log("  ......");
	print_dots = false;
	continue;
      }
      Int_t offset    = (i_row-1)*num_words_per_row;
      Int_t num_words = this->GetSize() - offset;
      if ( num_words > num_words_per_row ) num_words = num_words_per_row;

      std::ostringstream os;
      os << "  " << std::hex << std::setfill('0') << std::setw(6) << 8*offset;

      const ULong64_t* data = this->GetData() + offset;
      for (Int_t i_word = 0; i_word < num_words; ++i_word ) {
	if ( num_bytes ==  8 ) {
	  // Print as 64 bit word but with space between top and bottom 32-bit parts.
	  ULong64_t wd = *(data+i_word);
	  os << "  " << std::setfill('0') << std::setw(8) << (wd >> 32)
	     << " "  << std::setfill('0') << std::setw(8) << (wd & 0xffffffff);
	}
	else if ( num_bytes == 4 ) {
	  // Print as 2 32 bit words with space between them
	  const UInt_t* wd = reinterpret_cast<const UInt_t*>(data+i_word);
	  os << "  " << std::setfill('0') << std::setw(8) << *wd
	     << " "  << std::setfill('0') << std::setw(8) << *(wd+1);
	}
	else if (num_bytes == 2 ) {
	  // Print as 4 16 bit words with space between them
	  const UShort_t* wd = reinterpret_cast<const UShort_t*>(data+i_word);
	  os << "  " << std::setfill('0') << std::setw(4) << *wd
	     << " "  << std::setfill('0') << std::setw(4) << *(wd+1)
	     << " "  << std::setfill('0') << std::setw(4) << *(wd+2)
	     << " "  << std::setfill('0') << std::setw(4) << *(wd+3);
	}
	else  {
	  // Print as 8 bytes with space between them
	  const UChar_t* wd = reinterpret_cast<const UChar_t*>(data+i_word);
	  os << "  " << std::setfill('0') << std::setw(2) << (UShort_t) *wd
	     << " "  << std::setfill('0') << std::setw(2) << (UShort_t) *(wd+1)
	     << " "  << std::setfill('0') << std::setw(2) << (UShort_t) *(wd+2)
	     << " "  << std::setfill('0') << std::setw(2) << (UShort_t) *(wd+3)
	     << " "  << std::setfill('0') << std::setw(2) << (UShort_t) *(wd+4)
	     << " "  << std::setfill('0') << std::setw(2) << (UShort_t) *(wd+5)
	     << " "  << std::setfill('0') << std::setw(2) << (UShort_t) *(wd+6)
	     << " "  << std::setfill('0') << std::setw(2) << (UShort_t) *(wd+7);
	}

      }
      ND280Log(os.str());
    }

}

//_____________________________________________________________________________

Bool_t ND::TMidasBank::TestChecksum() const {

    return this->GetChecksum() == this->CalcChecksum();

}

std::string ND::TMidasBank::GetClassName() const {
    return Tools::Demangle(typeid(*this));
}

void ND::TMidasBank::ls(Option_t* opt) const {
    // Option settings:-
    //
    //  l  (log)          Default: Exactly one line of output
    //  i  (info)         Maximum 6 lines output
    //  v  (verbose)      Maximum 200 lines output
    //  vv (very verbose) Unlimited lines output
    //
    //  1b                Print as 1 byte words
    //  2b                Print as 2 byte words
    //  4b                Print as 4 byte words
    //  8b                Print as 8 byte words
    //                    Default: use bank type to decide.
    std::ios::fmtflags save = std::cout.flags();
    Int_t num_bytes_data = this->GetRawSize();

    std::string flags;
    if ( this->HasChecksum()   )  flags += "C";
    if ( this->HasT2KTrailer() )  flags += "T";

    ls_header(this, opt);
    
    std::ostringstream osT2KHeader;
    osT2KHeader << "0x" << std::hex << this->GetT2KHeader();

    if (typeid(*this) != typeid(ND::TMidasBank)) {
        std::cout << " (" << this->GetClassName() << ")";
    }

    std::cout << " V" << this->GetRawDataVer()
              << " Flg: " << flags
              << " " << this->GetTestValidity()
              << " T: " << this->GetType()
              << " S: " << this->GetSize() << "(" << num_bytes_data << ")"
              << " Hdr: " << osT2KHeader.str()
              << std::endl;

    if (strstr(opt,"full")) {
        TROOT::IncreaseDirLevel();
        TROOT::IndentLevel();
        std::cout << "Full name: " << GetFullName() << std::endl;
        TROOT::DecreaseDirLevel();
    }

    TString option = opt;
    option.ToLower();
    Int_t max_half_rows = 0;
    if ( option.Contains("dump") )  max_half_rows = 2; 
    if ( option.Contains("v") )  max_half_rows = 99; 
    if ( option.Contains("vv") ) max_half_rows = 999999999; 
    if ( max_half_rows == 0 ) {
        std::cout.fill(' ');
        std::cout.flags(save);
        return;
    }

    // Do a hex dump of the data. If there is too much to dump in full
    // at current verbosity level, print the begining and end and skip
    // the centre.

    Int_t num_words_per_row = 2;
    Int_t num_rows          = (this->GetSize() + num_words_per_row -1) /num_words_per_row;

    // Determine the number of bytes to print together.
    UInt_t bank_type = this->GetType();
    UShort_t num_bytes = 1;
    if      (    bank_type == 10 )  num_bytes = 8;
    else if (    bank_type ==  6
                 || bank_type ==  7
                 || bank_type ==  8
                 || bank_type ==  9
                 || bank_type == 11 )  num_bytes = 4;
    else if (    bank_type ==  4
                 || bank_type ==  5 )  num_bytes = 2;
    if ( option.Contains("1b") ) num_bytes = 1;
    if ( option.Contains("2b") ) num_bytes = 2;
    if ( option.Contains("4b") ) num_bytes = 4;
    if ( option.Contains("8b") ) num_bytes = 8;

    Int_t start_skip_row    = 99999999;
    Int_t end_skip_row      = 0;
    if ( num_rows > 2*max_half_rows) {
        start_skip_row = max_half_rows +1;
        end_skip_row   = num_rows - max_half_rows;
    }

    Bool_t print_dots = true;

    TROOT::IncreaseDirLevel();
    for (Int_t i_row = 1; i_row <= num_rows; ++i_row) {
        if ( i_row >= start_skip_row and i_row <= end_skip_row ) {
            if ( ! print_dots ) continue;
            TROOT::IncreaseDirLevel();
            TROOT::IndentLevel();
            std::cout << "          ......" << std::endl;;
            TROOT::DecreaseDirLevel();
            print_dots = false;
            continue;
        }
        Int_t offset    = (i_row-1)*num_words_per_row;
        Int_t num_words = this->GetSize() - offset;
        if ( num_words > num_words_per_row ) num_words = num_words_per_row;

        TROOT::IndentLevel();
        std::cout << "  "
                  << std::hex << std::setfill('0') << std::setw(6) 
                  << 8*offset
                  << std::dec
                  << " --";

        const ULong64_t* data = this->GetData() + offset;
        for (Int_t i_word = 0; i_word < num_words; ++i_word ) {
            if ( num_bytes ==  8 ) {
                // Print as 64 bit word but with space between top and bottom
                // 32-bit parts.
                ULong64_t wd = *(data+i_word);
                std::cout << " " 
                          << std::hex << std::setfill('0') << std::setw(8) 
                          << (wd >> 32)
                          << " "  
                          << std::hex << std::setfill('0') << std::setw(8) 
                          << (wd & 0xffffffff)
                          << std::dec;
            }
            else if ( num_bytes == 4 ) {
                // Print as 2 32 bit words with space between them
                const UInt_t* wd = reinterpret_cast<const UInt_t*>(data+i_word);
                std::cout << " " 
                          << std::hex << std::setfill('0') << std::setw(8) 
                          << *wd
                          << " " 
                          << std::hex << std::setfill('0') << std::setw(8) 
                          << *(wd+1)
                          << std::dec;
            }
            else if (num_bytes == 2 ) {
                // Print as 4 16 bit words with space between them
                const UShort_t* wd = reinterpret_cast<const UShort_t*>(data+i_word);
                std::cout << " " 
                          << std::hex << std::setfill('0') << std::setw(4) 
                          << *wd
                          << " "  
                          << std::hex << std::setfill('0') << std::setw(4) 
                          << *(wd+1)
                          << " "  
                          << std::hex << std::setfill('0') << std::setw(4) 
                          << *(wd+2)
                          << " "  
                          << std::hex << std::setfill('0') << std::setw(4) 
                          << *(wd+3)
                          << std::dec;
            }
            else  {
                // Print as 8 bytes with space between them
                const UChar_t* wd = reinterpret_cast<const UChar_t*>(data+i_word);
                std::cout << " " 
                          << std::hex << std::setfill('0') << std::setw(2) 
                          << (UShort_t) *wd
                          << " "  
                          << std::hex << std::setfill('0') << std::setw(2)
                          << (UShort_t) *(wd+1)
                          << " "  
                          << std::hex << std::setfill('0') << std::setw(2) 
                          << (UShort_t) *(wd+2)
                          << " "  
                          << std::hex << std::setfill('0') << std::setw(2) 
                          << (UShort_t) *(wd+3)
                          << " "  
                          << std::hex << std::setfill('0') << std::setw(2) 
                          << (UShort_t) *(wd+4)
                          << " "  
                          << std::hex << std::setfill('0') << std::setw(2) 
                          << (UShort_t) *(wd+5)
                          << " "  
                          << std::hex << std::setfill('0') << std::setw(2) 
                          << (UShort_t) *(wd+6)
                          << " "  
                          << std::hex << std::setfill('0') << std::setw(2)
                          << (UShort_t) *(wd+7)
                          << std::dec;
            }
        }
        std::cout << std::endl;
    }

    std::cout.fill(' ');
    std::cout.flags(save);
    TROOT::DecreaseDirLevel();
}
