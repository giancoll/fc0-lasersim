#include <TSFGDigit.hxx>

ClassImp(ND::TSFGDigit);

ND::TSFGDigit::~TSFGDigit() {}

ND::TSFGDigit::TSFGDigit()
    : ND::TDigit(),
      fGTSCounter(0),
      fRisingTDC(0), fFallingTDC(0),
      fHighGainADC(0), fLowGainADC(0) {}

ND::TSFGDigit::TSFGDigit(ND::TChannelId chan,
                     unsigned int risingEdgeTDC,
                     unsigned int fallingEdgeTDC,
                     unsigned int highGainADC,
                     unsigned int lowGainADC,
                     unsigned int risingEdgeGTSCounter,
                     bool matchedADCandTDC)
    : ND::TDigit(chan),
      fGTSCounter(risingEdgeGTSCounter),
      fRisingTDC(risingEdgeTDC), fFallingTDC(fallingEdgeTDC),
      fHighGainADC(highGainADC), fLowGainADC(lowGainADC) {
    // Be sneaky and pack in the bit.
    fGTSCounter *= 2;
    if (matchedADCandTDC) fGTSCounter += 1;
}

int ND::TSFGDigit::GetGTSCounter() const {return fGTSCounter/2;}
int ND::TSFGDigit::GetRisingEdgeTDC() const {return fRisingTDC;}
int ND::TSFGDigit::GetFallingEdgeTDC() const {return fFallingTDC;}
int ND::TSFGDigit::GetHighGainADC() const {return fHighGainADC;}
int ND::TSFGDigit::GetLowGainADC() const {return fLowGainADC;}
bool ND::TSFGDigit::IsMatchedADCandTDC() const {return (fGTSCounter & 1);}

void ND::TSFGDigit::ls(Option_t* opt) const {
    TROOT::IncreaseDirLevel();
    TROOT::IndentLevel();
    std::cout << GetChannelId().AsString()
              << (IsMatchedADCandTDC() ? " *" : " -")
              << "T: " << GetGTSCounter()
              << "+[" << GetRisingEdgeTDC()
              << ";" << GetFallingEdgeTDC() << "]"
              << " Q: (" << GetHighGainADC() << "," << GetLowGainADC() << ")"
              << std::endl;
    TROOT::DecreaseDirLevel();
}
