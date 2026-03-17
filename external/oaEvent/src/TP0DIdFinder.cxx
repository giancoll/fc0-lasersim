// $Id$

#include <iostream>

#include <TGeoManager.h>

#include "ND280GeomId.hxx"
#include "ND280GeomIdDef.hxx"
#include "TP0DIdFinder.hxx"

ND::TP0DIdFinder::TP0DIdFinder() 
    : fSP0Dule(-1), fP0Dule(-1), fLayer(-1), fBar(-1), fTarget(-1), 
      fECalRadiator(-1), fTargetRadiator(-1) {}
ND::TP0DIdFinder::~TP0DIdFinder() {}

bool ND::TP0DIdFinder::Search(const std::vector<std::string>& names, 
                              TGeometryId& id) {
    using namespace ND::GeomId::Def;

    // Assign a geometry identifier to the full P0D.
    if (names.size()<5) return false;
    if (names[4].find("P0D_")==std::string::npos) return false;

#ifdef DEBUG_P0DFinder
    for (std::vector<std::string>::const_iterator s = names.begin();
         s != names.end();
         ++s) std::cout << " " << *s;
    std::cout << std::endl;
#endif

    if (names.back().find("P0D_")!=std::string::npos) {
        id = ND::GeomId::P0D::Detector();
        return true;
    }

    // Assign the super-P0Dule number.
    if (names.back().find("USECal_")!=std::string::npos) {
        fSP0Dule = 0;
        id = ND::GeomId::P0D::SuperP0Dule(fSP0Dule);
        return true;
    }
    else if (names.back().find("USTarget_")!=std::string::npos) {
        fSP0Dule = 1;
        id = ND::GeomId::P0D::SuperP0Dule(fSP0Dule);
        return true;
    }
    else if (names.back().find("CTarget_")!=std::string::npos) {
        fSP0Dule = 2;
        id = ND::GeomId::P0D::SuperP0Dule(fSP0Dule);
        return true;
    }
    else if (names.back().find("CECal_")!=std::string::npos) {
        fSP0Dule = 3;
        id = ND::GeomId::P0D::SuperP0Dule(fSP0Dule);
        return true;
    }

    // Assign the p0dule number
    if (names.back().find("P0Dule_")!=std::string::npos) {
        ++fP0Dule;
        fLayer = -1;
        fBar = -1;
        id = ND::GeomId::P0D::P0Dule(fP0Dule);
        return true;
    }

    // Assign the target.
    if (names.back().find("Target_") == 0) {
        ++fTarget;
        id = ND::GeomId::P0D::Target(fTarget);
        return true;
    }

    // Assign the radiator.
    if (names.back().find("Radiator_") !=std::string::npos) {
        if (fSP0Dule == 0 || fSP0Dule==3) {
            ++fECalRadiator;
            id = ND::GeomId::P0D::ECalRadiator(fECalRadiator);
        }
        else {
            ++fTargetRadiator;
            id = ND::GeomId::P0D::TargetRadiator(fTargetRadiator);
        }
        return true;
    }

    // Assign the layer 
    if (names.back().find("X_")!=std::string::npos) {
        fLayer = 0;
        fBar = -1;
        return false;
    }
    else if (names.back().find("Y_")!=std::string::npos) {
        fLayer = 1;
        fBar = -1;
        return false;
    }
        
    if (names.back().find("Bar_")!=std::string::npos) {
        id = ND::GeomId::P0D::Bar(fP0Dule, fLayer, ++fBar);
        return true;
    }

    return false;
}
