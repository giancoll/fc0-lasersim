#pragma once

#include <vector>
#include <cmath>
#include <optional>

/// HAT (High Angle TPC) detector geometry
/// Defines pad layout, ERAM positioning, and coordinate utilities
class HATGeometry {
public:
    /// Module frame dimensions [mm]
    static constexpr double kModuleWidthZ  = 1865.0;   // horizontal (left-right)
    static constexpr double kModuleHeightY = 820.0;    // vertical (top-bottom)

    /// Pad dimensions [mm]
    static constexpr double kPadWidthZ  = 11.16;       // horizontal
    static constexpr double kPadHeightY = 10.05;       // vertical
    static constexpr double kPadGap     = 0.12;        // gap between pads (both axes)

    /// Pad array dimensions (per ERAM)
    static constexpr int kNPadsZ = 36;                 // horizontal (per ERAM)
    static constexpr int kNPadsY = 32;                 // vertical (per ERAM)
    static constexpr int kPadsPerERAM = kNPadsZ * kNPadsY;  // 1152 pads per ERAM

    /// ERAM configuration
    static constexpr int kNERAMs = 8;
    static constexpr int kNERAMsPerRow = 4;
    static constexpr int kNERAMRows = 2;
    static constexpr int kNPadsTotalZ = kNPadsZ * kNERAMsPerRow;  // 144 pads total Z
    static constexpr int kNPadsTotalY = kNPadsY * kNERAMRows;      // 64 pads total Y
    static constexpr int kNPadsTotal = kNPadsTotalZ * kNPadsTotalY; // 9216 pads total
    static constexpr double kERAMHeightY = (kNPadsY * kPadHeightY) + ((kNPadsY - 1) * kPadGap); // 325.32 mm
    static constexpr double kERAMWidthZ  = (kNPadsZ * kPadWidthZ) + ((kNPadsZ - 1) * kPadGap);   // 405.96 mm

    /// ERAM rectangular hole dimensions [mm]
    static constexpr double kERAMHoleHeightY = 300.0;   // vertical
    static constexpr double kERAMHoleWidthZ  = 378.0;   // horizontal

    struct ERAM {
        int id;           // 0-7
        int row;          // 0 (top) or 1 (bottom)
        int col;          // 0-3 (left to right)
        double centerY;   // center Y coordinate [mm]
        double centerZ;   // center Z coordinate [mm]
        double padAreaHeightY; // total height including gaps
        double padAreaWidthZ;  // total width including gaps
        bool isQuartzWindow;   // true if this ERAM position has quartz window instead of pads
    };

    struct QuartzWindow {
        double centerY;       // center Y [mm]
        double centerZ;       // center Z [mm]
        double frameWidthZ;   // frame width [mm]
        double frameHeightY;  // frame height [mm]
        double windowYMin, windowYMax;  // window bounds Y
        double windowZMin, windowZMax;  // window bounds Z
    };

public:
    /// Constructor
    HATGeometry();

    /// Get pad area dimensions (per ERAM, in mm)
    double GetPadAreaWidthZ() const { return m_padAreaWidthZ; }
    double GetPadAreaHeightY() const { return m_padAreaHeightY; }

    /// Get ERAM by ID (0-7)
    const ERAM& GetERAM(int eramId) const { return m_erams[eramId]; }

    /// Get ERAM by row and column
    int GetERAMId(int row, int col) const { return row * kNERAMsPerRow + col; }

    /// Map (Y,Z) position to local pad indices within ERAM
    /// Also returns eramId
    /// Returns true if position is within pad area of some ERAM
    bool GetPadIndices(double y, double z, int& eramId, int& padY, int& padZ) const;

    /// Map (Y,Z) position to ERAM ID
    /// Returns -1 if position is outside any ERAM
    int GetERAMFromPosition(double y, double z) const;

    /// Get center of a pad in ERAM local coordinates (mm)
    /// padY, padZ are local to the ERAM (0-31, 0-35)
    void GetPadCenter(int eramId, int padY, int padZ, double& y, double& z) const;

    /// Get bounds of a pad in mm
    /// padY, padZ are local to the ERAM (0-31, 0-35)
    void GetPadBounds(int eramId, int padY, int padZ,
                      double& yMin, double& yMax, double& zMin, double& zMax) const;

    /// Check if position (Y,Z) is within module bounds
    bool IsInModule(double y, double z) const;

    /// Check if position (Y,Z) is within active pad area of any ERAM
    bool IsInPadArea(double y, double z) const;

    /// Get quartz window (if any)
    const QuartzWindow* GetQuartzWindow() const { return m_quartzWindow ? &m_quartzWindow.value() : nullptr; }

    /// Check if position (Y,Z) is within quartz window aperture
    bool IsInQuartzWindow(double y, double z) const;

private:
    double m_padAreaWidthZ;   // total width of all pads + gaps
    double m_padAreaHeightY;  // total height of all pads + gaps
    std::vector<ERAM> m_erams;
    std::optional<QuartzWindow> m_quartzWindow;

    void InitializeERAMs();
};
