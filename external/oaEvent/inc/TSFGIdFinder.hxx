#ifndef TSFGIdFinder_hxx_seen
#define TSFGIdFinder_hxx_seen

#include "TGeomIdFinder.hxx"

namespace ND {
    class TSFGIdFinder;
};

/// Find TGeoManager node ids associated with the TGeometryId objects.
class ND::TSFGIdFinder : public TGeomIdFinder {
public:
    TSFGIdFinder();
    ~TSFGIdFinder();

    bool Search(const std::vector<std::string>& names, TGeometryId& id);

    bool Copy(TGeometryId& src, TGeometryId& dest);

private:
    /// The most recently seen X layer for a cube.
    int fXCube;

    /// The most recently seen Y layer for a cube.
    int fYCube;

    /// The most recently seen Z layer for a cube.
    int fZCube;

    /// A vector of all the cubes that are going to be used to mark fibers.
    std::vector<ND::TGeometryId> fFiberCubes;

    /// The index of the next projection to make copies for.
    int fNextProjection;

    /// The next entry to check
    unsigned int fNextCheck;
};
#endif
