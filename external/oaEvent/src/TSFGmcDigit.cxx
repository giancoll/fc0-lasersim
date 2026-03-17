#include "TSFGmcDigit.hxx"

ClassImp(ND::TSFGmcDigit);

ND::TSFGmcDigit::TSFGmcDigit() {}

ND::TSFGmcDigit::TSFGmcDigit(ND::TChannelId chan,
                             unsigned int risingEdgeTDC,
                             unsigned int fallingEdgeTDC,
                             unsigned int highGainADC,
                             unsigned int lowGainADC,
                             unsigned int risingEdgeGTSCounter,
                             bool matchedADCandTDC,
                             const ND::TMCDigit::ContributorContainer& contrib,
                             double chargePeak, double chargeIntegral,
                             double localIntegral,
                             double measuredTime,
                             double riseTime, double fallTime) :
    TSFGDigit(chan, risingEdgeTDC, fallingEdgeTDC, highGainADC, lowGainADC,
              risingEdgeGTSCounter, matchedADCandTDC),
    TMCDigit(contrib),
    fChargePeak(chargePeak), fChargeIntegral(chargeIntegral),
    fLocalIntegral(localIntegral),
    fMeasuredTime(measuredTime), fRiseTime(riseTime), fFallTime(fallTime) {}

ND::TSFGmcDigit::~TSFGmcDigit() {}

void ND::TSFGmcDigit::ls(Option_t* opt) const {
    TROOT::IncreaseDirLevel();
    TROOT::IndentLevel();
    std::cout << GetChannelId().AsString()
              << (IsMatchedADCandTDC() ? " *" : " -")
              << "T: " << GetGTSCounter()
              << "+[" << GetRisingEdgeTDC()
              << ";" << GetFallingEdgeTDC() << "]"
              << " Q: (" << GetHighGainADC() << "," << GetLowGainADC() << ")"
              << std::endl;
    TROOT::IncreaseDirLevel();
    TROOT::IndentLevel();
    std::cout << "True Q: " << GetLocalIntegral()
              << " T: " << GetMeasuredTime()
              << " (" << GetFallTime() - GetRiseTime()
              << " " << GetRiseTime() << "-" << GetFallTime() << ")"
              << std::endl;
    TROOT::DecreaseDirLevel();
    TROOT::DecreaseDirLevel();
}
