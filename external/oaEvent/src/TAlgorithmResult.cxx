#include "TAlgorithmResult.hxx"

#include "TAlgorithm.hxx"
#include "TAlgorithmTag.hxx"
#include "TDataSymLink.hxx"
#include "TND280Log.hxx"


ClassImp(ND::TAlgorithmResult);

ND::TAlgorithmResult::TAlgorithmResult(): fStatusSummary("") { }

ND::TAlgorithmResult::TAlgorithmResult(const char* name, const char* title) 
    : ND::TDataVector(name, title), fStatusSummary("") { }

ND::TAlgorithmResult::TAlgorithmResult(const ND::TAlgorithm& algo) 
    : ND::TDataVector(algo.GetName(), "T2K Algorithm Result"),
      fTag(algo.GetTag()), fStatusSummary("") { }

ND::TAlgorithmResult::TAlgorithmResult(const ND::THitSelection& hits)
    : ND::TDataVector(hits.GetName(), "T2K Algorithm Result"),
      fStatusSummary("") {
    SetName(hits.GetName());
    ND::THitSelection* internalHits = new ND::THitSelection(hits.GetName());
    for (ND::THitSelection::const_iterator h = hits.begin();
         h != hits.end();
         ++h) {
        internalHits->push_back(*h);
    }
    AddHitSelection(internalHits);
}

ND::TAlgorithmResult::~TAlgorithmResult() { }

void ND::TAlgorithmResult::AddStatus(const char* s) {
    std::string status(s);
    fStatusSummary = "(" + status + ") " + fStatusSummary;
}

void ND::TAlgorithmResult::AddStatus(const std::string& status) {
    fStatusSummary = "(" + status + ") " + fStatusSummary;
}

void ND::TAlgorithmResult::SetStatus(const char* status) {
    fStatusSummary = status;
}

void ND::TAlgorithmResult::SetStatus(const std::string& status) {
    fStatusSummary = status;
}

std::string ND::TAlgorithmResult::GetStatus() const {
    return fStatusSummary;
}

void ND::TAlgorithmResult::AddResultsContainer(
    ND::TReconObjectContainer* results) {
    SetDefaultResultsContainer(results->GetName());
    AddDatum(results);
}

void ND::TAlgorithmResult::SetDefaultResultsContainer(const char* name) {
    std::string trackName(name);
    if (trackName == "results") {
        ND280Severe("Illegal TReconObjectContainer name: " << name );
        throw EIllegalRecObjContainerName();
    }

    // Update or add the symbolic link to the default set of tracks.
    ND::TDataSymLink* link 
        = dynamic_cast<ND::TDataSymLink*>(FindDatum("results"));
    if (link) {
        link->SetLink(name);
    }
    else {
        AddDatum(new ND::TDataSymLink("results",name));
    }
}

ND::THandle<ND::TReconObjectContainer> 
ND::TAlgorithmResult::GetResultsContainer(const char* name) const {
    ND::THandle<ND::TReconObjectContainer> result 
        = Get<ND::TReconObjectContainer>(name);
    return result;
}

void ND::TAlgorithmResult::AddHitSelection(ND::THitSelection* hits) {
    SetDefaultHitSelection(hits->GetName());
    AddDatum(hits);
}

void ND::TAlgorithmResult::SetDefaultHitSelection(const char* name) {
    std::string hitName(name);
    if (hitName == "hits") {
        ND280Severe("Illegal hit name " << name);
        throw EIllegalHitSelectionName();
    }

    // Update or add the symbolic link to the default set of hits.
    ND::TDataSymLink* link = dynamic_cast<ND::TDataSymLink*>(FindDatum("hits"));
    if (link) {
        link->SetLink(name);
    }
    else {
        AddDatum(new ND::TDataSymLink("hits",name));
    }
}

ND::THandle<ND::THitSelection> ND::TAlgorithmResult::GetHitSelection(
    const char* name) const {
    ND::THandle<ND::THitSelection> result = Get<ND::THitSelection>(name);
    return result;
}

/// Print the hit information.
void ND::TAlgorithmResult::ls(Option_t *opt) const {
    ND::TData::ls(opt);
    TROOT::IncreaseDirLevel();
    TROOT::IndentLevel();
    std::cout << "Status: ";
    if (fStatusSummary != "") std::cout << fStatusSummary;
    else std::cout << "None";
    std::cout << std::endl;
    fTag.ls(opt);
    for (const_iterator v = begin(); v != end(); ++v) {
        (*v)->ls(opt);
    };
    TROOT::DecreaseDirLevel();
}
