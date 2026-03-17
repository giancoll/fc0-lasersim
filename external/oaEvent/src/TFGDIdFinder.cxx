// $Id$

#include <iostream>

#include <TGeoManager.h>

#include "ND280GeomId.hxx"
#include "ND280GeomIdDef.hxx"
#include "TFGDIdFinder.hxx"

ND::TFGDIdFinder::TFGDIdFinder()
    : fFGD(-1), fModule(-1), fLayer(-1), fBar(-1), fTarget(-1) {}
ND::TFGDIdFinder::~TFGDIdFinder() {}

bool ND::TFGDIdFinder::Search(const std::vector<std::string>& names, 
                              TGeometryId& id) {
    using namespace ND::GeomId::Def;

    // Assign a geometry identifier to the full FGD.
    if (names.size()<6) return false;
    if (names[5].find("FGD")==std::string::npos) return false;

#ifdef DEBUG_FGDFinder
    for (std::vector<std::string>::const_iterator s = names.begin();
         s != names.end();
         ++s) std::cout << " " << *s;
    std::cout << std::endl;
#endif

    if (names.back().find("FGD1_")!=std::string::npos) {
        id = ND::GeomId::FGD::FGD1();
        fFGD = 0;
        fModule = -1;
        fLayer = -1;
        fBar = -1;
        fTarget = -1;
        return true;
    }
    
    if (names.back().find("FGD2_")!=std::string::npos) {
        id = ND::GeomId::FGD::FGD2();
        fFGD = 1;
        fModule = -1;
        fLayer = -1;
        fBar = -1;
        fTarget = -1;
        return true;
    }

    if (names.back().find("ScintX_") != std::string::npos) {
        ++fModule;
        fLayer = 0;
        fBar = -1;
        id = ND::GeomId::FGD::Layer(fFGD,fModule,fLayer);
        return true;
    }

    if (names.back().find("ScintY_") != std::string::npos) {
        fLayer = 1;
        fBar = -1;
        id = ND::GeomId::FGD::Layer(fFGD,fModule,fLayer);
        return true;
    }

    if (names.back().find("Water") != std::string::npos) {
        ++fTarget;
        id = ND::GeomId::FGD::Target(fTarget);
        return true;
    }

    if (names.back().find("Bar_") != std::string::npos) {
        ++fBar;
        id = ND::GeomId::FGD::Bar(fFGD,fModule,fLayer,fBar);
        return true;
    }

    return false;
}
