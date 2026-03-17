#ifndef ND280TriggerWord_hxx_seen
#define ND280TriggerWord_hxx_seen

namespace ND {

    /// Define the structure of the DAQ trigger word.
    namespace Trigger {

        /// Define trigger word masks. 
        ///
        ///     - Bits  0-31:  Event number
        ///     - Bits 32-47: Spill number
        ///     - Bits 48-57: Trigger bits
        ///         - Bit 48: Beam spill
        ///         - Bit 49: Pedestal/noise
        ///         - Bit 50: TPC laser
        ///         - Bit 51: POD LED
        ///         - Bit 52: ECAL LED
        ///         - Bit 53: FGD LED
        ///         - Bit 54: FEE calibration
        ///         - Bit 55: TRIP-t cosmic
        ///         - Bit 56: Reserved for FGD cosmic
        ///         - Bit 57: unused
        ///     - Bits 58-63: Instruction
        ///         - Bit 58: Reserved for start-of-run
        ///         - Bit 59: Reserved for end-of-run
        ///         - Bit 60: 1 PPS / counter reset
        ///         - Bit 61: Go to spill mode
        ///         - Bit 62: Go to cosmic mode
        ///         - Bit 63: Decline CTM request/abort
        typedef enum {
            kEventMask   = 0x00000000FFFFFFFFLL,   ///< Bit 0-31:  Event number
            kSpillMask   = 0x0000FFFF00000000LL,   ///< Bit 32-47: Spill number
            kTriggerMask = 0x03FF000000000000LL,   ///< Bit 48-57: Trigger bits
            kInstMask    = 0xFC00000000000000LL    ///< Bit 58-63: Instructions
        } Masks;

        /// Define the shifts associates with each mask.
        typedef enum {
            kEventShift   = 0,   ///< Bit 0-31:  Event number
            kSpillShift   = 32,   ///< Bit 32-47: Spill number
            kTriggerShift = 48,   ///< Bit 48-57: Trigger bits
            kInstShift    = 58    ///< Bit 58-63: Instructions
        } Shifts;

        /// Define trigger word bits.  These are masks applying to the shifted
        /// trigger bits (e.g. the result of
        /// ND::TND280Event::Header::GetTriggerBits()).  If you need to have
        /// the bit definition in the full trigger word, then you must shift
        /// by 48 bits (e.g. kBeamSpill << kTriggerShift).
        typedef enum {
            kBeamSpill = 0x0001,            ///< Bit 48: Beam spill
            kPedestal  = 0x0002,            ///< Bit 49: Pedestal/noise
            kTPCLaser  = 0x0004,            ///< Bit 50: TPC laser
            kP0DLED    = 0x0008,            ///< Bit 51: POD LED
            kECalLED   = 0x0010,            ///< Bit 52: ECAL LED
            kFGDLED    = 0x0020,            ///< Bit 53: FGD LED
            kFEECalib  = 0x0040,            ///< Bit 54: FEE calibration
            kTFBCosmic = 0x0080,            ///< Bit 55: TRIP-t cosmic
            kFGDCosmic = 0x0100,            ///< Bit 56: FGD cosmic
            kNotUsed   = 0x0200             ///< Bit 57: unused
        } Bits;
    }
}

#endif
