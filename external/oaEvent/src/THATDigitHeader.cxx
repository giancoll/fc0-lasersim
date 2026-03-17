#include "THATDigitHeader.hxx"

ClassImp(ND::THATDigitHeader);

//***********************************
ND::THATDigitHeader::THATDigitHeader():
  fCompressed(-1), fLosFlag(-1), fSyncFail(-1) {
//************************************************************

}

//***********************************
ND::THATDigitHeader::THATDigitHeader(bool c, bool l, bool s): 
  TDigitHeader("hat"), fCompressed(c), fLosFlag(l), fSyncFail(s)  {
//************************************************************

}

//***********************************
ND::THATDigitHeader::~THATDigitHeader() {
//***********************************

}

