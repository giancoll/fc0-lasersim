// Include the ROOT base classes.
#include <iostream>
#include <sstream>
#include <iomanip>
#include <typeinfo>

#include <TString.h>

#include "Demangle.hxx"
#include "TMidasBankProxyRegistry.hxx"
#include "TMidasEventParser.hxx"
#include "TND280Log.hxx"
#include "TND280RawEvent.hxx"

ClassImp(ND::TND280RawEvent);

Int_t ND::TND280RawEvent::fgRun = 0;

//_____________________________________________________________________________
    
ND::TND280RawEvent::TND280RawEvent()
  :  TDataVector("",TND280_RAW_EVENT_TITLE),
     fEventSize(0),
     fHasUnpromotedBanks(true) {

   // Default constructor

    ND280Trace("TND280RawEvent: Default ctor @" << (void*) this);

    this->SetNameFromHeader();
    
}

//_____________________________________________________________________________

ND::TND280RawEvent::TND280RawEvent(const char* name, 
				   const TRawDataHeader& header /* = TRawDataHeader()       */,
                                   const char* title            /* = TND280_RAW_EVENT_TITLE */)
    :  TDataVector(name,title),
       fHeader(header),
       fEventSize(0),
       fHasUnpromotedBanks(true) {


    // Normal constructor

    ND280Trace("ND::TND280RawEvent: Standard ctor @" << (void*) this);

    this->SetNameFromHeader();

}


//_____________________________________________________________________________

ND::TND280RawEvent::TND280RawEvent(const ND::TMidasEventParser& mep,
				   const char* name,
				   const char* title           /* = TND280_RAW_EVENT_TITLE */) 
    :  TDataVector(name,title),
       fEventSize(mep.GetDataSize()),
       fHasUnpromotedBanks(true) {

    ND280Trace("ND::TND280RawEvent: TMidasEventParser ctor @" << (void*) this);

    // Drop any existing banks
    this->Clear();
    // Add in new banks and promote them.
    mep.BuildMidasBanks(*this);
    this->PromoteMidasBanks(false);

    // Special treatment for begin, end and messages for which context
    // isn't relevant.
    if ( mep.IsRunBegin() ) {
        fgRun = mep.GetSerialNumber();
        fHeader.SetRecordType(TRawDataHeader::BeginRun);
    }
    else if ( mep.IsRunEnd() ) {
        fHeader.SetRecordType(TRawDataHeader::EndRun);
    }
    else if ( mep.IsMessage() ){
        fHeader.SetRecordType(TRawDataHeader::Message);
    }
    else {
        fHeader.SetRecordType(TRawDataHeader::Data);
    }

    // Provide initial values for fields.  These will be over written by the
    // context setters (if oaRawEvent is available and XRUN is found.).
    fHeader.SetRun(fgRun);
    fHeader.SetSeqNo(mep.GetSerialNumber());
    fHeader.SetTimeStamp(mep.GetTimeStamp());

    // Now look for context setters banks.
    bool contextSet = this->FillHeader();

    // No context setting banks, use TMidasEventParser information.
    if (!contextSet) {
	static Bool_t warn_once = true;
        if (warn_once) {
  	    warn_once = false;
	    ND280Warn( "\n\n\nND::TND280RawEvent::TND280RawEvent Caution!!!\n"
                       "\n"
                       "Cannot find an XRUN bank which holds context information with which to \n"
                       "set up the TRawDataHeader.  Instead the information contained within a MIDAS \n"
                       "event header will be used even though it is inadequate, for example it contains \n"
                       "neither run or sub-run numbers.  The TRawDataHeader will only have a run number\n"
                       "once a begin or end of run record is encountered and will use the serial number\n"
		       "in place of the event number.  Apart from that it will be essentially empty.\n"
                );
	}
    }

    // Finally use header to construct event name.
    this->SetNameFromHeader();
}

//_____________________________________________________________________________

ND::TND280RawEvent::~TND280RawEvent() {

    ND280Trace("ND::TND280RawEvent: dtor @" << (void*) this);

}

//_____________________________________________________________________________

void ND::TND280RawEvent::NotImplemented(const char* method) const {

    ND280Warn("The ND::TND280RawEvent method " << method
              << " is blocked in order to prevent it accepting "
              << "objects that are not TMidasBanks");
}

//_____________________________________________________________________________

bool ND::TND280RawEvent::FillHeader() {
    bool contextSet(false);

    // Now look for context setters banks.
    ND::THandle<ND::TMidasBank> midasBank;
    while ( midasBank = this->GetMidasBank<ND::TMidasBank>("",midasBank) ) {
	if ( midasBank->IsContextSetter() ) {
            midasBank->SetContext(fHeader);
            contextSet = true;
	}
    }

    return contextSet;
}

void ND::TND280RawEvent::Print(const Option_t* opt /*  = ""  */) const {

    ND280Log("The ND::TND280RawEvent @" << (void*) this
	     << std::dec << " Length (" << this->GetEventSize()
	     << "+16hdr)bytes" 
	     << " (name: " << this->GetName() << ")");
    fHeader.Print(opt);
    ND280Log("It contains the following " << this->size()
	     << " TMidasBanks:-");
    ND280Log("");

    const_iterator itr(this->TDataVector::begin()),
	           itrEnd(this->TDataVector::end());
    for (; itr != itrEnd; ++itr) (*itr)->Print(opt);

}
//_____________________________________________________________________________

void ND::TND280RawEvent::PromoteMidasBanks(Bool_t warn /* = true */) const {

    // Quit if nothing to do.
    if ( ! fHasUnpromotedBanks ) return;

    // Initialise.  This includes getting a non-constant version of "this"
    // so that we can replace objects in a "logically constant" way.
    // Squeamish people may want to look away now.
    TND280RawEvent* this_nc           = const_cast<TND280RawEvent*>(this);

    TMidasBankProxyRegistry& rdbpr = TMidasBankProxyRegistry::Instance();

    iterator itr(this_nc->TDataVector::begin()),
	     itrEnd(this_nc->TDataVector::end());

    for ( ; itr != itrEnd; ++itr ) {
	// Pick up the current object.
	TDatum* datum = *itr;

	// Make sure that nobody has sneaked in an object that does not
	// inherit from TMidasBank
	TMidasBank* rdb = dynamic_cast<TMidasBank*>(datum);
	if ( ! rdb ) {
	    ND280Warn("ND::TND280RawEvent: Unexpectedly found an " 
                      << datum->ClassName()
                      << " object.  It cannot be promoted.");
	    continue;
	}

	// Skip any that are already promoted
	if ( typeid(*rdb) != typeid(TMidasBank) ) continue;

	// We have found one to promote, make sure we have a proxy for it
	const char* bank_name    = rdb->GetName();
	TMidasBankProxy* rdbp = rdbpr.LookUp(bank_name);
	if (! rdbp ) {
	    if ( warn ) ND280Warn("ND::TND280RawEvent: Cannot find"
                                  << " a proxy for MIDAS bank " << bank_name
				  << " object.  It cannot be promoted.");
	    continue;
	}

	// We have a proxy, create a replacement, transfer the data and delete
	// the old one.
	TMidasBank* new_rdb = rdbp->CreateMidasBank(NULL);
	ND280Trace("ND::TND280RawEvent: Promoting TMidasBank to "
                   << Tools::Demangle(typeid(new_rdb)));
	new_rdb->SetName(rdb->GetName());
	rdb->DonateDataBuffer(new_rdb);
        *itr = new_rdb;
	// Finally delete the old object
	delete rdb;
	rdb = 0;
    }

    fHasUnpromotedBanks = false;
}

//_____________________________________________________________________________

void ND::TND280RawEvent::SetNameFromHeader() {

    // Assign object a unique name based on header.

    std::ostringstream buffer;
    UInt_t run(fHeader.GetRun());
    UInt_t seqNo(fHeader.GetSeqNo());
    buffer << std::setfill('0');
    if      ( fHeader.IsBeginRun() ) {
        buffer << "rawevent.begin_run.";
        buffer << std::setw(5);
        if (run != ND::TRawDataHeader::Invalid) buffer << run;
        else buffer << "invalid";
    }
    else if ( fHeader.IsEndRun() ) {
        buffer << "rawevent.end_run.";
        buffer << std::setw(5);
        if (run != ND::TRawDataHeader::Invalid) buffer << run;
        else buffer << "invalid";
    }
    else if ( fHeader.IsMessage() )  {
        buffer << "rawevent.message.";
        buffer << std::setw(8);
        if (seqNo != ND::TRawDataHeader::Invalid) buffer << seqNo;
        else buffer << "invalid";
    }
    else {
        buffer << "rawevent.";
        buffer << std::setw(5);
        if (run != ND::TRawDataHeader::Invalid) buffer << run;
        else buffer << "invalid";
        buffer << ".";
        buffer << std::setw(8);
        if (seqNo != ND::TRawDataHeader::Invalid) buffer << seqNo;
        else buffer << "invalid";
    }
    this->SetName(buffer.str().c_str());
}

void ND::TND280RawEvent::ls(Option_t* opt) const {
    PromoteMidasBanks();
    ND::TData::ls(opt);
    TROOT::IncreaseDirLevel();
    std::ios::fmtflags save = std::cout.flags();
    TROOT::IndentLevel();
    std::cout << "Run: " << fHeader.GetRun();
    std::cout << " Sub-Run: " << fHeader.GetSubRun();
    std::cout << " Sequence: " << fHeader.GetSeqNo();
    std::cout << std::endl;
    
    TROOT::IndentLevel();
    std::cout << "Stamp: " << fHeader.GetTimeStamp();
    std::cout << " Record-type: ";
    if (fHeader.IsData()) std::cout << "data";
    else if (fHeader.IsMessage()) std::cout << "message";
    else if (fHeader.IsEndRun()) std::cout << "end-run";
    else if (fHeader.IsBeginRun()) std::cout << "begin-run";
    else std::cout << "invalid";
    std::cout << " Context: " << std::hex << fHeader.GetContext();
    std::cout << " Error: "   << std::hex << fHeader.GetErrorCode();
    std::cout << std::endl;

    TROOT::IndentLevel();
    std::cout << "Trigger Word: " << std::hex << fHeader.GetTriggerWord();
    std::cout << std::endl;
    
    if (fHeader.GetMCMSecond() != ND::TRawDataHeader::Invalid) {
        TROOT::IndentLevel();
        std::cout << std::dec << std::setw(11) 
                  << "MCM second:  " << fHeader.GetMCMSecond()
                  << " Sub-Second: " << fHeader.GetMCMSubSecond()
                  << " Busy: " << fHeader.GetMCMTimeSinceBusy()
                  << std::endl;
    }

    if (fHeader.GetP0DSecond() != ND::TRawDataHeader::Invalid) {
        TROOT::IndentLevel();
        std::cout << std::dec << std::setw(11) 
                  << "P0D second:  " << fHeader.GetP0DSecond()
                  << " Sub-Second: " << fHeader.GetP0DSubSecond()
                  << " Busy: " << fHeader.GetP0DTimeSinceBusy()
                  << std::endl;
    }

    if (fHeader.GetTPCSecond() != ND::TRawDataHeader::Invalid) {
        TROOT::IndentLevel();
        std::cout << std::dec << std::setw(11) 
                  << "TPC second:  " << fHeader.GetTPCSecond()
                  << " Sub-Second: " << fHeader.GetTPCSubSecond()
                  << " Busy: " << fHeader.GetTPCTimeSinceBusy()
                  << std::endl;
    }

    if (fHeader.GetFGDSecond() != ND::TRawDataHeader::Invalid) {
        TROOT::IndentLevel();
        std::cout << std::dec << std::setw(11) 
                  << "FGD second:  " << fHeader.GetFGDSecond()
                  << " Sub-Second: " << fHeader.GetFGDSubSecond()
                  << " Busy: " << fHeader.GetFGDTimeSinceBusy()
                  << std::endl;
    }

    if (fHeader.GetECalSecond() != ND::TRawDataHeader::Invalid) {
        TROOT::IndentLevel();
        std::cout << std::dec << std::setw(11) 
                  << "ECal second: " << fHeader.GetECalSecond()
                  << " Sub-Second: " << fHeader.GetECalSubSecond()
                  << " Busy: " << fHeader.GetECalTimeSinceBusy()
                  << std::endl;
    }

    if (fHeader.GetSMRDSecond() != ND::TRawDataHeader::Invalid) {
        TROOT::IndentLevel();
        std::cout << std::dec << std::setw(11) 
                  << "SMRD second: " << fHeader.GetSMRDSecond()
                  << " Sub-Second: " << fHeader.GetSMRDSubSecond()
                  << " Busy: " << fHeader.GetSMRDTimeSinceBusy()
                  << std::endl;
    }

    if (fHeader.GetINGRIDSecond() != ND::TRawDataHeader::Invalid) {
        TROOT::IndentLevel();
        std::cout << std::dec << std::setw(11) 
                  << "INGRID second: " << fHeader.GetINGRIDSecond()
                  << " Sub-Second: " << fHeader.GetINGRIDSubSecond()
                  << " Busy: " << fHeader.GetINGRIDTimeSinceBusy()
                  << std::endl;
    }

    std::cout.fill(' ');
    std::cout.flags(save);
    for (const_iterator v = begin(); v != end(); ++v) {
        (*v)->ls(opt);
    };
    TROOT::DecreaseDirLevel();

}
