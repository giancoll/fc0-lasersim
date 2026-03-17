// $Id$

#include <iostream>

#include <TGeoManager.h>
#include "TND280Log.hxx"

#include "ND280GeomId.hxx"
#include "ND280GeomIdDef.hxx"
#include "THATIdFinder.hxx"

ND::THATIdFinder::THATIdFinder()
    : fModule(-1), fHalf(-1), fMicroMega(-1) {}
ND::THATIdFinder::~THATIdFinder() {}

bool ND::THATIdFinder::Search(const std::vector<std::string>& names,
                              TGeometryId& id) {
    using namespace ND::GeomId::Def;

    // Assign a geometry identifier to the full HAT.
    if (names.size()<5) return false;
    if (names[4].find("HAT")==std::string::npos) return false;

#ifdef DEBUG_HATFinder
    for (std::vector<std::string>::const_iterator s = names.begin();
         s != names.end();
         ++s) std::cout << " " << *s;
    std::cout << std::endl;
#endif

    if (names.back().find("TopHAT_")!=std::string::npos) {
        id = ND::GeomId::HAT::Top();
        fModule = 1;
        fHalf = -1;
        fMicroMega = -1;
        return true;
    }

    if (names.back().find("BottomHAT_")!=std::string::npos) {
        id = ND::GeomId::HAT::Bottom();
        fModule = 0;
        fHalf = -1;
        fMicroMega = -1;
        return true;
    }

    if (names.back().find("Half_")!=std::string::npos) {
        if (fModule < 0 || fModule > 1) {
            ND280Error("Invalid Geometry");
            throw ND::EGeomIdBroken();
        }
        ++fHalf;
        fMicroMega = -1;
        return false;
    }

    if (names.back().find("ERAM_")!=std::string::npos) {
        if (fHalf < 0 || fHalf > 1 || fModule < 0 || fModule > 1) {
            ND280Error("Invalid Geometry");
            throw ND::EGeomIdBroken();
        }
        ++fMicroMega;
        id = ND::GeomId::HAT::MicroMega(fModule, fHalf, fMicroMega);
        return true;
    }

    return false;
}
