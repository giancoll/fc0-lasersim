#include "TTFBDigit.hxx"

ClassImp(ND::TTFBDigit);

ND::TTFBDigit::TTFBDigit() {}

ND::TTFBDigit::TTFBDigit(ND::TChannelId chan,
                         int highGainADC,
                         int lowGainADC,
                         unsigned int timeCounter,
                         int timeQuad,
                         bool timeDiscriminator)
    : TDigit(chan), fHighGainADC(highGainADC), fLowGainADC(lowGainADC),
      fTimeCounter(timeCounter) {
    fBits = timeQuad & 0x03;
    if (timeDiscriminator) fBits |= 0x04;
    else fBits &= 0xFB;
}

ND::TTFBDigit::~TTFBDigit() {}

void ND::TTFBDigit::ls(Option_t* opt) const {
    TROOT::IncreaseDirLevel();
    TROOT::IndentLevel();
    std::cout << GetChannelId().AsString()
              << " T: (" << CheckTimeDiscriminator()
              << "," << GetTimeQuad() << "," << GetTimeCounter() << ")"
              << " Q: (" << GetHighGainADC() << "," << GetLowGainADC() << ")"
              << std::endl;
    TROOT::DecreaseDirLevel();
}
