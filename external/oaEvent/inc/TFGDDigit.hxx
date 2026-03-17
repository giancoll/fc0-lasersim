#ifndef TFGDDigit_hxx_seen
#define TFGDDigit_hxx_seen

#include <vector>
#include <TROOT.h>

#include <TChannelId.hxx>
#include <TDigit.hxx>

namespace ND {
    class TFGDDigit;
};

/// Digit for FGD based detectors.  This holds the actual digitization data
/// for a FGD channel.
class ND::TFGDDigit : public TDigit {
public:
    TFGDDigit ();
    virtual ~TFGDDigit();

    /// Construct FGD digit.
    TFGDDigit(ND::TChannelId chan, 
              short startTime, short pulseTime, short pulseCharge, 
              std::vector<unsigned short> adcs,
              short dataType);

    /// Get the first time bin
    short GetFirstTimeSample() const {return fFirstTimeSample;}

    /// Get the time of pulse
    short GetPulseTime() const {return fPulseTime;}

    /// Get the charge of pulse
    short GetPulseCharge() const {return fPulseCharge;}

    // number of time bins in this digit
    int GetNumberOfTimeSamples() const {return fADCs.size();}

    /// Get the ADC value for a specific time bin
    int GetADC(unsigned int t) const{
        // Check bounds
        if (t<fADCs.size())  
            return fADCs[t];
        else return -1;  
    }

    /// vector of ADC counts
    std::vector<unsigned short> GetADCs() const {return fADCs;}

    /// Get the type of this FGD digit
    short GetDataType() const {return fDataType;}

private: 

    /// the first time bin of the pulse
    short fFirstTimeSample;    

    /// the pulse time
    short fPulseTime; 

    /// the pulse charge
    short fPulseCharge; 

    /// vector of ADC counts 
    std::vector<unsigned short > fADCs; 

    /// Data type
    /// 0 -> Full Waveform.
    /// 1 -> Compressed waveform; charge, time + samples around pulse.
    /// 2 -> Compressed waveform; just charge and time.
    short fDataType;

    ClassDef(TFGDDigit,2);
};
#endif
