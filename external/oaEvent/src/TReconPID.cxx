#include "TReconPID.hxx"

ClassImp(ND::TReconPID);

ND::TReconPID::TReconPID() {
    fParticleId= kNotSet;
    fParticleWeight = 0;
    fState = new TPIDState;
    fNodes = new TReconNodeContainerImpl<ND::TPIDState>;
}


void ND::TReconPID::CopyTReconPID(const ND::TReconPID& pid){
    fNodes = new TReconNodeContainerImpl<ND::TPIDState>;

    fParticleId = pid.GetParticleId();
    fParticleWeight = pid.GetPIDWeight();

    // Copy the nodes
    // Create new nodes with TPIDState's
    ND::TReconNodeContainer::const_iterator in;
    for (in=pid.GetNodes().begin(); in!=pid.GetNodes().end();in++){
        ND::THandle<ND::TReconNode> node(new ND::TReconNode);
        ND::THandle<ND::TReconBase> object = (*in)->GetObject();
        node->SetObject(object);
        ND::THandle<ND::TPIDState> tstate = (*in)->GetState();
        if (tstate){
            ND::THandle<ND::TReconState> pstate(new ND::TPIDState(*tstate));
            node->SetState(pstate);
        }
        node->SetQuality((*in)->GetQuality());

        fNodes->push_back(node);
    }


    if (pid.GetState()){
        ND::THandle<ND::TPIDState> state = pid.GetState();
        fState = new TPIDState(*state);
    }
    else {
        fState = new TPIDState;
    }
}

ND::TReconPID::TReconPID(const ND::TReconPID& pid, int i)
    : ND::TReconBase(pid) {
    // integer input parameter is dummy. Just to use a diferent constructor
    // which does not copy the alternates
    CopyTReconPID(pid);
}


ND::TReconPID::TReconPID(const ND::TReconPID& pid)
    : ND::TReconBase(pid) {
    CopyTReconPID(pid);
    // copy the alternates
    ND::TReconObjectContainer::const_iterator it;
    for (it = pid.GetAlternates().begin();
         it != pid.GetAlternates().end(); ++it) {
        ND::THandle<ND::TReconPID> alter = *it;
        AddAlternate(alter->GetParticleId(),alter->GetPIDWeight());
    }
}


ND::TReconPID::TReconPID(ND::THandle<ND::TReconTrack> track)
    : ND::TReconBase(*track){

  fParticleId= kNotSet;
  fParticleWeight = 0;
  fNodes = new TReconNodeContainerImpl<ND::TPIDState>;


  // copy the state by converting it from TTrackState to TPIDState
  ND::THandle<ND::TTrackState> state = track->GetState();
  if (state) {
      fState = new TPIDState(*state);
  }
  else {
      fState = new TPIDState;
  }

  // Copy the nodes
  // Create new nodes with TPIDState's
  TReconNodeContainer& nodes = GetNodes();
  GetNodes().clear();

  for (ND::TReconNodeContainer::const_iterator in=track->GetNodes().begin();
       in!=track->GetNodes().end();
       ++in){
      ND::THandle<ND::TTrackState> tstate = (*in)->GetState();
      ND::THandle<ND::TReconBase> object = (*in)->GetObject();
      if (!tstate) {
          ND280Error("Track node without track state");
          continue;
      }
      ND::THandle<ND::TReconNode> node(new ND::TReconNode);
      ND::THandle<ND::TReconState> pstate(new ND::TPIDState(*tstate));
      node->SetObject(object);
      node->SetState(pstate);
      node->SetQuality((*in)->GetQuality());
      nodes.push_back(node);
  }

  // remove constituents, since the input object has been promoted
  // to a TReconPID. This will have a unique constituent: the input object
  TDatum* c = FindDatum("constituents");
  if (c){
    erase(c);
    delete c;
  }


  // add the track as constituent (notice that this is not really a
  // constituent but the promoted object)
  AddConstituent(track);
}


ND::TReconPID::TReconPID(ND::THandle<ND::TReconShower> shower)
  : ND::TReconBase(*shower) {
  fParticleId= kNotSet;
  fParticleWeight = 0;
  fNodes = new TReconNodeContainerImpl<ND::TPIDState>;

  // copy the state by converting it from TShowerState to TPIDState
  ND::THandle<ND::TShowerState> state = shower->GetState();
  if (state) {
    fState = new TPIDState(*state);
  }
  else {
    fState = new TPIDState;
  }

  // Copy the nodes
  // Create new nodes with TPIDState's
  TReconNodeContainer& nodes = GetNodes();
  GetNodes().clear();
  if (shower->GetNodes().size()>0) {
      for (ND::TReconNodeContainer::const_iterator in
               = shower->GetNodes().begin();
           in!=shower->GetNodes().end();
           in++) {
          ND::THandle<ND::TShowerState> tstate = (*in)->GetState();
          ND::THandle<ND::TReconBase> object = (*in)->GetObject();
          if (!tstate) {
              ND280Error("Shower node without shower state");
              continue;
          }
          ND::THandle<ND::TReconNode> node(new ND::TReconNode);
          ND::THandle<ND::TReconState> pstate(new ND::TPIDState(*tstate));
          node->SetObject(object);
          node->SetState(pstate);
          node->SetQuality((*in)->GetQuality());
          nodes.push_back(node);
      }
  }
  else {
      ND::THandle<ND::TShowerState> tstate = shower->GetState();
      ND::THandle<ND::TReconBase> object = shower;
      if (!tstate) {
          ND280Error("Shower node without shower state");

      }
      else {
          ND::THandle<ND::TReconNode> node(new ND::TReconNode);
          ND::THandle<ND::TReconState> pstate(new ND::TPIDState(*tstate));
          node->SetObject(object);
          node->SetState(pstate);
          node->SetQuality(shower->GetQuality());
          nodes.push_back(node);
      }
  }

  // remove constituents, since the input object has been promoted
  // to a TReconPID. This will have a unique constituent: the input object
  TDatum* c = FindDatum("constituents");
  if (c){
    erase(c);
    delete c;
  }


  // add the shower as constituent (notice that this is not really a
  // constituent but the promoted object)
  AddConstituent(shower);

}

ND::TReconPID::~TReconPID() {}

TLorentzVector ND::TReconPID::GetPosition() const {
    // This is the preferred way to access a state field.
    THandle<ND::TPIDState> state = GetState();
    if (!state) throw EMissingField();
    return state->GetPosition();
}

TLorentzVector ND::TReconPID::GetPositionVariance() const {
    // This is the preferred way to access a state field.
    THandle<ND::TPIDState> state = GetState();
    if (!state) throw EMissingField();
    return state->GetPositionVariance();
}

bool ND::TReconPID::IsXPID() const {
    TLorentzVector var = GetPositionVariance();
    if (ND::TCorrValues::IsFree(var.X())) return false;
    return true;
}

bool ND::TReconPID::IsYPID() const {
    TLorentzVector var = GetPositionVariance();
    if (ND::TCorrValues::IsFree(var.Y())) return false;
    return true;
}

bool ND::TReconPID::IsZPID() const {
    TLorentzVector var = GetPositionVariance();
    if (ND::TCorrValues::IsFree(var.Z())) return false;
    return true;
}

int ND::TReconPID::GetDimensions() const{
    TLorentzVector var = GetPositionVariance();
    int dim = 0;
    if (IsXPID()) ++dim;
    if (IsYPID()) ++dim;
    if (IsZPID()) ++dim;
    return dim;
}

TVector3 ND::TReconPID::GetDirection() const {
    // This is the preferred way to access a state field.
    THandle<ND::TPIDState> state = GetState();
    if (!state) throw EMissingField();
    return state->GetDirection();
}

double ND::TReconPID::GetMomentum() const {
    // This is the preferred way to access a state field.
    THandle<ND::TPIDState> state = GetState();
    if (!state) throw EMissingField();
    return state->GetMomentum();
}

double ND::TReconPID::GetCharge() const {
    // This is the preferred way to access a state field.
    THandle<ND::TPIDState> state = GetState();
    if (!state) throw EMissingField();
    return state->GetCharge();
}

void ND::TReconPID::AddAlternate(ParticleId id, double weight) {
  // erase a previous alternate if it has the same id
  ND::TReconObjectContainer::iterator it;
  for (it = fAlternatives.begin(); it != fAlternatives.end(); it++){
    ND::THandle<ND::TReconPID> pid = *it;
    if ( pid->GetParticleId() == id){
      fAlternatives.erase(it);
      break;
    }
  }

  // create a new TReconPID copying the main TReconPID. Don't copy the
  // alternates.  is there any other solution ?
  ND::THandle<ND::TReconPID> alter(new ND::TReconPID(*this,1));

  // set the new information
  alter->SetParticleId(id);
  alter->SetPIDWeight(weight);

  AddAlternate(alter);

}

std::string ND::TReconPID::ConvertParticleId() const {
    return ConvertParticleId(fParticleId);
}

std::string ND::TReconPID::ConvertParticleId(ND::TReconPID::ParticleId id){
    std::string s("");

    if (id == kNotSet)     s= "Not Set";
    if (id == kOther)      s= "Other";
    if (id == kShower)     s= "Shower";
    if (id == kEM)         s= "EM";
    if (id == kElectron)   s= "Electron";
    if (id == kGamma)      s= "Gamma";
    if (id == kHadronic)   s= "Hadronic";
    if (id == kPiZero)     s= "Pi0";
    if (id == kLightTrack) s= "Light track";
    if (id == kMuon)       s= "Muon";
    if (id == kPion)       s= "Pion";
    if (id == kHeavyTrack) s= "Heavy track";
    if (id == kProton)     s= "Proton";
    if (id == kKaon)       s= "Kaon";

    return s;
}

void ND::TReconPID::ls(Option_t *opt) const {
    ND::TReconBase::ls_base(opt);
    TROOT::IncreaseDirLevel();
    TROOT::IndentLevel();
    std::cout << "Particle Id: " << ConvertParticleId()
              << "     Weight: " << GetPIDWeight()
              << std::endl;

    if (fState) {
        TROOT::IncreaseDirLevel();
        fState->ls(opt);
        TROOT::DecreaseDirLevel();
    }

    TROOT::IncreaseDirLevel();
    fAlternatives.ls(opt);
    TROOT::DecreaseDirLevel();

    if (fNodes) {
        TROOT::IncreaseDirLevel();
        fNodes->ls(opt);
        TROOT::DecreaseDirLevel();
    }

    TROOT::IncreaseDirLevel();
    for (const_iterator v = begin(); v != end(); ++v) {
        (*v)->ls(opt);
    };
    TROOT::DecreaseDirLevel();

    TROOT::DecreaseDirLevel();
}

ND::THandle<ND::TPIDState> ND::TReconPID::GetFront() const {
    if (!GetNodes().front()) throw EMissingField();
    return GetNodes().front()->GetState();
}

ND::THandle<ND::TPIDState> ND::TReconPID::GetBack() const {
    if (!GetNodes().back()) throw EMissingField();
    return GetNodes().back()->GetState();
}
