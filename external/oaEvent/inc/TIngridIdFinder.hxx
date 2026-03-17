#ifndef TIngridIdFinder_hxx_seen
#define TIngridIdFinder_hxx_seen

#include "TGeomIdFinder.hxx"

namespace ND {
    class TIngridIdFinder;
};

/// Find TGeoManager node ids associated with the TGeometryId objects for Ingrid.
class ND::TIngridIdFinder : public TGeomIdFinder {
    public:
            TIngridIdFinder();
            ~TIngridIdFinder();

            bool Search(const std::vector<std::string>& names, TGeometryId& id);

};
#endif
