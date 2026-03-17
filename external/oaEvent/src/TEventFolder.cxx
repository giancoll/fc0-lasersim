// $Id$
//
//  Implement an type of folder specifically for holding TEvents.

#include <stdlib.h>
#include <iostream>

#include <TROOT.h>
#include <TBrowser.h>
#include <TFolder.h>
#include <TQObject.h>

#include "TND280Event.hxx"
#include "TEventFolder.hxx"
#include "TND280Log.hxx"

ND::TEventFolder* ND::TEventFolder::fEventFolder = NULL;
ND::TND280Event* ND::TEventFolder::fCurrentEvent = NULL;

ND::TEventFolder::TEventFolder() {
    fFolderOfEvents = NULL;
}

ND::TEventFolder::~TEventFolder() {
    fFolderOfEvents = NULL;
    fEventFolder = NULL;
    fCurrentEvent = NULL;
}

TFolder* ND::TEventFolder::GetFolder(void) const {
    if (!fFolderOfEvents) return NULL;
    return fFolderOfEvents;
}

ND::TEventFolder* ND::TEventFolder::GetEventFolder(void) {
    if (!fEventFolder) {
        fEventFolder = new ND::TEventFolder();
    }
    if (!fEventFolder->fFolderOfEvents) {
        fEventFolder->fFolderOfEvents
            = gROOT->GetRootFolder()->AddFolder("Events","ND280 Events");
        gROOT->GetListOfBrowsables()->Add(fEventFolder->fFolderOfEvents,
                                          "Events");
        gROOT->GetListOfCleanups()->Add(fEventFolder->fFolderOfEvents);
    }
    #define CONNECT_SLOT
    #ifdef CONNECT_SLOT
    TQObject::Connect("TEvent","Selected(ND::TDatum*)",
                      "TEventFolder",fEventFolder,"EventSelected(TObject*)");
    #endif

    return fEventFolder;
}

ND::TND280Event* ND::TEventFolder::GetEvent(int indx) const {
    if (!fFolderOfEvents) return NULL;
    TList* folder = dynamic_cast<TList*>(fFolderOfEvents->GetListOfFolders());
    if (!folder) return NULL;
    ND::TND280Event* current = NULL;
    int count = 0;

    for ( TObjLink* objlink = folder->LastLink();
          objlink != NULL;
          objlink = objlink->Prev()) {
        current = dynamic_cast<ND::TND280Event*>(objlink->GetObject());
        if (current) ++count;
        if (count > indx) break;
    }
    if (current) fCurrentEvent = current;
    return current;
}

void ND::TEventFolder::SetCurrentEvent(ND::TND280Event* event) {
    if (!event) return;
    if (!fFolderOfEvents) return;
    TList* folder = dynamic_cast<TList*>(fFolderOfEvents->GetListOfFolders());
    if (!folder) return;

    ND::TND280Event* oldEvent = fCurrentEvent;
    for (TObjLink* objlink = folder->LastLink();
          objlink != NULL;
          objlink = objlink->Prev()) {
        ND::TND280Event* inList = dynamic_cast<ND::TND280Event*>(objlink->GetObject());
        if (inList == event) {
            fCurrentEvent = event;
        }
    }
    ND280Info("Current event changed from " << oldEvent
              << " to " << fCurrentEvent);
}

ND::TND280Event* ND::TEventFolder::FindEvent(int, int) const {
    return NULL;
}

ND::TND280Event* ND::TEventFolder::GetCurrentEvent(void) {
    return fCurrentEvent;
}

void ND::TEventFolder::RegisterEvent(ND::TND280Event* event) {
    fCurrentEvent = event;
    if (fCurrentEvent && fEventFolder && fEventFolder->fFolderOfEvents) {
        fEventFolder->fFolderOfEvents->Add(event);
        event->SetBit(kMustCleanup);
    }
}

void ND::TEventFolder::RemoveEvent(ND::TND280Event* event) {
    // Check if the event is the current event.
    if (fCurrentEvent == event) fCurrentEvent = NULL;

    // Check if the folder is active.  If not, then just return.
    if (!fEventFolder) return;
    if (!fEventFolder->GetFolder()) return;
    TList* folder = 
        dynamic_cast<TList*>(fEventFolder->GetFolder()->GetListOfFolders());
    if (!folder) return;

    // The folder is active, so find another event in the folder and make it
    // the current event.
    for (TObjLink* objlink = folder->LastLink(); 
         objlink != NULL;       
         objlink = objlink->Prev()) {
        fCurrentEvent = dynamic_cast<ND::TND280Event*>(objlink->GetObject());
        // Any event is OK as long as it's not the event being removed.
        if (fCurrentEvent != event) break;
    }
}

void ND::TEventFolder::EventSelected(TObject* theObject) {
    ND280Info("EventFolder had " << theObject->ClassName()  
              << " at " << theObject << " selected");
    ND::TND280Event* event = dynamic_cast<ND::TND280Event*>(theObject);
    SetCurrentEvent(event);
}


void ND::TEventFolder::ls(const Option_t* opt) const {
    if (fFolderOfEvents) fFolderOfEvents->ls(opt);
    else std::cout << "No Event Folder" << std::endl;
}

ClassImp(ND::TEventFolder);
