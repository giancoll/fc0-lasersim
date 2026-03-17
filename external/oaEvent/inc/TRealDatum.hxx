#ifndef TRealDatum_hxx_seen
#define TRealDatum_hxx_seen

#include <TDatum.hxx>
#include <method_deprecated.hxx>

#include <vector>

namespace ND {
    class TRealDatum;
}

/// Save floating point values as a TDatum.  This behaves a lot like a
/// standard vector, but doesn't implement all of the methods.  The type of
/// the element is defined by the value_type typedef.
class ND::TRealDatum: public TDatum {
public:
    typedef double value_type;
    typedef std::vector<double>::iterator iterator;
    typedef std::vector<double>::const_iterator const_iterator;

    TRealDatum();
    explicit TRealDatum(const char* name, double value=0);
    TRealDatum(const char* name, const char* title, double value=0);
    TRealDatum(const TRealDatum& rhs);

    virtual ~TRealDatum();

    /// Get the value of this datum.  This returns the value of the first
    /// element of the vector (if there are more than one elements.
    double GetValue(void) const;

    /// Set the value of this datum.  This sets the value of the first
    /// element of the vector (if there are more than one elements.
    void SetValue(double value);

    /// As defined in the STL vector.
    iterator begin() {return fValue.begin();}
    /// As defined in the STL vector.
    iterator end() {return fValue.end();}
    /// As defined in the STL vector.
    const_iterator begin() const {return fValue.begin();}
    /// As defined in the STL vector.
    const_iterator end() const {return fValue.end();}

    /// As defined in the STL vector.
    double& operator [] (int i) {return fValue[i];}
    /// As defined in the STL vector.
    const double& operator [] (int i) const {return fValue[i];}
    /// As defined in the STL vector.
    void clear() {fValue.clear();}
    /// As defined in the STL vector.
    double& at(int i) {return fValue.at(i);}
    /// As defined in the STL vector.
    const double& at(int i) const {return fValue.at(i);}
    /// As defined in the STL vector.
    unsigned int size() const {return fValue.size();}
    /// As defined in the STL vector.
    void push_back(double r) {fValue.push_back(r);}

    void ls(Option_t* opt = "") const;

    /// Do not use this method.  Most of the important vector methods are now
    /// provided (begin(), end(), clear(), at(), size(), push_back()).
    std::vector<double>& GetVector(void) METHOD_DEPRECATED;

    /// Do not use this method.  Most of the important vector methods are now
    /// provided (begin(), end(), clear(), at(), size(), push_back()).
    const std::vector<double>& GetVector(void) const METHOD_DEPRECATED;

    /// Do not use this method.  Use the at() method instead of At().
    double At(int i) const METHOD_DEPRECATED {return fValue.at(i);}

private:
    std::vector<double> fValue;

    ClassDef(TRealDatum,2);
};
#endif
