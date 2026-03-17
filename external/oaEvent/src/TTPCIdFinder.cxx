// $Id$

#include <iostream>

#include <TGeoManager.h>

#include "ND280GeomId.hxx"
#include "ND280GeomIdDef.hxx"
#include "TTPCIdFinder.hxx"

ND::TTPCIdFinder::TTPCIdFinder() 
    : fModule(-1), fHalf(-1), fMicroMega(-1) {}
ND::TTPCIdFinder::~TTPCIdFinder() {}

bool ND::TTPCIdFinder::Search(const std::vector<std::string>& names, 
                              TGeometryId& id) {
    using namespace ND::GeomId::Def;

    // Assign a geometry identifier to the full TPC.
    if (names.size()<6) return false;
    if (names[5].find("TPC")==std::string::npos) return false;

#ifdef DEBUG_TPCFinder
    for (std::vector<std::string>::const_iterator s = names.begin();
         s != names.end();
         ++s) std::cout << " " << *s;
    std::cout << std::endl;
#endif

    if (names.back().find("TPC1_")!=std::string::npos) {
        id = ND::GeomId::TPC::TPC1();
        fModule = 0;
        fHalf = -1;
        fMicroMega = -1;
        return true;
    }
    
    if (names.back().find("TPC2_")!=std::string::npos) {
        id = ND::GeomId::TPC::TPC2();
        fModule = 1;
        fHalf = -1;
        fMicroMega = -1;
        return true;
    }

    if (names.back().find("TPC3_")!=std::string::npos) {
        id = ND::GeomId::TPC::TPC3();
        fModule = 2;
        fHalf = -1;
        fMicroMega = -1;
        return true;
    }

    if (names.back().find("Half_")!=std::string::npos) {
        ++fHalf;
        fMicroMega = -1;
        return false;
    }

    if (names.back().find("MM_")!=std::string::npos) {
        ++fMicroMega;
        id = ND::GeomId::TPC::MicroMega(fModule, fHalf, fMicroMega);
        return true;
    }

    return false;
}
