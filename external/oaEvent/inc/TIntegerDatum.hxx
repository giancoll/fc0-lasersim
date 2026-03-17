#ifndef TIntegerDatum_hxx_seen
#define TIntegerDatum_hxx_seen

#include <TDatum.hxx>
#include <method_deprecated.hxx>

#include <vector>

namespace ND {
    class TIntegerDatum;
}

/// Save integer values as a TDatum.  This behaves a lot like a standard
/// vector, but doesn't implement all of the methods.  The type of the element
/// (int) is defined by the value_type typedef.
class ND::TIntegerDatum: public TDatum {
public:
    typedef int value_type;
    typedef std::vector<int>::iterator iterator;
    typedef std::vector<int>::const_iterator const_iterator;

    TIntegerDatum();
    explicit TIntegerDatum(const char* name, int value=0);
    TIntegerDatum(const char* name, const char* title, int value=0);
    TIntegerDatum(const TIntegerDatum&);

    virtual ~TIntegerDatum();

    /// Get the value of this datum.  This returns the value of the first
    /// element of the vector (if there are more than one elements.
    int GetValue(void) const;

    /// Set the value of this datum.  This sets the value of the first
    /// element of the vector (if there are more than one elements.
    void SetValue(int value);

    /// As defined in the STL vector.
    iterator begin() {return fValue.begin();}
    /// As defined in the STL vector.
    iterator end() {return fValue.end();}
    /// As defined in the STL vector.
    const_iterator begin() const {return fValue.begin();}
    /// As defined in the STL vector.
    const_iterator end() const {return fValue.end();}

    /// As defined in the STL vector.
    int& operator [] (int i) {return fValue[i];}
    /// As defined in the STL vector.
    const int& operator [] (int i) const {return fValue[i];}
    /// As defined in the STL vector.
    void clear() {fValue.clear();}
    /// As defined in the STL vector.
    int& at(int i) {return fValue.at(i);}
    /// As defined in the STL vector.
    const int& at(int i) const {return fValue.at(i);}
    /// As defined in the STL vector.
    unsigned int size() const {return fValue.size();}
    /// As defined in the STL vector.
    void push_back(int i) {fValue.push_back(i);}

    void ls(Option_t* opt = "") const;

    /// Do not use this method.  Most of the important vector methods are now
    /// provided (begin(), end(), clear(), at(), size(), push_back()).
    std::vector<int>& GetVector(void) METHOD_DEPRECATED;

    /// Do not use this method.  Most of the important vector methods are now
    /// provided (begin(), end(), clear(), at(), size(), push_back()).
    const std::vector<int>& GetVector(void) const METHOD_DEPRECATED;

    /// Do not use this method.  Use the at() method instead of At().
    int At(int i) const METHOD_DEPRECATED {return fValue.at(i);}

private:
    std::vector<int> fValue;

    ClassDef(TIntegerDatum,2);
};
#endif
