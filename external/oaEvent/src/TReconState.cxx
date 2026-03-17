#include <iostream>
#include <iomanip>
#include <cmath>

#include <TROOT.h>

#include "TND280Log.hxx"
#include "TReconState.hxx"

#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

ClassImp(ND::TReconState);

ND::TReconState::TReconState() {
    fPositionIndex=-1;
    fEDepositIndex=-1;
    fDirectionIndex=-1;
    fConeIndex=-1;
    fWidthIndex=-1;
    fMomentumIndex=-1;
    fChargeIndex=-1;
    fCurvatureIndex=-1;
}

ND::TReconState::TReconState(const TReconState& state)
    : TObject(state), fValues(state.fValues),
      fFieldNames(state.fFieldNames)  ,
      fPositionIndex(state.fPositionIndex),
      fEDepositIndex(state.fEDepositIndex),
      fDirectionIndex(state.fDirectionIndex),
      fConeIndex(state.fConeIndex),
      fWidthIndex(state.fWidthIndex),
      fMomentumIndex(state.fMomentumIndex),
      fChargeIndex(state.fChargeIndex),
      fCurvatureIndex(state.fCurvatureIndex)
       { }

ND::TReconState::~TReconState() { }

std::string ND::TReconState::GetStateFields(void) const {
    // Construct a type name out of the field names.  This in turn is used by
    // the TCorrValues class to construct a type hash which is used to make
    // sure that operations are done on compatible TCorrValues objects.
    std::string typeName;
    for (std::vector<std::string>::const_iterator n = fFieldNames.begin();
         n != fFieldNames.end();
         ++n) {
        typeName += *n;
        typeName += " ";
    };
    return typeName;
}

// Build the internal state vector.
void ND::TReconState::Init() {
    fValues.ResizeTo(fFieldNames.size());
    fValues.SetType(GetStateFields().c_str());

    // Added by Alex Finch 1st July 2020
    // Set  unset values to NAN as this is an error and should be caught
    for(int i=0;i<fValues.GetDimensions();i++)
    //   fValues.SetValue(i,NAN);
    // 18/9/2020 This experiment resulted in too many
    // differences in the reconstruction results so it has been
    // ended. Anyone who wants to try again is welcome to.
        fValues.SetValue(i,0.0);

}

bool ND::TReconState::check() {
    if (ND::TND280Log::GetDebugLevel()>=ND::TND280Log::SevereLevel) {
        for (int i = 0; i<fValues.GetDimensions(); ++i) {
            if (std::isnan(fValues.GetValue(i))) {
                ND280Severe(" State value "<<i<<" ( "
                            <<fFieldNames[i]<<" ) is not a number.");
            }
        }
    }
    return true;
}

int ND::TReconState::GetDimensions() const {
    return fValues.GetDimensions();
}

double ND::TReconState::GetValue(int i) const {
    return fValues.GetValue(i);
}

void ND::TReconState::SetValue(int i, double val) {
    fValues.SetValue(i, val);
}

double ND::TReconState::GetCovarianceValue(int i, int j) const {
    return fValues.GetCovarianceValue(i,j);
}

void ND::TReconState::SetCovarianceValue(int i, int j, double val) {
    fValues.SetCovarianceValue(i,j,val);
}

void ND::TReconState::SetFree(int i) {
    fValues.SetFree(i);
}

bool ND::TReconState::IsFree(int i) const {
    return fValues.IsFree(i);
}

bool ND::TReconState::IsFree(double v) const {
    return fValues.IsFree(v);
}

void ND::TReconState::SetFixed(int i) {
    fValues.SetFixed(i);
}

bool ND::TReconState::IsFixed(int i) const {
    return fValues.IsFixed(i);
}

bool ND::TReconState::IsFixed(double v) const {
    return fValues.IsFixed(v);
}

void ND::TReconState::Validate() {
    fValues.Validate(true);
}

#ifdef TRECONSTATE_INCLUDE_BROKEN_PROJECTSTATE_METHOD
ND::TCorrValues ND::TReconState::ProjectState(
    const ND::THandle<ND::TReconState>& state) {
    return state->fValues;
}
#endif

bool ND::TReconState::HasFieldName(std::string name) const{
    std::string typeName;
    for (std::vector<std::string>::const_iterator n = fFieldNames.begin();
         n != fFieldNames.end();
         ++n) {
        if (n->compare(name)==0)
            return true;
    }
    return false;
}

/// Print the object information.
void ND::TReconState::ls(Option_t*) const {
    TROOT::IndentLevel();
    std::cout << ClassName() << "(" << this << ")::" << std::endl;
    TROOT::IncreaseDirLevel();
    TROOT::IndentLevel();
    std::cout << GetStateFields() << std::endl;
    TROOT::IncreaseDirLevel();
    std::ios::fmtflags save = std::cout.flags();
    for (int i = 0; i<GetDimensions(); ++i) {
        if (IsFree(i)) continue;
        TROOT::IndentLevel();
        std::cout << "  " << std::setw(6) << fFieldNames[i];
        std::cout << ":: "
                  << std::setw(9) << std::setprecision(3)
                  << GetValue(i);
        if (IsFixed(i)) {
            std::cout << "    fixed";
        }
        else {
            std::cout << " +- "
                      << std::setw(5) << std::setprecision(3)
                      << std::sqrt(GetCovarianceValue(i,i));
        }
        std::cout << std::endl;
    }
    std::cout.flags(save);
    TROOT::DecreaseDirLevel();
    TROOT::DecreaseDirLevel();
}
