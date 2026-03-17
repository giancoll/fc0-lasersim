#include "THATDigit.hxx"

ClassImp(ND::THATDigit);

//**********************************************
ND::THATDigit::THATDigit() {}
//**********************************************

//**********************************************
ND::THATDigit::THATDigit(ND::TChannelId chan, short time, std::vector<short> adc)
    : TDigit(chan), fFirstTimeSample(time), fADCs(adc) {}
//**********************************************

//**********************************************
ND::THATDigit::~THATDigit() {}
//**********************************************

//**********************************************
int ND::THATDigit::GetFirstTimeSample() const {
//**********************************************
  return fFirstTimeSample;
}

//**********************************************
int ND::THATDigit::GetNumberOfTimeSamples() const {
//**********************************************
return fADCs.size();
}

//**********************************************
int ND::THATDigit::GetADC(unsigned int t) const {
//**********************************************
  if (t>= 0 && t<fADCs.size())
    return fADCs[t];
  else return 0;
}


//**********************************************
std::vector<short> ND::THATDigit::GetADCs() const {
//**********************************************
return fADCs;
}

void ND::THATDigit::ls(Option_t* opt) const {
    TROOT::IncreaseDirLevel();
    TROOT::IndentLevel();
    std::cout << GetChannelId().AsString()
              << " T: (" << GetFirstTimeSample() << ")"
              << " N: (" <<GetNumberOfTimeSamples() << ")"
              << std::endl;
    TROOT::DecreaseDirLevel();
}
