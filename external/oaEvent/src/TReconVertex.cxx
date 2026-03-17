#include "TReconVertex.hxx"

ClassImp(ND::TReconVertex);

ND::TReconVertex::TReconVertex() {
    fState = new TVertexState;
    fNodes = new TReconNodeContainerImpl<ND::TVertexState>;
}

ND::TReconVertex::~TReconVertex() {}

TLorentzVector ND::TReconVertex::GetPosition() const {
    // This is the preferred way to access a state field.  
    THandle<ND::TVertexState> state = GetState();
    if (!state) throw EMissingField();
    return state->GetPosition();
}

TLorentzVector ND::TReconVertex::GetPositionVariance() const {
    // This is the preferred way to access a state field.  
    THandle<ND::TVertexState> state = GetState();
    if (!state) throw EMissingField();
    return state->GetPositionVariance();
}

bool ND::TReconVertex::IsXVertex() const {
    TLorentzVector var = GetPositionVariance();
    if (ND::TCorrValues::IsFree(var.X())) return false;
    return true;
}

bool ND::TReconVertex::IsYVertex() const {
    TLorentzVector var = GetPositionVariance();
    if (ND::TCorrValues::IsFree(var.Y())) return false;
    return true;
}

bool ND::TReconVertex::IsZVertex() const {
    TLorentzVector var = GetPositionVariance();
    if (ND::TCorrValues::IsFree(var.Z())) return false;
    return true;
}

int ND::TReconVertex::GetDimensions() const{
    TLorentzVector var = GetPositionVariance();
    int dim = 0;
    if (IsXVertex()) ++dim;
    if (IsYVertex()) ++dim;
    if (IsZVertex()) ++dim;
    return dim;
}

