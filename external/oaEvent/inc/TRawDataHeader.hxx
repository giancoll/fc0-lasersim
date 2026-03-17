#ifndef TRawDataHeader_hxx_seen
#define TRawDataHeader_hxx_seen

// Jan 2008: We don't need to include <stream> but without it a bug in
// rootcint 5/18.00 means that the dictionary is missing Streamer and
// ShowMember code and loading the classes produces the warning:-
//
//  Cannot find any ShowMembers function for ND::TRawDataHeader!
//
// rootcint 5.20 shows the same problem but 5.22 is fine.
#include <string>

#include <Rtypes.h>

typedef Int_t TRawContext;

namespace ND {
    class TND280Event;
    class TRawDataHeader;
}

/// The TRawDataHeader class records information that is global to a single
/// MIDAS event held in a TDN280RawEvent.  All of the information in the
/// TRawDataHeader will be available in a more convenient form through the
/// TND280Event header interface.  This class is used to fill the TND280Event
/// information, and should not be used in regular code.
///
/// Currently this is work in progress, there is very little
/// implementation and the state and API will almost certainly change.
class ND::TRawDataHeader  {
    friend class ND::TND280Event;

public:
    /// A special field value that specifies that the field is invalid.
    static const unsigned int Invalid;
    
    /// Do not allow the implicit constructor.
    explicit TRawDataHeader();
    
    TRawDataHeader(const TRawDataHeader& rhs);
    virtual ~TRawDataHeader();
    
    /// Print contents to ND280Log
    void Print(const Option_t* opt = "") const;
    
    /// Get the raw context for the event.  This is not documented for the
    /// DAQ.  The context contains information about the (detector, data type,
    /// date and time).
    TRawContext GetContext() const { return fContext; }
    
    /// Get the run number saved from the XRUN bank
    UInt_t    GetRun() const        { return fRun; }
    
    /// Get the subrun if it's available.  Return TRawDataHeader::Invalid if
    /// it is not available.
    UInt_t    GetSubRun() const     { return fSubRun; }
    
    /// Get the run type if it's available.  Return TRawDataHeader::Invalid if
    /// it is not available.  The run types are not documented for the DAQ.
    UInt_t    GetRunType() const    { return fRunType; }
    
    /// Get the sequence number of the event.  This is equivalent to the event
    /// number.
    UInt_t    GetSeqNo() const      { return fSeqNo; }
    
    /// Get the time stamp of this event from the event builder.  This is set
    /// by DAQ, not the hardware.  Accurate times should be derived from the
    /// MCM clocks which are synchronized to GPS.
    UInt_t   GetTimeStamp() const  { return fTimeStamp; }
    
    /// Get any error codes associated with this event.  The event codes are
    /// not documented for the DAQ.
    UInt_t   GetErrorCode() const  { return fErrorCode; }
    
    /// Return the trigger word for this event.  The trigger word is read from
    /// the MCM, however it should be the same in ALL banks.
    ///
    /// Trigger word definition (64 bit unsigned long):
    ///     - Bits  0-31:  Event number
    ///     - Bits 32-47: Spill number
    ///     - Bits 48-57: Trigger bits
    ///         - Bit 48: Beam spill
    ///         - Bit 49: Pedestal/noise
    ///         - Bit 50: TPC laser
    ///         - Bit 51: POD LED
    ///         - Bit 52: ECAL LED
    ///         - Bit 53: FGD LED
    ///         - Bit 54: FEE calibration
    ///         - Bit 55: TRIP-t cosmic
    ///         - Bit 56: Reserved for FGD cosmic
    ///         - Bit 57: unused
    ///     - Bits 58-63: Instruction
    ///         - Bit 58: Reserved for start-of-run
    ///         - Bit 59: Reserved for end-of-run
    ///         - Bit 60: 1 PPS / counter reset
    ///         - Bit 61: Go to spill mode
    ///         - Bit 62: Go to cosmic mode
    ///         - Bit 63: Decline CTM request/abort
    ///
    ULong64_t GetTriggerWord() const {return fTriggerWord;}

    /// Get the MCM unix time of the last GPS pulse per second.  This is
    /// syncronized to the GPS pulse per second.  Note that the pulse per
    /// second signal is inhibited for 100 micro seconds before a beam
    /// trigger, so this may be more than one second in the past.
    UInt_t  GetMCMSecond() const {return fMCMSecond;}
    
    /// Get the MCM 100 MHz counter since last pulse per second.  This could
    /// be greater than 1 second if the pulse per second was inhibited due to
    /// a beam trigger.
    UInt_t  GetMCMSubSecond() const {return fMCMSubSecond;}

    /// Get the 100 MHz counter since the last busy state.  For a beam spill,
    /// this is expected to be between 5000 and 10000 (50 ms to 100 ms).
    UInt_t  GetMCMTimeSinceBusy() const {return fMCMTimeSinceBusy;}
 
    /// Get the clock module time for the P0D.  See the documentation for
    /// GetMCMSecond().  If the P0D clock module is not present in the event,
    /// this will be Invalid.
    UInt_t  GetP0DSecond() const {return fP0DSecond;}

    /// Get the P0D 100 MHz counter since last pulse per second.  This could
    /// be greater than 1 second if the pulse per second was inhibited due to
    /// a beam trigger.  See GetMCMSubSecond() for more documentation.
    UInt_t  GetP0DSubSecond() const {return fP0DSubSecond;}

    /// Get the 100 MHz counter since the last busy state.  For a beam spill,
    /// this is expected to be between 5000 and 10000 (50 ms to 100 ms).
    UInt_t  GetP0DTimeSinceBusy() const {return fP0DTimeSinceBusy;}
 
    /// Get the clock module time for the TPC.  See the documentation for
    /// GetMCMSecond().  If the TPC clock module is not present in the event,
    /// this will be Invalid.
    UInt_t  GetTPCSecond() const {return fTPCSecond;}

    /// Get the TPC 100 MHz counter since last pulse per second.  This could
    /// be greater than 1 second if the pulse per second was inhibited due to
    /// a beam trigger.  See GetMCMSubSecond() for more documentation.
    UInt_t  GetTPCSubSecond() const {return fTPCSubSecond;}

    /// Get the 100 MHz counter since the last busy state.  For a beam spill,
    /// this is expected to be between 5000 and 10000 (50 ms to 100 ms).
    UInt_t  GetTPCTimeSinceBusy() const {return fTPCTimeSinceBusy;}
 
    /// Get the clock module time for the FGD.  See the documentation for
    /// GetMCMSecond().  If the FGD clock module is not present in the event,
    /// this will be Invalid.
    UInt_t  GetFGDSecond() const {return fFGDSecond;}

    /// Get the FGD 100 MHz counter since last pulse per second.  This could
    /// be greater than 1 second if the pulse per second was inhibited due to
    /// a beam trigger.  See GetMCMSubSecond() for more documentation.
    UInt_t  GetFGDSubSecond() const {return fFGDSubSecond;}

    /// Get the 100 MHz counter since the last busy state.  For a beam spill,
    /// this is expected to be between 5000 and 10000 (50 ms to 100 ms).
    UInt_t  GetFGDTimeSinceBusy() const {return fFGDTimeSinceBusy;}
 
    /// Get the clock module time for the ECal.  See the documentation for
    /// GetMCMSecond().  If the ECal clock module is not present in the event,
    /// this will be Invalid.
    UInt_t  GetECalSecond() const {return fECalSecond;}

    /// Get the ECal 100 MHz counter since last pulse per second.  This could
    /// be greater than 1 second if the pulse per second was inhibited due to
    /// a beam trigger.  See GetMCMSubSecond() for more documentation.
    UInt_t  GetECalSubSecond() const {return fECalSubSecond;}

    /// Get the 100 MHz counter since the last busy state.  For a beam spill,
    /// this is expected to be between 5000 and 10000 (50 ms to 100 ms).
    UInt_t  GetECalTimeSinceBusy() const {return fECalTimeSinceBusy;}
 
    /// Get the clock module time for the SMRD.  See the documentation for
    /// GetMCMSecond().  If the SMRD clock module is not present in the event,
    /// this will be Invalid.
    UInt_t  GetSMRDSecond() const {return fSMRDSecond;}

    /// Get the SMRD 100 MHz counter since last pulse per second.  This could
    /// be greater than 1 second if the pulse per second was inhibited due to
    /// a beam trigger.  See GetMCMSubSecond() for more documentation.
    UInt_t  GetSMRDSubSecond() const {return fSMRDSubSecond;}

    /// Get the 100 MHz counter since the last busy state.  For a beam spill,
    /// this is expected to be between 5000 and 10000 (50 ms to 100 ms).
    UInt_t  GetSMRDTimeSinceBusy() const {return fSMRDTimeSinceBusy;}
 
    /// Get the clock module time for the INGRID.  See the documentation for
    /// GetMCMSecond().  If the INGRID clock module is not present in the event,
    /// this will be Invalid.
    UInt_t  GetINGRIDSecond() const {return fINGRIDSecond;}

    /// Get the INGRID 100 MHz counter since last pulse per second.  This could
    /// be greater than 1 second if the pulse per second was inhibited due to
    /// a beam trigger.  See GetMCMSubSecond() for more documentation.
    UInt_t  GetINGRIDSubSecond() const {return fINGRIDSubSecond;}

    /// Get the 100 MHz counter since the last busy state.  For a beam spill,
    /// this is expected to be between 5000 and 10000 (50 ms to 100 ms).
    UInt_t  GetINGRIDTimeSinceBusy() const {return fINGRIDTimeSinceBusy;}
 
    /// Get the clock module time for the Upgrade.  See the documentation for
    /// GetMCMSecond().  If the Upgrade clock module is not present in the event,
    /// this will be Invalid.
    UInt_t  GetUpgradeSecond() const {return fUpgradeSecond;}

    /// Get the Upgrade 100 MHz counter since last pulse per second.  This could
    /// be greater than 1 second if the pulse per second was inhibited due to
    /// a beam trigger.  See GetMCMSubSecond() for more documentation.
    UInt_t  GetUpgradeSubSecond() const {return fUpgradeSubSecond;}

    /// Get the 100 MHz counter since the last busy state.  For a beam spill,
    /// this is expected to be between 5000 and 10000 (50 ms to 100 ms).
    UInt_t  GetUpgradeTimeSinceBusy() const {return fUpgradeTimeSinceBusy;}

    /// Get one of the 64 bit fields containing the CTM trigger bit pattern.
    ULong64_t GetCTMTriggerPattern(int i) const {return fCTMTriggerPattern[i];}

    /// Get one of the 64 bit fields containing the trigger bit pattern for
    /// the FGD.
    ULong64_t GetFGDCTMTriggerPattern(int i) const {return fFGDCTMTriggerPattern[i];}

    /// @{ Setters associated with the data defined above.
    void SetContext(const TRawContext& context) { fContext = context; }
    void SetRun(UInt_t run)                      { fRun = run; }
    void SetSubRun(UInt_t sub_run)               { fSubRun = sub_run; }
    void SetRunType(UInt_t run_type)             { fRunType = run_type; }
    void SetSeqNo(UInt_t seq_no)                 { fSeqNo = seq_no; }
    void SetTimeStamp(UInt_t time_stamp)        { fTimeStamp = time_stamp; }
    void SetErrorCode(UInt_t error_code)        { fErrorCode = error_code; }
    void SetTriggerWord(ULong64_t i) {fTriggerWord = i;}
    void SetMCMSecond(UInt_t i) {fMCMSecond = i;}
    void SetMCMSubSecond(UInt_t i) {fMCMSubSecond = i;}
    void SetMCMTimeSinceBusy(UInt_t i) {fMCMTimeSinceBusy = i;}
    void SetP0DSecond(UInt_t i) {fP0DSecond = i;}
    void SetP0DSubSecond(UInt_t i) {fP0DSubSecond = i;}
    void SetP0DTimeSinceBusy(UInt_t i) {fP0DTimeSinceBusy = i;}
    void SetTPCSecond(UInt_t i) {fTPCSecond = i;}
    void SetTPCSubSecond(UInt_t i) {fTPCSubSecond = i;}
    void SetTPCTimeSinceBusy(UInt_t i) {fTPCTimeSinceBusy = i;}
    void SetFGDSecond(UInt_t i) {fFGDSecond = i;}
    void SetFGDSubSecond(UInt_t i) {fFGDSubSecond = i;}
    void SetFGDTimeSinceBusy(UInt_t i) {fFGDTimeSinceBusy = i;}
    void SetECalSecond(UInt_t i) {fECalSecond = i;}
    void SetECalSubSecond(UInt_t i) {fECalSubSecond = i;}
    void SetECalTimeSinceBusy(UInt_t i) {fECalTimeSinceBusy = i;}
    void SetSMRDSecond(UInt_t i) {fSMRDSecond = i;}
    void SetSMRDSubSecond(UInt_t i) {fSMRDSubSecond = i;}
    void SetSMRDTimeSinceBusy(UInt_t i) {fSMRDTimeSinceBusy = i;}
    void SetINGRIDSecond(UInt_t i) {fINGRIDSecond = i;}
    void SetINGRIDSubSecond(UInt_t i) {fINGRIDSubSecond = i;}
    void SetINGRIDTimeSinceBusy(UInt_t i) {fINGRIDTimeSinceBusy = i;}
    void SetUpgradeSecond(UInt_t i) {fUpgradeSecond = i;}
    void SetUpgradeSubSecond(UInt_t i) {fUpgradeSubSecond = i;}
    void SetUpgradeTimeSinceBusy(UInt_t i) {fUpgradeTimeSinceBusy = i;}
    void SetCTMTriggerPattern(int i, ULong64_t v) {fCTMTriggerPattern[i] = v;}
    void SetFGDCTMTriggerPattern(int i, ULong64_t v) {fFGDCTMTriggerPattern[i] = v;}
    /// @} 

    /// Record types
    typedef enum {BeginRun = 0,
		  EndRun,
		  Message,
		  Data} RecordTypes;

    /// @{ Check the run type.  
    ///    - IsBeginRun -- This event contains begin of run information.
    ///    - IsEndRun   -- This event contains end of run information.
    ///    - IsData     -- This is an actual data event.
    ///    - IsMessage  -- This event contains message data banks.  The
    ///           message type will be infered from the banks in the event. 
    Bool_t IsBeginRun() const { return fRecordType == BeginRun;}
    Bool_t IsEndRun() const   { return fRecordType == EndRun;}
    Bool_t IsMessage() const  { return fRecordType == Message;}
    Bool_t IsData() const     { return fRecordType == Data;}
    /// @}

    /// Set the record type.
    void SetRecordType(RecordTypes recType) { fRecordType = recType; }

private:
    void Init();

    /// The context (detector, data type, date and time)
    TRawContext fContext;

    /// Record Type: One of BeginRun, EndRun, Message, or Data
    UInt_t fRecordType;

    /// The run number from the DAQ
    UInt_t fRun;

    /// Subrun number.  This is a "pseudo" field since it's really just the
    /// index of the file currently being written.  May not be filled.
    UInt_t fSubRun; 

    /// Unix event timestamp in seconds.  This is the time derived from the
    /// event builder.
    UInt_t fTimeStamp;

    /// Run type
    UInt_t fRunType;

    /// MIDAS Sequence number
    UInt_t fSeqNo;

    /// Error code
    UInt_t fErrorCode;

    /// The trigger word.  See GetTriggerWord() for documentation.
    ULong64_t fTriggerWord;

    /// The pulse per second counter for the MCM.
    UInt_t fMCMSecond;

    /// The 100 MHz counter since the last pulse per second for the MCM.
    UInt_t fMCMSubSecond;

    /// The time since the MCM was previously busy.
    UInt_t fMCMTimeSinceBusy;

    /// The pulse per second counter for the P0D.
    UInt_t fP0DSecond;

    /// The 100 MHz counter since the last pulse per second for the P0D.
    UInt_t fP0DSubSecond;

    /// The time since the P0D was previously busy.
    UInt_t fP0DTimeSinceBusy;

    /// The pulse per second counter for the TPC.
    UInt_t fTPCSecond;

    /// The 100 MHz counter since the last pulse per second for the TPC.
    UInt_t fTPCSubSecond;

    /// The time since the TPC was previously busy.
    UInt_t fTPCTimeSinceBusy;

    /// The pulse per second counter for the FGD.
    UInt_t fFGDSecond;

    /// The 100 MHz counter since the last pulse per second for the FGD.
    UInt_t fFGDSubSecond;

    /// The time since the FGD was previously busy.
    UInt_t fFGDTimeSinceBusy;

    /// The pulse per second counter for the ECal.
    UInt_t fECalSecond;

    /// The 100 MHz counter since the last pulse per second for the ECal.
    UInt_t fECalSubSecond;

    /// The time since the ECal was previously busy.
    UInt_t fECalTimeSinceBusy;

    /// The pulse per second counter for the SMRD.
    UInt_t fSMRDSecond;

    /// The 100 MHz counter since the last pulse per second for the SMRD.
    UInt_t fSMRDSubSecond;

    /// The time since the SMRD was previously busy.
    UInt_t fSMRDTimeSinceBusy;

    /// The pulse per second counter for the INGRID.
    UInt_t fINGRIDSecond;

    /// The 100 MHz counter since the last pulse per second for the INGRID.
    UInt_t fINGRIDSubSecond;

    /// The time since the INGRID was previously busy.
    UInt_t fINGRIDTimeSinceBusy;

    /// The pulse per second counter for the Upgrade SCM.
    UInt_t fUpgradeSecond;

    /// The 100 MHz counter since the last pulse per second for the Upgrade SCM.
    UInt_t fUpgradeSubSecond;

    /// The time since the Upgrade SCM was previously busy.
    UInt_t fUpgradeTimeSinceBusy;

    /// The bit pattern from the cosmic trigger module.
    ULong64_t fCTMTriggerPattern[3];

    /// The bit pattern for the FGD cosmic trigger module (if present).
    ULong64_t fFGDCTMTriggerPattern[3];

    ClassDef(TRawDataHeader,5);
};

#endif
