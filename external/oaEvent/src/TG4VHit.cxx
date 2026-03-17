
#include <TClass.h>
#include <TStreamerInfo.h>

#include "TG4VHit.hxx"

ClassImp(ND::TG4VHit);
ClassImp(ND::TG4HitContainer);

ND::TG4VHit::TG4VHit() {}

ND::TG4VHit::~TG4VHit() {}

void ND::TG4VHit::ls(Option_t *opt) const {
    ND::ls_header(this,opt);
    std::cout << "Abstract Base Class"
              << std::endl;
}

ND::TG4HitContainer::~TG4HitContainer() {
    for (const_iterator v = begin();
         v != end(); 
         ++v) {
        delete *v;
    };
}

void ND::TG4HitContainer::ls(Option_t* opt) const {
    ND::TDatum::ls(opt);
    std::string option(opt);
    if (option.find("dump") != std::string::npos) {
        TROOT::IncreaseDirLevel();
        for (const_iterator v = begin();
             v != end(); 
             ++v) {
            (*v)->ls(opt);
        };
        TROOT::DecreaseDirLevel();
    }
}

