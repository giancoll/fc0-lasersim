#include "TTPCmcDigit.hxx"

ClassImp(ND::TTPCmcDigit);

ND::TTPCmcDigit::TTPCmcDigit() {}

ND::TTPCmcDigit::TTPCmcDigit(ND::TChannelId chan, 
                             short startTime, 
                             std::vector<short> adcs,
                             const ND::TMCDigit::ContributorContainer& contrib)
    : TTPCDigit(chan,startTime,adcs), 
      TMCDigit(contrib) {}


ND::TTPCmcDigit::~TTPCmcDigit() {}
