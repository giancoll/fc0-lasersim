#include "TTOFDigitHeader.hxx"

ClassImp(ND::TTOFDigitHeader);

//***********************************
ND::TTOFDigitHeader::TTOFDigitHeader() {}
//***********************************

//***********************************
ND::TTOFDigitHeader::TTOFDigitHeader(double triggerTimestamp, uint rawDataSize, uint spillNumber, uint numberTriggers, 
    uint trigIDFromExtTrig, uint trigIDFromFPGA):
  TDigitHeader("tof"), 
  fTriggerTimestamp(triggerTimestamp), fRawDataSize(rawDataSize), fSpillNumber(spillNumber), fNumberTriggers(numberTriggers),
  fTrigIDFromExtTrig(trigIDFromExtTrig), fTrigIDFromFPGA(trigIDFromFPGA) {
}
//***********************************

//***********************************
ND::TTOFDigitHeader::~TTOFDigitHeader() {}
//***********************************

