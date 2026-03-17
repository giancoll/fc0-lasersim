#include "TReconShower.hxx"

ClassImp(ND::TReconShower);

ND::TReconShower::TReconShower() {
    fState = new TShowerState;
    fNodes = new TReconNodeContainerImpl<ND::TShowerState>;
}

ND::TReconShower::TReconShower(const ND::TReconShower& shower)
    : ND::TReconBase(shower) {
    
    fNodes = new TReconNodeContainerImpl<ND::TShowerState>;
    
    // Copy the nodes 
    // Create new nodes with TShowerState's 
    ND::TReconNodeContainer::const_iterator in;
    for (in=shower.GetNodes().begin(); in!=shower.GetNodes().end();in++){
        ND::THandle<ND::TReconNode> node(new ND::TReconNode);
        ND::THandle<ND::TReconBase> object = (*in)->GetObject();
        node->SetObject(object);
        ND::THandle<ND::TShowerState> tstate = (*in)->GetState();
        if (tstate){
            ND::THandle<ND::TReconState> pstate(new ND::TShowerState(*tstate));
            node->SetState(pstate);
        }
        node->SetQuality((*in)->GetQuality());
        
        fNodes->push_back(node);
    }
    
    
    if (shower.GetState()) {
        ND::THandle<ND::TShowerState> state = shower.GetState();  
        fState = new TShowerState(*state);
    }
    else {
        fState = new TShowerState;
    }
}

ND::TReconShower::~TReconShower() {}

double ND::TReconShower::GetEDeposit() const {
    THandle<ND::TShowerState> state = GetState();
    if (!state) throw EMissingField();
    return state->GetEDeposit();
}
    
TLorentzVector ND::TReconShower::GetPosition() const {
    // This is the preferred way to access a state field.  
    THandle<ND::TShowerState> state = GetState();
    if (!state) throw EMissingField();
    return state->GetPosition();
}


TLorentzVector ND::TReconShower::GetPositionVariance() const {
    // This is the preferred way to access a state field.  
    THandle<ND::TShowerState> state = GetState();
    if (!state) throw EMissingField();
    return state->GetPositionVariance();
}

bool ND::TReconShower::IsXShower() const {
    TLorentzVector var = GetPositionVariance();
    if (ND::TCorrValues::IsFree(var.X())) return false;
    return true;
}

bool ND::TReconShower::IsYShower() const {
    TLorentzVector var = GetPositionVariance();
    if (ND::TCorrValues::IsFree(var.Y())) return false;
    return true;
}

bool ND::TReconShower::IsZShower() const {
    TLorentzVector var = GetPositionVariance();
    if (ND::TCorrValues::IsFree(var.Z())) return false;
    return true;
}

int ND::TReconShower::GetDimensions() const{
    TLorentzVector var = GetPositionVariance();
    int dim = 0;
    if (IsXShower()) ++dim;
    if (IsYShower()) ++dim;
    if (IsZShower()) ++dim;
    return dim;
}

TVector3 ND::TReconShower::GetDirection() const {
    // This is the preferred way to access a state field.  
    THandle<ND::TShowerState> state = GetState();
    if (!state) throw EMissingField();
    return state->GetDirection();
}

TVector3 ND::TReconShower::GetConeAngle() const {
    THandle<ND::TShowerState> state = GetState();
    if (!state) throw EMissingField();
    return state->GetCone();
}
