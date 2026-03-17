#ifndef TTOFDigit_hxx_seen
#define TTOFDigit_hxx_seen

#include <vector>
#include <TROOT.h>

#include <TChannelId.hxx>
#include <TDigit.hxx>

namespace ND {
    class TTOFDigit;
};

/// Digit for the TOF detector.  This holds the actual digitization data
/// for a TOF channel.
class ND::TTOFDigit : public TDigit {
public:
    TTOFDigit ();
    virtual ~TTOFDigit();

    /// Construct a digit for a particular channel and cycle number.
    TTOFDigit(ND::TChannelId chan, double cell0time, std::vector<float> waveform, float timeOverThreshold);

    /// Get the first time bin
    double GetCell0Time() const;

    // number of time bins in this digit
    int GetNumberOfSamples() const;

    float GetTimeOverThreshold() const;

    /// Get the ADC value for a specific time bin
    float GetWfSample(unsigned int t) const;

    /// vector of ADC counts
    std::vector<float> GetWaveform() const;

    /// Print the digit information.
    virtual void ls(Option_t* opt = "") const;
    
    /// Parameters that refer to the electronics, the SAMPIC
    /// written like this so they can be accessed without defining a TTOFDigit object
    static constexpr int NWaveformSamplesToRead = 64;
    static constexpr int NWaveformSamplesToExclude = 1;  // the first sample in a waveform is not to be used
    static constexpr int NWaveformValidSamples = 63;     // the first sample is not valid, it is excluded at calibration. Other variable for simplicity
    static constexpr double SampleLength = 0.3125;       // ns, in default conditions

private: 

    /// the first time bin of the pulse
    double fCell0Time {0.}; // this needs to be a double here, necessarily. Timestamps are long.

    /// vector of waveform samples
    std::vector<float> fSamples {};

    /// Time over threshold
    float fTimeOverThreshold {0.};

    
    ClassDef(TTOFDigit,2); // updated to 2 in May '24
};
#endif