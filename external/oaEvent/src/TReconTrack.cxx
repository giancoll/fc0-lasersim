#include "TReconTrack.hxx"
#include "TCorrValues.hxx"

ClassImp(ND::TReconTrack);

ND::TReconTrack::TReconTrack() {
    fState = new TTrackState;
    fNodes = new TReconNodeContainerImpl<ND::TTrackState>;
}

ND::TReconTrack::TReconTrack(const ND::TReconTrack& track)
    : ND::TReconBase(track) {
    fNodes = new TReconNodeContainerImpl<ND::TTrackState>;

    // Copy the nodes
    // Create new nodes with TTrackState's
    ND::TReconNodeContainer::const_iterator in;
    for (in=track.GetNodes().begin(); in!=track.GetNodes().end(); ++in){
        ND::THandle<ND::TReconNode> node(new ND::TReconNode);
        ND::THandle<ND::TReconBase> object = (*in)->GetObject();
        node->SetObject(object);
        ND::THandle<ND::TTrackState> tstate = (*in)->GetState();
        if (tstate) {
            ND::THandle<ND::TReconState> pstate(new ND::TTrackState(*tstate));
            node->SetState(pstate);
        }
        node->SetQuality((*in)->GetQuality());

        fNodes->push_back(node);
    }

    if (track.GetState()) {
        ND::THandle<ND::TTrackState> state = track.GetState();
        fState = new TTrackState(*state);
    }
    else {
        fState = new TTrackState;
    }
}

ND::TReconTrack::~TReconTrack() {}

double ND::TReconTrack::GetEDeposit() const {
    THandle<ND::TTrackState> state = GetState();
    if (!state) throw EMissingField();
    return state->GetEDeposit();
}

TLorentzVector ND::TReconTrack::GetPosition() const {
    // This is the preferred way to access a state field.
    THandle<ND::TTrackState> state = GetState();
    if (!state) throw EMissingField();
    return state->GetPosition();
}

TLorentzVector ND::TReconTrack::GetPositionVariance() const {
    // This is the preferred way to access a state field.
    THandle<ND::TTrackState> state = GetState();
    if (!state) throw EMissingField();
    return state->GetPositionVariance();
}

bool ND::TReconTrack::IsXTrack() const {
    TLorentzVector var = GetPositionVariance();
    if (ND::TCorrValues::IsFree(var.X())) return false;
    return true;
}

bool ND::TReconTrack::IsYTrack() const {
    TLorentzVector var = GetPositionVariance();
    if (ND::TCorrValues::IsFree(var.Y())) return false;
    return true;
}

bool ND::TReconTrack::IsZTrack() const {
    TLorentzVector var = GetPositionVariance();
    if (ND::TCorrValues::IsFree(var.Z())) return false;
    return true;
}

int ND::TReconTrack::GetDimensions() const{
    TLorentzVector var = GetPositionVariance();
    int dim = 0;
    if (IsXTrack()) ++dim;
    if (IsYTrack()) ++dim;
    if (IsZTrack()) ++dim;
    return dim;
}

TVector3 ND::TReconTrack::GetDirection() const {
    // This is the preferred way to access a state field.
    THandle<ND::TTrackState> state = GetState();
    if (!state) throw EMissingField();
    return state->GetDirection();
}

double ND::TReconTrack::GetCurvature() const {
    // This is the preferred way to access a state field.
    THandle<ND::TTrackState> state = GetState();
    if (!state) throw EMissingField();
    return state->GetCurvature();
}

TVector3 ND::TReconTrack::GetWidth() const {
    THandle<ND::TTrackState> state = GetState();
    if (!state) throw EMissingField();
    return state->GetWidth();
}

ND::THandle<ND::TTrackState> ND::TReconTrack::GetFront() const {
    if (!GetNodes().front()) throw EMissingField();
    return GetNodes().front()->GetState();
}

ND::THandle<ND::TTrackState> ND::TReconTrack::GetBack() const {
    if (!GetNodes().back()) throw EMissingField();
    return GetNodes().back()->GetState();
}
