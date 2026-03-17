#ifndef TMemoryUsage_hxx_seen
#define TMemoryUsage_hxx_seen
#include <TND280Output.hxx>
#include <typeinfo>
#include <TSystem.h>
#include <vector>

namespace ND {
    class TMemoryUsage;
};

/// This utility class can be used to generate histograms of the memory used
/// by a program.  By default, logging is disabled. Enable by calling
/// Enable().  The expected usage is to call LogMemory() once per event, and
/// Write() once all event have been processed.  This class is used inside the
/// default nd280EventLoop, so it isn't generally used outside of oaEvent.
class ND::TMemoryUsage {
public:
    /// Construct the memory usage class with logging disabled.
    TMemoryUsage();
    ~TMemoryUsage();
    
    /// Called to enable the memory logging.  Enable the logging of memory
    /// usage. If this is not called, LogMemory() and Write() have no effects.
    /// This can be called with a bool argument to set the enabled flag.  If
    /// the argument is false, then memory logging is disabled.
    void Enable(bool enable = true);

    /// Log the current memory usage.  Record the current memory being used by
    /// the process.  If Enable() has not been called, this function does
    /// nothing.  In general, the memory usage should be logged once per
    /// event, but it can be called any number of times.  Logging memory
    /// requires saving two float and two longs into a vector.
    void LogMemory();

    /// Write histograms of the memory usage to an output file.  Write
    /// histograms of the resident and virtual memory usage of the program to
    /// the top-level directory of the output file.  The histograms are
    /// indexed by the number of calls to LogMemory().  If Enable() has not
    /// been called, this function does nothing.
    void Write(ND::TND280Output* output);
    
private:
    /// Whether to log memory usage. Set using Enable()
    bool fEnabled;
    
    /// Record of the memory usage each time LogMemory() is called.
    ///
    /// From TSystem.h:
    ///   struct ProcInfo_t {
    ///     Float_t fCpuUser; // user time used by this process in seconds
    ///     Float_t fCpuSys; // system time used by this process in seconds
    ///     Long_t fMemResident; // resident memory used by this process in KB
    ///     Long_t fMemVirtual; // virtual memory used by this process in KB
    ///   }
    std::vector<ProcInfo_t> fEventMemory;
};
#endif

