#define TND280EventsSelector_cxx_seen
// The class definition in ND::TND280EventsSelector.hxx has been generated
// automatically by the ROOT utility TTree::MakeSelector().  
//
// This should be used as a base class for user selectors.
//
// This class is derived from the ROOT class TSelector.  The following members
// functions are called by the TTree::Process() functions:

//    Begin():       called everytime a loop on the tree starts,
//                   a convenient place to create your histograms.
//    Notify():      this function is called at the first entry of a new Tree
//                   in a chain.
//    Process():     called for each event. In this function you decide what 
//                   to read and you fill your histograms.
//    Terminate():   called at the end of a loop on the tree,
//                   a convenient place to draw/fit your histograms.
//
//   To use this file, derive from it as a public base class, and then
//   override Process() (possible Begin and Terminate).  Then try the following
//   session on your Tree T
//
// Root > T->Process("YourSelector.C")
// Root > T->Process("YourSelector.C","some options")
// Root > T->Process("YourSelector.C+")
//

#include "TND280EventsSelector.hxx"
#include "TH2.h"
#include "TStyle.h"

void ND::TND280EventsSelector::Begin(TTree *tree) {
   // Function called before starting the event loop.
   // When running with PROOF Begin() is only called in the client.
   // Initialize the tree branches.
}

void ND::TND280EventsSelector::SlaveBegin(TTree *tree) {
   // Function called before starting the event loop.
   // When running with PROOF SlaveBegin() is called in each slave
   // Initialize the tree branches.
   Init(tree);
}

Bool_t ND::TND280EventsSelector::Process(Long64_t entry) {
   // Processing function. This function is called
   // to process an event. It is the user's responsability to read
   // the corresponding entry in memory (may be just a partial read).
   // Once the entry is in memory one can apply a selection and if the
   // event is selected histograms can be filled.
   return kTRUE;
}

void ND::TND280EventsSelector::SlaveTerminate() {
   // Function called at the end of the event loop in each PROOF slave.
}

void ND::TND280EventsSelector::Terminate() {
   // Function called at the end of the event loop.
   // When running with PROOF Terminate() is only called in the client.
}

void ND::TND280EventsSelector::Init(TTree * tree) {
    //   Set object pointer
    ND280Event = 0;
    //   Set branch addresses
    if (tree == 0) return;
    fChain = tree;
    fChain->SetMakeClass(1);
    
    fChain->SetBranchAddress("ND280Event", &ND280Event);
}

Bool_t ND::TND280EventsSelector::Notify() {
    // Called when loading a new file.
    // Get branch pointers.
    b_ND280Event = fChain->GetBranch("ND280Event");
    return kTRUE;
}
