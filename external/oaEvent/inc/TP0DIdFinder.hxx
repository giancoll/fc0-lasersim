#ifndef TP0DIdFinder_hxx_seen
#define TP0DIdFinder_hxx_seen

#include "TGeomIdFinder.hxx"

namespace ND {
    class TP0DIdFinder;
};

/// Find TGeoManager node ids associated with the TGeometryId objects.
class ND::TP0DIdFinder : public TGeomIdFinder {
public: 
    TP0DIdFinder();
    ~TP0DIdFinder();

    bool Search(const std::vector<std::string>& names, TGeometryId& id);

private: 
    /// The current super-P0Dule
    int fSP0Dule;

    /// The current p0dule within the super-P0Dule
    int fP0Dule;

    /// The current layer within the p0dule
    int fLayer;
    
    /// The current bar within the layer
    int fBar;

    /// The current target.
    int fTarget;

    /// The most recently seen radiator in the ECal.
    int fECalRadiator;
    
    /// The most recently seen radiator in the target.
    int fTargetRadiator;
};
#endif
