//////////////////////////////////////////////////////////
//   This class has been automatically generated 
//     (Sat Apr 24 10:39:14 2004 by ROOT version3.10/02)
//   from TTree ND280Events/ND280 Event Tree
//   found on file: /home/mcgrew/data/jparc/test/nd280-tracker.root
//////////////////////////////////////////////////////////


#ifndef TND280EventsSelector_hxx
#define TND280EventsSelector_hxx

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TSelector.h>

#include "TND280Event.hxx"

namespace ND {
    class TND280EventsSelector;
}

/// A base class that users selectors should be derived from.
class ND::TND280EventsSelector: public TSelector {
public:

    TTree * fChain;	//!pointer to the analyzed TTree or TChain

    //Declaration of leaves types
    TND280Event *ND280Event;
        
    //List of branches
    TBranch *b_ND280Event;	//!
        
    TND280EventsSelector(TTree * tree = 0) {
    }

    virtual ~ TND280EventsSelector() {
    }

    Int_t Version() const {
        return 1;
    }
        
    void Begin(TTree * tree);

    void SlaveBegin(TTree * tree);

    void Init(TTree * tree);

    Bool_t Notify();

    Bool_t Process(Long64_t entry);

    void SetOption(const char *option) {
        fOption = option;
    }

    void SetObject(TObject * obj) {
        fObject = obj;
    }

    void SetInputList(TList * input) {
        fInput = input;
    }

    TList *GetOutputList() const {
        return fOutput;
    }
        
    void SlaveTerminate();

    void Terminate();
	
    ClassDef(TND280EventsSelector, 0);
};
#endif

