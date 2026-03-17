#ifndef THATIdFinder_hxx_seen
#define THATIdFinder_hxx_seen

#include "TGeomIdFinder.hxx"

namespace ND {
    class THATIdFinder;
};

/// Find TGeoManager node ids associated with the TGeometryId objects.
class ND::THATIdFinder : public TGeomIdFinder {
public:
    THATIdFinder();
    ~THATIdFinder();

    bool Search(const std::vector<std::string>& names, TGeometryId& id);

private:
    /// The current HAT module
    int fModule;

    /// The current half of the HAT module
    int fHalf;

    /// The current micromega
    int fMicroMega;
};
#endif
