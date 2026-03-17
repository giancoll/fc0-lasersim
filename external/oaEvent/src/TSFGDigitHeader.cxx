#include "TSFGDigitHeader.hxx"

ClassImp(ND::TSFGDigitHeader);
ClassImp(ND::TSFGFEBDigitHeader);
ClassImp(ND::TSFGOCBDigitHeader);

ND::TSFGDigitHeader::~TSFGDigitHeader(){}
ND::TSFGDigitHeader::TSFGDigitHeader():
    TDigitHeader("sfg"),
    fMCBEventNumber(0), fMCBSpillNumber(0), fMCBTrigger(0xDEADBEEF),
    fMCBTimeStamp(0xDEADBEEF) {}

ND::TSFGFEBDigitHeader::~TSFGFEBDigitHeader(){}
ND::TSFGFEBDigitHeader::TSFGFEBDigitHeader() :
    fBoardID(0xDEADBEEF), fGateType(0xDEADBEEF), fGateNumber(0),
    fGateGTSTag(0), fGateTime10ns(0), fGateTime10ms(0),
    fHoldStart(0xDEADBEEF), fHoldStop(0xDEADBEEF) {}

ND::TSFGOCBDigitHeader::~TSFGOCBDigitHeader(){}
ND::TSFGOCBDigitHeader::TSFGOCBDigitHeader() :
    fOCBID(0xDEADBEEF), fOCBGateType(0xDEADBEEF), fOCBEventNumber(0) {}

const ND::TSFGFEBDigitHeader* ND::TSFGDigitHeader::FindFEB(int board) const {
    if (board < 0) return nullptr;
    // Do a linear search for now.  This can be turned into a binary search if
    // TSFGDigitManager guarrantees that the FEB headers are in order.  We are
    // limited to a couple hundred boards, so a linear search is probably fast
    // enough anyway.
    for (std::size_t i = 0; i < GetFEBCount(); ++i) {
        if (GetFEB(i).GetID() == (UInt_t) board) return &(GetFEB(i));
    }
    return nullptr;
}

void ND::TSFGDigitHeader::ls(Option_t* opt) const {
    TROOT::IndentLevel();
    std::cout<<"ND::TSFGDigitHeader("<<this<<"):: ";
    std::cout<<GetName()<<std::endl;
    TROOT::IncreaseDirLevel();
    TROOT::IndentLevel();
    std::cout << "Event: " << GetMCBEventNumber();
    std::cout << " Spill: " << GetMCBSpillNumber();
    std::cout << " Stamp: " << GetMCBTimeStamp();
    std::cout << std::endl;
    TROOT::IndentLevel();
    std::cout << "Boards: " << GetFEBHeaders().size() << std::endl;
    TROOT::IncreaseDirLevel();
    for (const ND::TSFGFEBDigitHeader& b : GetFEBHeaders()) {
        TROOT::IndentLevel();
        std::cout << b.GetID()
                  << " -- T: " << b.GetGateType()
                  << " time: " << b.GetGateTime10ms()
                  << "+" << b.GetGateTime10ns()
                  << " hold: " << b.GetHoldStart() << "-" << b.GetHoldStop()
                  << std::endl;
    }
    TROOT::DecreaseDirLevel();
    TROOT::DecreaseDirLevel();
}
