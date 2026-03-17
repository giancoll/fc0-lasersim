#ifndef TG4VHit_hxx_seen
#define TG4VHit_hxx_seen

#include <vector>

#include <TObject.h>

#include "TDatum.hxx"

namespace ND {
    class TG4VHit;
    class TG4HitContainer;
}

/// DO NOT USE THIS CLASS IN ANALYSIS CODE.
///
/// This is an abstract base class used to save G4Hit objects into a root
/// output file.  It contains the information that all hits from the
/// simulation should save into the output file, and allows the rest of the
/// code to handle hits in a polymorphic way.  It must be inherited by an
/// specific instance class to actually save MC information.  The nd280mc
/// expects the electronics response to be simulated off-line, and to assign
/// the sensor identifiers to each hit.
///
/// In the nd280mc, this class is multiply inherited with G4VHit to create a
/// class that can be used in a G4HitCollection.  The behavior is designed so
/// that the TG4VHit inherited classes in oaEvent contain all of the payload
/// for the hit, and class in nd280mc contains all of the interface related to
/// the G4 infrastructure.
class ND::TG4VHit: public TObject {
public:
    TG4VHit();
    virtual ~TG4VHit();

    /// Print the hit information.
    virtual void ls(Option_t * = "") const;

protected:
    ClassDef(TG4VHit,1);
};

/// A container for the G4 hit detector element information.
class ND::TG4HitContainer : public TDatum, public std::vector<TG4VHit*> {
public:
    TG4HitContainer(const char* name="G4hits",
                        const char* title="G4 Hit Information")
        : TDatum(name,title) {};
    virtual ~TG4HitContainer();

    /// Print the data vector information.
    virtual void ls(Option_t* opt = "") const;

private:
    ClassDef(TG4HitContainer,1);
};
#endif
