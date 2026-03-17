#include "TND280Log.hxx"
#include "TRawDataHeader.hxx"

ClassImp(ND::TRawDataHeader);

const unsigned int ND::TRawDataHeader::Invalid = 0xdeadbeef;
//_____________________________________________________________________________
    
ND::TRawDataHeader::TRawDataHeader() {
    Init();
}

//_____________________________________________________________________________
    
ND::TRawDataHeader::TRawDataHeader(const TRawDataHeader& rhs) {
    Init();
    *this = rhs;
}

//_____________________________________________________________________________

ND::TRawDataHeader::~TRawDataHeader() {

    ND280Trace("ND::TRawDataHeader: dtor at:" << (void*) this);

}


//_____________________________________________________________________________

void ND::TRawDataHeader::Print(const Option_t* opt /* = "" */) const {

  ND280Log("Header: Run number " << std::dec << this->GetRun() 
	     << " MIDAS Sequence number " << this->GetSeqNo()
	     << " Unix time stamp " << this->GetTimeStamp());
}

void ND::TRawDataHeader::Init() {
    fContext = Invalid;
    fRecordType = Invalid;
    fRun = Invalid;
    fSubRun = Invalid; 
    fTimeStamp = Invalid;
    fRunType = Invalid;
    fSeqNo = Invalid;
    fErrorCode = 0;
    fTriggerWord = Invalid;
    fMCMSecond = Invalid;
    fMCMSubSecond = Invalid;
    fMCMTimeSinceBusy = Invalid;
    fP0DSecond = Invalid;
    fP0DSubSecond = Invalid;
    fP0DTimeSinceBusy = Invalid;
    fTPCSecond = Invalid;
    fTPCSubSecond = Invalid;
    fTPCTimeSinceBusy = Invalid;
    fFGDSecond = Invalid;
    fFGDSubSecond = Invalid;
    fFGDTimeSinceBusy = Invalid;
    fECalSecond = Invalid;
    fECalSubSecond = Invalid;
    fECalTimeSinceBusy = Invalid;
    fSMRDSecond = Invalid;
    fSMRDSubSecond = Invalid;
    fSMRDTimeSinceBusy = Invalid;
    fINGRIDSecond = Invalid;
    fINGRIDSubSecond = Invalid;
    fINGRIDTimeSinceBusy = Invalid;
    fUpgradeSecond = Invalid;
    fUpgradeSubSecond = Invalid;
    fUpgradeTimeSinceBusy = Invalid;
    fCTMTriggerPattern[0] = Invalid;
    fCTMTriggerPattern[1] = Invalid;
    fCTMTriggerPattern[2] = Invalid;
    fFGDCTMTriggerPattern[0] = Invalid;
    fFGDCTMTriggerPattern[1] = Invalid;
    fFGDCTMTriggerPattern[2] = Invalid;
}
