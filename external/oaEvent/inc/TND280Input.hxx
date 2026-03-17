#ifndef TND280Input_hxx_seek
#define TND280Input_hxx_seek

#include <TFile.h>

#include "EoaCore.hxx"
#include "TVInputFile.hxx"

class TTree;

namespace ND {
    OA_EXCEPTION(END280Input,EoaCore);
    OA_EXCEPTION(ENoND280InputFile,END280Input);
    OA_EXCEPTION(ENoEvents,END280Input);

    class TND280Event;
    class TND280Input;
}

/// Attach to a file so that the events can be read.  This also checks
/// to see if the geometry is saved in the input file and reads it if
/// it can.  This will only read files that were written using the
/// TND280Output class.  This will work with any file name, but the preferred
/// file extension is [name].root (using this extension will help root
/// identify this file as an ND280 event file).
class ND::TND280Input : public TVInputFile {
public:
    /// Open an input file. 
    TND280Input(const char* fName, Option_t* option="", Int_t compress = 1);

    /// Use a TFile that was opened someplace else.  The ND280Input object
    /// takes ownership of the file.  This is particularly useful if the file
    /// has been openned using the static TFile::Open method.  This method
    /// returns a pointer to a TFile derived class that is appropriate to the
    /// type of file (dist, http, &c).
    TND280Input(TFile* file);

    virtual ~TND280Input(void);
    
    /// Overload the base class filename.
    const char* GetFilename() const {return GetInputName();}

    /// Check that an input file is able to read an event.  If the
    /// file is not ready, then this will try to set up for reading.
    virtual bool IsAttached(void);

    /// Return the total number of events in this file 
    virtual Int_t GetEventsInFile(void);

    /// Get the number of events that have been read. 
    virtual Int_t GetEventsRead(void);

    /// Flag that the input file is open and ready to be read.
    virtual bool IsOpen(void);

    /// Flag that we are past an end of the file.  This is true if an attempt
    /// was made to read before the first event of the file, or after the last
    /// event in the file.
    virtual bool EndOfFile(void);

    /// Return the first event in the file.
    virtual TND280Event* FirstEvent(void) {return ReadEvent(0);}

    /// Read the next event in the file.
    virtual TND280Event* NextEvent(int skip = 0);

    /// Read the previous event in the file.
    virtual TND280Event* PreviousEvent(int skip = 0);

    /// Return the first event in the file.
    virtual TND280Event* LastEvent(void) {
        return ReadEvent(GetEventsInFile()-1);
    }

    /// Read the n'th event in the file.  The events are counted from zero.
    virtual TND280Event* ReadEvent(Int_t n);

    /// Make sure that the file is closed.  This method is specific to
    /// TND280Input.
    virtual void Close(Option_t* opt = "");

    /// The generic way to close the file.  This is inherited from TVInputFile
    /// and must be provided for nd280EventLoop (it's pure virtual).
    virtual void CloseFile() {Close();}

    /// Return the entry number of the event within the tree.
    virtual int GetPosition(void) const {
        if (fSequence<0) return -1;
        return fSequence;
    }

    /// Return the TFile that events are being read from.
    TFile* GetFilePointer(void) const {return fFile;}

    /// Return the file name to provide the base abstract input name class.
    virtual const char* GetInputName(void) const;

private:
    TFile* fFile;               // The file to get events from.
    Int_t fSequence;            // The sequence number of the last event read.

    TTree* fEventTree;          // the TTTree of event objects. 
    TND280Event* fEventPointer; //! the last event read.
    
    Int_t fEventsRead;          //! count of events read from file
    bool fAttached;             //! are we prepared to read from the file?

#ifdef PRIVATE_COPY
private:
    TND280Input(const TND280Input& aFile);
#endif

    ClassDef(TND280Input,0);
};
#endif
