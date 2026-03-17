////////////////////////////////////////////////////////////
// $Id$
//
#include <TClass.h>
#include <TStreamerInfo.h>
#include <TDatabasePDG.h>
#include <TParticlePDG.h>

#include "TG4PrimaryParticle.hxx"
#include "TND280Log.hxx"

ClassImp(ND::TG4PrimaryParticle);

ND::TG4PrimaryParticle::TG4PrimaryParticle(void)
    : fTrackId(-1), fPDGCode(0), fMomentum(0,0,0,0) {}

ND::TG4PrimaryParticle::~TG4PrimaryParticle() {}

const TParticlePDG* ND::TG4PrimaryParticle::GetParticle(void) const {
    TParticlePDG* particle = TDatabasePDG::Instance()->GetParticle(fPDGCode);
    if (!particle) {
        ND280Log("TG4PrimaryParticle:: Warning -- Undefined PDG code: "
                 << fPDGCode << " [RETURNING NULL]");
    }
    return particle;
}

void ND::TG4PrimaryParticle::ls(Option_t *opt) const {
    ND::ls_header(this,opt);
    std::cout << " Id: " << fTrackId
              << " Particle: " << fPDGCode << " (" << GetTitle() << ")"
              << " E: " << fMomentum.E()
              << std::endl;
}

