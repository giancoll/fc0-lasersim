#ifndef TTFBDigit_hxx_seen
#define TTFBDigit_hxx_seen

#include <vector>
#include <TROOT.h>

#include <TChannelId.hxx>
#include <TDigit.hxx>

namespace ND {
    class TTFBDigit;
};

/// Digit for TFB based detectors.  This holds the actual digitization data
/// for a TFB channel.
class ND::TTFBDigit : public TDigit {
public:
    TTFBDigit ();
    virtual ~TTFBDigit();

    /// Construct a digit for a particular channel and cycle number.
    TTFBDigit(ND::TChannelId chan, 
              int highGainADC,
              int lowGainADC, 
              unsigned int timeCounter,
              int timeQuad,
              bool timeDiscriminator);

    /// Get the high gain ADC value.  Only the low twelve bits of this value
    /// are used.
    short int GetHighGainADC() const {return fHighGainADC;}

    /// Get the low gain ADC value.  Only the low twelve bits of this value
    /// are used.
    short int GetLowGainADC() const {return fLowGainADC;}

    /// Get the 32 bit TFB time counter running with a 10ns tick.
    unsigned int GetTimeCounter() const {return fTimeCounter;}

    /// Get the 2 bit TFB time quad value.  This counts the edges of the 32
    /// bit clock (2.5ns precision).
    unsigned int GetTimeQuad() const {return fBits & 0x3;}

    /// Get the full TDC value (2.5ns tick).  This is a combination of the
    /// 10ns tick and the 2.5 ns tick.
    ULong64_t GetTDC() const {return 4LL*GetTimeCounter() + GetTimeQuad();}

    /// Check that the discriminator threshold was reached for this digit.  If
    /// the discriminator threshold was not reached, the time counter returns
    /// the time of the beginning of the cycle.
    bool CheckTimeDiscriminator() const {return (fBits & 0x04) != 0;}

    /// Check if the TDC value is good for this digit.  Good in this case
    /// means that the charge went over the TDC discriminator level, so this
    /// is a synonym for CheckTimeDiscriminator().
    bool HasGoodTDC() const {return CheckTimeDiscriminator();}

    /// Print the digit information.
    virtual void ls(Option_t* opt = "") const;
    
private: 
    /// The high gain ADC information from the TFB.  This only uses the low 12
    /// bits.
    short int fHighGainADC;

    /// The low gain ADC information from the TFB.  This only uses the low 12
    /// bits.
    short int fLowGainADC;

    /// The 32 bit time counter on the TFB.  This is the latched 100MHz clock
    unsigned int fTimeCounter;

    /// The other bits in the digit.
    unsigned char fBits;

    ClassDef(TTFBDigit,1);
};
#endif
