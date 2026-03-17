#ifndef TPackageSet_hxx_seen
#define TPackageSet_hxx_seen
/////////////////////////////////////////////////////////////////////
// $Id$
// 
// Define a container for the package versions used in an executable.

#include "TPackageVersion.hxx"
#include <set>

namespace ND {
    /// The set of package versions used in the currently running executable.
    typedef std::set < TPackageVersion * > TPackageSet;
}

#endif
