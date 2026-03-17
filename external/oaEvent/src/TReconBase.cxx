#include <iostream>
#include <iomanip>

#include <TBrowser.h>

#include "TReconBase.hxx"
#include "TReconNode.hxx"
#include "TReconState.hxx"
#include "TRealDatum.hxx"
#include "TIntegerDatum.hxx"

ClassImp(ND::TReconBase);

namespace {
    // This keeps track of the previous TReconBase unique identifier.
    static UInt_t gReconBaseId = 0;
}

ND::TReconBase::TReconBase() 
    : TDataVector("", "Reconstruction Object"),
      fQuality(0), fState(NULL), fNodes(NULL), fStatus(0), fNDOF(0) {
    SetUniqueID(++gReconBaseId);
    SetComposite(false);
}

ND::TReconBase::TReconBase(const char* name, const char* title)
    : TDataVector(name,title),
      fQuality(0), fState(NULL), fNodes(NULL), fStatus(0), fNDOF(0) {
    SetUniqueID(++gReconBaseId);
    SetComposite(false);
}
      
ND::TReconBase::TReconBase(const ND::TReconBase& object)
    : TDataVector(object.GetName(), object.GetTitle()),
      fState(NULL), fNodes(NULL) {
    SetUniqueID(++gReconBaseId);
    
    fQuality = object.GetQuality();
    fStatus = object.GetStatus();
    fNDOF = object.GetNDOF();  
    
    if (object.GetHits()) {
        ND::THitSelection* hits = new THitSelection();
        std::copy(object.GetHits()->begin(), object.GetHits()->end(), 
                  std::back_inserter(*hits));
        AddHits(hits);
    }
    
    if (object.GetConstituents()) {
        AddConstituents(object.GetConstituents());
    }

    // Explicitly look for TRealDatum and TIntegerDatum objects and copy them.
    for (ND::TReconBase::const_iterator obj = object.begin(); 
         obj != object.end(); ++obj) {
        ND::TDatum* datum = dynamic_cast<ND::TDatum*>(*obj);
        // Copy any TRealDatum objects.
        ND::TRealDatum *rDatum = dynamic_cast<ND::TRealDatum*>(datum);
        if (rDatum) {
            ND::TRealDatum* nDatum = new TRealDatum(*rDatum);
            AddDatum(nDatum);
            continue;
        }

        // Copy any TIntegerDatum objects.
        ND::TIntegerDatum *iDatum = dynamic_cast<ND::TIntegerDatum*>(datum);
        if (iDatum) {
            ND::TIntegerDatum* nDatum = new TIntegerDatum(*iDatum);
            AddDatum(nDatum);
            continue;
        }
    }

    // Don't copy the nodes and the state since they depend of the object type 
}

ND::TReconBase::~TReconBase() {
    if (fNodes) delete fNodes;
    if (fState) delete fState;
}

ND::TReconBase::Status ND::TReconBase::GetStatus() const {
    // Notice that this returns both the status bits and the deetector bits.
    return fStatus;
}

bool ND::TReconBase::IsComposite() const {
    return TestBit(kCompositeObject);
}

/// Mark this as a composite object.
void ND::TReconBase::SetComposite(bool comp) {
    SetBit(kCompositeObject,comp);
}

void ND::TReconBase::SetStatus(ND::TReconBase::Status status) {
    fStatus |= (status & kStatusMask);
}

void ND::TReconBase::ClearStatus(ND::TReconBase::Status status) {
    fStatus &= ~ (status & kStatusMask);
}

bool ND::TReconBase::CheckStatus(ND::TReconBase::Status status) const {
    return 0 != (fStatus & (status & kStatusMask));
}

ND::TReconBase::Status ND::TReconBase::GetDetectors() const {
    return (fStatus & kDetectorMask);
}

void ND::TReconBase::AddDetector(ND::TReconBase::Status status) {
    fStatus |= (status & kDetectorMask);
}

bool ND::TReconBase::UsesDetector(ND::TReconBase::Status status) const{
    return 0 != (fStatus & (status & kDetectorMask));
}

void ND::TReconBase::RemoveDetector(ND::TReconBase::Status status) {
    fStatus &= ~(status & kDetectorMask);
}

ND::THandle<ND::THitSelection> ND::TReconBase::GetHits() const {
    return Get<ND::THitSelection>("hits");
}

void ND::TReconBase::AddHits(ND::THitSelection* hits) {
    TDatum* h = FindDatum("hits");
    while (h) {
        erase(h);
        delete h;
        h = FindDatum("hits");
    }
    hits->SetName("hits");
    AddDatum(hits);
}

void ND::TReconBase::AddConstituent(ND::THandle<ND::TReconBase> obj) {
    ND::THandle<ND::TReconObjectContainer> container =
        Get<ND::TReconObjectContainer>("constituents");
    if (!container) {
        // Make sure that there aren't any data vector entries with a
        // conflicting name.
        TDatum* h = FindDatum("constituents");
        while (h) {
            erase(h);
            delete h;
            h = FindDatum("constituents");
        }
        // Create the needed object.
        ND::TReconObjectContainer* c
            = new TReconObjectContainer("constituents", 
                                        "Constituents of this object");
        AddDatum(c);
        // And fill the handle so the creation happens transparently.
        container = Get<ND::TReconObjectContainer>("constituents");
    }
    container->push_back(obj);
}

void ND::TReconBase::AddConstituents(
    ND::THandle<ND::TReconObjectContainer> objs) {
    ND::TReconObjectContainer::const_iterator it;
    for (it = objs->begin(); it!= objs->end(); ++it) {
        AddConstituent((*it));
    }
}

ND::THandle<ND::TReconObjectContainer> ND::TReconBase::GetConstituents() const {
    return Get<ND::TReconObjectContainer>("constituents");
}

std::string ND::TReconBase::ConvertStatus() const { 
    std::string s("(");
    bool notFirst = false;
    if (CheckStatus(kRan)) {
        s+= "ran";
        notFirst = true;
    }
    if (CheckStatus(kSuccess)) {
        if (notFirst) s+=":";
        s+= "success";
        notFirst = true;
    }
    if (CheckStatus(kChi2Fit)) {
        if (notFirst) s+=":";
        s+= "chi2";
        notFirst = true;
    }
    if (CheckStatus(kLikelihoodFit)) {
        if (notFirst) s+=":";
        s+= "likelihood";
        notFirst = true;
    }
    if (CheckStatus(kKalmanFit)) {
        if (notFirst) s+=":";
        s+= "kalman";
        notFirst = true;
    }
    s+= ")";
    return s;
}

std::string ND::TReconBase::ConvertDetector() const {
    std::string s("(");
    bool notFirst = false;
    if (UsesDetector(kP0D)) {
        s+= "P0D";
        notFirst = true;
    }
    if (UsesDetector(kTPC1)) {
        if (notFirst) s+="-";
        s+= "TPC1";
        notFirst = true;
    }
    if (UsesDetector(kTPC2)) {
        if (notFirst) s+="-";
        s+= "TPC2";
        notFirst = true;
    }
    if (UsesDetector(kTPC3)) {
        if (notFirst) s+="-";
        s+= "TPC3";
        notFirst = true;
    }
    if (UsesDetector(kFGD1)) {
        if (notFirst) s+="-";
        s+= "FGD1";
        notFirst = true;
    }
    if (UsesDetector(kFGD2)) {
        if (notFirst) s+="-";
        s+= "FGD2";
        notFirst = true;
    }
    if (UsesDetector(kSFG)) {
        if (notFirst) s+="-";
        s+= "SFG";
        notFirst = true;
    }
    if (UsesDetector(kTOF)) {
        if (notFirst) s+="-";
        s+= "TOF";
        notFirst = true;
    }
    if (UsesDetector(kTopHAT)) {
        if (notFirst) s+="-";
        s+= "TopHAT";
        notFirst = true;
    }
    if (UsesDetector(kBottomHAT)) {
        if (notFirst) s+="-";
        s+= "BottomHAT";
        notFirst = true;
    }
    if (UsesDetector(kDSECal)) {
        if (notFirst) s+="-";
        s+= "DSECal";
        notFirst = true;
    }
    if (UsesDetector(kRightTECal)) {
        if (notFirst) s+="-";
        s+= "RTECal";
        notFirst = true;
    }
    if (UsesDetector(kTopTECal)) {
        if (notFirst) s+="-";
        s+= "TTECal";
        notFirst = true;
    }
    if (UsesDetector(kLeftTECal)) {
        if (notFirst) s+="-";
        s+= "LTECal";
        notFirst = true;
    }
    if (UsesDetector(kBottomTECal)) {
        if (notFirst) s+="-";
        s+= "BTECal";
        notFirst = true;
    }
    if (UsesDetector(kRightPECal)) {
        if (notFirst) s+="-";
        s+= "RPECal";
        notFirst = true;
    }
    if (UsesDetector(kTopPECal)) {
        if (notFirst) s+="-";
        s+= "TPECal";
        notFirst = true;
    }
    if (UsesDetector(kLeftPECal)) {
        if (notFirst) s+="-";
        s+= "LPECal"; 
        notFirst = true;
    }
    if (UsesDetector(kBottomPECal)) {
        if (notFirst) s+="-";
        s+= "BPECal";
        notFirst = true;
    }
    if (UsesDetector(kRightSMRD)) {
        if (notFirst) s+="-";
        s+= "RSMRD";
        notFirst = true;
    }
    if (UsesDetector(kTopSMRD)) {
        if (notFirst) s+="-";
        s+= "TSMRD";
        notFirst = true;
    }
    if (UsesDetector(kLeftSMRD)) {
        if (notFirst) s+="-";
        s+= "LSMRD";
        notFirst = true;
    }
    if (UsesDetector(kBottomSMRD)) {
        if (notFirst) s+="-";
        s+= "BSMRD";
        notFirst = true;
    }
    s+= ")";
    return s;
}

void ND::TReconBase::ls(Option_t *opt) const {
    ls_base(opt);

    TROOT::IncreaseDirLevel();
    std::string option(opt);
    if (fState) {
        TROOT::IncreaseDirLevel();
        fState->ls(opt);
        TROOT::DecreaseDirLevel();
    }
    if (fNodes && option.find("dump") != std::string::npos) {
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

void ND::TReconBase::ls_base(Option_t *opt) const {
    ND::TData::ls(opt);
    TROOT::IncreaseDirLevel();
    TROOT::IndentLevel();
    std::cout << "Status: " << ConvertStatus();
    std::string tmp;
    tmp = GetAlgorithmName();
    if (tmp == "") tmp = "not-set";
    if (tmp != "") std::cout << " Algorithm: \"" << tmp << "\"";
    tmp = ConvertDetector();
    if (tmp == "()") tmp = "(not-set)";
    std::cout << "  In: " << tmp; 
    std::cout << std::endl;

    TROOT::IndentLevel();
    std::ios::fmtflags save = std::cout.flags();
    std::cout << "Quality: " << std::setprecision(4) << GetQuality() 
              << " for " << GetNDOF() << " d.o.f." << std::endl;
    std::cout.flags(save);
    TROOT::DecreaseDirLevel();
}

// Add all of the contents to the browser.
void ND::TReconBase::Browse(TBrowser *b) {
    ND::TDataVector::Browse(b);
    if (!b) return;
    b->Add(fNodes,"Nodes");
    b->Add(fState,"State");
}

ClassImp(ND::TReconObjectContainer);

ND::TReconObjectContainer::TReconObjectContainer() 
    : ND::TDatum("unnamed","Recon Object Container") {}

ND::TReconObjectContainer::TReconObjectContainer(const char* name,
                                                 const char* title) 
    : ND::TDatum(name,title) {}

ND::TReconObjectContainer::~TReconObjectContainer() {}

void ND::TReconObjectContainer::push_back(
    ND::THandle<ND::TReconBase> data) {
    std::string name = data->GetName();
    if (name == "unnamed") data->SetName(data->ClassName());
    std::vector< ND::THandle<ND::TReconBase> >::push_back(data);
}

void ND::TReconObjectContainer::ls(Option_t* opt) const {
    ND::TDatum::ls(opt);
    TROOT::IncreaseDirLevel();
    for (const_iterator t = begin();
         t != end();
         ++t) {
        t->ls(opt);
    }
    TROOT::DecreaseDirLevel();
}

// Add all of the contents to the browser.
void ND::TReconObjectContainer::Browse(TBrowser *b) {
    if (!b) return;
    for (iterator i = begin(); i != end(); ++i) {
        TObject* obj = ND::GetPointer(*i);
        if (!obj) continue;
        std::string name = obj->ClassName();
        if (obj->GetName()) name = obj->GetName();
        b->Add(obj,name.c_str());
    }
}

