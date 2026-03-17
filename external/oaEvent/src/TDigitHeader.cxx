#include "TDigitHeader.hxx"
#include "TDataVector.hxx"

ClassImp(ND::TDigitHeader);

ND::TDigitHeader::TDigitHeader()
    : fName(""), fBeginValid(0), fEndValid(0), fDataVector(0) {}

ND::TDigitHeader::TDigitHeader(const std::string& name)
    : fName(name), fBeginValid(0), fEndValid(0), fDataVector(0) {};

ND::TDigitHeader::~TDigitHeader() {}

void ND::TDigitHeader::AddDatum(ND::TDatum* val, const char* name) {
    if (!fDataVector) {
        fDataVector = new ND::TDataVector("info",
                                          "Digit Header Information");
    }
    fDataVector->AddDatum(val,name);
}

void ND::TDigitHeader::AddDatum(ND::THandle<ND::TDatum> val, const char* name) {
    if (!fDataVector) {
        fDataVector = new ND::TDataVector("DigitHeader",
                                          "Digit Header Information");
    }
    fDataVector->AddDatum(val,name);
}

void ND::TDigitHeader::ls(Option_t* opt) const {
    TROOT::IndentLevel();
    std::cout<<"ND::TDigitHeader("<<this<<"):: ";
    std::cout<<GetName()<<std::endl;
    TROOT::IncreaseDirLevel();
    TROOT::IndentLevel();
    std::cout << "Valid for digits: " << GetBeginValid()
              << " -- " << GetEndValid() << std::endl;
    if (fDataVector) fDataVector->ls(opt);
    TROOT::DecreaseDirLevel();
}
