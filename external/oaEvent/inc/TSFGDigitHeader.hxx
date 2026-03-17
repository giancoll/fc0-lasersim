#ifndef TSFGDigitHeader_hxx_seen
#define TSFGDigitHeader_hxx_seen

#include <vector>
#include <TDigitHeader.hxx>

namespace ND {
    class TSFGFEBDigitHeader;
    class TSFGOCBDigitHeader;
    class TSFGDigitHeader;
};

// The per FEB header information.
class ND::TSFGFEBDigitHeader {
public:
    TSFGFEBDigitHeader();
    virtual ~TSFGFEBDigitHeader();

    /// Get the board ID for this header.
    UInt_t GetID() const { return fBoardID; }
    void SetID(int i) { fBoardID = i; }

    /// Get the gate type.
    UInt_t GetGateType() const { return fGateType; }
    void SetGateType(int i) { fGateType = i; }

    /// Get the gate number.
    UInt_t GetGateNumber() const { return fGateNumber; }
    void SetGateNumber(int i) { fGateNumber = i; }

    /// Get the GTS tag for the GTS window containing the gate.
    UInt_t GetGateGTSTag() const { return fGateGTSTag; }
    void SetGateGTSTag(int i) { fGateGTSTag = i; }

    /// Get the gate time (10ns tick).
    UInt_t GetGateTime10ns() const { return fGateTime10ns; }
    void SetGateTime10ns(int i) { fGateTime10ns = i; }

    /// Get the gate time (10ms tick).
    UInt_t GetGateTime10ms() const { return fGateTime10ms; }
    void SetGateTime10ms(int i) { fGateTime10ms = i; }

    /// Get the hold start
    UInt_t GetHoldStart() const { return fHoldStart; }
    void SetHoldStart(int i) { fHoldStart = i; }

    /// Get the hold stop
    UInt_t GetHoldStop() const { return fHoldStop; }
    void SetHoldStop(int i) { fHoldStop = i; }

private:
    UInt_t fBoardID;
    UInt_t fGateType;
    UInt_t fGateNumber;
    UInt_t fGateGTSTag;
    UInt_t fGateTime10ns;
    UInt_t fGateTime10ms;
    UInt_t fHoldStart;
    UInt_t fHoldStop;
};

// The per OCB header information.
class ND::TSFGOCBDigitHeader {
public:
    TSFGOCBDigitHeader();
    virtual ~TSFGOCBDigitHeader();

    /// The OCB number.  This is the top 4 bits of each board ID, and also
    /// encoded in the bank name.
    UInt_t GetID() const { return fOCBID; }
    void SetID(int i) { fOCBID = i; }

    /// The OCB gate type.  This comes from the OCB data packet header word.
    UInt_t GetGateType() const { return fOCBGateType; }
    void SetGateType(int i) { fOCBGateType = i; }

    /// The OCB event number.  This comes from the OCB data packet header word.
    UInt_t GetEventNumber() const { return fOCBEventNumber; }
    void SetEventNumber(int i) { fOCBEventNumber = i; }

private:
    UInt_t fOCBID;
    UInt_t fOCBGateType;
    UInt_t fOCBEventNumber;
};

/// Header information for SuperSFGD.  This is shared by all of the SFG banks,
/// so it has vectors of header information for each FEB and OCB.
class ND::TSFGDigitHeader : public ND::TDigitHeader {
public:
    TSFGDigitHeader();
    virtual ~TSFGDigitHeader();

    /// Get the event number provided by the SuperFGD MCB
    UInt_t GetMCBEventNumber() const { return fMCBEventNumber; }
    void SetMCBEventNumber(int i) { fMCBEventNumber = i; }

    /// Get the spill number provided by the SuperFGD MCB
    UInt_t GetMCBSpillNumber() const { return fMCBSpillNumber; }
    void SetMCBSpillNumber(int i) { fMCBSpillNumber = i; }

    /// Get the trigger type provided by the SuperFGD MCB
    UInt_t GetMCBTrigger() const { return fMCBTrigger; }
    void SetMCBTrigger(int i) { fMCBTrigger = i; }

    /// Get the time stamp relative to the last PPS from the MCB.
    UInt_t GetMCBTimeStamp() const { return fMCBTimeStamp; }
    void SetMCBTimeStamp(int i) { fMCBTimeStamp = i; }

    /// Get the vector of FEB headers.  The FEB headers will be in the order
    /// that the bank information was processed (the index will not be equal
    /// to the board id).
    const std::vector<ND::TSFGFEBDigitHeader>& GetFEBHeaders() const {
        return fFEBHeaders;
    }

    /// Get the count of FEB headers
    UInt_t GetFEBCount() const { return fFEBHeaders.size(); }

    /// Get a single FEB header indexed by it's order in the data.
    const ND::TSFGFEBDigitHeader& GetFEB(int i) const {
        return fFEBHeaders[i];
    }

    /// Get a single FEB header indexed by the board id.  This returns a
    /// null pointer if the board is not found.
    const ND::TSFGFEBDigitHeader* FindFEB(int board) const;

    /// Get the vector of OCB headers.  The OCB headers will be in the order
    /// that the banks were processed (the index will not be equal to the OCB
    /// id).
    const std::vector<ND::TSFGOCBDigitHeader>& GetOCBHeaders() const {
        return fOCBHeaders;
    }

    /// Get the count of OCB headers.
    UInt_t GetOCBCount() const { return fOCBHeaders.size(); }

    /// Get a single OCB header.
    const ND::TSFGOCBDigitHeader& GetOCB(int i) const {
        return fOCBHeaders[i];
    }

    /// Get a mutable vector of the FEB headers.
    std::vector<ND::TSFGFEBDigitHeader>& ModifyFEBHeaders() {
        return fFEBHeaders;
    }

    /// Get a mutable vector of the OCB headers.
    std::vector<ND::TSFGOCBDigitHeader>& ModifyOCBHeaders() {
        return fOCBHeaders;
    }

    /// Print the headera information.
    virtual void ls(Option_t* opt = "") const;

private:

    UInt_t fMCBEventNumber;
    UInt_t fMCBSpillNumber;
    UInt_t fMCBTrigger;
    UInt_t fMCBTimeStamp;

    std::vector<ND::TSFGFEBDigitHeader> fFEBHeaders;
    std::vector<ND::TSFGOCBDigitHeader> fOCBHeaders;

    ClassDef(TSFGDigitHeader, 2);
};
#endif
