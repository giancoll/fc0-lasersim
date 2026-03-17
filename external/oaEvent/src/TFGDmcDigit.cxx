#include "TFGDmcDigit.hxx"

ClassImp(ND::TFGDmcDigit);

ND::TFGDmcDigit::TFGDmcDigit() {}

ND::TFGDmcDigit::TFGDmcDigit(ND::TChannelId chan, 
                             short startTime, short pulseTime, short pulseCharge, 
                             std::vector<unsigned short> adcs,
                             short dataType,
                             const ND::TMCDigit::ContributorContainer& contrib)
    : TFGDDigit(chan,startTime,pulseTime,pulseCharge,adcs,dataType), 
      TMCDigit(contrib) {}


ND::TFGDmcDigit::~TFGDmcDigit() {}
