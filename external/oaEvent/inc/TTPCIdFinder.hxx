#ifndef TTPCIdFinder_hxx_seen
#define TTPCIdFinder_hxx_seen

#include "TGeomIdFinder.hxx"

namespace ND {
    class TTPCIdFinder;
};

/// Find TGeoManager node ids associated with the TGeometryId objects.
class ND::TTPCIdFinder : public TGeomIdFinder {
public: 
    TTPCIdFinder();
    ~TTPCIdFinder();

    bool Search(const std::vector<std::string>& names, TGeometryId& id);

private: 
    /// The current TPC module
    int fModule;

    /// The current half of the TPC module
    int fHalf;
 
    /// The current micromega
    int fMicroMega;
};
#endif
