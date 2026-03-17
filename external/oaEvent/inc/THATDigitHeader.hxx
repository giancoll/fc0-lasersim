#ifndef THATDigitHeader_hxx_seen
#define THATDigitHeader_hxx_seen

#include <TDigitHeader.hxx>

namespace ND {
    class THATDigitHeader;
};

class ND::THATDigitHeader : public ND::TDigitHeader{
public:
    THATDigitHeader();
    THATDigitHeader(bool comp, bool lflag, bool sync);
    virtual ~THATDigitHeader();

private:

  bool fCompressed;
  bool fLosFlag;
  bool fSyncFail;


    ClassDef(THATDigitHeader, 1);
};
#endif
