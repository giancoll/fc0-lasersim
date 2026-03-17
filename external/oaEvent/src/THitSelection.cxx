////////////////////////////////////////////////////////////
// $Id$
//

#include <algorithm>
#include "THitSelection.hxx"

ClassImp(ND::THitSelection);

ND::THitSelection::THitSelection(const char* name,
                             const char* title)
    : ND::TDatum(name,title) { }

ND::THitSelection::~THitSelection() { }

void ND::THitSelection::push_back(const ND::THandle<ND::THit>& hit) {
    if (!hit) {
        ND280Severe("Attempting to add a NULL hit");
        throw ND::EInvalidHit();
    }
    std::vector< ND::THandle<ND::THit> >::push_back(hit);
}

void ND::THitSelection::AddHit(const ND::THandle<ND::THit>& hit) {
    ND::THitSelection::iterator location
        = std::find(begin(), end(), hit);
    if (location == end()) push_back(hit);
}

void ND::THitSelection::RemoveHit(const ND::THandle<ND::THit>& hit) {
    ND::THitSelection::iterator location
        = std::find(begin(), end(), hit);
    if (location != end()) erase(location);
}

void ND::THitSelection::ls(Option_t* opt) const {
    ND::TDatum::ls(opt);
    std::string option(opt);
    if (option.find("dump") != std::string::npos
        || option.find("hits") != std::string::npos) {
        TROOT::IncreaseDirLevel();
        for (const_iterator v = begin();
             v != end();
             ++v) {
            v->ls(opt);
        }
        TROOT::DecreaseDirLevel();
    }
}
