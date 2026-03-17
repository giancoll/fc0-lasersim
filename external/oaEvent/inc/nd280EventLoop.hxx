#ifndef nd280EventLoop_hxx_seen
#define nd280EventLoop_hxx_seen

#include "TND280EventLoopFunction.hxx"

namespace ND {
/// The ND::nd280EventLoop() function provides a basic event loop that will call
/// user code for each event.  See ND::TND280EventLoopFunction for a base class
/// that can be used with this event loop.  This is used in code like this:
///
/// \code
/// class TDumpEvent: public ND::TND280EventLoopFunction {
/// public:
///     bool operator () (ND::TND280Event& event) {
///         // User Code
///         return true;
///     }
/// };
///
/// int main(int argc, char **argv) {
///     TDumpEvent userCode;
///     return ND::nd280EventLoop(argc,argv,userCode);
/// }
/// \endcode
///
/// The default number of events to be processed can be specified as the
/// fourth argument to ND::nd280EventLoop().  If the argument is not
/// specified, or is less than or equal to zero, then all events in the file
/// will be read.  See \ref eventLoop for more usage documentation.
///
/// The above main program results in a basic event loop that provides several
/// user options and capabilities.  The program options can be listed by
/// running the program without an arguments.  The standard options are:
///
/*! \verbatim
    -o <file>         Set the name of an output file.
                        Multiple output files can be provided.
    -a                Read all events
    -c <file>         Set the logging config file name
    -d                Increase the debug level
    -D <name>=[error,severe,warn,debug,trace]
                      Change the named debug level
    -f <event>        Look for an event number
    -f <run>,<event>  Look for a run and event number
    -G <file>         Force a geometry file
    -g                Don't save geometry in output
    -H                Debug THandle (slow)
    -m                MIDAS input file
    -n <cnt>          Only read <cnt> events  [Default: 1]
    -q                Decrease the verbosity
    -r                ND280 ROOT file
    -R                Replace output file if it exists
    -s <cnt>          Skip <cnt> events
    -t [bpTPEFecC]    Only accept these triggers
                        b  Beam triggers
                        p  Pedestal/noise triggers
                        T  TPC laser triggers
                        P P0D light injection triggers
                        E  ECal LED triggers
                        F  FGD LED triggers
                        e  Electronics calib triggers
                        c  TFB cosmic ray triggers
                        C  FGD cosmic ray triggers
    -T <file>         Set the tag file name
    -u                Log the memory and CPU usage
    -v                Increase the verbosity
    -V <name>=[quiet,log,info,verbose]
                      Change the named log level
    -O <opt>[=<val>]  Set an option for the user code

    -O quiet    Don't call TObject::ls()
    -O list     List events in the file
    -O <option> Set the option for TObject::ls()
 \endverbatim
*/
///
/// 
/// There is skeleton code available in 
/// <a href="../doc/exampleEventLoop.cxx">exampleEventLoop.cxx</a> that can
/// be copied to a local directory and modified.  If you use this code, do a
/// global search and replace on "UserLoop", changing it to some more useful
/// name.  That will help keep the doxygen documentation useful.
/// 
    int nd280EventLoop(int argc, char** argv, 
                        ND::TND280EventLoopFunction& userCode, 
                        int defaultReadCount=0);
}
 
#endif
