#ifndef TPackageVersion_hxx_seen
#define TPackageVersion_hxx_seen
/////////////////////////////////////////////////////////////////////
// $Id$
// 
// Define the base type for all of the package version classes.

#include <vector>
#include <TObject.h>

namespace ND {
    class TPackageVersion;
}

/// This provides the facility to track the compilation history of packages.
/// The build environment creates a file ../src/[package]_version.hxx which
/// should be included in the implementation of classes which inherit from
/// this one.  The information in that include file allows useful information
/// to be returned from the Getter methods.
class ND::TPackageVersion: public TObject {
public:
    TPackageVersion();
    virtual ~ TPackageVersion();

    /// Do any initialization needed for this package.  This is called
    /// by the constructor.  
    virtual void Initialize(void) {}

    /// Return the version of this library.
    virtual const char *GetName(void) const {
        return "none";
    }

    /// Return the version of this library.
    virtual const char *GetVersion(void) const {
        return "none";
    }

    /// Return the date that this library was compiled.
    virtual const char *GetCompilationDate(void) const {
        return "none";
    }

    /// Return the host that this library was compiled on.
    virtual const char *GetCompilationHost(void) const {
        return "none";
    }

    /// Return the directory from which this library was compiled.
    virtual const char *GetCompilationDirectory(void) const {
        return "none";
    }

    /// Return the machine information for the machine that compiled this 
    /// library.  On most machines this is generated from "uname -a".
    const char *GetCompilationMachineInfo(void) const {
        return "none";
    }

    ClassDef(TPackageVersion, 0);
};

#endif
