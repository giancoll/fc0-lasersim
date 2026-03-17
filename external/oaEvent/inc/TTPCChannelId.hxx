#ifndef TTPCChannelId_hxx_seen
#define TTPCChannelId_hxx_seen

#include "method_deprecated.hxx"
#include "TChannelId.hxx"

namespace ND {
    class TTPCChannelId;
};

///Channel ID class for Channels using the AFTER ASIC and the TPC
///electronics hierachy.  Can answer detailed questions about its
///elements.  This should be used for constructing TTPCChannelIds from
///the information available in MIDAS data block.
class ND::TTPCChannelId : public ND::TChannelId {
public:
    ///Construct a TPC channel ID from a raw UInt_t
    explicit TTPCChannelId(UInt_t id = 0);

    ///Constructor to specialise a Channel ID as a TPC channel ID 
    explicit TTPCChannelId(const ND::TChannelId& src);

    ///Construct a TPC channel ID from its electronics-space
    ///co-ordinates.  The sub-detector identifier is ND::TChannelId::kTPC(=2).
    TTPCChannelId(UInt_t dcc, UInt_t fem, UInt_t fec, 
                  UInt_t asic, UInt_t channel);

#ifdef USE_DEPRECATED_TTPCChannelId_Constructor
#warning Useing deprecated TTPCChannelIdConstructor.
    /// temporary geom representation (until we get FEM and DCC from header.
    /// \deprecated
    TTPCChannelId(UInt_t tpc, UInt_t mm, UInt_t channel) METHOD_DEPRECATED;
#endif

    virtual ~TTPCChannelId();

    ///Format as a human readable string.
    virtual std::string AsString() const; 

    /// Get the DCC number for this channel [0-17].
    virtual UInt_t GetDCC() const;

    /// Get the FEM number for this channel [0-11].
    virtual UInt_t GetFEM() const;
    
    /// Get the FEC number for this channel [0-5].
    virtual UInt_t GetFEC() const;

    /// Get the Asic on the FEC for this channel [0-3].
    virtual UInt_t GetAsic() const;

    /// Get the channel on the ASIC [0-79].
    virtual UInt_t GetChannel() const;

#ifdef USE_DEPRECATED_TPCCode
    /// @{ Temporary while the TPC DAQ code is debugged.
    /// \deprecated
    virtual UInt_t GetTPC() const METHOD_DEPRECATED;
    virtual UInt_t GetMM() const METHOD_DEPRECATED;
    virtual UInt_t GetMMChannel() const METHOD_DEPRECATED;
    /// @}
#endif

protected:
    /// @{ Define the sub-detector specific part of the channel id.  The
    /// sub-detector specific part of the channel id is a 25 bit field with
    /// bits number from 0 (lsb) to 24 (msb).  The sub-fields are defined
    ///
    /// RRRR ddddd mmmm fffa accc cccc
    ///
    ///    - R(4) Bits 21-24: Reserved bits.  These must be set to zero.
    ///    - d(5) Bits 16-20: The DCC index field (18 DCC)
    ///    - m(4) Bits 12-15: The FEM index field (12 FEM/DCC)
    ///    - f(3) Bits 9-11: The FEC index field (6 FEC/DCC)
    ///    - a(2) Bits 7-8: The ASIC index field (4 ASIC/FEC)
    ///    - c(7) Bits 0-6: The channel on the ASIC (80 channels/ASIC)
    /// 
    /// Note: The shift and the least significant bit (lsb) must be the same.
    enum BitDefinitions {
        kReserved1MSB=24, kReserved1LSB=21,
        kDCCMSB=20, kDCCLSB=16,
        kFEMMSB=15, kFEMLSB=12,
        kFECMSB=11,  kFECLSB=9,
        kAsicMSB=8, kAsicLSB=7,
        kChanMSB=6, kChanLSB=0
    };
    /// @}

    /// @{ Temporary bit definitions until the FEM and DCC are available from
    /// the header.
    ///
    ///   RRRRR tttt mmmm cccc cccc cccc
    ///
    ///    - R(5) Bits 20-24: Reserved bits.  These must be set to zero.
    ///    - t(4) Bits 16-19: The TPC index field (6 TPC)
    ///    - m(4) Bits 12-15: The micromega index field (12 MM/TPC)
    ///    - c(12) Bits 0-11: The channel on the ASIC (1728 channels/MM)
    /// 
    /// Note: The shift and the least significant bit (lsb) must be the same.
    enum DeprecatedDefinitions {
        kReserved2MSB=24, kReserved2LSB=20,
        kTPCMSB=19, kTPCLSB=16,
        kMMegaMSB=15, kMMegaLSB=12,
        kMMChanMSB=11, kMMChanLSB=0
    };
    /// @}

private:
    /// Get the DCC number for this channel.
    virtual void SetDCC(int val);

    /// Set the FEM number for this channel.
    virtual void SetFEM(int val);

    /// Set the FEC number for this channel.
    virtual void SetFEC(int val);

    /// Set the ASIC number for this channel.
    virtual void SetAsic(int val);

    /// Set the channel on the ASIC.
    virtual void SetChannel(int val);

#ifdef USE_DEPRECATED_TPCCode
    /// @{ Temporary while the TPC DAQ code is debugged.
    /// \deprecated
    virtual void SetTPC(int val) METHOD_DEPRECATED;
    virtual void SetMM(int val) METHOD_DEPRECATED;
    virtual void SetMMChannel(int val) METHOD_DEPRECATED;
    /// @}
#endif

    ClassDef(TTPCChannelId, 1)
};
#endif //TTPCChannelId_hxx_seen
