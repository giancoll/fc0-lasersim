#include "TTOFDigit.hxx"

ClassImp(ND::TTOFDigit);

//**********************************************
ND::TTOFDigit::TTOFDigit() {}
//**********************************************

//**********************************************
ND::TTOFDigit::TTOFDigit(ND::TChannelId chan, double cell0time, std::vector<float> waveform, float timeOverThreshold) 
    : TDigit(chan), fCell0Time(cell0time), fSamples(waveform), fTimeOverThreshold(timeOverThreshold) {}
//**********************************************

//**********************************************
ND::TTOFDigit::~TTOFDigit() {}
//**********************************************

//**********************************************
double ND::TTOFDigit::GetCell0Time() const {
//**********************************************
  return fCell0Time;
}

//**********************************************
int ND::TTOFDigit::GetNumberOfSamples() const {
//**********************************************
return fSamples.size();
}

//**********************************************
float ND::TTOFDigit::GetTimeOverThreshold() const {
//**********************************************
return fTimeOverThreshold;
}

//**********************************************
float ND::TTOFDigit::GetWfSample(unsigned int t) const {
//**********************************************
  if (t>= 0 && t<fSamples.size())  
    return fSamples[t];
  else return 0;  
}


//**********************************************
std::vector<float> ND::TTOFDigit::GetWaveform() const {
//**********************************************
return fSamples;
}

void ND::TTOFDigit::ls(Option_t* opt) const {
    TROOT::IncreaseDirLevel();
    TROOT::IndentLevel();
    std::cout << GetChannelId().AsString() 
              << " T: (" << GetCell0Time() << ")" 
              << " N: (" << GetNumberOfSamples() << ")" 
              << std::endl;
    TROOT::DecreaseDirLevel();
}
