#include "TOADatabase.hxx"

// Source for ToaEvent_version.cxx auto-generated using the
// Tpackage_version.cxx.in template file.

#include "ToaEvent_version.hxx"
#include "oaEvent_version.h"
#include "oaEvent_make_date.hxx"

ClassImp(ND::ToaEvent_version);

// Trickiness so that the package version is automatically added to the
// list of used packages.
static ND::ToaEvent_version oaEvent_version;

ND::ToaEvent_version* ND::ToaEvent_version::fThis = NULL;

ND::ToaEvent_version::ToaEvent_version() {
    fThis = ND::ToaEvent_version::Get();
}

ND::ToaEvent_version::~ToaEvent_version() {}

void ND::ToaEvent_version::Initialize(void) {
    // register this package.
    ND::TOADatabase::Get().PackageSet().insert(fThis);
}

ND::ToaEvent_version* ND::ToaEvent_version::Get(void) {
    // Make sure that fThis is initialized;
    if (!fThis) {
        // Make sure that fThis is not null before allocating a real pointer.
        // This cruft is required so that there isn't an infinite recursion
        // while fThis is initialized.
        fThis = (ND::ToaEvent_version*) 1;
        // Allocate real space for the fThis pointer.
        fThis = new ND::ToaEvent_version;
        // Now initialize
        fThis->Initialize();
    }
    // Return the pointer.
    return fThis;
}

const char* ND::ToaEvent_version::GetName(void) const {
    return oaEvent_NAME;
}

const char* ND::ToaEvent_version::GetVersion(void) const {
    return oaEvent_VERSION;
}

const char* ND::ToaEvent_version::GetCompilationDate(void) const {
    return oaEvent_COMPILE_DATE;
}

const char* ND::ToaEvent_version::GetCompilationHost(void) const {
    return oaEvent_COMPILE_HOST;
}

const char* ND::ToaEvent_version::GetCompilationDirectory(void) const {
    return oaEvent_COMPILE_DIR;
}

const char* ND::ToaEvent_version::GetCompilationMachineInfo(void) const {
    return oaEvent_COMPILE_UNAME;
}
