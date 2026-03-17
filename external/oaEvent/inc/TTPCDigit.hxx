#ifndef TTPCDigit_hxx_seen
#define TTPCDigit_hxx_seen

#include <vector>
#include <TROOT.h>

#include <TChannelId.hxx>
#include <TDigit.hxx>

namespace ND {
    class TTPCDigit;
};

/// Digit for TPC based detectors.  This holds the actual digitization data
/// for a TPC channel.
class ND::TTPCDigit : public TDigit {
public:
    TTPCDigit ();
    virtual ~TTPCDigit();

    /// Construct a digit for a particular channel and cycle number.
    TTPCDigit(ND::TChannelId chan, short time, std::vector<short> adcs);

    /// Get the first time bin
    int GetFirstTimeSample() const;

    // number of time bins in this digit
    int GetNumberOfTimeSamples() const;

    /// Get the ADC value for a specific time bin
    int GetADC(unsigned int t) const;

    /// vector of ADC counts
    std::vector<short> GetADCs() const;

    /// Print the digit information.
    virtual void ls(Option_t* opt = "") const;
    
private: 

    /// the first time bin of the pulse
    short fFirstTimeSample;    

    /// vector of ADC counts 
    std::vector<short> fADCs;

    ClassDef(TTPCDigit,1);
};
#endif
