#ifndef TFGDChannelId_hxx_seen
#define TFGDChannelId_hxx_seen

#include "TChannelId.hxx"

namespace ND {
    class TFGDChannelId;
};

///Channel ID class for Channels using the AFTER ASIC and the FGD
///electronics hierachy.  Can answer detailed questions about its
///elements.  This should be used for constructing TFGDChannelIds from
///the information available in MIDAS data block.
class ND::TFGDChannelId : public ND::TChannelId {
public:
    ///Construct a FGD channel ID from a raw UInt_t
    explicit TFGDChannelId(UInt_t id =0);

    ///Constructor to specialise a Channel ID as a FGD channel ID 
    explicit TFGDChannelId(const ND::TChannelId& src);

    ///Construct a FGD channel ID from its electronics-space
    ///co-ordinates.  The sub-detector ID should be
    ///ND::TChannelId::kFGD(=3), but is explicitly required for
    ///symmetery with TTfbChannelId. (And in case we have another
    ///detector with FGD electronics.)
    TFGDChannelId(UInt_t subDet, UInt_t minicrate, UInt_t feb, 
                  UInt_t afterChip, UInt_t afterChannel);

    ///Construct a FGD channel ID from its minicrate, feb, feb channel
    ///co-ordinates.  The sub-detector ID should be
    ///ND::TChannelId::kFGD(=3), but is explicitly required for
    ///symmetery with TTfbChannelId. (And in case we have another
    ///detector with FGD electronics.)
    /// We specify the FEB channel only; in the internal conversion
    /// we hardcode that the low attenuation channel is used.
    TFGDChannelId(UInt_t subDet, UInt_t minicrate, UInt_t feb, 
                  UInt_t febCh);

    virtual ~TFGDChannelId();

    ///Format as a human readable string
    virtual std::string AsString() const; 

    ///Get the minicrate for this channel.
    virtual UInt_t GetMinicrate() const;

    ///Get the front end board for this channel.
    virtual UInt_t GetFEB() const;

    /// Get the chip on the front end board.
    virtual UInt_t GetAfterChip() const;

    /// Get the channel of the AFTER asic.
    virtual UInt_t GetChannel() const;

    /// The following methods get information derived from channel id.
    /// Method returns the FEB (ie MPPC) channel, which goes from 0-63 for 
    /// each FEB.
    /// If channel does not correspond to an MPPC channel, 
    /// then returns -1.
    virtual Int_t GetFebChannel() const;

    /// Method returns the attenuation for this MPPC channel.
    /// 0  => low attenuation channel
    /// 1  => high attenuation channel
    /// -1 => Channel does not correspond to an MPPC channel.
    virtual Int_t GetFebChAttenuation() const;

protected:
    /// @{ Define the sub-detector specific part of the channel id.  The
    /// sub-detector specific part of the channel id is a 25 bit field with
    /// bits number from 0 (lsb) to 24 (msb).  The sub-fields are defined
    ///
    /// RRRRRRRRR mmmmmm ff a ccccccc
    ///
    ///    - R(9) Bits 16-24:  A reserved bits.  These must be set to zero.
    ///    - m(6) Bits 10-15: A six bit minicrate index field.
    ///    - f(2) Bit 8-9: A two bit front end board index field.
    ///    - a(1) Bit 7: A one bit field to indicate which after chip.
    ///    - c(7) Bits 0-6: A seven bit field indicating which afer chip
    ///           channel.  
    /// 
    /// Note: The shift and the least significant bit (lsb) must be the same.
    enum BitDefinitions {
        kReserved1MSB=24, kReserved1LSB=16,
        kMinicrateMSB=15, kMinicrateLSB=10,
        kFEBMSB=9, kFEBLSB=8,
        kAfterMSB=7,  kAfterLSB=7,
        kAfterChanMSB=6, kAfterChanLSB=0
    };
    /// @}

private:
    ///Set the minicrate for this channel.
    virtual void SetMinicrate(int val);

    ///Set the front end board for this channel.
    virtual void SetFEB(int val);

    /// Set the chip on the front end board.
    virtual void SetAfterChip(int val);

    /// Get the channel of the AFTER asic.
    virtual void SetChannel(int val);

    ClassDef(TFGDChannelId, 1)
};
#endif //TFGDChannelId_hxx_seen
