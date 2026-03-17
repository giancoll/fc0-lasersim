#ifndef TSMRDIdFinder_hxx_seen
#define TSMRDIdFinder_hxx_seen

#include "TGeomIdFinder.hxx"

namespace ND {
    class TSMRDIdFinder;
};

/// Find TGeoManager node ids associated with the TGeometryId objects.
class ND::TSMRDIdFinder : public TGeomIdFinder {
public: 
    TSMRDIdFinder();
    ~TSMRDIdFinder();

    bool Search(const std::vector<std::string>& names, TGeometryId& id);

private: 
    /// The current clam.
    int fClam;

    /// The current yoke read from the MC.
    int fYoke;

    /// The current layer read from the MC.
    int fLayer;

    /// The current slot read from the MC.
    int fSlot;

    /// The current bar.
    int fBar;
};
#endif
