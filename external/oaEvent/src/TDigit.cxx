#include "TDigit.hxx"

ClassImp(ND::TDigit);

ND::TDigit::TDigit() {}

ND::TDigit::TDigit(const ND::TChannelId& channel)
  : fChannelId(channel) {}

ND::TDigit::~TDigit() {}

const ND::TChannelId& ND::TDigit::GetChannelId() const {
    return fChannelId;
}

void ND::TDigit::ls(Option_t* opt) const {
    TROOT::IncreaseDirLevel();
    TROOT::IndentLevel();
    std::cout << "TDigit: " << GetChannelId().AsString() << std::endl;
    TROOT::DecreaseDirLevel();
}
