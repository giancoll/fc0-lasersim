#ifndef THATChannelId_hxx_seen
#define THATChannelId_hxx_seen

#include "method_deprecated.hxx"
#include "TChannelId.hxx"

namespace ND {
    class THATChannelId;
};

///Channel ID class for Channels using the AFTER ASIC and the HA-TPC
///electronics hierarchy.  Can answer detailed questions about its
///elements.  This should be used for constructing THATChannelIds from
///the information available in MIDAS data block.
class ND::THATChannelId : public ND::TChannelId {
public:
    ///Construct a HAT channel ID from a raw UInt_t
    explicit THATChannelId(UInt_t id = 0);

    ///Constructor to specialise a Channel ID as a HAT channel ID 
    explicit THATChannelId(const ND::TChannelId& src);

    ///Construct a HAT channel ID from its electronics-space
    ///co-ordinates.  The sub-detector identifier is ND::TChannelId::kHAT(=10).
    THATChannelId(UInt_t hat, UInt_t plate, UInt_t fem, UInt_t fec, 
                  UInt_t asic, UInt_t channel);

    virtual ~THATChannelId();

    ///Format as a human readable string.
    virtual std::string AsString() const; 

    /// Get the HAT number for this channel [0-1].
    virtual UInt_t GetHAT() const;

    /// Get the Plate number for this channel [0-1].
    virtual UInt_t GetPlate() const;

    /// Get the FEM number for this channel [0-7].
    virtual UInt_t GetFEM() const;
    
    /// Get the FEC number for this channel [0-1].
    virtual UInt_t GetFEC() const;

    /// Get the Asic on the FEC for this channel [0-7].
    virtual UInt_t GetAsic() const;

    /// Get the channel on the ASIC [0-71].
    virtual UInt_t GetChannel() const;

protected:
    
    /// @{ Define the sub-detector specific part of the channel id. 
    /// Suggested bits from D. Calvet and C. McGrew:
    ///    - V (23-24): 2 bits for choosing electronic/DAQ version (right now only one version)
    ///    - R (16-22): 7 bits for reserved bits. These must be set to zero.
    ///    - T (15): 1 bit for choosing the HA-TPC (2) (bot one is 0 and top one is 1)
    ///    - E (14): 1 bit for choosing a HA-TPC end plate (2)
    ///    - M (11-13): 3 bits to identify a FEM (8 per end-plate)
    ///    - C (10): 1 bit to identify a FEC (2 per FEM)
    ///    - A (7-9): 3 bits to identify a Asic chip (8 per FEC)
    ///    - c (0-6): 7 bits to encode the channel (72 pads per chip)
    /// So 25 bits total
    /// 
    /// VVRRR RRRR TEMM MCAA Accc cccc
    enum BitDefinitions {
        kVersionMSB=24, kVersionLSB=23,
        kReservedMSB=22, kReservedLSB=16,
        kHATMSB=15, kHATLSB=15,
        kPlateMSB=14, kPlateLSB=14,
        kFEMMSB=13, kFEMLSB=11,
        kFECMSB=10,  kFECLSB=10,
        kAsicMSB=9, kAsicLSB=7,
        kChanMSB=6, kChanLSB=0
    };
    /// @}

private:
    /// Get the HAT number for this channel.
    virtual void SetHAT(int val);

    /// Set the Plate number for this channel.
    virtual void SetPlate(int val);

    /// Set the FEM number for this channel.
    virtual void SetFEM(int val);

    /// Set the FEC number for this channel.
    virtual void SetFEC(int val);

    /// Set the ASIC number for this channel.
    virtual void SetAsic(int val);

    /// Set the channel on the ASIC.
    virtual void SetChannel(int val);

    ClassDef(THATChannelId, 1)
};
#endif //THATChannelId_hxx_seen
