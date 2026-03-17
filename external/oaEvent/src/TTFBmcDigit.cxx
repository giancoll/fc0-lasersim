#include "TTFBmcDigit.hxx"

ClassImp(ND::TTFBmcDigit);

ND::TTFBmcDigit::TTFBmcDigit() {}

ND::TTFBmcDigit::TTFBmcDigit(ND::TChannelId chan, 
			     int highGainADC, 
			     int lowGainADC,
			     unsigned int timeCounter,
			     int timeQuad,
			     bool timeDiscriminator,
                             const ND::TMCDigit::ContributorContainer& contrib) 
  : TTFBDigit(chan,highGainADC,lowGainADC, timeCounter, timeQuad, timeDiscriminator), 
    TMCDigit(contrib) {}

ND::TTFBmcDigit::~TTFBmcDigit() {}
