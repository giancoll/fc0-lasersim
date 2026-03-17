#ifndef TECalIdFinder_hxx_seen
#define TECalIdFinder_hxx_seen

#include "TGeomIdFinder.hxx"

namespace ND {
    class TECalIdFinder;
};

/// Find TGeoManager node ids associated with the TGeometryId objects.
class ND::TECalIdFinder : public TGeomIdFinder {
public: 
    TECalIdFinder();
    ~TECalIdFinder();

    bool Search(const std::vector<std::string>& names, TGeometryId& id);

private: 

    /// The current ECal detector,
    int fECal;

    /// The current magnet clam.
    int fClam;

    /// The current ECal module
    int fModule;

    /// The current ECal layer
    int fLayer;

    /// The current bar in the layer.
    int fBar;

    /// The radiator in the layer.
    int fRadiator;
};
#endif
