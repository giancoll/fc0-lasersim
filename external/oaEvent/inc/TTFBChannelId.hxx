#ifndef TTFBChannelId_hxx_seen
#define TTFBChannelId_hxx_seen

#include "TChannelId.hxx"

namespace ND {
    class TTFBChannelId;
}

///Channel ID class for Channels using the TripT ASIC and the TFB
///electronics hierachy.  Can answer detailed questions about its
///elements.  This should be used for constructing TTFBChannelIds from
///the information available in MIDAS data block.
class ND::TTFBChannelId : public ND::TChannelId {
public:
    ///Construct a TFB channel ID from a raw UInt_t
    explicit TTFBChannelId(UInt_t id = 0);

    ///Constructor to specialise a Channel ID as a TFB channel ID 
    explicit TTFBChannelId(const ND::TChannelId& src);

    ///Construct a TFB channel ID from its electronics-space co-ordinates
    TTFBChannelId(UInt_t subDet,  UInt_t rmm,  UInt_t tfb,
                  UInt_t tripChip, UInt_t tripChannel, 
                  UInt_t capacitor = 24);

    virtual ~TTFBChannelId();

    ///Format as a human readable string
    virtual std::string AsString() const; 

    ///Return a new TTFBChannelId representing the channnel independent of
    ///capacitor. The new TTFBchannelId will have 24 in the capacitor
    ///field. The original TTFBChannelId is unchanged.  This localizes the
    ///channel id to a particular channel on a trip chip.
    virtual TTFBChannelId IgnoreCapacitor() const;

    /// Return a new TTFBChannelId representing the channel independent of the
    /// channel number of the trip chip.  This localizes the channel to a
    /// particular trip chip on a particular TFB.
    virtual TTFBChannelId IgnoreChannel() const;

    /// Return a new TTFBChannelId representing the channel independent of the
    /// trip chip on the TFB.  This localizes the channel to a particular TFB.
    virtual TTFBChannelId IgnoreTripChip() const;

    /// Return a new TTFBChannelId representing the channel independent of the
    /// TFB on the RMM.  This localizes the channel to a
    /// particular RMM.
    virtual TTFBChannelId IgnoreTFB() const;

    /// Get the RMM number for this channel.
    virtual UInt_t GetRMM() const;

    /// Get the port number on the RMM for this TFB.  This is not the TFB
    /// serial number, and will change to a different TFB if the RMM is cables
    /// are changed.
    virtual UInt_t GetTFB() const;

    /// Get the capacitor on the Trip-T for this channel.  There are 23
    /// different capacitors, one for each integration cycle.  The capacitors
    /// are numbered zero to 22.
    virtual UInt_t GetCapacitor() const;

    /// Get the Trip-T chip.  There are four chips per TFB board.
    virtual UInt_t GetTripChip() const;

    /// Get the channel on the Trip-T.  There are 16 channels on a Trip-T, and
    /// each channel has 23 capacitors (or integration cycles).
    virtual UInt_t GetChannel() const;

    /// Get the cable identifier as an integer.  This uniquely identifies the
    /// cable connected to a TFB, but groups all of the capacitors together.
    /// This will have a one to one correspondence with an MPPC.
    virtual UInt_t GetCableId() const;

    /// Get the TripT identifier as an integer.  This uniquely identifies the
    /// tript chip, but groups all of the tript channels and capacitors
    /// together.
    virtual UInt_t GetTripTId() const;

protected:

    /// @{ Define the sub-detector specific part of the channel id.  The
    /// sub-detector specific part of the channel id is a 25 bit field with
    /// bits number from 0 (lsb) to 24 (msb).  The sub-fields are defined
    ///
    /// R rrrrr R TTTTTT CCCCC tt ccccc
    ///
    ///    - R(1) Bit 24:  A reserved bit.  This must be set to zero.
    ///    - r(5) Bits 19-23: A five bit RMM index field.
    ///    - R(1) Bit 18: A reserved bit.  This must be set to zero.
    ///    - T(6) Bits 12-17: A six bit field identifying the TFB port 
    ///           on the RMM board.  
    ///    - C(5) Bits 7-11: A five bit field identifying the capacitor 
    ///           on the trip-t.
    ///    - t(2) Bits 5-6: A two bit field identifying the trip-t chip.
    ///    - c(5) Bits 0-4: A 5 bit field identifying the trip-t channel.
    /// 
    /// Note: The shift and the least significant bit (lsb) must be the same.
    enum BitDefinitions {
        kReserved1MSB    = 24,    kReserved1LSB    = 24,
        kRMMBoardMSB     = 23,    kRMMBoardLSB     = 19,
        kReserved2MSB    = 18,    kReserved2LSB    = 18,
        kTFBPortMSB      = 17,    kTFBPortLSB      = 12,
        kCapacitorMSB    = 11,    kCapacitorLSB    = 7,
        kTripTMSB        = 6,     kTripTLSB        = 5,
        kTripTChannelMSB = 4,     kTripTChannelLSB = 0
    };
    /// @}

private:		
    /// Set the RMM number for this channel.
    virtual void SetRMM(int val);

    /// Set the port number on the RMM for this TFB.  This is not the TFB
    /// serial number, and will change to a different TFB if the RMM cables
    /// are changed.
    virtual void SetTFB(int val);

    /// Set the capacitor on the Trip-T for this channel.  There are
    /// 23 different capacitors.  In spill mode the capacitor number
    /// should match the cycle number but this is not true in other
    /// modes, notably for cosmics. The capacitors are numbered zero to 22.
    virtual void SetCapacitor(int val);

    /// Set the Trip-T chip.  There are four chips per TFB board.
    virtual void SetTripChip(int val);

    /// Set the channel on the Trip-T.  There are 16 connected
    /// channels on a Trip-T (numbered 0 to 15), plus 2 unconnected
    /// channels. Each channel has 23 capacitors (or integration
    /// cycles).
    virtual void SetChannel(int val);

    ClassDef(TTFBChannelId, 1)
};
#endif //TTFBChannelId_hxx_seen
