#ifndef TTPCDigitHeader_hxx_seen
#define TTPCDigitHeader_hxx_seen

#include <TDigitHeader.hxx>

namespace ND {
    class TTPCDigitHeader;
};

class ND::TTPCDigitHeader : public ND::TDigitHeader{
public:
    TTPCDigitHeader();
    TTPCDigitHeader(bool comp, bool lflag, bool sync);
    virtual ~TTPCDigitHeader();

private:

  bool fCompressed;
  bool fLosFlag;
  bool fSyncFail;


    ClassDef(TTPCDigitHeader, 1);
};
#endif
