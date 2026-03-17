#ifndef TND280Event_hxx_seen
#define TND280Event_hxx_seen

#include <string>

#include "TObject.h"

#include "TND280Log.hxx"

#include "TDataVector.hxx"
#include "THandle.hxx"
#include "THitSelection.hxx"
#include "TAlgorithmResult.hxx"
#include "TND280Context.hxx"
#include "TND280RawEvent.hxx"
#include "TDigitContainer.hxx"
#include "ND280TriggerWord.hxx"
#include "TSHAHashValue.hxx"
#include "TAlignmentId.hxx"

/// The standard namespace for nd280 offline software (see namespaces).  
namespace ND {
    class TND280Event;
}

/// The event container (get the current event using
/// TEventFolder::GetCurrentEvent()).
///
/// Provide the highest level event container for ND280 off-axis events.  This
/// is a TDataVector and provides an effective "root directory" for all of the
/// information saved with an event.  This also contains the infrastructure to
/// interface with TEventFolder class that couples events to the ROOT browser
/// and will appear as a root folder in //root/Events/ named by the event
/// number.  For example, an event that is read will appear as a folder,
/// "//root/Events/event.0.00023.00001324".
///
/// The TND280Event class provides the following event structure.
///
/// - event/
///   - raw/    Raw MIDAS data (if it is available).  This directory won't
///             exist if the raw data is not available for this event. 
///   - digits/ Raw data for the sensor hits.  This is a copy of the data 
///             available from the MIDAS DAQ data files.
///   - hits/   Data about the sensor hits.  This contains calibrated hit
///             data. 
///   - truth/  Exists only for MC events.  This is all of the information 
///             saved from the MC.   
///   - fits/   Results from fitters.  This is a data vector full of results 
///             saved from any fitter run on this event.
///
/// TND280Event objects are usually read from an input file using a
/// TND280Input object (output is with a TND280Output).  The last event read
/// will be available from TEventFolder::GetCurrentEvent() which is a static
/// method.  Interactive programs which want to provide a method to keep
/// multiple events in memory (e.g. the event display), should get the current
/// event using the TEventFolder::GetCurrentEvent() method.  Events can then
/// be saved automatically into a root folder by calling the
/// TEventFolder::GetEventFolder() static method.
///
/// The event Header information can be accessed using the GetHeader() method.
/// For instance, to get the MCM time from an event, a program would use the
/// following snippet.
///
/// \code
/// unsigned int mcmTime = event.GetHeader().GetMCMSecond();
/// \endcode
///
/// The trigger type of an event can be conveniently checked using
/// CheckTrigger() which is used:
///
/// \code
/// if (event.CheckTrigger(ND::Trigger::kBeamSpill)) {
///     This is a beam spill...
/// }
/// \endcode
class ND::TND280Event: public TDataVector {
public:
    // Class defined below...
    class Header;

    TND280Event();

    /// Create a new TND280Event with the context, but not the header, filled.
    TND280Event(const ND::TND280Context& context);

    /// Create a new TND280Event from a TND280RawEvent pointer.  The input
    /// context contains extra information foraged from the midas file that is
    /// not available in the raw event.  Information found in the raw header
    /// overrides information in the preliminary context.  The new TND280Event
    /// takes ownership of the raw event and places it in the "raw" folder.
    TND280Event(const ND::TND280Context& prelimContext, 
                ND::TND280RawEvent* raw);

    virtual ~TND280Event();

    /// Get the context for this event.
    const ND::TND280Context& GetContext() const {return fContext;}

    /// Get the hash value of the geometry associated with this event.  If the
    /// event doesn't have a particular geometry associated with it, then
    /// the hash value will be zero (This happens for files processed with
    /// early versions of the calibration).  The hash code is used to
    /// associate the proper geometry with the event when determining the
    /// positions of the hits (the hit positions are not saved with the
    /// event).
    const ND::TSHAHashValue& GetGeometryHash() const {return fGeometryHash;}

    /// Get the alignment id of the alignment constants associated with this
    /// event.  The alignment identifier is a SHA1 hash code of the alignment
    /// constats plus a documentation string.  If the event doesn't have a
    /// particular alignment associated with it, then the hash code will be
    /// zero (this happens for files processed with early versions of the
    /// calibration).  The hash code is used to associate the proper alignment
    /// constants with the event when determining the positions of the hits
    /// (the hit positions are not saved with the event).
    const ND::TAlignmentId& GetAlignmentId() const {return fAlignmentId;}

    /// Get the header information for this event
    const ND::TND280Event::Header& GetHeader() const {return fHeader;}

    /// Conveniently check if the event contains particular trigger bit.
    /// This checks the bit value against the trigger word and returns
    /// true if the bit is set.  For instance, to check for a beam spill
    /// trigger, code should do:
    ///
    /// \code
    /// if (event.CheckTrigger(ND::Trigger::kBeamSpill) {
    ///    This is a beam spill trigger.
    /// }
    /// \endcode
    ///
    /// The trigger bits are defined by the
    /// ND::Trigger::Bits enumeration.  The some commonly
    /// used definitions are:
    ///
    ///   - ND::Trigger::kBeamSpill
    ///   - ND::Trigger::kTFBCosmic
    ///   - ND::Trigger::kFGDCosmic
    ///
    bool CheckTrigger(ND::Trigger::Bits bit) {
        return (GetHeader().GetTriggerBits() & bit);
    }

    /// Set the context for this event.
    void SetContext(ND::TND280Context context) {
        fContext = context;
        Build();
    }

    /// Set the hash value for the geometry.  This should be used by the
    /// calibrations as the hits are created.
    void SetGeometryHash(const TSHAHashValue& h) {fGeometryHash = h;}
    
    /// Set the alignment identifier for the event.  This should be used by the
    /// calibrations as the hits are created.
    void SetAlignmentId(const TAlignmentId& h) {fAlignmentId = h;}
    
    /// Set the event number.
    void SetRunId(int i) {
        fContext.SetRun(i);
        Build();
    }
    
    /// Get the event number.
    int GetRunId(void) const {return fContext.GetRun();}

    /// Set the event number.
    void SetEventId(int i) {
        fContext.SetEvent(i);
        Build();
    }
    
    /// Get the event number.
    int GetEventId(void) const {return fContext.GetEvent();}

    /// Get the digit container by name (may not exist).  This will get the
    /// digits for a sub-detector.  They will be available for if eventUnpack can
    /// be loaded, or if digits were saved by the detResponseSim.  
    /// 
    /// \note (For advanced users) If the event contains the raw data, this
    /// will try to unpack the digits from MIDAS using eventUnpack.  By default,
    /// the cached digits are saved as temporary datum, but can be made
    /// persistent using TOADatabase::Get().Digits().PersistentDigits().  If
    /// the digits are made to be persistent, then they will continue to be
    /// available even if the TND280RawEvent datum is deleted.
    THandle<ND::TDigitContainer> GetDigits(const char *name);

    /// Get the hit selection by name.
    THandle<ND::THitSelection> GetHitSelection(const char *name) const;

    /// Get a TAlgorithmResult by name.
    THandle<ND::TAlgorithmResult> GetFit(const char* name) const;

    /// Add a TAlgorithmResult object.
    void AddFit(TAlgorithmResult* fit, const char* name=NULL);
    void AddFit(THandle<ND::TAlgorithmResult> fit, const char* name=NULL);

    /// Get a handle to a TND280RawEvent for this event.  
    THandle<ND::TND280RawEvent> GetRaw(int i=0) const;

    /// Add a TND280RawEvent to this event.  Adding a TND280RawEvent will
    /// create the ~/raw folder.
    void AddRaw(TND280RawEvent* rawEvent);

    /// Print the object information.
    virtual void ls(Option_t *opt = "") const;

    void Register();

    /// Get a copy of the header that can be modified.  This is provided so
    /// that the MC can build a header.  
    ///
    /// This is used by creating a local writable event header (see the
    /// tutHeader.cxx test for a working example) and filling the fields being
    /// set.  For example
    /// 
    /// \code
    /// class TWritableMCHeader : public ND::TND280EventHeader {
    /// public:
    ///     TWritableMCHeader() {}
    ///     void SetMCMSecond(UInt_t s) {fMCMSecond = s;}
    /// }
    /// \endcode
    /// 
    /// This can then be used as follows:
    /// \code
    /// TND280Event event;
    /// TWritableMCHeader mcHeader;
    /// 
    /// mcHeader.SetMCMSecond(42);
    /// event.SetHeader() = mcHeader;
    /// \endcode
    /// 
    /// Why not just add setters?  For the most part, the header is copied
    /// from the MIDAS data and values will never be set by a user.  Adding
    /// setters for the very rare use case would require adding almost a dozen
    /// new methods, most of which would never be used.  The current situation
    /// allow the rare place where header values need to be set (e.g. detResponseSim)
    /// have full control of the header internal values, while keeping the
    /// interface clean.
    ND::TND280Event::Header& SetHeader() {return fHeader;}

    /// Fill the header and context information from the raw event header.
    void FillHeaderAndContext(const ND::TRawDataHeader& rawHeader);

    /// All of the global event header information copied from the raw data.
    /// The information is copied from the MIDAS file as it is translated into
    /// a TND280Event.  In addition to being a simple copy of the data, this
    /// provides more convenient access to the data.
    class Header {
        friend class ND::TND280Event;
    public:
        /// A special field value that specifies that the field is invalid.
        static const unsigned int Invalid;
        Header();
        virtual ~Header();

        /// Get the run type if it's available.  Return
        /// TRawDataHeader::Invalid if it is not available.  The run types are
        /// not documented for the DAQ.
        UInt_t    GetRunType() const    { return fRunType; }
    
        /// Get the time stamp of this event from the event builder.  This is
        /// set by DAQ, not the hardware.  Accurate times should be derived
        /// from the MCM clocks which are synchronized to GPS.
        UInt_t   GetTimeStamp() const  { return fTimeStamp; }
    
        /// Get any error codes associated with this event.  The event codes
        /// are not documented for the DAQ.
        UInt_t   GetErrorCode() const  { return fErrorCode; }
    
        /// Return the trigger word for this event.  The trigger word is
        /// read from the MCM, however it should be the same in ALL banks.
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
        /// In the off line header, these values are accessed separately.  The
        /// trigger bits include the instruction field in bits 58 to 63.
        ULong64_t GetTriggerWord() const {return fTriggerWord;}

        /// Return the trigger bits from the trigger word.  These are bits 48
        /// to 63 of the word returned by GetTriggerWord().  The bit
        /// definitions are defined by the ND::Trigger::Bits enumeration.  The
        /// trigger type can be checked using
        ///
        /// \code
        /// if (event.GetHeader().GetTriggerBits()
        ///          & ND::Trigger::kBeamSpill) {
        ///    ... This is a beam spill event
        /// }
        /// \endcode
        /// 
        /// The ND::TND280Event::CheckTrigger() convenience method is provided
        /// as an easy to use interface to the trigger bits.
        UInt_t GetTriggerBits() const {
            return ((fTriggerWord & (ND::Trigger::kInstMask|
                                     ND::Trigger::kTriggerMask))
                    >>ND::Trigger::kTriggerShift);
        }

        /// Return the spill number from the trigger word.  These are bits 32
        /// to 47 of the word returned by GetTriggerWord().
        UInt_t GetSpillNumber() const {
            return ((fTriggerWord & ND::Trigger::kSpillMask)
                    >>ND::Trigger::kSpillShift);
        }

        /// Return the event number from the trigger word.  These are bits 0
        /// to 31 of the word returned by GetTriggerWord().
        UInt_t GetEventNumber() const {
            return ((fTriggerWord & ND::Trigger::kEventMask)
                    >>ND::Trigger::kEventShift);
        }

        /// Get the MCM unix time of the last GPS pulse per second.  This is
        /// syncronized to the GPS pulse per second.  Note that the pulse per
        /// second signal is inhibited for 100 micro seconds before a beam
        /// trigger, so this may be more than one second in the past.  This
        /// will always be filled, even if the data was taken in a local
        /// partition.
        UInt_t  GetMCMSecond() const {return fMCMSecond;}
    
        /// Get the MCM 100 MHz counter since last pulse per second.  This
        /// could be greater than 1 second if the pulse per second was
        /// inhibited due to a beam trigger.  This will always be filled, even
        /// if the data was taken in a local partition.
        UInt_t  GetMCMSubSecond() const {return fMCMSubSecond;}

        /// Get the 100 MHz counter since the last busy state.  For a beam
        /// spill, this is expected to be between 5000 and 10000 (50 ms to 100
        /// ms).  This will always be filled, even if the data was taken in a
        /// local partition.
        UInt_t  GetMCMTimeSinceBusy() const {return fMCMTimeSinceBusy;}
 
        /// Get the clock module time for the P0D.  See the documentation for
        /// GetMCMSecond().  If the P0D clock module is not present in the
        /// event, this will be Invalid.
        UInt_t  GetP0DSecond() const {return fP0DSecond;}

        /// Get the P0D 100 MHz counter since last pulse per second.  This
        /// could be greater than 1 second if the pulse per second was
        /// inhibited due to a beam trigger.  See GetMCMSubSecond() for more
        /// documentation.
        UInt_t  GetP0DSubSecond() const {return fP0DSubSecond;}

        /// Get the 100 MHz counter since the last busy state.  For a beam
        /// spill, this is expected to be between 5000 and 10000 (50 ms to 100
        /// ms).
        UInt_t  GetP0DTimeSinceBusy() const {return fP0DTimeSinceBusy;}
 
        /// Get the clock module time for the TPC.  See the documentation for
        /// GetMCMSecond().  If the TPC clock module is not present in the
        /// event, this will be Invalid.
        UInt_t  GetTPCSecond() const {return fTPCSecond;}

        /// Get the TPC 100 MHz counter since last pulse per second.  This
        /// could be greater than 1 second if the pulse per second was
        /// inhibited due to a beam trigger.  See GetMCMSubSecond() for more
        /// documentation.
        UInt_t  GetTPCSubSecond() const {return fTPCSubSecond;}

        /// Get the 100 MHz counter since the last busy state.  For a beam
        /// spill, this is expected to be between 5000 and 10000 (50 ms to 100
        /// ms).
        UInt_t  GetTPCTimeSinceBusy() const {return fTPCTimeSinceBusy;}

        /// Get the clock module time for the HAT.  See the documentation for
        /// GetMCMSecond().  If the HAT clock module is not present in the
        /// event, this will be Invalid.
        UInt_t  GetHATSecond() const {return fHATSecond;}

        /// Get the HAT 100 MHz counter since last pulse per second.  This
        /// could be greater than 1 second if the pulse per second was
        /// inhibited due to a beam trigger.  See GetMCMSubSecond() for more
        /// documentation.
        UInt_t  GetHATSubSecond() const {return fHATSubSecond;}

        /// Get the 100 MHz counter since the last busy state.  For a beam
        /// spill, this is expected to be between 5000 and 10000 (50 ms to 100
        /// ms).
        UInt_t  GetHATTimeSinceBusy() const {return fHATTimeSinceBusy;}
 
        /// Get the clock module time for the FGD.  See the documentation for
        /// GetMCMSecond().  If the FGD clock module is not present in the
        /// event, this will be Invalid.
        UInt_t  GetFGDSecond() const {return fFGDSecond;}

        /// Get the FGD 100 MHz counter since last pulse per second.  This
        /// could be greater than 1 second if the pulse per second was
        /// inhibited due to a beam trigger.  See GetMCMSubSecond() for more
        /// documentation.
        UInt_t  GetFGDSubSecond() const {return fFGDSubSecond;}

        /// Get the 100 MHz counter since the last busy state.  For a beam
        /// spill, this is expected to be between 5000 and 10000 (50 ms to 100
        /// ms).
        UInt_t  GetFGDTimeSinceBusy() const {return fFGDTimeSinceBusy;}
 
        /// Get the clock module time for the ECal.  See the documentation for
        /// GetMCMSecond().  If the ECal clock module is not present in the
        /// event, this will be Invalid.
        UInt_t  GetECalSecond() const {return fECalSecond;}

        /// Get the ECal 100 MHz counter since last pulse per second.  This
        /// could be greater than 1 second if the pulse per second was
        /// inhibited due to a beam trigger.  See GetMCMSubSecond() for more
        /// documentation.
        UInt_t  GetECalSubSecond() const {return fECalSubSecond;}

        /// Get the 100 MHz counter since the last busy state.  For a beam
        /// spill, this is expected to be between 5000 and 10000 (50 ms to 100
        /// ms).
        UInt_t  GetECalTimeSinceBusy() const {return fECalTimeSinceBusy;}
 
        /// Get the clock module time for the SMRD.  See the documentation for
        /// GetMCMSecond().  If the SMRD clock module is not present in the
        /// event, this will be Invalid.
        UInt_t  GetSMRDSecond() const {return fSMRDSecond;}

        /// Get the SMRD 100 MHz counter since last pulse per second.  This
        /// could be greater than 1 second if the pulse per second was
        /// inhibited due to a beam trigger.  See GetMCMSubSecond() for more
        /// documentation.
        UInt_t  GetSMRDSubSecond() const {return fSMRDSubSecond;}

        /// Get the 100 MHz counter since the last busy state.  For a beam
        /// spill, this is expected to be between 5000 and 10000 (50 ms to 100
        /// ms).
        UInt_t  GetSMRDTimeSinceBusy() const {return fSMRDTimeSinceBusy;}
 
        /// Get the clock module time for the INGRID.  See the documentation
        /// for GetMCMSecond().  If the INGRID clock module is not present in
        /// the event, this will be Invalid.
        UInt_t  GetINGRIDSecond() const {return fINGRIDSecond;}

        /// Get the INGRID 100 MHz counter since last pulse per second.  This
        /// could be greater than 1 second if the pulse per second was
        /// inhibited due to a beam trigger.  See GetMCMSubSecond() for more
        /// documentation.
        UInt_t  GetINGRIDSubSecond() const {return fINGRIDSubSecond;}

        /// Get the 100 MHz counter since the last busy state.  For a beam
        /// spill, this is expected to be between 5000 and 10000 (50 ms to 100
        /// ms).
        UInt_t  GetINGRIDTimeSinceBusy() const {return fINGRIDTimeSinceBusy;}
 
        /// Get one of the 64 bit fields containing the CTM trigger bit
        /// pattern.
        ULong64_t GetCTMTriggerPattern(int i) const {
            if (i<0 || 2<i) return Invalid;
            return fCTMTriggerPattern[i];
        }

        /// Return the SMRD CTM trigger primitives for the top.  The bit
        /// pattern describes which trigger primitives from the top of the
        /// SMRD are active during the CTM trigger.
        UInt_t GetCTMSMRDTopPrimitives() const {
            if (fCTMTriggerPattern[0] == Invalid) return 0;
            return (fCTMTriggerPattern[0] & 0x00000000FFFFFFFFull);
        }
        
        /// Return the SMRD CTM trigger primitives for the south side.  The
        /// bit pattern describes which trigger primitives from the south of
        /// the SMRD are active during the CTM trigger.
        UInt_t GetCTMSMRDSouthPrimitives() const {
            if (fCTMTriggerPattern[0] == Invalid) return 0;
            return ((fCTMTriggerPattern[0] & 0xFFFFFFFF00000000ull) >> 32);
        }
        
        /// Return the SMRD CTM trigger primitives for the top.  The bit
        /// pattern describes which trigger primitives from the top of the
        /// SMRD are active during the CTM trigger.
        UInt_t GetCTMSMRDBottomPrimitives() const {
            if (fCTMTriggerPattern[1] == Invalid) return 0;
            return (fCTMTriggerPattern[1] & 0x00000000FFFFFFFFull);
        }
        
        /// Return the SMRD CTM trigger primitives for the south side.  The
        /// bit pattern describes which trigger primitives from the south of
        /// the SMRD are active during the CTM trigger.
        UInt_t GetCTMSMRDNorthPrimitives() const {
            if (fCTMTriggerPattern[1] == Invalid) return 0;
            return ((fCTMTriggerPattern[1] & 0xFFFFFFFF00000000ull) >> 32);
        }
        
        /// Return the P0D CTM trigger primitives.
        UInt_t GetCTMP0DPrimitives() const {
            if (fCTMTriggerPattern[2] == Invalid) return 0;
            return (fCTMTriggerPattern[2] & 0x00000000FFFFFFFFull);
        }
        
        /// Return the DSECal CTM trigger primitives.
        UInt_t GetCTMDSECalPrimitives() const {
            if (fCTMTriggerPattern[2] == Invalid) return 0;
            return ((fCTMTriggerPattern[2] & 0xFFFFFFFF00000000ull) >> 32);
        }
        
        /// Get one of the 64 bit fields containing the trigger bit pattern
        /// for the FGD.
        ULong64_t GetFGDCTMTriggerPattern(int i) const {
            if (i<0 || 2<i) return Invalid;
            return fFGDCTMTriggerPattern[i];
        }

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

    protected:
        /// Record Type: One of BeginRun, EndRun, Message, or Data
        UInt_t fRecordType;

        /// Unix event timestamp in seconds.  This is the time derived from the
        /// event builder.
        UInt_t fTimeStamp;

        /// Run type
        UInt_t fRunType;

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

        /// The pulse per second counter for the HAT.
        UInt_t fHATSecond;

        /// The 100 MHz counter since the last pulse per second for the HAT.
        UInt_t fHATSubSecond;

        /// The time since the HAT was previously busy.
        UInt_t fHATTimeSinceBusy;

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

        /// The bit pattern from the cosmic trigger module (if present).
        ULong64_t fCTMTriggerPattern[3];

        /// The bit pattern for the FGD cosmic trigger module (if present).
        ULong64_t fFGDCTMTriggerPattern[3];

        ClassDef(ND::TND280Event::Header,3);
    };

private:

    /// The context for this event.
    TND280Context fContext;

    /// The hash value of the geometry that should be associated with this
    /// event.  This must be set during calibration.
    TSHAHashValue fGeometryHash;

    /// The alignment identifier of the constants that should be applied to
    /// the geometry when handling this event.
    TAlignmentId fAlignmentId;

    /// The off line event header built from the raw data.
    Header fHeader;

    /// Build internal structure of the event.
    void Build();

    ClassDef(TND280Event,8);
};
#endif
