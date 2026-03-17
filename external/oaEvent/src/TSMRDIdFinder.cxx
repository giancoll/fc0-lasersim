// $Id$

#include <iostream>
#include <sstream>
#include <string>

#include <TGeoManager.h>

#include "ND280GeomId.hxx"
#include "ND280GeomIdDef.hxx"
#include "TSMRDIdFinder.hxx"

ND::TSMRDIdFinder::TSMRDIdFinder() 
    : fClam(-1), fYoke(-1), fLayer(-1), fSlot(-1), fBar(-1) {}
ND::TSMRDIdFinder::~TSMRDIdFinder() {}

bool ND::TSMRDIdFinder::Search(const std::vector<std::string>& names, 
                               TGeometryId& id) {
    using namespace ND::GeomId::Def;

    // Assign a geometry identifier to the full SMRD.
    if (names.size()<5) return false;
    if (names[4].find("MRD")==std::string::npos) return false;

    if (names[3].find("LeftClam_")!=std::string::npos) fClam = 0;
    else if (names[3].find("RightClam_")!=std::string::npos) fClam = 1;
    else fClam = -1;

    // Get the information about the slot from the MRD arm 
    std::istringstream cvt;
    if (names.back().find("MRDArm:")!=std::string::npos
        || names.back().find("MRDSide:")!=std::string::npos) {
        unsigned colon = names.back().find(":");
        cvt.str(names.back().substr(colon+1,3));
        cvt >> fYoke;
        fSlot = fYoke % 10;
        fLayer = (fYoke/10) % 10;
        fYoke = fYoke/100;
        fBar = -1;
        id = ND::GeomId::SMRD::Module(fClam,fYoke,fLayer,fSlot);
        return true;
    }

    if (names.back().find("Bar_") != std::string::npos) {
        ++fBar;
        id = ND::GeomId::SMRD::Bar(fClam,fYoke,fLayer,fSlot,fBar);
        return true;
    }

    return false;
}
