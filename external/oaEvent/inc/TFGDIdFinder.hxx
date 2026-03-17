#ifndef TFGDIdFinder_hxx_seen
#define TFGDIdFinder_hxx_seen

#include "TGeomIdFinder.hxx"

namespace ND {
    class TFGDIdFinder;
};

/// Find TGeoManager node ids associated with the TGeometryId objects.
class ND::TFGDIdFinder : public TGeomIdFinder {
public: 
    TFGDIdFinder();
    ~TFGDIdFinder();

    bool Search(const std::vector<std::string>& names, TGeometryId& id);

private: 

    /// The current FGD module
    int fFGD;

    /// The current FGD plane
    int fModule;

    /// The current layer in the plane (X or Y);
    int fLayer;

    /// The current bar in the layer.
    int fBar;

    /// The current target.
    int fTarget;
};
#endif
