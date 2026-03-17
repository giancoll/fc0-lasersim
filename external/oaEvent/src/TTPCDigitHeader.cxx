#include "TTPCDigitHeader.hxx"

ClassImp(ND::TTPCDigitHeader);

//***********************************
ND::TTPCDigitHeader::TTPCDigitHeader():
  fCompressed(-1), fLosFlag(-1), fSyncFail(-1) {
//************************************************************

}

//***********************************
ND::TTPCDigitHeader::TTPCDigitHeader(bool c, bool l, bool s): 
  TDigitHeader("tpc"), fCompressed(c), fLosFlag(l), fSyncFail(s)  {
//************************************************************

}

//***********************************
ND::TTPCDigitHeader::~TTPCDigitHeader() {
//***********************************

}

