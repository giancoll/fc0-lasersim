////////////////////////////////////////////////////////////
// $Id$
//
#include <sstream>
#include <iostream>
#include <iomanip>
#include <string>

#include "TND280Event.hxx"
#include "TEventFolder.hxx"
#include "THitSelection.hxx"
#include "TAlgorithmResult.hxx"
#include "TOADatabase.hxx"
#include "TDigitManager.hxx"
#include "TDigitContainer.hxx"
#include "TRawDataHeader.hxx"

ClassImp(ND::TND280Event);
ClassImp(ND::TND280Event::Header);

ND::TND280Event::TND280Event() {
    /// All other fields are self initializing.
    Build();
}

ND::TND280Event::TND280Event(const ND::TND280Context& context)
    : fContext(context){
    /// All other fields are self initializing.
    Build();
}

// A utility macro to copy sub-detector information from the raw header into
// the main event header.  This also builds the partition code.  If the MCM
// time was not set, then this will copy the sub-detector clock into the MCM
// clock (that's because the sub-detector board was acting as an MCM).
#define COPYCLOCK(CLOCK,RAW,HEADER,PART) {                              \
        if (RAW.Get##CLOCK##Second() != ND::TRawDataHeader::Invalid) {  \
            PART |= ND::TND280Context::k##CLOCK##ClockPresent;          \
            if (!(PART & ND::TND280Context::kGlobalPartition)) {        \
                HEADER.fMCMSecond = RAW.Get##CLOCK##Second();           \
                HEADER.fMCMSubSecond = RAW.Get##CLOCK##SubSecond();     \
                HEADER.fMCMTimeSinceBusy = RAW.Get##CLOCK##TimeSinceBusy(); \
            }                                                           \
            HEADER.f##CLOCK##Second = RAW.Get##CLOCK##Second();         \
            HEADER.f##CLOCK##SubSecond = RAW.Get##CLOCK##SubSecond();   \
            HEADER.f##CLOCK##TimeSinceBusy = RAW.Get##CLOCK##TimeSinceBusy(); \
        }                                                               \
    }        
    
void 
ND::TND280Event::FillHeaderAndContext(const ND::TRawDataHeader& rawHeader) {
    // Build the partition code and copy the clock data.
    int partition = 0;
    if (rawHeader.GetMCMSecond() != ND::TRawDataHeader::Invalid) {
        partition |= ND::TND280Context::kGlobalPartition;
        fHeader.fMCMSecond = rawHeader.GetMCMSecond();
        fHeader.fMCMSubSecond = rawHeader.GetMCMSubSecond();
        fHeader.fMCMTimeSinceBusy = rawHeader.GetMCMTimeSinceBusy();
    }
    if (rawHeader.GetP0DSecond() != ND::TRawDataHeader::Invalid) {
        // could set the MCM fields.
        COPYCLOCK(P0D, rawHeader, fHeader, partition);
    }
    if (rawHeader.GetTPCSecond() != ND::TRawDataHeader::Invalid) {
        // could set the MCM fields.
        COPYCLOCK(TPC, rawHeader, fHeader, partition);
    }
    if (rawHeader.GetFGDSecond() != ND::TRawDataHeader::Invalid) {
        // could set the MCM fields.
        COPYCLOCK(FGD, rawHeader, fHeader, partition);
    }
    if (rawHeader.GetECalSecond() != ND::TRawDataHeader::Invalid) {
        // could set the MCM fields.
        COPYCLOCK(ECal, rawHeader, fHeader, partition);
    }
    if (rawHeader.GetSMRDSecond() != ND::TRawDataHeader::Invalid) {
        // could set the MCM fields.
        COPYCLOCK(SMRD, rawHeader, fHeader, partition);
    }
    if (rawHeader.GetINGRIDSecond() != ND::TRawDataHeader::Invalid) {
        // could set the MCM fields.
        COPYCLOCK(INGRID, rawHeader, fHeader, partition);
    }

    // Fill the rest of the header.
    fHeader.fRecordType = rawHeader.fRecordType;
    fHeader.fRunType = rawHeader.GetRunType();
    fHeader.fTimeStamp = rawHeader.GetTimeStamp();
    fHeader.fErrorCode = rawHeader.GetErrorCode();
    fHeader.fTriggerWord = rawHeader.GetTriggerWord();
    fHeader.fCTMTriggerPattern[0] = rawHeader.GetCTMTriggerPattern(0);
    fHeader.fCTMTriggerPattern[1] = rawHeader.GetCTMTriggerPattern(1);
    fHeader.fCTMTriggerPattern[2] = rawHeader.GetCTMTriggerPattern(2);
    fHeader.fFGDCTMTriggerPattern[0] = rawHeader.GetFGDCTMTriggerPattern(0);
    fHeader.fFGDCTMTriggerPattern[1] = rawHeader.GetFGDCTMTriggerPattern(1);
    fHeader.fFGDCTMTriggerPattern[2] = rawHeader.GetFGDCTMTriggerPattern(2);

    // Build the event context.
    fContext.SetRun(rawHeader.GetRun());

    // Set the event number in the context.  This will usually be provided by
    // the trigger word.  If that doesn't work, then it will come from the
    // midas sequence number (rawHeader.GetSeqNo()).
    if (fHeader.GetEventNumber() == Header::Invalid && fHeader.IsData()) {
        static int ErrorCount = 0;
        if (ErrorCount<4) {
            ND280Error("Incomplete conversion from MIDAS to oaEvent," 
                       << " check oaRawEvent availability.");
            ++ErrorCount;
        }
        fContext.SetEvent(rawHeader.GetSeqNo());
    }
    else {
        fContext.SetEvent(fHeader.GetEventNumber());
    }

    // Set the spill number if we have it (could be invalid).
    fContext.SetSpill(fHeader.GetSpillNumber());

    // Set the context time.  This will usually come from the XRUN bank time,
    // but if that is not available, then the MIDAS time stamp from the
    // rawHeader will be used.  Note that the MIDAS time stamp is the computer
    // time of when the event was written to disk, not the time that the event
    // happened.
    fContext.SetTimeStamp(rawHeader.GetTimeStamp());

    // Set the sub-run number.  This should come from the rawHeader, but since
    // the DAQ doesn't know what sub-run it is writing, we need to read the
    // number from the file name that the DAQ generated when it changed
    // sub-runs.  This is kludged around by providing a preliminary context
    // with the raw event header.  The preliminary context is filled from the
    // MIDAS file name.
    if (rawHeader.GetSubRun() != ND::TRawDataHeader::Invalid) {
        fContext.SetSubRun(rawHeader.GetSubRun());
    }

    // Set the partition code.
    fContext.SetPartition(partition);

}

// Note that information in the raw event overrides information in the
// preliminary context.  The preliminary context is used as a back-up for any
// context not in the raw header.
ND::TND280Event::TND280Event(const ND::TND280Context& prelimContext,
                             ND::TND280RawEvent* raw) {
    const ND::TRawDataHeader& rawHeader = raw->GetHeader();

    fContext.SetSubRun(prelimContext.GetSubRun());
    FillHeaderAndContext(rawHeader);

    // Build the rest of the event structure.
    Build();

    // Finally, add the raw data.
    AddRaw(raw);
}
#undef COPYCLOCK

ND::TND280Event::~TND280Event() {
    ND::TEventFolder::RemoveEvent(this);
}

void ND::TND280Event::Build(void) {
    if (GetRunId() == ND::TND280Context::Invalid
        || GetEventId() == ND::TND280Context::Invalid) {
        SetName("event.uninitialized");
    }
    else {
        std::stringstream name;
        name << "event." 
             << std::setw(5) << std::setfill('0') << GetRunId()
             << std::setw(8) << std::setfill('0') << GetEventId();
        SetName(name.str().c_str());
    }
    if (!FindDatum("digits")) {
        AddDatum(new ND::TDataVector("digits","Uncalibrated digit data"));
    }
    if (!FindDatum("hits")) {
        AddDatum(new ND::TDataVector("hits","Calibrated hit data"));
    }
    if (!FindDatum("fits")) {
        AddDatum(new ND::TDataVector("fits","Fit results from reconstruction"));
    }
    if (!FindDatum("tags")) {
        AddDatum(new ND::TDataVector("tags","Generator information etc."));
    }
    Register();
}

const unsigned int ND::TND280Event::Header::Invalid = 0xDEADBEEF;

ND::TND280Event::Header::~Header() {}

ND::TND280Event::Header::Header() {
    fRecordType = Data;
    fTimeStamp = Invalid;
    fRunType = Invalid;
    fErrorCode = Invalid;
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
    fCTMTriggerPattern[0] = Invalid;
    fCTMTriggerPattern[1] = Invalid;
    fCTMTriggerPattern[2] = Invalid;
    fFGDCTMTriggerPattern[0] = Invalid;
    fFGDCTMTriggerPattern[1] = Invalid;
    fFGDCTMTriggerPattern[2] = Invalid;
}

void ND::TND280Event::ls(Option_t* opt) const {
    ND::TDatum::ls(opt);
    TROOT::IncreaseDirLevel();
    TROOT::IndentLevel();
    std::cout << "Context: " << GetContext() << std::endl;
    std::ios::fmtflags save = std::cout.flags();
    TROOT::IndentLevel();
    std::cout << "Stamp: " << fHeader.GetTimeStamp();
    std::cout << " Record-type: ";
    if (fHeader.IsData()) std::cout << "data";
    else if (fHeader.IsMessage()) std::cout << "message";
    else if (fHeader.IsEndRun()) std::cout << "end-run";
    else if (fHeader.IsBeginRun()) std::cout << "begin-run";
    else std::cout << "invalid";
    std::cout << " Error: "   << std::hex << fHeader.GetErrorCode();
    std::cout << std::endl;

    TROOT::IndentLevel();
    std::cout << "Trigger Word::";
    std::cout << std::hex << std::setfill('0') << std::setw(16)
              << fHeader.GetTriggerWord();
    std::cout << " bits: " << std::hex << fHeader.GetTriggerBits();
    std::cout << " spill: " << std::dec << fHeader.GetSpillNumber();
    std::cout << " event: " << fHeader.GetEventNumber();
    std::cout << std::endl;
    std::cout.fill(' ');
    std::cout.flags(save);
    
    if (fHeader.GetMCMSecond() != ND::TRawDataHeader::Invalid) {
        TROOT::IndentLevel();
        std::cout << std::dec << std::setw(11) 
                  << "MCM  second: " << fHeader.GetMCMSecond()
                  << " Sub-Second: " << fHeader.GetMCMSubSecond()
                  << " Busy: " << fHeader.GetMCMTimeSinceBusy()
                  << std::endl;
    }

    if (fHeader.GetP0DSecond() != ND::TRawDataHeader::Invalid) {
        TROOT::IndentLevel();
        std::cout << std::dec << std::setw(11) 
                  << "P0D  second: " << fHeader.GetP0DSecond()
                  << " Sub-Second: " << fHeader.GetP0DSubSecond()
                  << " Busy: " << fHeader.GetP0DTimeSinceBusy()
                  << std::endl;
    }

    if (fHeader.GetTPCSecond() != ND::TRawDataHeader::Invalid) {
        TROOT::IndentLevel();
        std::cout << std::dec << std::setw(11) 
                  << "TPC  second: " << fHeader.GetTPCSecond()
                  << " Sub-Second: " << fHeader.GetTPCSubSecond()
                  << " Busy: " << fHeader.GetTPCTimeSinceBusy()
                  << std::endl;
    }

    if (fHeader.GetFGDSecond() != ND::TRawDataHeader::Invalid) {
        TROOT::IndentLevel();
        std::cout << std::dec << std::setw(11) 
                  << "FGD  second: " << fHeader.GetFGDSecond()
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
    TROOT::IndentLevel();
    std::cout << "TFB CTM "
              << std::hex << std::setfill('0')
              << std::setw(16) << fHeader.GetCTMTriggerPattern(0)
              << std::setw(16) << fHeader.GetCTMTriggerPattern(1)
              << std::setw(16) << fHeader.GetCTMTriggerPattern(2)
              << std::endl;
    std::cout.fill(' ');

    TROOT::IndentLevel();
    std::cout << "FGD CTM "
              << std::hex << std::setfill('0')
              << std::setw(16) << fHeader.GetFGDCTMTriggerPattern(0)
              << std::setw(16) << fHeader.GetFGDCTMTriggerPattern(1)
              << std::setw(16) << fHeader.GetFGDCTMTriggerPattern(2)
              << std::endl;
    std::cout.fill(' ');

    if (GetGeometryHash().Valid()) {
        TROOT::IndentLevel();
        std::cout << "Geometry:  "
                  << GetGeometryHash()
                  << std::endl;
    }

    if (GetAlignmentId().Valid()) {
        TROOT::IndentLevel();
        std::cout << "Alignment: "
                  << GetAlignmentId()
                  << std::endl;
    }

    std::cout.flags(save);
    for (const_iterator v = begin();
         v != end(); 
         ++v) {
        (*v)->ls(opt);
    }
    std::cout.fill(' ');
    std::cout.flags(save);
    TROOT::DecreaseDirLevel();
}

void ND::TND280Event::Register(void) {
    ND::TEventFolder::RegisterEvent(this);
}

ND::THandle<ND::TDigitContainer> ND::TND280Event::GetDigits(const char* name) {
    return ND::TOADatabase::Get().Digits().CacheDigits(*this,name);
}

ND::THandle<ND::THitSelection> ND::TND280Event::GetHitSelection(const char* name) 
    const {
    std::string hitName("hits/");
    hitName += name;
    ND::THandle<ND::TDatum> hitsData = Get<ND::TDatum>(hitName.c_str());
    if (!hitsData) return ND::THandle<ND::THitSelection>();
    return Get<ND::THitSelection>(hitName.c_str());
}

ND::THandle<ND::TAlgorithmResult> ND::TND280Event::GetFit(const char* name) const {
    std::string fitName = "~/fits/";
    return Get<ND::TAlgorithmResult>(fitName + name);
}

void ND::TND280Event::AddFit(ND::TAlgorithmResult* fit, const char* name) {
    ND::THandle<ND::TDataVector> fits = Get<ND::TDataVector>("~/fits");
    fits->AddDatum(fit,name);
}

void ND::TND280Event::AddFit(ND::THandle<ND::TAlgorithmResult> fit, const char* name) {
    ND::THandle<ND::TDataVector> fits = Get<ND::TDataVector>("~/fits");
    fit.Release();
    fits->AddDatum(GetPointer(fit),name);
}

void ND::TND280Event::AddRaw(ND::TND280RawEvent* rawEvent) {
    ND::THandle<ND::TDataVector> raw = Get<ND::TDataVector>("~/raw");
    if (!raw) {
        AddDatum(new ND::TDataVector("raw","Raw MIDAS data for this event."));
        raw = Get<ND::TDataVector>("~/raw");
    }
    raw->AddDatum(rawEvent);
}

ND::THandle<ND::TND280RawEvent> ND::TND280Event::GetRaw(int i) const {
    if (i<0) return ND::THandle<ND::TND280RawEvent>();
    ND::THandle<ND::TDataVector> raw = Get<ND::TDataVector>("~/raw");
    if (!raw) return ND::THandle<ND::TND280RawEvent>();
    if ((int) raw->size()<=i) return ND::THandle<ND::TND280RawEvent>();
    return ND::THandle<ND::TND280RawEvent>(raw->At<ND::TND280RawEvent>(i),
                                           false);
}
