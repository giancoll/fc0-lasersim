#ifndef THandleHack_hxx_Seen
#define THandleHack_hxx_Seen

namespace ND {
    /// A useful debugging/memory leak detecting routine to find places where
    /// THandle objects are somehow being misused.  I used it to find a
    /// THitSelection that was not correctly deleted, and which was causing a
    /// huge memory leak.
    bool CleanHandleRegistry(bool eraseAll = false);
    
    /// Dump a list of all the handles that have not been deleted.  To be used
    /// for debugging, the user must have first called
    /// CreateHandleRegistry().
    void DumpHandleRegistry();

    /// Enable or disable an internal set of all active handles.  If called
    /// with an argument of "true", this will create an internal handle
    /// registry and cause your program to run very slowly.  If called with an
    /// argument of "false", the internal registry will be deleted (if it
    /// exists).  After ND::EnableHandleRegistry(true) is called, the
    /// ND::DumpHandleRegistry() function will print a list of handles
    /// remaining in memory.
    ///
    /// The ND::EnableHandleRegistry() and ND::DumpHandleRegistry() are used
    /// by the event loop when it is run using the "-H" command line option.
    void EnableHandleRegistry(bool create);
}
#endif
