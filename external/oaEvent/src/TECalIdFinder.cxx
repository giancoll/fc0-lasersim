

#include <iostream>

#include <TGeoManager.h>

#include "ND280GeomId.hxx"
#include "ND280GeomIdDef.hxx"
#include "TECalIdFinder.hxx"

ND::TECalIdFinder::TECalIdFinder()
    : fECal(-1), fClam(-1), fModule(-1), 
      fLayer(-1), fBar(-1), fRadiator(-1) {}
ND::TECalIdFinder::~TECalIdFinder() {}

bool ND::TECalIdFinder::Search(const std::vector<std::string>& names, 
                              TGeometryId& id) {
    using namespace ND::GeomId::Def;

    // Assign a geometry identifier to the full ECal.
    if (names.size()<2) return false;
    if (names[1].find("DsECal_")==std::string::npos) {
      if (names.size() < 5) return false;
      if (names[4].find("DsECal_")==std::string::npos
          && names[4].find("BrlECal_")==std::string::npos
          && names[4].find("P0DECal_")==std::string::npos) return false;
    }

#ifdef DEBUG_ECalFinder
    for (std::vector<std::string>::const_iterator s = names.begin();
         s != names.end();
         ++s) std::cout << " " << *s;
    std::cout << std::endl;
#endif

    if (names.back().find("DsECal_")!=std::string::npos) {
        fECal = ND::GeomId::Def::kDSECal;
        fClam = ND::GeomId::Def::ECal::kNoClam;
        fModule = ND::GeomId::Def::ECal::kNoModule;
        fLayer = -1;
        fBar = -1;
        id = ND::GeomId::DSECal::Detector();
        return true;
    }
    else if (names.back().find("BrlECal_")!=std::string::npos ) {
        fECal = ND::GeomId::Def::kTECal;
         
        fLayer = -1;
        fBar = -1;
        
        if (names[3].find("RightClam_") != std::string::npos) {
            fClam = ND::GeomId::Def::ECal::kNegXClam;
        }
        if (names[3].find("LeftClam_") != std::string::npos) {
            fClam = ND::GeomId::Def::ECal::kPosXClam;
        }
        
        char edge_id = names.back()[names.back().size()-1];
        if (edge_id=='0') {
            fModule = ND::GeomId::Def::ECal::kTopModule;
        }
        else if (edge_id=='1') {
            fModule = ND::GeomId::Def::ECal::kSideModule;
        }
        else if (edge_id=='2') {
            fModule = ND::GeomId::Def::ECal::kBottomModule;
        }
        else {
            return false;
        }
        
        id = ND::GeomId::ECal::Container(fECal,fClam,fModule);
        
        return true;
    }
    
    else if (names.back().find("P0DECal_")!=std::string::npos) {
        fECal = ND::GeomId::Def::kPECal;
         
        fLayer = -1;
        fBar = -1;
        
        if (names[3].find("RightClam_") != std::string::npos) {
            fClam = ND::GeomId::Def::ECal::kNegXClam;
        }
        if (names[3].find("LeftClam_") != std::string::npos) {
            fClam = ND::GeomId::Def::ECal::kPosXClam;
        }
        
        char edge_id = names.back()[names.back().size()-1];
        if (edge_id=='0') {
            fModule = ND::GeomId::Def::ECal::kTopModule;
        }
        else if (edge_id=='1') {
            fModule = ND::GeomId::Def::ECal::kSideModule;
        }
        else if (edge_id=='2') {
            fModule = ND::GeomId::Def::ECal::kBottomModule;
        }
        else {
            return false;
        }
        
        id = ND::GeomId::ECal::Container(fECal,fClam,fModule);
        
        return true;
    }

    if (names.back().find("Bottom_") != std::string::npos) {
        fModule = ND::GeomId::Def::ECal::kBottomModule;
        fLayer = -1;
        fBar = -1;
        id = ND::GeomId::ECal::Module(fECal,fClam,fModule);
        return true;
    }
    else if (names.back().find("Side_") != std::string::npos) {
        fModule = ND::GeomId::Def::ECal::kSideModule;
        fLayer = -1;
        fBar = -1;
        id = ND::GeomId::ECal::Module(fECal,fClam,fModule);
        return true;
    }
    else if (names.back().find("Top_") != std::string::npos) {
        fModule = ND::GeomId::Def::ECal::kTopModule;
        fLayer = -1;
        fBar = -1;
        id = ND::GeomId::ECal::Module(fECal,fClam,fModule);
        return true;
    }

    if (names.back().find("Absorber") != std::string::npos) {
        ++fRadiator;
        id = ND::GeomId::ECal::Radiator(fECal,fClam,fModule,fRadiator);
        return true;
    }

    if (names.back().find("Scint") != std::string::npos) {
        ++fLayer;
        fBar = -1;
        id = ND::GeomId::ECal::Layer(fECal,fClam,fModule,fLayer);
        return true;
    }

    if (names.back().find("Bar_") != std::string::npos) {
        ++fBar;
        id = ND::GeomId::ECal::Bar(fECal,fClam,fModule,fLayer,fBar);
        return true;
    }

    return false;
}
