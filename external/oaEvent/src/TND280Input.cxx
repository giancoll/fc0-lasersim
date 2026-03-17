//
// Implement a class to read an event from a file.
// 

#include <iostream>
#include <TFile.h>
#include <TTree.h>
#include <TFolder.h>

#include "TND280Input.hxx"

#include "TND280Event.hxx"
#include "TOADatabase.hxx"
#include "TND280Log.hxx"

ND::TND280Input::TND280Input(const char* name, Option_t* option, int compress) 
    : fFile(NULL), fSequence(0), fEventTree(NULL), fEventPointer(0),
      fEventsRead(0), fAttached(false) {
    fFile = new TFile(name, option, "ROOT Input File", compress);
    if (!fFile || !fFile->IsOpen()) {
        throw ND::ENoND280InputFile();
    }
    //ND280Error("ROOT may produce warning messages about missing ShowMembers" 
    //           << " methods which may be ignored");
    IsAttached();
    ND::TOADatabase::Get().SetCurrentInputFile(fFile);
    ND280Verbose("Input file " << fFile->GetName() << " is open");
}

ND::TND280Input::TND280Input(TFile* file) 
    : fFile(file), fSequence(0), fEventTree(NULL), fEventPointer(0),
      fEventsRead(0), fAttached(false) {
    if (!fFile || !fFile->IsOpen()) {
        throw ND::ENoInputFile();
    }
    //ND280Error("ROOT may produce warning messages about missing ShowMembers" 
    //           << " methods which may be ignored");
    IsAttached();
    ND::TOADatabase::Get().SetCurrentInputFile(fFile);
    ND280Verbose("Input file " << fFile->GetName() << " is open");
}

ND::TND280Input::~TND280Input(void) {
    Close();
    if (fFile) delete fFile;
}

#ifdef PRIVATE_COPY
ND::TND280Input::TND280Input(const ND::TND280Input& aFile) {
    // Copy constructor not implemented
    MayNotUse("Copy Constructor");
    return;
}
#endif

const char* ND::TND280Input::GetInputName() const {
    if (fFile) return fFile->GetName();
    return NULL;
}

bool ND::TND280Input::IsAttached(void) {
    if (!IsOpen()) return false;

    // Make sure the **** global file pointer is pointing to this file.
    if (gFile != fFile) {
        if (gFile) {
            ND280Debug("Changing current file from " << gFile->GetName()
                       << " to " << fFile->GetName() << " to read.");
        }
        fFile->cd();
    }

    // Make sure that we have the event tree.
    if (!fEventTree) {
        fEventTree = dynamic_cast<TTree*>(fFile->Get("ND280Events"));
        if (!fEventTree) throw ENoEvents();
    }

    return true;
}

Int_t ND::TND280Input::GetEventsInFile(void) {
    // Returns number of events in this file that can be read.
    if (!IsAttached()) return 0;
    return static_cast<Int_t>(fEventTree->GetEntries());
}

Int_t ND::TND280Input::GetEventsRead(void) {
    // Returns number of events read from this file
    return fEventsRead;
}

bool ND::TND280Input::IsOpen(void) {
    return fFile->IsOpen();
}

bool ND::TND280Input::EndOfFile(void) {
    // Flag that we are past an end of the file.  This is true if the next
    // event to be read is before the first event, or after the last event.  
    if (fSequence<0) {
        fSequence = -1;
        return true;
    }
    if (GetEventsInFile()<=fSequence) {
        fSequence = GetEventsInFile();
        return true;
    }
    return false;
}

ND::TND280Event* ND::TND280Input::NextEvent(int skip) {
    if (skip>0) fSequence += skip;
    return ReadEvent(++fSequence);
}

ND::TND280Event* ND::TND280Input::PreviousEvent(int skip) {
    if (skip>0) fSequence -= skip;
    return ReadEvent(--fSequence);
}

ND::TND280Event* ND::TND280Input::ReadEvent(Int_t n) {
    // Read the n'th event (starting from 0) in the file
    fSequence = n;
    if (fSequence<0) {
        fSequence = -1;
        return NULL;
    }

    if (!IsAttached()) return NULL;

    // Set up for a new event structure to be allocated.
    fEventPointer = new ND::TND280Event;
    fEventTree->SetBranchAddress("ND280Event",&fEventPointer);

    // Read the new event.
    int nBytes = fEventTree->GetEntry(fSequence);
    if (nBytes > 0) {
        fEventsRead++;
        fEventPointer->Register();
    } else {
        fSequence = GetEventsInFile();
        delete fEventPointer;
        fEventPointer = NULL;
    }

    ND::TOADatabase::Get().SetCurrentInputFile(fFile);
    return fEventPointer;
}

void ND::TND280Input::Close(Option_t* opt) {
    TFile* current = ND::TOADatabase::Get().CurrentInputFile();
    if (fFile == current) ND::TOADatabase::Get().SetCurrentInputFile(NULL);
    fFile->Close(opt);
}

ClassImp(ND::TND280Input)

    
