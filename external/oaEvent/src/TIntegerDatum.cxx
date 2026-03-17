#include "TIntegerDatum.hxx"

ClassImp(ND::TIntegerDatum);

ND::TIntegerDatum::TIntegerDatum() 
    : ND::TDatum("unnamed","Integer Value") {
    fValue.push_back(0);
}

ND::TIntegerDatum::TIntegerDatum(const char *name, int value) 
    : ND::TDatum(name,"Integer Value") {
    fValue.push_back(value);
}

ND::TIntegerDatum::TIntegerDatum(const char *name, const char* title, int value) 
    : ND::TDatum(name,title) {
    fValue.push_back(value);
}

ND::TIntegerDatum::TIntegerDatum(const TIntegerDatum& rhs)
    : ND::TDatum(rhs.GetName(),rhs.GetTitle()), fValue(rhs.fValue) {}

ND::TIntegerDatum::~TIntegerDatum() {}

int ND::TIntegerDatum::GetValue(void) const {return fValue.front();}

void ND::TIntegerDatum::SetValue(int v) {fValue.front() = v;}

std::vector<int>& ND::TIntegerDatum::GetVector(void) {return fValue;}

const std::vector<int>& ND::TIntegerDatum::GetVector(void) const {return fValue;}

void ND::TIntegerDatum::ls(Option_t*opt) const {
    ls_header(this, opt);
    for (std::vector<int>::const_iterator v = fValue.begin();
         v != fValue.end();
         ++v) {
        std::cout << " " << *v;
    }
    std::cout << std::endl;
}
