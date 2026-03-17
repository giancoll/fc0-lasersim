#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <cstdlib>

#include "TMidasFile.hxx"
#include "TMidasEventParser.hxx"
#include "TND280RawEvent.hxx"
#include "TND280Event.hxx"
#include "TND280Context.hxx"

ClassImp(ND::TMidasFile);

ND::TMidasFile::TMidasFile()
{
  UInt_t endian = 0x12345678;

  fLastErrno  = 0;
  fDoByteSwap = *(char*)(&endian) != 0x78;
  fFile       = -1;
  fGzFile     = NULL;
  fEndOfFile  = false;
  fEventsRead   = 0;
}

ND::TMidasFile::~TMidasFile()
{
  Close();
}

bool ND::TMidasFile::IsOpen() {
    return ((fFile > 0) || fGzFile);
}

bool ND::TMidasFile::Open(const char *filename)
{
  /// Opens a midas .mid file with given file name.
  ///
  /// \param [in] filename The file to open.
  ///
  /// \returns "true" for succes, "false" for error, use GetLastError() to
  /// see why

  if (IsOpen()) Close();

  fFilename = filename;
  fEventsRead = 0;

#ifndef O_LARGEFILE
#define O_LARGEFILE 0
#endif

  // Open either compressed (if last 2 characters of filename are 'gz') or
  // uncompressed file.

  int len = strlen(filename);
  if (filename[len-2]=='g' && filename[len-1]=='z') {

      fGzFile = gzopen(filename,"rb");
      if (fGzFile == NULL) {
          fLastErrno = -1;
          fLastError = "zlib gzdopen() error";
          return false;
      }
  }
  else {

      fFile = open(filename, O_RDONLY | O_LARGEFILE);
      if (fFile <= 0) {
	  fLastErrno = errno;
	  fLastError = strerror(errno);
	  return false;
      }
  }

  return true;
}

static int readpipe(int fd, char* buf, int length)
{
  int count = 0;
  while (length > 0)
    {
      int rd = read(fd, buf, length);
      if (rd > 0)
        {
          buf += rd;
          length -= rd;
          count += rd;
        }
      else if (rd == 0)
        {
          return count;
        }
      else
        {
          return -1;
        }
    }
  return count;
}

bool ND::TMidasFile::Read(TMidasEventParser *midasEvent)
{
  /// \param [in] midasEvent Pointer to an empty TMidasEventParser
  ///
  /// \returns "true" for success, "false" for failure, see GetLastError() to
  /// see why

  midasEvent->Clear();

  int rd = 0;

  if (fGzFile) {
      rd = gzread(fGzFile, midasEvent->GetEventHeader(),
                  midasEvent->GetEventHeaderSize());
  }
  else {
      rd = readpipe(fFile, midasEvent->GetEventHeader(),
                    midasEvent->GetEventHeaderSize());
  }

  if (rd == 0)
    {
      fLastErrno = 0;
      fLastError = "EOF";
      return false;
    }
  else if (rd != (int)midasEvent->GetEventHeaderSize())
    {
      fLastErrno = errno;
      fLastError = strerror(errno);
      return false;
    }

  if (fDoByteSwap) {
    midasEvent->SwapBytesEventHeader();
  }

  if (!midasEvent->IsGoodSize())
    {
      fLastErrno = -1;
      fLastError = "Invalid event size";
      return false;
    }

  if (fGzFile) {
      rd = gzread(fGzFile, midasEvent->GetData(), midasEvent->GetDataSize());
  }
  else {
      rd = readpipe(fFile, midasEvent->GetData(), midasEvent->GetDataSize());
  }

  if (rd != (int)midasEvent->GetDataSize())
    {
      fLastErrno = errno;
      fLastError = strerror(errno);
      return false;
    }

  midasEvent->SwapBytes(false);

  return true;
}

void ND::TMidasFile::Close() {
    if (fGzFile)   gzclose(fGzFile);
    if (fFile > 0) close(fFile);
    fGzFile = NULL;
    fFile = -1;
    fFilename = "";
    fLastErrno  = 0;
    fLastError  = "";
    fEndOfFile  = false;
}

int ND::TMidasFile::GetPosition(void) const {
    // We are past the end of file, so the position is the number of events
    // read.
    if (fEndOfFile) return fEventsRead;

    // The position of the event just read.
    int position = fEventsRead - 1;

    // If we haven't read an event yet, then the position is -1;
    if (position<0) return -1;

    return position;
}

ND::TND280RawEvent* ND::TMidasFile::ReadRawEvent() {

    // Create a scratch TMidasEventParser and if filled succesfully return
    // a TND280RawEvent filled by it.
    ND::TMidasEventParser mep;
    if ( ! this->Read(&mep) ) {
        if (this->GetLastErrno()) {
            std::cout << "ERROR: TMidasFile -- " << this->GetLastError()
                      << std::endl;
        }
        fEndOfFile = true;
        return 0;
    }
    ++ fEventsRead;
    return new ND::TND280RawEvent(mep,"Unnamed");

}

ND::TND280Event* ND::TMidasFile::NextEvent(int skip) {
    TND280RawEvent* raw = NULL;
    do {
        raw = ReadRawEvent();
        if (!raw) return NULL;
    } while (0 < skip--);

    // Information in the context will be overridden by information found in
    // the raw event when the TND280Event is built.  This context provides
    // back up information incase the raw event is missing data (e.g. the
    // sub-run number).
    ND::TND280Context context;
    const ND::TRawDataHeader& header = raw->GetHeader();

    context.SetRun(header.GetRun());

    // KLUDGE ALERT: Extract the sub run number from the filename, using
    // the assumption that the filename has convention *_XXXX.daq.mid or
    // *_XXXX.daq.mid.gz where XXXX is the four digit sub run number.
    std::string::size_type nn = fFilename.find(".daq.mid");
    if(nn != std::string::npos
       && nn >= 5
       && fFilename[nn-5] == '_'){
        std::string str2 = fFilename.substr(nn-4,4);
        context.SetSubRun(std::atoi(str2.c_str()));
    }

    TND280Event* event = new TND280Event(context, raw);
    return event;
}

ND::TND280Event* ND::TMidasFile::FirstEvent() {
    // Take a copy of the filename (Open calls Close which wipes fFilename).
    std::string file_name(fFilename);
    this->Open(file_name.c_str());
    return NextEvent();
}
