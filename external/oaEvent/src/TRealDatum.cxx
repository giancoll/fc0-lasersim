#include "TRealDatum.hxx"

ClassImp(ND::TRealDatum);

ND::TRealDatum::TRealDatum() 
    : ND::TDatum("unnamed","Real Value") {
    fValue.push_back(0);
}

ND::TRealDatum::TRealDatum(const char *name, double value) 
    : ND::TDatum(name,"Real Value") {
    fValue.push_back(value);
}

ND::TRealDatum::TRealDatum(const char *name, const char* title, double value) 
    : ND::TDatum(name,title) {
    fValue.push_back(value);
}

ND::TRealDatum::TRealDatum(const TRealDatum& rhs)
    : ND::TDatum(rhs.GetName(),rhs.GetTitle()), fValue(rhs.fValue) {}

ND::TRealDatum::~TRealDatum() {}

double ND::TRealDatum::GetValue(void) const {return fValue.front();}

void ND::TRealDatum::SetValue(double v) {fValue.front() = v;}

std::vector<double>& ND::TRealDatum::GetVector(void) {return fValue;}

const std::vector<double>& ND::TRealDatum::GetVector(void) const {return fValue;}

void ND::TRealDatum::ls(Option_t* opt) const {
    ls_header(this, opt);
    for (std::vector<double>::const_iterator v = fValue.begin();
         v != fValue.end();
         ++v) {
        std::cout << " " << *v;
    }
    std::cout << std::endl;
}
