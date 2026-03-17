// $Id$
//
// Implement the ND::TND280Output class which is a specialization of the
// TEventOutput class that writes the events in a root format.  This is the
// native format.
//

#include <TFile.h>
#include <TTree.h>
#include <TGeoManager.h>
#include <TKey.h>

#include "TND280Output.hxx"
#include "TND280Event.hxx"
#include "TOADatabase.hxx"
#include "TTPCPadManager.hxx"
#include "TND280Log.hxx"

ClassImp(ND::TND280Output);

ND::TND280Output::TND280Output(const char *fileName,
                               Option_t* opt, 
                               Int_t compress) 
    : TFile(fileName, opt, "ROOT Output File", compress),
      fEventTree(NULL), fEventPointer(NULL), fAttached(false), 
      fEventsWritten(0), fGeometry(NULL) {
    ND280Verbose("Open output file " << fileName);
    IsAttached();
}

ND::TND280Output::~TND280Output(void) {
    if (IsOpen()) Close();
}

bool ND::TND280Output::IsAttached(void) {
    if (!IsOpen()) return false;
    if (gFile != this) {
        if (gFile) {
            ND280Debug("Changing current file from " << gFile->GetName()
                       << " to " << this->GetName() << " to write.");
        }
        cd();
    }
    if (fAttached) return true;
    ND280Info("Attaching to " << this->GetName());
    
    //////////////
    // Hmm... Not attached, so make sure it is attached.
    // Make sure the object is attached
    fEventPointer = NULL;
    if (!fEventTree) {
        ND280Trace("Create a new tree");
        fEventTree = new TTree("ND280Events", "ND280 Event Tree");
    }
    ND280Trace("Add the branch pointer");
    fEventTree->Branch("ND280Event","ND::TND280Event",&fEventPointer,128000,0);
    fEventPointer = NULL;       // Make sure it's empty.
    fAttached = true;

    ND280Trace("Attached");
    return fAttached;
}

int ND::TND280Output::GetEventsWritten(void) {return fEventsWritten;}

void ND::TND280Output::WriteEvent(ND::TND280Event& event) {
    if (!IsAttached()) return;
    // Copy the pointer into the location attached to the file.
    fEventPointer = &event;
    // Put the event into the tree;
    if (fEventTree->Fill()<0) {
        ND280Error("Error while writing an event");
        throw ND::END280OutputWriteFailed();
    }
    // Empty out the fEventPointer so that it can't be written twice.
    fEventPointer = NULL;
}

// Save a geometry to the output file.
void ND::TND280Output::WriteGeometry(TGeoManager* geom) {
    if (!IsAttached()) return;
    if (!geom) return;
    TKey *key = FindKey(geom->GetName());
    if (key) return;
    fGeometry = geom;
    if (geom->Write()<1) {
        ND280Error("Error while writing geometry");
        throw ND::END280OutputWriteFailed();
    }
    Flush();
    ND280Log("** Geometry " << geom->GetName() << " written to output file ");
}

bool ND::TND280Output::GeometryWritten(void) {
    return fGeometry;
}
    
void ND::TND280Output::Commit(void) {
    if (!IsAttached()) return;
    fEventTree->AutoSave();
    Flush();
}

void ND::TND280Output::Close(Option_t* opt) {
    Write();
    if (ND::TND280Log::LogLevel <= ND::TND280Log::GetLogLevel()) {
        TFile::ls();
    }
    TFile::Close(opt);
}

