#ifndef TG4PrimaryVertex_hxx_seen
#define TG4PrimaryVertex_hxx_seen

#include <vector>

#include <TLorentzVector.h>

#include "TDatum.hxx"
#include "TG4PrimaryParticle.hxx"

namespace ND {
    class TG4PrimaryVertexContainer;
    class TG4PrimaryVertex;
}

/// A container of primary vertices.  Normally, there is only one element, but
/// you need to be careful since some neutrino events don't have any primary
/// particles (e.g. neutral current events where the scattered nucleon is
/// absorbed by the nucleus).  It's also possible that an event could have
/// multiple vertices (i.e. more than one interaction per spill).
/// A container for all of the trajectory information.
class ND::TG4PrimaryVertexContainer 
    : public TDatum , public std::vector<TG4PrimaryVertex> {
public:
    TG4PrimaryVertexContainer(const char* name,
                              const char* title);
    TG4PrimaryVertexContainer();
    ~TG4PrimaryVertexContainer();
  
    /// Print the data vector information.
    virtual void ls(Option_t* opt = "") const;

private:
    
    ClassDef(TG4PrimaryVertexContainer,2);
};

/// A class to save a G4 primary vertex into a root output file without linking
/// to geant.
class ND::TG4PrimaryVertex: public TObject {
public:
    TG4PrimaryVertex(void);
    TG4PrimaryVertex(const TG4PrimaryVertex& rhs);
    virtual ~TG4PrimaryVertex();

    /// Return the name of the generator that made this vertex.
    const char* GetGeneratorName() const {return fGeneratorName.c_str();}

    /// Set the name of this vertex
    void SetGeneratorName(const char* name);

    /// Return the name of the reaction that made this vertex.
    const char* GetReaction() const {return fReaction.c_str();}

    /// Set the name of the reaction that made this vertex.
    void SetReaction(const char* reaction);

    /// Return the position of the vertex.  Analysis code should always get
    /// the position into a const TLorentzVector& or into a copy.
    const TLorentzVector& GetPosition() const {
        return fPosition;
    }

    /// Return a mutable position of the vertex used to build the original
    /// object. 
    TLorentzVector& GetPosition() {
        return fPosition;
    }

    /// Return the particles that are associated with this vertex.  Analysis
    /// code should always use the const version of this method (assign to a
    /// const TG4PrimaryParticleContainer). 
    const TG4PrimaryParticleContainer& GetPrimaryParticles(void) const {
        return fPrimaryParticles;
    }
    
    /// A mutable version used to build the object.
    TG4PrimaryParticleContainer& GetPrimaryParticles(void) {
        return fPrimaryParticles;
    }
    
    /// Get the informational vertex container.
    const TG4PrimaryVertexContainer& GetInfoVertex() const {
        return fInformational;
    }

    /// Add a new informational vertex.
    void SetInfoVertex(const TG4PrimaryVertexContainer& info);

    /// Get the name of the input file.  For a root file, the input filename
    /// includes the path to the input tree.
    const char* GetFilename() const {return fFilename.c_str();}

    /// Set the name of the input file.  For a root file, the input filename
    /// has the format "filename.root:path/to/tree:entry".  This is the full
    /// path name of a folder as specified by TDirectory, plus the entry
    /// number.
    void SetFilename(const char* filename);

    /// Set the index of the interaction within the input interaction file.
    void SetInteractionNumber(int i) {fInteractionNumber = i;}

    /// Get the index of the interaction within the input interaction file.
    int GetInteractionNumber() const {return fInteractionNumber;}

    /// Set the cross section for the reaction that created this vertex.  This
    /// should be in normal HEP units.
    void SetCrossSection(double xs) {fCrossSection = xs;}

    /// Get the cross section for the reaction that created this vertex.
    double GetCrossSection() const {return fCrossSection;}

    /// Set the differential cross section for the kinematics that created
    /// this vertex.  This should be in normal HEP units.
    void SetDiffCrossSection(double xs) {fDiffCrossSection = xs;}
    
    /// Get the differential cross section for the kinematics of the reaction
    /// that created this vertex.
    double GetDiffCrossSection() const {return fDiffCrossSection;}

    /// Set the weight for the vertex.  This is used if the generator is using
    /// a weighted sample (e.g. over sampling the high energy part of the
    /// neutrino spectrum smooth out the distributions).
    void SetWeight(double w) {fWeight = w;}
    
    /// Get the weight of the vertex.  This will be one if it's not filled.
    double GetWeight() const {return fWeight;}

    /// Set the probability of the interaction that created this vertex.  This
    /// includes the effect of the cross section, path length through the
    /// material, etc.
    void SetProbability(double p) {fProbability = p;}

    /// Get the probability of the interaction.
    double GetProbability() const {return fProbability;}

    /// Print the hit information.
    virtual void ls(Option_t* opt = "") const;

private:
    /// The initial position of the particle.
    TLorentzVector fPosition;

    /// The PrimaryVertex points for this PrimaryVertex.
    TG4PrimaryParticleContainer fPrimaryParticles;

    /// The name of the generator that created this vertex.
    std::string fGeneratorName;

    /// The reaction that created this vertex.
    std::string fReaction;

    /// The informational vertices associated with this vertex.
    TG4PrimaryVertexContainer fInformational;

    /// The name of the input file.
    std::string fFilename;

    /// The index (or identifier) of the interaction in the kinematics file.
    int fInteractionNumber;

    /// The cross section for the reaction that created this vertex.
    float fCrossSection;

    /// The differential cross section for the kinematics of the reaction that
    /// created this vertex.
    float fDiffCrossSection;

    /// The weight of the interaction.  This will be set to one if the
    /// interaction is not reweighted.  If the vertex is oversampled, this
    /// will be less than one.
    float fWeight;

    /// The overall probability of the interaction that created this vertex.
    /// This includes the effect of the cross section, path length through the
    /// material, etc.  This will be one if it is not filled.
    float fProbability;

    ClassDef(TG4PrimaryVertex,6);
};
#endif
