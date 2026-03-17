#ifndef TTOFDigitHeader_hxx_seen
#define TTOFDigitHeader_hxx_seen

#include <TDigitHeader.hxx>

namespace ND {
    class TTOFDigitHeader;
};

class ND::TTOFDigitHeader : public ND::TDigitHeader{
public:
    TTOFDigitHeader();
    TTOFDigitHeader( double triggerTimestamp, uint rawDataSize, uint spillNumber, uint trigger, 
        uint trigIDFromExtTrig, uint trigIDFromFPGA);

    virtual ~TTOFDigitHeader();

    // Setters
    void SetTriggerTimestamp(double triggerTimestamp) { fTriggerTimestamp = triggerTimestamp; }
    void SetRawDataSize(uint rawDataSize) { fRawDataSize = rawDataSize; }
    void SetSpillNumber(uint spillNumber) { fSpillNumber = spillNumber; }
    void SetNumberTriggers(uint numberTriggers) { fNumberTriggers = numberTriggers; }
    void SetTrigIDFromExtTrig(uint trigIDFromExtTrig) { fTrigIDFromExtTrig = trigIDFromExtTrig; }
    void SetTrigIDFromFPGA(uint trigIDFromFPGA) { fTrigIDFromFPGA = trigIDFromFPGA; }

    // Getters
    double GetTriggerTimestamp() const { return fTriggerTimestamp; }
    uint GetRawDataSize() const { return fRawDataSize; }
    uint GetSpillNumber() const { return fSpillNumber; }
    uint GetNumberTriggers() const { return fNumberTriggers; }
    uint GetTrigIDFromExtTrig() const { return fTrigIDFromExtTrig; }
    uint GetTrigIDFromFPGA() const { return fTrigIDFromFPGA; }

private:

  // here all the complementary info about the hit, that does not fit in the TTOFDigit itself
  double fTriggerTimestamp {0}; // this is important, to be subtracted to the cell0time at calibration
  uint fRawDataSize {0}; 
  uint fSpillNumber {0};
  uint fNumberTriggers {0};
  uint fTrigIDFromExtTrig {0};
  uint fTrigIDFromFPGA {0};

  ClassDef(TTOFDigitHeader, 2); // updated in May '24, before it was just a wrong copypaste
};
#endif
