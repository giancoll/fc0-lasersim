#include "THATmcDigit.hxx"

ClassImp(ND::THATmcDigit);

ND::THATmcDigit::THATmcDigit() {}

ND::THATmcDigit::THATmcDigit(ND::TChannelId chan, 
                             short startTime, 
                             std::vector<short> adcs,
                             const ND::TMCDigit::ContributorContainer& contrib)
    : THATDigit(chan,startTime,adcs), 
      TMCDigit(contrib) {}


ND::THATmcDigit::~THATmcDigit() {}
