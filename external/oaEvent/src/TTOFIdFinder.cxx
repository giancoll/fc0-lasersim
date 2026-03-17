#include <iostream>

#include <TGeoManager.h>

#include "TND280Log.hxx"
#include "ND280GeomId.hxx"
#include "ND280GeomIdDef.hxx"
#include "TTOFIdFinder.hxx"

ND::TTOFIdFinder::TTOFIdFinder() : fModule(-1), fBar(-1),
                                   fNextBar(-1), fNextSensor(0) {}

ND::TTOFIdFinder::~TTOFIdFinder() {}

bool ND::TTOFIdFinder::Search(const std::vector<std::string>& names,
                              TGeometryId& id) {
    using namespace ND::GeomId::Def;

    if (names.size()<5) return false;
    if (names[4].find("TOF_")==std::string::npos) return false;

#ifdef DEBUG_TOFFinder
    for (std::vector<std::string>::const_iterator s = names.begin();
         s != names.end();
         ++s) {
        std::cout << " " << *s;
    }
    std::cout << std::endl;
#endif

    if (names.back().find("NTOF_")!=std::string::npos) {
        id = ND::GeomId::TOF::North();
        fModule = ND::GeomId::Def::TOF::kNorthModule;
        fBar = -1;
        return true;
    }
    if (names.back().find("STOF_")!=std::string::npos) {
        id = ND::GeomId::TOF::South();
        fModule = ND::GeomId::Def::TOF::kSouthModule;
        fBar = -1;
        return true;
    }
    if (names.back().find("BTOF_")!=std::string::npos) {
        id = ND::GeomId::TOF::Bottom();
        fModule = ND::GeomId::Def::TOF::kBottomModule;
        fBar = -1; 
        return true;
    }
    if (names.back().find("TTOF_")!=std::string::npos) {
        id = ND::GeomId::TOF::Top();
        fModule = ND::GeomId::Def::TOF::kTopModule;
        fBar = -1;
        return true;
    }
    if (names.back().find("UTOF_")!=std::string::npos) {
        id = ND::GeomId::TOF::Upstream();
        fModule = ND::GeomId::Def::TOF::kUpstreamModule;
        fBar = -1;
        return true;
    }
    if (names.back().find("DTOF_")!=std::string::npos) {
        id = ND::GeomId::TOF::Downstream();
        fModule = ND::GeomId::Def::TOF::kDownstreamModule;
        fBar = -1;
        return true;
    }

    if (names.back().find("Bar_")!=std::string::npos) {
        
        fBar++;
        int thisBar = fBar;
        
        // The Bottom module has only 18 bars, first one is 1 ( not 0)
        if (fModule == ND::GeomId::Def::TOF::kBottomModule) 
            thisBar++;

        id = ND::GeomId::TOF::Bar(fModule,thisBar);
        fBarIdentifiers.push_back(id);
        return true;
    }

    return false;
}

bool ND::TTOFIdFinder::Copy(TGeometryId& src, TGeometryId& dest) {
    if (fNextBar < 0) {
        fNextBar = 0;
        fNextSensor = -1;
    }

    // Check to see if there are more bars that need sensors.
    if (fNextBar >= (int) fBarIdentifiers.size()) return false;

    // Make the destination identifier
    src = fBarIdentifiers[fNextBar];
    dest = ND::GeomId::TOF::Sensor(ND::GeomId::TOF::GetModule(src),
                                   ND::GeomId::TOF::GetBar(src),
                                   fNextSensor);

    // If the current sensor is negative, the next is positive (but on the
    // same bar.
    if (fNextSensor < 0) {
        fNextSensor = 1;
        return true;
    }

    // If the current sensor is positive, then the next is negative (but on
    // the NEXT bar).
    if (fNextSensor > 0) {
        ++fNextBar;
        fNextSensor = -1;
        return true;
    }

    ND280Error("This cannot happen");
    throw std::runtime_error("Logic error in TTOFIdFinder::Copy");

    return false;
}
