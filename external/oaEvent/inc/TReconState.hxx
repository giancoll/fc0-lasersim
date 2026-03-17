#ifndef TReconState_hxx_seen
#define TReconState_hxx_seen
////////////////////////////////////////////////////////////////
//
// A class to hold the state information for the reconstruction objects.  This
// file defines several classes and needs to be split into multiple files
// after the design is complete.
#include <vector>
#include <string>

#include <TObject.h>
#include <TLorentzVector.h>
#include <TVector3.h>

#include "TCorrValues.hxx"
#include "THandle.hxx"
#include "TND280Log.hxx"

namespace ND {
    class TReconState;
    class TClusterState;
    class TShowerState;
    class TTrackState;
    class TPIDState;
    class TVertexState;
};


/// A macro that adds the energy deposit property to a state.
/// The position state is defined as a scaler and the
/// associated uncertainty.
///
/// \bug The units of the energy deposit depend on the context.  After
/// particle ID, the deposit can be in uncalibrated or calibrated energy.  In
/// the TPC the deposit might be in terms of fempto-coulombs, but for a PMT
/// the deposit might be based on photo-electrons.
#define ENERGY_DEPOSIT_STATE_DECLARATION                                \
    public:                                                             \
    /** Get the value of the EDeposit. */                               \
    double GetEDeposit() const {return GetValue(fEDepositIndex);}       \
    /** Set the value of the EDeposit. */                               \
    void SetEDeposit(double enr) {SetValue(fEDepositIndex,enr);}        \
    /** Get the variance of the EDeposit.*/                             \
    double GetEDepositVariance(void) const                              \
    {return GetCovarianceValue(fEDepositIndex,fEDepositIndex);}         \
    /** Set the variance of the EDeposit.  */                           \
    void SetEDepositVariance(double var)                                \
    {SetCovarianceValue(fEDepositIndex,fEDepositIndex,var);}

/// This should be included in the class constructor.
#define ENERGY_DEPOSIT_STATE_DEFINITION                                 \
    fEDepositIndex=fFieldNames.size();                                  \
    fFieldNames.push_back("EDeposit")

#define PROJECT_STATE_DECLARATION(MethodName,CLASS) \
    static ND::TCorrValues MethodName(const ND::THandle<CLASS>& state);

#define PROJECT_STATE_DEFINITION(CLASS,MethodName,GetAttributeIndex,AttributeStateSize,Types) \
ND::TCorrValues ND::CLASS::MethodName(const ND::THandle<ND::CLASS>& proj) {\
    int Size=ND::TReconState::AttributeStateSize;\
    TCorrValues values(Size);\
    values.SetType("Types ");\
    int base = 0;\
    const int offset = proj->GetAttributeIndex();\
    for (int i = 0; i < Size; ++i) {\
        values.SetValue(i+base,proj->GetValue(i+offset));\
        for (int j = 0; j < Size; ++j) {\
            values.SetCovarianceValue(\
                i+base, j+base,\
                proj->GetCovarianceValue(i+offset,j+offset));\
        }\
    }\
    return values;\
}

/// A macro that adds the position property to a state.  The position state is
/// defined as a four vector (X,Y,Z,T) and the associated uncertainties.
#define POSITION_STATE_DECLARATION                                      \
    public:                                                             \
    /** Get the index of the X,Y,Z,T fields in the TCorrValues vector.*/ \
    int GetXIndex() const {return fPositionIndex;}                      \
    int GetYIndex() const {return fPositionIndex+1;}                    \
    int GetZIndex() const {return fPositionIndex+2;}                    \
    int GetTIndex() const {return fPositionIndex+3;}                    \
    /** Get the value of the Position. */                               \
    TLorentzVector GetPosition() const {                                \
        return TLorentzVector(GetValue(fPositionIndex+0),               \
                              GetValue(fPositionIndex+1),               \
                              GetValue(fPositionIndex+2),               \
                              GetValue(fPositionIndex+3));              \
    }                                                                   \
    /** Set the value of the Position.*/                                \
    void SetPosition(double x, double y, double z, double t) {          \
        SetValue(fPositionIndex+0,x);                                   \
        SetValue(fPositionIndex+1,y);                                   \
        SetValue(fPositionIndex+2,z);                                   \
        SetValue(fPositionIndex+3,t);                                   \
    }                                                                   \
    /** Set the value of the Position.*/                                \
    void SetPosition(const TLorentzVector& pos) {                       \
        SetPosition(pos.X(),pos.Y(),pos.Z(),pos.T());                   \
    }                                                                   \
    /** Set the covariance of the position variables.  The indices */   \
    /* run from 0 to 3 (0: X, 1: Y, 2: Z, 3: T).*/                      \
    void SetPositionCovariance(int i, int j, double v) {                \
        SetCovarianceValue(fPositionIndex+i,fPositionIndex+j,v);        \
    }                                                                   \
    /** Get the covariance of the position variables.  The indices */   \
    /* run from 0 to 3 (0: X, 1: Y, 2: Z, 3: T).*/                      \
    double GetPositionCovariance(int i, int j) const {                  \
        return GetCovarianceValue(fPositionIndex+i,fPositionIndex+j);   \
    }                                                                   \
    /** Get the variance of the Position.*/                             \
    TLorentzVector GetPositionVariance(void) const {                    \
        return TLorentzVector(                                          \
            GetPositionCovariance(0,0),                                 \
            GetPositionCovariance(1,1),                                 \
            GetPositionCovariance(2,2),                                 \
            GetPositionCovariance(3,3));                                \
    }                                                                   \
    /** Set the variance of the Position.*/                             \
    void SetPositionVariance(double x, double y, double z, double t) {  \
        SetPositionCovariance(0,0,x);                                   \
        SetPositionCovariance(1,1,y);                                   \
        SetPositionCovariance(2,2,z);                                   \
        SetPositionCovariance(3,3,t);                                   \
    }

/// This should be included in the class constructor.
#define POSITION_STATE_DEFINITION                                       \
    fPositionIndex=fFieldNames.size();                                  \
    fFieldNames.push_back("X");                                         \
    fFieldNames.push_back("Y");                                         \
    fFieldNames.push_back("Z");                                         \
    fFieldNames.push_back("T")

#define PROJECT_POSITION_DEFINITION(CLASS)          \
ND::TCorrValues ND::CLASS::ProjectPosition(ND::THandle<TVertexState> state)    \
{\
    int PositionStateSize=ND::TReconState::kPositionStateSize;\
    TCorrValues values(PositionStateSize);\
    values.SetType("X Y Z T ");    \
    int base = 0;\
    const int offset = state->GetPositionIndex();\
    for (int i = 0; i < PositionStateSize; ++i) {\
        values.SetValue(i+base, \
            state->GetValue(i+offset));\
        for (int j = 0; j < PositionStateSize; ++j) {\
            values.SetCovarianceValue(\
                i+base, j+base,\
                state->GetCovarianceValue(i+offset,\
                 j+offset));\
        }\
    }\
    return values;\
}

/// A macro that adds the direction property to a state.  The direction state
/// is defined as a three vector (DX,DY,DZ) and the associated uncertainties.
/// The corelations between the components are enforced in the covariance
/// matrix.
#define DIRECTION_STATE_DECLARATION                                     \
    public:                                                             \
    /** Get the value of the direction. */                              \
    TVector3 GetDirection() const {                                     \
        return TVector3(GetValue(fDirectionIndex+0),                    \
                        GetValue(fDirectionIndex+1),                    \
                        GetValue(fDirectionIndex+2));                   \
    }                                                                   \
    /** Set the value of the direction.*/                               \
    void SetDirection(double x, double y, double z) {                   \
        SetValue(fDirectionIndex+0,x);                                  \
        SetValue(fDirectionIndex+1,y);                                  \
        SetValue(fDirectionIndex+2,z);                                  \
    }                                                                   \
    /** Set the value of the direction.*/                               \
    void SetDirection(const TVector3& dir) {                            \
        SetDirection(dir.X(),dir.Y(),dir.Z());                          \
    }                                                                   \
    /** Set the covariance of the direction variables.  The indices */  \
    /* run from 0 to 2 (0: DX, 1: DY, 2: DZ).*/                         \
    void SetDirectionCovariance(int i, int j, double v) {               \
        SetCovarianceValue(fDirectionIndex+i,fDirectionIndex+j,v);      \
    }                                                                   \
    /** Get the covariance of the direction variables.  The indices */  \
    /* run from 0 to 2 (0: DX, 1: DY, 2: DZ).*/                         \
    double GetDirectionCovariance(int i, int j) const {                 \
        return GetCovarianceValue(fDirectionIndex+i,fDirectionIndex+j); \
    }                                                                   \
    /** Get the variance of the Direction.*/                            \
    TVector3 GetDirectionVariance(void) const {                         \
        return TVector3(                                                \
            GetDirectionCovariance(0,0),                                \
            GetDirectionCovariance(1,1),                                \
            GetDirectionCovariance(2,2));                               \
    }                                                                   \
    /** Set the variance of the Direction.*/                            \
    void SetDirectionVariance(double x, double y, double z) {           \
        SetDirectionCovariance(0,0,x);                                  \
        SetDirectionCovariance(1,1,y);                                  \
        SetDirectionCovariance(2,2,z);                                  \
    }

/// This should be included in the class constructor.
#define DIRECTION_STATE_DEFINITION                                      \
    fDirectionIndex=fFieldNames.size();                                 \
    fFieldNames.push_back("DX");                                        \
    fFieldNames.push_back("DY");                                        \
    fFieldNames.push_back("DZ")

/// A macro that adds the width of a TReconShower object.  The cone value
/// depends on the type of the shower fit.  For a EM fit, the cone value will
/// represent the local width of the shower.
#define CONE_STATE_DECLARATION                                          \
    public:                                                             \
    /** Get the value of the cone.   */                                 \
    TVector3 GetCone() const { return TVector3( GetValue(fConeIndex),   \
                                                GetValue(fConeIndex+1), \
                                                0); }                   \
    /** Set the value of the cone. */                                   \
    void SetCone(double x, double y) {SetValue(fConeIndex,x);           \
        SetValue(fConeIndex+1,y);}                                      \
    /** Get the variance of the cone.     */                            \
    TVector3 GetConeVariance(void) const                                \
    {return TVector3(GetCovarianceValue(fConeIndex,fConeIndex),         \
                     GetCovarianceValue(fConeIndex+1,fConeIndex+1),0) ;} \
    /** Set the variance of the cone.  */                               \
    void SetConeVariance(double x , double y)                           \
    {SetCovarianceValue(fConeIndex,fConeIndex,x);                       \
        SetCovarianceValue(fConeIndex+1,fConeIndex+1,y);}


/// This should be included in the class constructor.
#define CONE_STATE_DEFINITION                                        \
    fConeIndex=fFieldNames.size();                                   \
    fFieldNames.push_back("C1");                                     \
    fFieldNames.push_back("C2")


/// A macro that adds the width of a curvilinear energy deposit property to a
/// state.  The width is the extent of an energy deposition perpendicular to
/// local direction and the associated uncertainties.
#define WIDTH_STATE_DECLARATION                                         \
    public:                                                             \
    /** Get the value of the width. */                                  \
    TVector3 GetWidth() const {                                         \
        return TVector3(GetValue(fWidthIndex),GetValue(fWidthIndex+1),0);}; \
    /** Set the value of the width. */                                  \
    void SetWidth(double w1,double w2) {                                \
        SetValue(fWidthIndex,w1);                                       \
        SetValue(fWidthIndex+1,w2);};                                   \
    /** Get the variance of the width. */                               \
    TVector3 GetWidthVariance(void) const                               \
    {return TVector3(GetCovarianceValue(fWidthIndex,fWidthIndex),       \
                     GetCovarianceValue(fWidthIndex+1,fWidthIndex+1),0);} \
    /** Set the variance of the width. */                               \
    void SetWidthVariance(double w1, double w2)                         \
    {SetCovarianceValue(fWidthIndex,fWidthIndex,w1);                    \
        SetCovarianceValue(fWidthIndex+1,fWidthIndex+1,w2);};

/// This should be included in the class constructor.
#define WIDTH_STATE_DEFINITION                                       \
    fWidthIndex=fFieldNames.size();                                  \
    fFieldNames.push_back("W1");                                     \
    fFieldNames.push_back("W2")

/// A macro that adds a property for the magnitude of the momentum to a state.
#define MOMENTUM_STATE_DECLARATION                                      \
    public:                                                             \
    /** Get the value of the momentum. */                               \
    double GetMomentum() const {return GetValue(fMomentumIndex);}       \
    /** Set the value of the momentum. */                               \
    void SetMomentum(double momentum) {SetValue(fMomentumIndex,momentum);} \
    /** Get the variance of the momentum. */                            \
    double GetMomentumVariance(void) const                              \
    {return GetCovarianceValue(fMomentumIndex,fMomentumIndex);}         \
    /** Set the variance of the momentum. */                            \
    void SetMomentumVariance(double momentum)                           \
    {SetCovarianceValue(fMomentumIndex,fMomentumIndex,momentum);}

/// This should be included in the class constructor.
#define MOMENTUM_STATE_DEFINITION                                       \
    fMomentumIndex=fFieldNames.size();                                  \
    fFieldNames.push_back("Momentum")

/// A macro that adds a property for the magnitude of the particle charge to a
/// state.
#define CHARGE_STATE_DECLARATION                                        \
    public:                                                             \
    /** Get the value of the charge. */                                 \
    double GetCharge() const {return GetValue(fChargeIndex);}           \
    /** Set the value of the charge. */                                 \
    void SetCharge(double charge) {SetValue(fChargeIndex,charge);}      \
    /** Get the variance of the charge. */                              \
    double GetChargeVariance(void) const                                \
    {return GetCovarianceValue(fChargeIndex,fChargeIndex);}             \
    /** Set the variance of the charge. */                              \
    void SetChargeVariance(double charge)                               \
    {SetCovarianceValue(fChargeIndex,fChargeIndex,charge);}

/// This should be included in the class constructor.
#define CHARGE_STATE_DEFINITION                                       \
    fChargeIndex=fFieldNames.size();                                  \
    fFieldNames.push_back("Charge")

/// A macro that adds a property for the curvature of a track to a
/// state.
#define CURVATURE_STATE_DECLARATION                                     \
    public:                                                             \
    /** Get the value of the Curvature.  */                             \
    double GetCurvature() const {return GetValue(fCurvatureIndex);}     \
    /** Set the value of the Curvature.  */                             \
    void SetCurvature(double curvature){SetValue(fCurvatureIndex,curvature);} \
    /** Get the variance of the Curvature. */                           \
    double GetCurvatureVariance(void) const                             \
    {return GetCovarianceValue(fCurvatureIndex,fCurvatureIndex);}       \
    /** Set the variance of the Curvature.        */                    \
    void SetCurvatureVariance(double var)                               \
    {SetCovarianceValue(fCurvatureIndex,fCurvatureIndex,var);}

/// This should be included in the class constructor.
#define CURVATURE_STATE_DEFINITION                                      \
    fCurvatureIndex=fFieldNames.size();                                 \
    fFieldNames.push_back("Curvature")

/// The TReconState class represents the value of parameters and covariances
/// associated with reconstruction objects.  It does not contain general
/// status information such as the goodness, degree's of freedom, or objects
/// which contributed to the reconstruction.  The values saved in the particle
/// states are described in \ref recObjGeometric and \ref recObjPhysics.
///
/// This is the polymorphic base class that all other state classes are
/// derived from, and it provides minimal operations.  The main purpose of
/// this class is to allow polymorphic vectors of states.  However, it
/// provides some minimal access to the contained data.
class ND::TReconState: public TObject {
public:
    TReconState();
    TReconState(const ND::TReconState& state);
    virtual ~TReconState();

    /// Return a string with all of the state field names.  This name is used
    /// to build a type has for the state.
    std::string GetStateFields() const;

    /// Return the number of dimensions in the state (the size of the state).
    int GetDimensions() const;

    /// Not the preferred interface, but get the value by index.
    double GetValue(int i) const;

    /// Not the preferred interface, but set the value by index.
    void SetValue(int i, double value);

    /// Not the preferred interface: get the covariance by index.
    double GetCovarianceValue(int i, int j) const;

    /// Not the preferred interface: set the covariance by index.
    void SetCovarianceValue(int i, int j, double value);

    /// Check whether any state values are invalid. If they are print a warning.

    bool check();

#ifdef TRECONSTATE_INCLUDE_BROKEN_PROJECTSTATE_METHOD
    /// The projection operator to get the full state.
    static ND::TCorrValues ProjectState(const ND::THandle<TReconState>& state);
#endif

    /// Set a parameter to be free (unconstrained).
    void SetFree(int i);

    /// Check if a parameter is free.
    bool IsFree(int i) const;

    /// Check if a variance corresponds to a free parameter.
    bool IsFree(double v) const;

    /// Set a parameter to be fixed.
    void SetFixed(int i);

    /// Check if a parameter is fixed.
    bool IsFixed(int i) const;

    /// Check if a variance corresponds to a fixed parameter.
    bool IsFixed(double v) const;

    /// Validate the covariance.
    void Validate();

    /// Print the object information.
    virtual void ls(Option_t *opt = "") const;
    virtual void Print(Option_t *opt = "") const {ls(opt);}

    // As we no longer have mixin classes, we need a way to know what data has
    // been stored. This is not a clean piece of code as the base class has to
    // know about what inherits from it but this is to avoid major rewrites in
    // other parts of nd280 code which relied on virtual inheritance.  Relies
    // on the TReconState constructor setting all indices to a default value
    // of -1, and subsequent constructors setting them to their true value.

    /// State has energy deposit information
    bool HasEDeposit() const   {return ( fEDepositIndex != -1 );}
    /// State has position information.
    bool HasPosition()  const { return true;}
    /// State has direction information
    bool HasDirection() const  { return ( fDirectionIndex != -1 );}
    /// State has cone information
    bool HasCone()  const { return (fConeIndex != -1) ;}
    /// State has width information
    bool HasWidth()  const { return (fWidthIndex != -1) ;}
    /// State has momentum information
    bool HasMomentum() const  { return (fMomentumIndex != -1) ;}
    /// State has charge information
    bool HasCharge()  const  { return (fChargeIndex != -1) ;}
    /// State has curvatute information
    bool HasCurvature()  const  { return (fCurvatureIndex != -1) ;}
    /// State has a field of a given name
    bool HasFieldName(std::string name)  const ;


    /// Get the list of field names
    std::vector<std::string> GetFieldNames() const {return fFieldNames;}

    /// All of the state sizes.  These are combined by the different derived
    /// classes to get the full state dimensionality.
    enum {
        kPositionStateSize=4,
        kEDepositStateSize=1,
        kDirectionStateSize=3,
        kConeStateSize=2,
        kCurvatureStateSize=1,
        kWidthStateSize=2,
        kMomentumStateSize=1,
        kChargeStateSize=1
    };

    /// Get the index of the Position field.
    int GetPositionIndex() const {return fPositionIndex;}

    ///  Get the index of the EDeposit field in the TCorrValues vector.
    int GetEDepositIndex() const {return fEDepositIndex;}

    /// Get the index of the direction field.
    int GetDirectionIndex() const {return fDirectionIndex;}

    /// Get the index of the cone field in the TCorrValues vector.
    int GetConeIndex() const {return fConeIndex;}

    /// Get the index of the width field in the TCorrValues vector.
    int GetWidthIndex() const {return fWidthIndex;}

    /// Get the index of the momentum field in the TCorrValues vector.
    int GetMomentumIndex() const {return fMomentumIndex;}

    /// Get the index of the charge field in the TCorrValues vector.
    int GetChargeIndex() const {return fChargeIndex;}

    /// Get the index of the Curvature field in the TCorrValues vector.
    int GetCurvatureIndex() const {return fCurvatureIndex;}   ;

protected:
    /// A final initialization routine that is called in the constructor of
    /// the instantiated class.  This builds the actual state vector.
    void Init();

    /// The vector of correlated values (a vector and a covariance) that holds
    /// the state information.
    TCorrValues fValues;

    /// A vector of parameter names.  This identifies the fields in the state.
    std::vector<std::string> fFieldNames;

    // Store the indices here for ease of access even though they will be
    // unused in many cases. They are initialised to -1 to indicate that they
    // have not been set.
    int fPositionIndex;         //! Index of position in fValues
    int fEDepositIndex;         //! Index of energy deposit in fValues
    int fDirectionIndex;        //! Index of direction in fValues
    int fConeIndex;             //! Index of cone in fValues
    int fWidthIndex;            //! Index of width in fValues
    int fMomentumIndex;         //! Index of momentum in fValues
    int fChargeIndex;           //! Index of charge in fValues
    int fCurvatureIndex;        //! Index of curvature in fValues

    ClassDef(TReconState,2);
};
#endif
