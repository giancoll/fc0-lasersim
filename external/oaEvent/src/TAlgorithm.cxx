#include "TAlgorithm.hxx"
#include "TOADatabase.hxx"
#include "TND280Log.hxx"

ClassImp(ND::TAlgorithm);

ND::TAlgorithm::TAlgorithm(const char* name, const char* title) :
    TNamed(name, title) , fVerbose(Quiet), fQuiet(false) {

    SetBriefDescription(
        "A quick description of the algorithm :"
        " override this with relevant information");

    SetSummaryDescription(
        "A summary of the algorithm :"
        " override this with relevant information.");

    SetDetailedDescription(
        "A full description of the algorithm :"
        " override this with relevant information");

    SetVersion("none set");
}

ND::TAlgorithm::~TAlgorithm(){ }

std::string ND::TAlgorithm::GetDescription() const {
    switch (fVerbose){
    case ND::TAlgorithm::Quiet: return std::string(GetName());
    case ND::TAlgorithm::Brief: return fBriefDescription;
    case ND::TAlgorithm::Summary: return fSummaryDescription;
    case ND::TAlgorithm::Detailed: return fDetailedDescription;
    }
    return std::string("No Description");
}

void ND::TAlgorithm::Clear(Option_t*) {}

void ND::TAlgorithm::SetVersion(const char* v) {
    fVersion = v;
}

ND::THandle<ND::TAlgorithmResult> ND::TAlgorithm::Execute() {
    ND::TAlgorithmResult empty;
    return Process(empty);
}

ND::THandle<ND::TAlgorithmResult> 
ND::TAlgorithm::MaybeProcess(const ND::TAlgorithmResult& input) {
    ND::THandle<ND::TAlgorithmResult>
        result(GetPointer(GetEvent().GetFit(GetName())));
    if (!result) {
        result = Process(input);
        if (result) GetEvent().AddFit(GetPointer(result));
    }
    return result;
}

ND::THandle<ND::TAlgorithmResult> 
ND::TAlgorithm::Process(const ND::TAlgorithmResult&) {
    // The very most trivial Process method that can be written.
    return ND::THandle<ND::TAlgorithmResult>();
}

ND::TAlgorithmTag ND::TAlgorithm::GetTag() const {
    return ND::TAlgorithmTag(*this);
}

ND::TND280Event& ND::TAlgorithm::GetEvent() const {
    TND280Event* event = ND::TEventFolder::GetCurrentEvent();
    if (!event) {
        ND280Error("No current event");
        throw EAlgorithmWithoutEvent();
    }
    return *event;
}

TGeoManager* ND::TAlgorithm::GetGeom() const {
    TGeoManager* geom = NULL;
    try {
        geom = ND::TOADatabase::Get().Geometry();
    }
    catch (...) {
        ND280Trace("Geometry not found");
        geom = NULL;
    }
    return geom;
}
