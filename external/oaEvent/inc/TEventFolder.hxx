#ifndef T_EVENT_FOLDER_H
#define T_EVENT_FOLDER_H

#include <iostream>
#include <TObject.h>


class TFolder;

namespace ND {
    class TND280Event;
    class TEventFolder;
}

/// Provide management for events read using the TND280Input object and kept
/// in the //root/Events folder.
///
/// TND280Event objects are usually read from an input file using a
/// TND280Input object (output is with a TND280Output).  The last event read
/// will be available from TEventFolder::GetCurrentEvent() which is a static
/// method.  Interactive programs which want to provide a method to keep
/// multiple events in memory (e.g. the event display), should get the current
/// event using the TEventFolder::GetCurrentEvent() method.  Events can then
/// be saved automatically into a root folder by calling the
/// TEventFolder::GetEventFolder() static method.
class ND::TEventFolder : public TObject {

private:
    TEventFolder(); 
    TEventFolder(const TEventFolder& folder) : TObject(folder) { } // no copies
    void operator=(const TEventFolder&) { } // no copies

#ifdef INTERACTIVE_TEVENTFOLDER
public:
#endif
    /// Return a pointer to the //root/Events folder and should be only be
    /// used in interactive programs.  Events are entered into this folder by
    /// the Register method (the Build method automatically calls Register).
    /// Any input class (eg TRootInput) should call Register for each event
    /// that is successfully read.  This gives programs a convenient way to
    /// keep track of which events must be deleted, and which are available
    /// for processing.  This is a trivial way to keep track of more than one
    /// event in memory.  Events are saved using the event number.  Note: that
    /// events will not be saved into the event folder unless GetEventFolder
    /// has been called first. This means that programs that don't want the
    /// folder overhead don't pay the price.  The macro
    /// "INTERACTIVE_TEVENTFOLDER" must be defined before the
    /// "TEventFolder.hxx" file is included for this method to be available in
    /// user code.
    static TEventFolder* GetEventFolder(void);

public:
    virtual ~TEventFolder(void);

    /// A static method to return a pointer to the current event which should
    /// be accessed as "ND::TEventFolder::GetCurrentEvent()".  This will work
    /// even if the events are not being kept in a folder since each event
    /// will handle the manipulation of this field as it is created or
    /// destroyed.  This method should not be accessed through a
    /// ND::TEventFolder object.
    /// \code
    /// // Do not use
    /// ND::TEventFolder::GetEventFolder()->GetCurrentEvent();
    /// \endcode
    /// Accessing GetCurrentEvent through TEventFolder will cause significant
    /// code slow downs.
    static TND280Event* GetCurrentEvent(void);

    /// Set the pointer to the current event.  The event will become the
    /// current event if it is saved in the event folder.  If it is not in the
    /// event folder, then this does nothing.
    void SetCurrentEvent(TND280Event* event);

    /// Register an event with event folder.  This is used in the TND280Event
    /// constructor to make sure that TEventFolder knows about the new event.  
    static void RegisterEvent(TND280Event* event);

    /// Delete an event from the event folder.  Root will handle removing an
    /// event if the folder is active, but since we may be working in a
    /// non-interactive program, that isn't good enough.  This is used in the
    /// TND280Event distructor to tell TEventFolder when an event goes away.
    static void RemoveEvent(TND280Event* event);

    /// Return the actual folder which can be NULL if //root/Events does not
    /// exist.  
    virtual TFolder* GetFolder(void) const;

    /// The current (most recent) event.  If there is no current event, this
    /// will return zero.  If event registration is enabled (it isn't by
    /// default), this can also be used to retrieve events that are on the
    /// stack because they haven't been deleted yet.
    virtual TND280Event* GetEvent(int prev = 0) const;

    /// Find a particular event (not implmented).
    virtual TND280Event* FindEvent(int run, int evt) const;

    /// A slot to capture the event selected in the root browser.  This is
    /// connected when the event folder is constructed.
    virtual void EventSelected(TObject* theObject);

    /// Print the object information.
    virtual void ls(Option_t *opt = "") const;

private:
    static TEventFolder* fEventFolder; 
    static TND280Event* fCurrentEvent;
    TFolder* fFolderOfEvents;
    ClassDef(TEventFolder,2);  
};
#endif
