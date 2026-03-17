#ifndef TND280Context_hxx_seen
#define TND280Context_hxx_seen

#include <TROOT.h>
#include <TObject.h>

namespace ND {
    class TND280Context;
}

/// Define the context of an event.  The event context uniquely identifies an
/// event within T2K (ND280).  Due to the way events are formed, not all event
/// (or slow control data) will have all fields of the context information
/// defined.  Fields that do not contain valid data should be marked using the
/// value ND::TND280Context::Invalid.
class ND::TND280Context {
public:
    typedef unsigned int Time;
    
    /// A special field value that specifies that the field is invalid.
    static const int Invalid;
    
    /// Construct an empty context.
    TND280Context();

    /// Construct a context.
    TND280Context(int partition, 
                  int run, int subRun, int event, 
                  int spill, int stamp);

    virtual ~TND280Context();

    enum {
        /// DAQ running in global mode.
        kGlobalPartition       = 1,

        /// The P0D clock module bank is present in the event.  If not running
        /// in global mode, this in P0D local mode.
        kP0DClockPresent       = 1<<1,

        /// The TPC clock module bank is present in the event.  If not running
        /// in global mode, this in TPC local mode.
        kTPCClockPresent       = 1<<2,

        /// The FGD clock module bank is present in the event.  If not running
        /// in global mode, this in FGD local mode.
        kFGDClockPresent       = 1<<3,

        /// The ECal clock module bank is present in the event.  If not running
        /// in global mode, this in ECal local mode.
        kECalClockPresent      = 1<<4,

        /// The SMRD clock module bank is present in the event.  If not running
        /// in global mode, this in SRMD local mode.
        kSMRDClockPresent      = 1<<5,

        /// The INGRID clock module bank is present in the event.
        kINGRIDClockPresent    = 1<<6,

        /// This context is for MC data.  If this is detector data, then this
        /// bit will be 0.  If this is MC data, then this bit will be 1.
        kMCData           = 1<<16
    };

    /// Return the partition for this context.  The partition code provides a
    /// quick way to identify the general contents of the event.
    ///
    /// \note The partition value must first be checked to make sure it is
    /// valid before the individual bits are checked.
    ///
    /// \code
    /// if (context.GetPartition() != ND::TND280Context::Invalid
    ///     && (context.GetPartition & ND::TND280Context::kGlobalPartition)) {
    ///     // The data is for the global partition.
    /// }
    /// \endcode
    int GetPartition() const;

    /// Add a convenient check to see if this is MC data.  Note that if the
    /// context partition is invalid, this will return false.
    bool IsMC() const;
    
    /// Add a convenient check to see if this is detector data.  Note that if
    /// the context partition is invalid this will return false.
    bool IsDetector() const;

    /// Set the partition for this context.
    void SetPartition(int p);

    /// Return the run number for this context.
    int GetRun() const;

    /// Set the run number for this context.
    void SetRun(int r);

    /// Return the sub-run number for this context.
    int GetSubRun() const;

    /// Set the sub-run number for this context.
    void SetSubRun(int s);

    /// Return the event number for this context.
    int GetEvent() const;

    /// Set the event number for this context.
    void SetEvent(int e);

    /// Return the spill number for this context.
    int GetSpill() const;

    /// Set the spill number for this context
    void SetSpill(int s);
    
    /// Return the time stamp for this event (only valid to 1 sec)
    int GetTimeStamp() const;

    /// Return the time stamp for this event as a MYSQL compatible time string. 
    std::string GetTimeStampString() const;

    /// Set the time stamp for this event.
    void SetTimeStamp(int t);

    /// Return true if the context is valid.  A valid context is defined as a
    /// context where one or more fields is valid.
    bool Valid() const;

private:
    /// The partition associated with this context
    int fPartition;

    /// The run number associated with this context
    int fRun;

    /// The sub-run number associated with this context
    int fSubRun;

    /// The event number associated with this context.
    int fEvent;

    /// The spill number associated with this context.
    int fSpill;

    /// The unix time stamp associated with this context
    Time fTimeStamp;

    ClassDef(TND280Context,1);
};

std::ostream& operator<<(std::ostream& s, const ND::TND280Context& c);
#endif
