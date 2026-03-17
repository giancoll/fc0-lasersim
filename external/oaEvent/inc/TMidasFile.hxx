#ifndef TMidasFile_hxx_seen
#define TMidasFile_hxx_seen

#include <string>

#include <Rtypes.h>
#include <zlib.h>

#include "TVInputFile.hxx"

namespace ND {
    class TMidasEventParser;
    class TMidasFile;
    class TND280RawEvent;
};
/// Acknowledgements: This class derives from, and is almost identical to, 
/// the class of the same name developed by 
///
///   Stefan Ritt, Pierre-Andre Amaudruz and Konstantin Olchanski 
///
/// in the rootana package which is part of the Midas Data
/// Acquisition:-
/// 
///     https://midas.psi.ch/
/// 
/// The code was extracted from revision 55 of Subversion repository:-
///
///      svn://ladd00.triumf.ca/rootana/trunk rootana
///
/// The T2K group should monitor changes to that code looking for enhancements
/// and should report back any code improvements we make to Konstantin.
/// 
/// Last checked rootana revision: 66.
/// 
/// TMidasFile is a class that can sequentially read a MIDAS .mid file using C
/// I/O and return a TND280RawEvent using the method:-
/// 
///   ND::TND280RawEvent* TMidasFile::ReadRawEvent() 
/// 
/// The current implementation first fills a TMidasEventParser, which is
/// another class derived from rootana, using the
/// 
///   bool Read(TMidasEventParser *event);
/// 
/// method and then uses the TND280RawEvent(TMidasEventParser&) constructor.

class  ND::TMidasFile : public ND::TVInputFile
{
public:
    /// default constructor
    TMidasFile();          
    /// destructor
    virtual ~TMidasFile(); 

    /// Open input file
    bool Open(const char* filename);

    /// Close input file
    void Close(); 

    /// Read one event from the file
    TND280RawEvent* ReadRawEvent(); 

    /// Get the name of this file
    const char* GetFilename()  const { return fFilename.c_str();  } 

    /// Get error value for the last file error
    int         GetLastErrno() const { return fLastErrno; }

    /// Get error text for the last file error
    const char* GetLastError() const { return fLastError.c_str(); } 

    /// Return the first event in the input file.  If the file does not
    /// support rewind, then this should throw an exception if it is called a
    /// second time.
    virtual ND::TND280Event* FirstEvent();

    /// Get the next event from the input file.  If skip is greater than zero,
    /// then skip this many events before returning.
    virtual ND::TND280Event* NextEvent(int skip=0);
    
    /// Return the position of the event just read inside of the file.  A
    /// position of zero is the first event.  After reading the last event,
    /// the position will be the total number of events in the file.
    virtual int GetPosition(void) const;

    /// Flag that the file is open.
    virtual bool IsOpen();

    /// Flag that the end of the file has been reached.
    virtual bool EndOfFile() {return fEndOfFile;}

    /// Close the input file.
    virtual void CloseFile() {Close();}

protected:

    /// Internal method to read one event from the file into TMidasEventParser
    bool Read(ND::TMidasEventParser *midasEvent); 

    /// name of the currently open file
    std::string fFilename; 

    /// errno from the last operation
    int         fLastErrno; 
    /// error string from last errno
    std::string fLastError; 

    /// "true" if file has to be byteswapped
    bool fDoByteSwap; 

    /// open file descriptor used for uncompressed files
    int         fFile;   
    /// zlib compressed file reader
    gzFile      fGzFile; 

    /// The end of file has been found.
    bool fEndOfFile;

    /// The number of events read from the file.
    int fEventsRead;

    ClassDef(TMidasFile,0)
};

#endif // TMidasFile_hxx_seen
