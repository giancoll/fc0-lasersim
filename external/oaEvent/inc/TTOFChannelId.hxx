#ifndef TTOFChannelId_hxx_seen
#define TTOFChannelId_hxx_seen

#include "TChannelId.hxx"

namespace ND {
    class TTOFChannelId;
};

/// Channel Identifier class for channels using the TOF electronics hierachy.
/// This should be used for constructing TTOFChannelIds from the information
/// available in MIDAS data block.
class ND::TTOFChannelId : public ND::TChannelId {
public:
    /// Construct a TOF channel ID from a raw UInt_t
    explicit TTOFChannelId(UInt_t id = 0); 
    // this in inherited from TChannelId and should be fine already, this would be a channel number from 0 to 255 for us

    /// Constructor to specialise a Channel ID as a TOF channel ID
    explicit TTOFChannelId(const ND::TChannelId& src);

    /// Construct a TOF channel ID from the MC electronics channel space.
    /// This exists before the electronics are defined, and probably won't be
    /// used after we have electronics.  It's closely related to the bar
    /// geometry identifiers.  Specifically, this has been used for the first
    /// implementation of TOF electronics simulation.  The end must be -1 or
    /// +1, anything else is an error.  The initial "flag" is to make sure
    /// this won't collide with the real electronics based id, and it should
    /// have a value of NULL.
    TTOFChannelId(char* flag, UInt_t subDet, UInt_t mod, UInt_t bar, Int_t end); // end can be negative

    // Construct a ToF channel ID from its feb id, Sampic ID and Channel.
    // TTOFChannelId(UInt_t subDet, Int_t feb, Int_t sampic, Int_t channel);

    virtual ~TTOFChannelId();

    // this is a local function that can be called from anywhere
    // it returns the channel number from 0 to 255
    static UInt_t GetLocalChannelId(int febIndex_, int sampicIndex_, int channelIndex_);
    
    // this returns the channel number from 0 to 255 from the class variables
    // it is not the same as AsUInt, which returns the global ND280 channel number
    virtual UInt_t GetLocalChannelId() const;

    // this is a function to access the channel number from the class, using the class variables
    // this is the equivalent of TChannelId::AsUInt(), than can't be ovverriden
    // this is the channel number from 0 to 255
    // virtual UInt_t GetChannelIdAsUInt(){
    //     UInt_t feb = GetFeb();
    //     UInt_t sampic = GetSampic();
    //     UInt_t channel = GetChannel();
    //     return GetChannelIdAsUInt(feb, sampic, channel); 
    // };

    /// Format as a human readable string
    virtual std::string AsString() const;

    // these are indexes 
    virtual int GetFeb() const;
    virtual int GetSampic() const;
    virtual int GetChannel() const;

    // defining this virtual function of the base class
    // virtual UInt_t AsUInt() const; 

    // I don't put GetModule and GetBar here, since I think they should just come from TOFChannelMap ~EV
    // not sure though

    /// Check if this is a MC style of channel identifier.  The MC can also
    /// use a real electronics channel identifier, but this exists to have
    /// channel identifiers before the electronics is defined.
    virtual bool IsMC() const;

    /// Get the module for the channel identifier (XZ, YZ, or XY).  The MC
    /// can also use a real electronics channel identifier, but this exists to
    /// have channel identifiers before the electronics is defined.
    virtual int GetMCModule() const;

    /// Get the bar the channel.  The MC can also use a real
    /// electronics channel identifier, but this exists to have channel
    /// identifiers before the electronics is defined.
    virtual int GetMCBar() const;

    /// Get the end of the channel.  The MC can also use a real
    /// electronics channel identifier, but this exists to have channel
    /// identifiers before the electronics is defined.
    virtual int GetMCEnd() const;

    /// These are variables to compute the number of channels. Total is 256
    constexpr static int NFebs{4};
    constexpr static int NSampicsPerFeb{4};
    constexpr static int NChannelsPerSampic{16};

protected:

    /// @{ Define the sub-detector specific part of the channel id. The
    /// sub-detector specific part of the channel id is a 25 bit field with
    /// bits number from 0 (lsb) to 24 (msb).  This has two "branches" to
    /// accomodate a MC specific channel identifer, as well as a real channel
    /// identifier.  The sub-fields are defined
    ///
    /// VV FFFFFFFFFFFFFFFFFFFFFFF
    ///
    ///   - VV(2) Bits 23,24:  A version.  Zero is for the MC.
    ///   - F(23) Bits 0-22: Bits for the actual channel identification.
    ///
    /// For the MC
    ///
    /// 00 RRRRRRRRR ee mmmm bbbbbbbb
    ///
    ///   - 0(2)  Bits 23,24:  Must be zero.
    ///   - R(9) Bits 12-22: Reserved.  Must be zero.
    //    - e(2) Bits 12-13: Bits for the bar end (1 is negative, 2 is positive)
    ///   - m(4) Bits 8-11: Bits for the module
    ///   - b(8) Bits 0-7: Bits for the bar in the module.
    ///
    /// For the electronics (TBD)
    ///
    /// XX RRRRRRRRRRRRRRRRRRRRRRR
    ///
    /// The value of XX needs to be defined!  (but can't be zero).
    ///
    enum BitDefinitions {
        kVersionMSB=24, kVersionLSB=23,
        // The bits for the MC. 
        kMCReservedMSB=22, kMCReservedLSB=10,
        kMCModuleMSB=9, kMCModuleLSB=7, // module goes from 0 to 5
        kMCBarMSB=6, kMCBarLSB=2, // bar goes from 0 to 19
        kMCEndMSB=1, kMCEndLSB=0, // end is 0 or 1, that is read as -1 and +1 elswehere

        // The bits for the electronics
        kReservedMSB=22, kReservedLSB=9,
        kFebMSB=8, kFebLSB=6, // feb from 0 to 3. This is one bit more than it's needed, just in case we add more FEBs one day
        kSampicMSB=5, kSampicLSB=4, // sampic goes from 0 to 3
        kChannelMSB=3, kChannelLSB=0 // channel goes from 0 to 15
    };
    /// @}

    ClassDef(TTOFChannelId, 1)
};
#endif //TTOFChannelId_hxx_seen
