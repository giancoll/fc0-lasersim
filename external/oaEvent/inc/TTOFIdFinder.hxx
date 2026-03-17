#ifndef TTOFIdFinder_hxx_seen
#define TTOFIdFinder_hxx_seen

#include "TGeomIdFinder.hxx"

namespace ND {
    class TTOFIdFinder;
};

/// Find TGeoManager node ids associated with the TGeometryId objects.
class ND::TTOFIdFinder : public TGeomIdFinder {
public:
    TTOFIdFinder();
    ~TTOFIdFinder();

    // Check if the current node gets a geometry id.
    bool Search(const std::vector<std::string>& names, TGeometryId& id);

    // Copy the bar identifiers into the sensor identifiers.
    bool Copy(TGeometryId& src, TGeometryId& dest);

private:

    /// The most recently seen module.
    int fModule;

    /// The most recently seen bar in the most recent module.
    int fBar;

    /// A vector of all the bars that will have sensors (that means all of the
    /// bars).
    std::vector<ND::TGeometryId> fBarIdentifiers;

    /// The index of the next bar to add a sensor too.
    int fNextBar;

    /// The next sensor to create;
    int fNextSensor;
};
#endif
