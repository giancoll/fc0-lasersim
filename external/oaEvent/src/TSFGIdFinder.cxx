#include <iostream>

#include <TGeoManager.h>

#include "ND280GeomId.hxx"
#include "ND280GeomIdDef.hxx"
#include "TSFGIdFinder.hxx"

ND::TSFGIdFinder::TSFGIdFinder()
    : fXCube(-1), fYCube(-1), fZCube(-1), fNextProjection(-1) {}

ND::TSFGIdFinder::~TSFGIdFinder() {}

bool ND::TSFGIdFinder::Search(const std::vector<std::string>& names,
                              TGeometryId& id) {
    using namespace ND::GeomId::Def;

    // Assign a geometry identifier to the full P0D.
    if (names.size()<5) return false;
    if (names[4].find("SFG_") != 0) return false;

#ifdef DEBUG_SFGFinder
    for (std::vector<std::string>::const_iterator s = names.begin();
         s != names.end();
         ++s) {
        std::cout << " " << *s;
    }
    std::cout << std::endl;
#endif

    if (names.back().find("SFG_") == 0) {
        id = ND::GeomId::SFG::Detector();
        return true;
    }

    // Assign the Z plane of the cube.
    if (names.back().find("Plane_") == 0) {
        ++fZCube;
        fXCube = -1;
        fYCube = -1;
        return false;
    }

    // Assign the Z plane of the cube.
    if (names.back().find("Column_") == 0) {
        ++fXCube;
        fYCube = -1;
        return false;
    }

    if (names.back().find("Cube_") == 0) {
        ++fYCube;
        id = ND::GeomId::SFG::Cube(fXCube,fYCube,fZCube);
        if (fXCube == 0 || fYCube == 0 || fZCube == 0) {
            // Save this cube as one that will "provide" the volume for a
            // fiber.
            fFiberCubes.push_back(id);
        }
        return true;
    }

    return false;
}

// Make copies of the cubes (on the "zero planes") that mark where the fibers
// are.  The fiber identifier will always correspond to a cube that is at the
// most negative position in the superFGD for that axis.
bool ND::TSFGIdFinder::Copy(TGeometryId& src, TGeometryId& dest) {
    if (fNextProjection < 0) {
        fNextProjection = 0;
        fNextCheck = 0;
    }

    while (fNextProjection < 3) {
        for (;fNextCheck < fFiberCubes.size(); ++fNextCheck) {
            if (fNextProjection == 0) {
                // Copy the YZ fibers.
                int p = ND::GeomId::SFG::GetCubeX(fFiberCubes[fNextCheck]);
                if (p>0) continue;
                src = fFiberCubes[fNextCheck];
                int u = ND::GeomId::SFG::GetCubeY(src);
                int v = ND::GeomId::SFG::GetCubeZ(src);
                dest = ND::GeomId::SFG::YZFiber(u,v);
                ++fNextCheck;
                return true;
            }
            else if (fNextProjection == 1) {
                // Copy the XZ fibers.
                int p = ND::GeomId::SFG::GetCubeY(fFiberCubes[fNextCheck]);
                if (p>0) continue;
                src = fFiberCubes[fNextCheck];
                int u = ND::GeomId::SFG::GetCubeX(src);
                int v = ND::GeomId::SFG::GetCubeZ(src);
                dest = ND::GeomId::SFG::XZFiber(u,v);
                ++fNextCheck;
                return true;
            }
            else if (fNextProjection == 2) {
                // Copy the XY fibers.
                int p = ND::GeomId::SFG::GetCubeZ(fFiberCubes[fNextCheck]);
                if (p>0) continue;
                src = fFiberCubes[fNextCheck];
                int u = ND::GeomId::SFG::GetCubeX(src);
                int v = ND::GeomId::SFG::GetCubeY(src);
                dest = ND::GeomId::SFG::XYFiber(u,v);
                ++fNextCheck;
                return true;
            }
        }
        ++fNextProjection;
        fNextCheck = 0;
    }

    return false;
}
