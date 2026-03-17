#ifndef TSFGChannelId_hxx_seen
#define TSFGChannelId_hxx_seen

#include "TChannelId.hxx"

namespace ND {
    class TSFGChannelId;
};

/// Channel ID class for channels using the citiroc ASIC and the SFG
/// electronics hierachy.  This should be used for constructing TSFGChannelId
/// objects from the information available in MIDAS data block.
class ND::TSFGChannelId : public ND::TChannelId {
public:
    /// Construct a SFG channel ID from a raw UInt_t
    explicit TSFGChannelId(UInt_t id =0);

    /// Constructor to specialise a Channel ID as a SFG channel ID
    explicit TSFGChannelId(const ND::TChannelId& src);

    /// Construct a SFG channel ID from its OCB, FEB, and channel coordinates.
    /// The sub-detector ID should be ND::TChannelId::kSFG(=9), but is
    /// explicitly required for symmetery with TTFBChannelId and TFGDChannelId
    /// (and in case we have another detector with SFGD electronics). The
    /// channel id type must be 01.  The ocb specifies the crate, and the
    /// feb specifies the front end board within a crate.  These are
    /// equivalent to the nibbles in the board id (4 MSB are the OCB, and the
    /// 4 LSB are the slot).  The channel is the citiroc channel within one
    /// FEB (3 MSB are the CITIROC on the FEB, and 5 LSB are the 32 channels
    /// on each citiroc).  This corresponds to the "channel id" in the hit
    /// timing/amplitude words on in the FEB Protocol.
    TSFGChannelId(UInt_t subDet, UInt_t typeId, UInt_t ocb,
                  UInt_t feb, UInt_t channel);

    /// Construct a SFG channel ID from its board id and FEB channel
    /// coordinates.  The sub-detector ID should be ND::TChannelId::kSFG(=9),
    /// but is explicitly required for symmetery with TTFBChannelId and
    /// TFGDChannelId (and in case we have another detector with SFGD
    /// electronics). The channel id type will be set to 01. The boardId is the
    /// value of the board id in the gate open word and specifies the crate,
    /// and the feb specifies the front end board within a crate (the 4 MSB
    /// are the OCB, and the 4 LSB are the slot).  The channel is the citiroc
    /// channel within one FEB (3 MSB are the CITIROC on the FEB, and 5 LSB
    /// are the 32 channels on each citiroc).  This corresponds to the
    /// "channel id" in the hit timing/amplitude words on in the FEB Protocol.
    TSFGChannelId(UInt_t subDet, UInt_t boardId, UInt_t channel);

    /// Construct a SFG channel ID from the MC electronics channel space.
    /// This exists before the electronics are defined, and probably won't be
    /// used after we have electronics.  It's closely related to the fiber
    /// geometry identifiers.  Specifically, this has been used for the first
    /// implementation of SFG electronics simulation.
    TSFGChannelId(UInt_t subDet, UInt_t projection, UInt_t u, UInt_t v);

    /// Construct a SFG channel
    virtual ~TSFGChannelId();

    /// Format as a human readable string
    virtual std::string AsString() const;

    /// Get the OCB number (also called crate ID) for this channel (4 bits).
    /// This is only valid for a channel identifier type "01".
    virtual int GetOCB() const;

    /// Get the FEB slot (number within crate) for this channel (4 bits, 0-13).
    /// This is only valid for a channel identifier with type "01".
    virtual int GetFEBSlot() const;

    /// Get the board id for this channel (8 bits).  This is only valid for a channel
    /// identifier with type "01".
    virtual int GetBoard() const;

    /// Get the citiroc channel on the FEB (8 bits).  This is called "channel
    /// identifier" in the FEB documentation, but NOT called that here to
    /// avoid confusion between differen meanings of channel.
    virtual int GetFEBChannel() const;

    /// Get the citiroc on the FEB (3 bits).
    virtual int GetCITIROC() const;

    /// Get the channel on a single citiroc (5 bits).
    virtual int GetCITIROCChannel() const;

    /// The type of the channel identifier.  The values are "0" for a
    /// channel identifier using the projection and cube XYZ (also called an
    /// MC identifier), and "1" for a channel identifier specifying the OCB,
    /// FEB and citiroc index.
    virtual int GetType() const;

    /// Check if this is a MC style of channel identifier.  The MC can also
    /// use a real electronics channel identifier, but this exists to have
    /// channel identifiers before the electronics is defined.
    virtual bool IsMC() const;

    /// Get the projection for the channel identifier (XZ, YZ, or XY).  The MC
    /// can also use a real electronics channel identifier, but this exists to
    /// have channel identifiers before the electronics is defined.
    virtual int GetMCProjection() const;

    /// Get the "U" coordinate of the fiber.  The MC can also use a real
    /// electronics channel identifier, but this exists to have channel
    /// identifiers before the electronics is defined.
    virtual int GetMC_U() const;

    /// Get the "V" coordinate of the fiber.  The MC can also use a real
    /// electronics channel identifier, but this exists to have channel
    /// identifiers before the electronics is defined.
    virtual int GetMC_V() const;

protected:
    /// @{ Define the sub-detector specific part of the channel id. The
    /// sub-detector specific part of the channel id is a 25 bit field with
    /// bits number from 0 (lsb) to 24 (msb).  This has two "branches" to
    /// accomodate a MC specific channel identifer, as well as a real channel
    /// identifier.  The sub-fields are defined
    ///
    /// TT FFFFFFFFFFFFFFFFFFFFFFF
    ///
    ///   - TT(2) Bits 23,24:  The channel id type.  The value "00" is for
    ///            the MC and "01" is for the electronics.
    ///   - F(23) Bits 0-22: Bits for the actual channel identification.
    ///
    /// For the MC
    ///
    /// 00 RRRRR pp uuuuuuuu vvvvvvvv
    ///
    ///   - 0(2) Bits 23,24:  Must be zero.
    ///   - R(5) Bits 18-22: Reserved.  Must be zero.
    ///   - p(2) Bits 16-17: Bits for the projection (0: XZ, 1: YZ, 2: XY).
    ///            (See geom id)
    ///   - u(8) Bits 8-15: Bits for u coordinate of the channel (see geom id).
    ///   - v(8) Bits 0-7: Bits for v coordinate of the channel (see geom id).
    ///
    /// For the electronics. This is channel id type 1.
    ///
    /// 01 RRRRRRR oooo ssss mmm mmmmm
    ///
    ///   - 01(2) Bits 23,24: Must be 01
    ///   - R(5) Bits 16-22: Reserved. Must be zero.
    ///   - o(4) Bits 12-15: Crate -- The OCB/crate.  The OCB and crate are
    ///           equivalent.  The current SFGD only has 16 crates.  This
    ///           corresponds to the 4 MSB of the board id in the gate open
    ///           word.
    ///   - s(4) Bits 8-11: Slot -- The FEB slot within one crate.  The Crate
    ///           and Slot together constitute the "Board Id", which
    ///           identifies the FEB (slot) uniquely within all 16 crates.
    ///           Corresponds to the 4 LSB of the "board id" in the gate open
    ///           word etc. of the FEB Protocol.
    ///    - m(8) Bits 0-7: Channel -- The CITIROC channel uniquely within one
    ///           FEB (8 CITIROCs per FEB, each having 32 ch). Corresponds to
    ///           the "channel id" in the hit timing/amplitude words on in the
    ///           FEB Protocol.
    enum BitDefinitions {
        kTypeMSB=24, kTypeLSB=23,

        // The bits for the MC
        kMCReservedMSB=22, kMCReservedLSB=18,
        kMCProjectionMSB=17, kMCProjectionLSB=16,
        kMCUMSB=15, kMCULSB=8,
        kMCVMSB=7, kMCVLSB=0,

        // The bits for the electronics
        kReservedMSB=22, kReservedLSB=16,
        kCrateMSB=15, kCrateLSB=12,
        kSlotMSB=11, kSlotLSB=8,
        kCitirocMSB=7, kCitirocLSB=5,
        kCitirocChannelMSB=4, kCitirocChannelLSB=0
    };
    /// @}

    ClassDef(TSFGChannelId, 1)
};
#endif //TSFGChannelId_hxx_seen
