#include "TTPCDigit.hxx"

ClassImp(ND::TTPCDigit);

//**********************************************
ND::TTPCDigit::TTPCDigit() {}
//**********************************************

//**********************************************
ND::TTPCDigit::TTPCDigit(ND::TChannelId chan, short time, std::vector<short> adc)
    : TDigit(chan), fFirstTimeSample(time), fADCs(adc) {}
//**********************************************

//**********************************************
ND::TTPCDigit::~TTPCDigit() {}
//**********************************************

//**********************************************
int ND::TTPCDigit::GetFirstTimeSample() const {
//**********************************************
  return fFirstTimeSample;
}

//**********************************************
int ND::TTPCDigit::GetNumberOfTimeSamples() const {
//**********************************************
return fADCs.size();
}

//**********************************************
int ND::TTPCDigit::GetADC(unsigned int t) const {
//**********************************************
  if (t>= 0 && t<fADCs.size())
    return fADCs[t];
  else return 0;
}


//**********************************************
std::vector<short> ND::TTPCDigit::GetADCs() const {
//**********************************************
return fADCs;
}

void ND::TTPCDigit::ls(Option_t* opt) const {
    TROOT::IncreaseDirLevel();
    TROOT::IndentLevel();
    std::cout << GetChannelId().AsString()
              << " T: (" << GetFirstTimeSample() << ")"
              << " N: (" << GetNumberOfTimeSamples() << ")"
              << std::endl;
    TROOT::DecreaseDirLevel();
}
