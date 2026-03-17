#include "TFGDDigit.hxx"

ClassImp(ND::TFGDDigit);

ND::TFGDDigit::TFGDDigit() {}

ND::TFGDDigit::TFGDDigit(ND::TChannelId chan, 
                         short startTime, short pulseTime, short pulseCharge, 
                         std::vector<unsigned short> adc, short dataType)
    : TDigit(chan), fFirstTimeSample(startTime), fPulseTime(pulseTime),
      fPulseCharge(pulseCharge), 
      fADCs(adc), fDataType(dataType) {}

ND::TFGDDigit::~TFGDDigit() {}

