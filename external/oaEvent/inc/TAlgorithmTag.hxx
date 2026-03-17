#ifndef TAlgorithmTag_hxx_seen
#define TAlgorithmTag_hxx_seen

#include <string>
#include <vector>

#include <TNamed.h>

namespace ND {
    class TAlgorithmTag;
    class TAlgorithm;
}

/// A tag that carries information about an algorithm name, version and
/// compilation date.
///
/// \todo The TAlgorithmTag creation API is relatively well defined and the
/// constructor creates a filled tag.  However, this still allows the
/// possiblity of a user to add extra information.  The public setters should
/// be removed from the interface.
class ND::TAlgorithmTag: public TNamed {
public:
    TAlgorithmTag();
    TAlgorithmTag(const TAlgorithm& algo);
    virtual ~TAlgorithmTag();

    /// Add a string of information to the tag.
    virtual void Add(const char* info);

    /// Add a string of information to the tag.
    virtual void Add(const std::string& info);

    /// Return the tags for the algorithm as a vector of strings.
    std::vector<std::string> GetTagList() const {return fTag;}

    /// Print the result information.
    virtual void ls(Option_t *opt = "") const;

private:   
    
    std::vector<std::string> fTag; 

    ClassDef(TAlgorithmTag,2);
};
#endif
