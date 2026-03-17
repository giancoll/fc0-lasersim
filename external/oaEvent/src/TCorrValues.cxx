#include <limits>
#include <cmath>
#include <algorithm>

#include "TCorrValues.hxx"
#include "TND280Log.hxx"

ClassImp(ND::TCorrValues);

/// Set the variance value for a free parameter.  The value is about 1E+154
/// (1E+19) for a double (float).
const ND::TCorrValues::Element ND::TCorrValues::kFreeValue = 
    std::sqrt(std::numeric_limits<float>::max());
const ND::TCorrValues::Element ND::TCorrValues::kFreeThreshold =
    0.1*ND::TCorrValues::kFreeValue;
const ND::TCorrValues::Element 
    ND::TCorrValues::kFixedValue = 1.0/ND::TCorrValues::kFreeValue;
const ND::TCorrValues::Element 
    ND::TCorrValues::kFixedThreshold = 1.0/ND::TCorrValues::kFreeThreshold;

ND::TCorrValues::TCorrValues(int n) 
    : fVector(n), fMatrix(n), fNDOF(0), fTypeHash(0), fHessian(NULL) {
    // All parameters are initially free.
    for (int i=0; i<GetDimensions(); ++i) {
        fMatrix(i,i) = kFreeValue;
    }
}

ND::TCorrValues::TCorrValues(double value, double uncertainty)
    : fVector(1), fMatrix(1), fNDOF(1), fTypeHash(0), fHessian(NULL) {
    SetValue(0,value);
    SetCovarianceValue(0,0,uncertainty*uncertainty);
}

ND::TCorrValues::TCorrValues(const TCorrValues& hv) 
    : TObject(hv), fVector(hv.fVector), fMatrix(hv.fMatrix), 
      fNDOF(hv.fNDOF), fTypeHash(hv.fTypeHash), fHessian(NULL) {}

ND::TCorrValues::TCorrValues(const TVectorT<float>& v) 
    : fVector(v), fMatrix(v.GetNoElements()),
      fNDOF(0), fTypeHash(0), fHessian(NULL) {
    // All parameters are initially free.
    for (int i=0; i<GetDimensions(); ++i) {
        fMatrix(i,i) = kFreeValue;
    }
}

ND::TCorrValues::TCorrValues(const TVectorT<float>& v,
                               const TVectorT<float>& err) 
    : fVector(v), fMatrix(v.GetNoElements()),
      fNDOF(0), fTypeHash(0), fHessian(NULL) {
    if (err.GetNoElements() != v.GetNoElements()) {
        ND280Error("Mismatched number of elements");
        throw ECorrValuesConstructor();
    }
    for (int i = 0; i < GetDimensions(); ++i) {
        fMatrix(i,i) = err(i);
    }
}

ND::TCorrValues::TCorrValues(const TVectorT<float>& v,
                               const TMatrixTSym<float>& cov) 
    : fVector(v), fMatrix(cov),
      fNDOF(0), fTypeHash(0), fHessian(NULL) {
    if (cov.GetNcols() != fVector.GetNoElements()) {
        ND280Error("Mismatch between element count and covariance columns.");
        throw ECorrValuesConstructor();
    }
    if (cov.GetNrows() != fVector.GetNoElements()) {
        ND280Error("Mismatch between element count and covariance rows.");
        throw ECorrValuesConstructor();
    }
}

void ND::TCorrValues::SetValues(const TVectorT<float>& v) {
    fVector = v;
}

void ND::TCorrValues::SetCovariance(const TMatrixTSym<float>& m) {
    fMatrix = m;
    fNDOF = 0;
    if (fHessian) delete fHessian;
    fHessian = NULL;
}

void ND::TCorrValues::SetValue(int i, double v) {
    if (i<0) {
        ND280Error("Negative element index: " << i);
        throw ECorrValuesRange();
    }
    if (GetDimensions()<=i) {
        ND280Error("Out of bounds element index: " << i 
                   << " (dim is " << GetDimensions() << ")");
        throw ECorrValuesRange();
    }
    fVector[i] = v;
}

double ND::TCorrValues::GetValue(int i) const {
    if (i<0) {
        ND280Error("Negative element index: " << i);
        throw ECorrValuesRange();
    }
    if (GetDimensions()<=i) {
        ND280Error("Out of bounds element index: " << i 
                   << " (dim is " << GetDimensions() << ")");
        throw ECorrValuesRange();
    }
    return fVector[i];
}

void ND::TCorrValues::SetCovarianceValue(int i, int j, double v) {
    if (i<0 || j<0) {
        ND280Error("Negative covariance index: (" << i << "," << j << ")");
        throw ECorrValuesRange();
    }
    if (GetDimensions()<=i || GetDimensions()<=j) {
        ND280Error("Negative covariance index: (" << i << "," << j << ")");
        throw ECorrValuesRange();
    }
    fMatrix(i,j) = v;
    fMatrix(j,i) = v;
    fNDOF = 0;
    if (fHessian) delete fHessian;
    fHessian = NULL;
}

double ND::TCorrValues::GetCovarianceValue(int i, int j) const {
    if (i<0 || j<0) {
        ND280Error("Negative covariance index: (" << i << "," << j << ")");
        throw ECorrValuesRange();
    }
    if (GetDimensions()<=i || GetDimensions()<=j) {
        ND280Error("Negative covariance index: (" << i << "," << j << ")");
        throw ECorrValuesRange();
    }
    return fMatrix(i,j);
}

double ND::TCorrValues::GetUncertainty(int i) const {
    double val = GetCovarianceValue(i,i);
    if (val>0) val = std::sqrt(val);
    return val;
}

void ND::TCorrValues::SetValue(double v) {
    if (GetDimensions() != 1) {
        ND280Error("Not 1 dimensional");
        throw ECorrValuesRange();
    }
    SetValue(0,v);
}

double ND::TCorrValues::GetValue() const {
    if (GetDimensions() != 1) {
        ND280Error("Not 1 dimensional");
        throw ECorrValuesRange();
    }
    return GetValue(0);
}

void ND::TCorrValues::SetUncertainty(double v) {
    if (GetDimensions() != 1) {
        ND280Error("Not 1 dimensional");
        throw ECorrValuesRange();
    }
    SetCovarianceValue(0,0,v*v);
}

double ND::TCorrValues::GetUncertainty() const {
    if (GetDimensions() != 1) {
        ND280Error("Not 1 dimensional");
        throw ECorrValuesRange();
    }
    return GetUncertainty(0);
}

void ND::TCorrValues::ResizeTo(int n) {
    fVector.ResizeTo(n);
    fMatrix.ResizeTo(n,n);
    fNDOF = 0;
    fTypeHash = 0;
    for (int i=0; i<GetDimensions(); ++i) {
        fMatrix(i,i) = kFreeValue;
    }
    if (fHessian) delete fHessian;
    fHessian = NULL;
}

int ND::TCorrValues::GetDimensions(void) const {
    return fVector.GetNoElements();
}

int ND::TCorrValues::GetNDOF() {
    if (fNDOF < 1) { 
        fNDOF = 0;
        for (int i=0; i<GetDimensions(); ++i) {
            if (IsFixed(i)) continue;
            if (IsFree(i)) continue;
            ++ fNDOF;
        }
    }
    return fNDOF;
}

void ND::TCorrValues::SetType(const char* type) {
    // Use a modified Fowler, Noll, and Vo hash type 1a (FNV-1).  This is the
    // hash function uses the prime and offset suggested in TR1 for strings.
    // Note: The offset value for the normal FNV-1a is chosen so that an empty
    // string doesn't map to zero.  In TR1, the offset is chosen so that an
    // empty string will satisfy (hash_value == 0).
#define USE_TR1_FNV
#ifndef USE_TR1_FNV
    const int prime = 0x83; // The TR1 value
    const int offset = 0;   // The TR1 value
#else
    const int prime = 0x01000193;  // The FNV-1 value;
    const int offset = 0x811c9dc5; // The FNV-1 value;
#endif
    fTypeHash = offset;
    for (const char* c = type; *c != 0; ++c) {
        fTypeHash *= prime;
        fTypeHash ^= (unsigned int) *c;
    }
}

void ND::TCorrValues::SetFixed(int i) {
    if (i<0) {
        ND280Error("Negative element index: " << i);
        throw ECorrValuesRange();
    }
    if (GetDimensions()<=i) {
        ND280Error("Out of bounds element index: " << i 
                   << " (dim is " << GetDimensions() << ")");
        throw ECorrValuesRange();
    }
    for (int j=0; j<GetDimensions(); ++j) {
        fMatrix(j,i) = fMatrix(i,j) = 0.0;
    }
    fMatrix(i,i) = kFixedValue;
}

bool ND::TCorrValues::IsFixed(int i) const {
    if (i<0) {
        ND280Error("Negative element index: " << i);
        throw ECorrValuesRange();
    }
    if (GetDimensions()<=i) {
        ND280Error("Out of bounds element index: " << i 
                   << " (dim is " << GetDimensions() << ")");
        throw ECorrValuesRange();
    }
    if (fMatrix(i,i)<kFixedThreshold) return true;
    return false;
}

void ND::TCorrValues::SetFree(int i) {
    if (i<0) {
        ND280Error("Negative element index: " << i);
        throw ECorrValuesRange();
    }
    if (GetDimensions()<=i) {
        ND280Error("Out of bounds element index: " << i 
                   << " (dim is " << GetDimensions() << ")");
        throw ECorrValuesRange();
    }
    for (int j=0; j<GetDimensions(); ++j) {
        fMatrix(j,i) = fMatrix(i,j) = 0.0;
    }
    fMatrix(i,i) = kFreeValue;
}

bool ND::TCorrValues::IsFree(int i) const {
    if (i<0) {
        ND280Error("Negative element index: " << i);
        throw ECorrValuesRange();
    }
    if (GetDimensions()<=i) {
        ND280Error("Out of bounds element index: " << i 
                   << " (dim is " << GetDimensions() << ")");
        throw ECorrValuesRange();
    }
    return IsFree(fMatrix(i,i));
}

ND::TCorrValues ND::TCorrValues::Sum() const {
    double val = 0;
    double var = 0;

    for (int i=0; i<GetDimensions(); ++i) {
        val += GetValue(i);
        var += GetCovarianceValue(i,i);
        for (int j=i+1; j<GetDimensions(); ++j) {
            var += 2*GetCovarianceValue(i,j);
        }
    }

    return TCorrValues(val,var);
}

void ND::TCorrValues::ls(Option_t *opt) const {
    fVector.ls(opt);
    fMatrix.ls(opt);
}

void ND::TCorrValues::Print(Option_t *opt) const {
    fVector.Print(opt);
    fMatrix.Print(opt);
}
bool ND::TCorrValues::Validate(bool fix) {
    int dim = GetDimensions();
    if (dim<1) return false;
    bool ok = true;
    for (int i=0; i < dim; ++i) {
        if (IsFixed(i)) SetFixed(i);
        if (IsFree(i)) SetFree(i);
    }
    for (int i=0; i < dim; ++i) {
        for (int j=i+1; j < dim; ++j) {
            // Check the off-diagonal elements to make sure they are equal.
            // I'm being overly careful here and checking both elements
            // against the average.  
            Element mag = 0.5*(fMatrix(i,j) + fMatrix(j,i));
            if (std::abs(fMatrix(i,j)-mag)>2*fMatrix.GetTol()*mag) ok = false;
            if (std::abs(fMatrix(j,i)-mag)>2*fMatrix.GetTol()*mag) ok = false;
            if (fix) {
                fMatrix(i,j) = mag;
                fMatrix(j,i) = mag;
            }
        }
    }
    if (!ok && fix) {
        fNDOF = 0;
        if (fHessian) delete fHessian;
        fHessian = 0;
    }
    return ok;
}

const TMatrixTSym<float>& ND::TCorrValues::GetHessian() const {
    if (fHessian) return (*fHessian);
    fHessian = new TMatrixTSym<float>(GetDimensions());

    // Collect the non-zero elements of the covariance.
    std::vector<int> goodList;
    for (int i = 0; i<GetDimensions(); ++i) {
        if (IsFixed(i)) continue;
        if (IsFree(i)) continue;
        goodList.push_back(i);
    }

    if (goodList.size()>0) {
        // Make a symetric matrix of just the good elements and invert.
        TMatrixT<Element> h(goodList.size(),goodList.size());
        for (unsigned int i = 0; i<goodList.size(); ++i) {
            for (unsigned int j = i; j<goodList.size(); ++j) {
                h(j,i) = h(i,j) = fMatrix(goodList[i],goodList[j]);
            }
        }
        h.Invert();
        // Copy the values of the inverted covariance matrix into the Hessian.
        for (unsigned int i = 0; i<goodList.size(); ++i) {
            for (unsigned int j = 0; j<goodList.size(); ++j) {
                (*fHessian)(goodList[i],goodList[j]) = h(i,j);
            }
        }
    }

    // Set the Hessian values for the free and fixed variances.
    for (int i = 0; i<GetDimensions(); ++i) {
        if (IsFree(i)) (*fHessian)(i,i) = 1.0/kFreeValue;
        if (IsFixed(i)) (*fHessian)(i,i) = 1.0/kFixedValue;
    }
    return (*fHessian);
}

const ND::TCorrValues& ND::TCorrValues::operator =(const ND::TCorrValues&
                                                     rhs) {
    int dim = rhs.GetDimensions();
    
    if (dim != GetDimensions()) {
        ResizeTo(dim);
        if (fHessian) delete fHessian;
    }

    for (int i=0; i<dim; ++i) {
        fVector[i] = rhs.fVector[i];
        for (int j=0; j<dim; ++j) {
            fMatrix(i,j) = rhs.fMatrix(i,j);
        }
    }

    fNDOF = rhs.fNDOF;
    fTypeHash = rhs.fTypeHash;
    if (rhs.fHessian) fHessian = new TMatrixTSym<float>(*rhs.fHessian);

    return rhs;
}

ND::TCorrValues operator +(const ND::TCorrValues& a, 
                           const ND::TCorrValues& b) {
    if (a.GetDimensions() != b.GetDimensions()) {
        ND280Error("Dimensions mismatch: " 
                   << a.GetDimensions() << " != " << b.GetDimensions());
        throw ND::ECorrValuesRange();
    }
    ND::TCorrValues result(a);

    int dim = a.GetDimensions();
    for (int i=0; i<dim; ++i) {
        result.SetValue(i,a.GetValue(i) + b.GetValue(i));
        for (int j=0; j<dim; ++j) {
            double val = a.GetCovarianceValue(i,j) + b.GetCovarianceValue(i,j);
            result.SetCovarianceValue(i,j,val);
        }
    }

    return result;
}
    
ND::TCorrValues operator +(double a, const ND::TCorrValues& b) {
    ND::TCorrValues result(b);
    int dim = b.GetDimensions();
    for (int i = 0; i<dim; ++i) result.SetValue(i,a + b.GetValue(i));
    return result;
}

ND::TCorrValues operator +(const ND::TCorrValues& a, double b) {return b + a;}

ND::TCorrValues operator -(const ND::TCorrValues& a, 
                           const ND::TCorrValues& b) {
    if (a.GetDimensions() != b.GetDimensions()) {
        ND280Error("Dimensions mismatch: " 
                   << a.GetDimensions() << " != " << b.GetDimensions());
        throw ND::ECorrValuesRange();
    }
    ND::TCorrValues result(a);

    int dim = a.GetDimensions();
    for (int i=0; i<dim; ++i) {
        result.SetValue(i,a.GetValue(i) - b.GetValue(i));
        for (int j=0; j<dim; ++j) {
            double val = a.GetCovarianceValue(i,j) + b.GetCovarianceValue(i,j);
            result.SetCovarianceValue(i,j,val);
        }
    }

    return result;
}

ND::TCorrValues operator -(double a, const ND::TCorrValues& b) {
    ND::TCorrValues result(b);
    int dim = b.GetDimensions();
    for (int i = 0; i<dim; ++i) result.SetValue(i,a - b.GetValue(i));
    return result;
}

ND::TCorrValues operator -(const ND::TCorrValues& a, double b)  {
    ND::TCorrValues result(a);
    int dim = a.GetDimensions();
    for (int i = 0; i<dim; ++i) result.SetValue(i,a.GetValue(i)-b);
    return result;
}

ND::TCorrValues operator *(double a, const ND::TCorrValues& b) {
    ND::TCorrValues result(b);
    int dim = b.GetDimensions();
    for (int i=0; i<dim; ++i) {
        result.SetValue(i,a*b.GetValue(i));
        for (int j=0; j<dim; ++j) {
            result.SetCovarianceValue(i,j,a*b.GetCovarianceValue(i,j));
        }
    }
    return result;
}

ND::TCorrValues operator *(const ND::TCorrValues& b, double a) {
    return (a*b);
}

ND::TCorrValues operator /(const ND::TCorrValues& b, double a) {
    return ((1/a)*b);
}

ND::TCorrValues operator /(double a, const ND::TCorrValues& x) {
    ND::TCorrValues result(x);
    int dim = x.GetDimensions();
    for (int i=0; i<dim; ++i) {
        result.SetValue(i,a/x.GetValue(i));
        for (int j=0; j<dim; ++j) {
            double xi = x.GetValue(i);
            double xj = x.GetValue(j);
            double val = x.GetCovarianceValue(i,j)/(xi*xi*xj*xj);
            result.SetCovarianceValue(i,j,a*val);
        }
    }
    return result;
}

ND::TCorrValues operator *(const ND::TCorrValues& x, 
                           const ND::TCorrValues& y) {
    if (x.GetDimensions() != y.GetDimensions()) {
        ND280Error("Dimensions mismatch: " 
                   << x.GetDimensions() << " != " << y.GetDimensions());
        throw ND::ECorrValuesRange();
    }
    ND::TCorrValues result(x);

    int dim = y.GetDimensions();
    for (int i=0; i<dim; ++i) {
        result.SetValue(i,x.GetValue(i)*y.GetValue(i));
        for (int j=i; j<dim; ++j) {
            double cov = x.GetCovarianceValue(i,j)*y.GetValue(i)*y.GetValue(j);
            cov += y.GetCovarianceValue(i,j)*x.GetValue(i)*x.GetValue(j);
            if (cov<0) cov = -cov;
            result.SetCovarianceValue(i,j,cov);
            result.SetCovarianceValue(j,i,cov);
        }
    }
    return result;
}

ND::TCorrValues operator /(const ND::TCorrValues& x, 
                           const ND::TCorrValues& y) {
    return x * (1.0/y);
}

