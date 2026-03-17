////////////////////////////////////////////////////////////
// $Id$
//

#include <algorithm>
#include <cmath>

#include <TClass.h>
#include <TStreamerInfo.h>

#include "HEPUnits.hxx"
#include "TG4PrimaryVertex.hxx"

ClassImp(ND::TG4PrimaryVertex);
ClassImp(ND::TG4PrimaryVertexContainer);

ND::TG4PrimaryVertexContainer::TG4PrimaryVertexContainer(const char* name, const char* title)
    : ND::TDatum(name,title) {}

ND::TG4PrimaryVertexContainer::TG4PrimaryVertexContainer() {}

ND::TG4PrimaryVertexContainer::~TG4PrimaryVertexContainer() {}

ND::TG4PrimaryVertex::TG4PrimaryVertex(void)
    : fPosition(0,0,0,0), fGeneratorName("none"),
      fInteractionNumber(0), fCrossSection(0.0), fDiffCrossSection(0.0),
      fWeight(0.0), fProbability(0.0) {}

ND::TG4PrimaryVertex::TG4PrimaryVertex(const ND::TG4PrimaryVertex& rhs) 
    : TObject(rhs),
      fPosition(rhs.fPosition), fPrimaryParticles(rhs.fPrimaryParticles),
      fGeneratorName(rhs.fGeneratorName), fReaction(rhs.fReaction), 
      fInformational(rhs.fInformational), fFilename(rhs.fFilename),
      fInteractionNumber(rhs.fInteractionNumber), 
      fCrossSection(rhs.fCrossSection),
      fDiffCrossSection(rhs.fDiffCrossSection),
      fWeight(rhs.fWeight), fProbability(rhs.fWeight) {}

ND::TG4PrimaryVertex::~TG4PrimaryVertex() { }

void ND::TG4PrimaryVertex::ls(Option_t* opt) const {
    ND::ls_header(this,opt);
    std::cout << " #" << fInteractionNumber 
              << " at: (" << fPosition.X()/unit::cm
              << "," << fPosition.Y()/unit::cm
              << "," << fPosition.Z()/unit::cm
              << "," << fPosition.T()/unit::ns
              << ") (cm,ns)" 
              << std::endl;
    TROOT::IncreaseDirLevel();
    std::string generatorName = GetGeneratorName();
    std::string reactionMode = GetReaction();
    if (generatorName != "" || reactionMode != "") {
        TROOT::IndentLevel();
        std::cout << "Generator: " << generatorName 
                  << "  Reaction: <" << reactionMode << ">"
                  << std::endl;
    }
    std::string filename = GetFilename();
    if (filename != "") {
        TROOT::IndentLevel();
        std::cout << "Filename: " << filename << std::endl;
    }
    if (fCrossSection>0.0 || fDiffCrossSection>0.0) {
        TROOT::IndentLevel();
        std::cout << "Cross Section: " 
                  << fCrossSection/(unit::femto*unit::barn) << "fb"
                  << "   Diff: "
                  << fDiffCrossSection/(unit::femto*unit::barn) << "fb"
                  << std::endl;
    }
    if (std::abs(fProbability-1.0)>1E-3 || std::abs(fWeight-1.0)>1E-3) {
        TROOT::IndentLevel();
        std::cout << "Probability: " << fProbability 
                  << "  Weight: "  << fWeight
                  << std::endl;
    }
    for (ND::TG4PrimaryParticleContainer::const_iterator v 
             = fPrimaryParticles.begin();
         v != fPrimaryParticles.end(); 
         ++v) {
        v->ls(opt);
    };
    if (fInformational.size()>0) {
        TROOT::IncreaseDirLevel();
        fInformational.ls(opt);
        TROOT::DecreaseDirLevel();
    }
    TROOT::DecreaseDirLevel();
}

void ND::TG4PrimaryVertexContainer::ls(Option_t* opt) const {
    ND::TDatum::ls(opt);
    TROOT::IncreaseDirLevel();
    for (const_iterator v = begin();
         v != end(); 
         ++v) {
        v->ls(opt);
    };
    TROOT::DecreaseDirLevel();
}

void ND::TG4PrimaryVertex::SetGeneratorName(const char *name) {
    fGeneratorName = name;
}

void ND::TG4PrimaryVertex::SetReaction(const char *reaction) {
    fReaction = reaction;
}

void ND::TG4PrimaryVertex::SetInfoVertex(const ND::TG4PrimaryVertexContainer& info) {
    fInformational = info;
}

void ND::TG4PrimaryVertex::SetFilename(const char *filename) {
    fFilename = filename;
}
