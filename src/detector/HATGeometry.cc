#include "detector/HATGeometry.hh"
#include <stdexcept>

HATGeometry::HATGeometry() {
    // Calculate pad area dimensions for ONE ERAM (including gaps)
    m_padAreaWidthZ  = kNPadsZ * kPadWidthZ + (kNPadsZ - 1) * kPadGap;   // 405.96 mm
    m_padAreaHeightY = kNPadsY * kPadHeightY + (kNPadsY - 1) * kPadGap;  // 325.32 mm

    // Initialize ERAM positions
    InitializeERAMs();
}

void HATGeometry::InitializeERAMs() {
    m_erams.clear();
    m_erams.reserve(kNERAMs);

    // ERAM center positions (from module frame geometry constraints)
    // Y positions (vertical, top to bottom)
    const double eramYPositions[kNERAMRows] = {
        +170.5,   // Top row
        -170.5    // Bottom row
    };

    // Z positions (horizontal, left to right)
    const double eramZPositions[kNERAMsPerRow] = {
        -631.5,   // Column 0 (leftmost)
        -210.5,   // Column 1
        +210.5,   // Column 2 (will be replaced by quartz window)
        +631.5    // Column 3 (rightmost)
    };

    // Dimensions per ERAM (including gaps)
    // Each ERAM has 36×32 pads
    const double eramHeightY = (kNPadsY * kPadHeightY) +
                               ((kNPadsY - 1) * kPadGap);
    const double eramWidthZ  = (kNPadsZ * kPadWidthZ) +
                               ((kNPadsZ - 1) * kPadGap);

    int eramId = 0;
    for (int row = 0; row < kNERAMRows; ++row) {
        for (int col = 0; col < kNERAMsPerRow; ++col) {
            ERAM eram;
            eram.id = eramId;
            eram.row = row;
            eram.col = col;
            eram.centerY = eramYPositions[row];
            eram.centerZ = eramZPositions[col];
            eram.padAreaHeightY = eramHeightY;
            eram.padAreaWidthZ = eramWidthZ;

            // ERAM 3 (top-right) is replaced by quartz window
            eram.isQuartzWindow = (eramId == 3);

            m_erams.push_back(eram);
            ++eramId;
        }
    }

    // Initialize quartz window (replaces ERAM 3 at position top-right)
    // Rectangle dimensions: 420 mm (Z) × 340 mm (Y), centered at ERAM 3 position
    // Window borders: 108 mm from left, 38 mm from right, 33 mm from top/bottom
    const ERAM& eram3 = m_erams[3];
    QuartzWindow qwindow;
    qwindow.centerY = eram3.centerY;
    qwindow.centerZ = eram3.centerZ;
    qwindow.frameWidthZ = 420.0;
    qwindow.frameHeightY = 340.0;

    // Calculate window bounds within the frame
    // Frame bounds
    double frameZMin = eram3.centerZ - 210.0;   // 420 / 2
    double frameZMax = eram3.centerZ + 210.0;
    double frameYMin = eram3.centerY - 170.0;   // 340 / 2
    double frameYMax = eram3.centerY + 170.0;

    // Window aperture bounds (with borders)
    qwindow.windowZMin = frameZMin + 108.0;     // 108 mm from left edge
    qwindow.windowZMax = frameZMax - 38.0;      // 38 mm from right edge
    qwindow.windowYMin = frameYMin + 33.0;      // 33 mm from bottom edge
    qwindow.windowYMax = frameYMax - 33.0;      // 33 mm from top edge

    m_quartzWindow = qwindow;
}

bool HATGeometry::GetPadIndices(double y, double z, int& eramId, int& padY, int& padZ) const {
    // First, find which ERAM contains this position
    eramId = GetERAMFromPosition(y, z);
    if (eramId < 0) return false;

    const ERAM& eram = m_erams[eramId];

    // Convert from global coordinates to ERAM-local coordinates
    double localY = y - eram.centerY + m_padAreaHeightY / 2.0;
    double localZ = z - eram.centerZ + m_padAreaWidthZ / 2.0;

    // Map to pad indices within this ERAM
    // Account for pad width and gaps
    padY = static_cast<int>(localY / (kPadHeightY + kPadGap));
    padZ = static_cast<int>(localZ / (kPadWidthZ + kPadGap));

    // Clamp to valid range (0-31 for Y, 0-35 for Z)
    if (padY < 0) padY = 0;
    if (padY >= kNPadsY) padY = kNPadsY - 1;
    if (padZ < 0) padZ = 0;
    if (padZ >= kNPadsZ) padZ = kNPadsZ - 1;

    return true;
}

int HATGeometry::GetERAMFromPosition(double y, double z) const {
    // Check each ERAM to see if the position falls within its active area
    for (int id = 0; id < kNERAMs; ++id) {
        const ERAM& eram = m_erams[id];

        double halfW = m_padAreaWidthZ / 2.0;
        double halfH = m_padAreaHeightY / 2.0;

        double zMin = eram.centerZ - halfW;
        double zMax = eram.centerZ + halfW;
        double yMin = eram.centerY - halfH;
        double yMax = eram.centerY + halfH;

        // Check if position is within the ERAM frame
        if (y >= yMin && y <= yMax && z >= zMin && z <= zMax) {
            // For ERAM 3 (quartz window), only return if in window aperture
            if (eram.isQuartzWindow && m_quartzWindow) {
                if (IsInQuartzWindow(y, z)) {
                    return id;
                }
                // Position is in ERAM 3 frame but outside window - not in active area
            } else {
                // Regular ERAM with pads
                return id;
            }
        }
    }

    return -1;
}

void HATGeometry::GetPadCenter(int eramId, int padY, int padZ, double& y, double& z) const {
    if (eramId < 0 || eramId >= kNERAMs ||
        padY < 0 || padY >= kNPadsY || padZ < 0 || padZ >= kNPadsZ) {
        throw std::out_of_range("Pad indices out of range");
    }

    const ERAM& eram = m_erams[eramId];
    const double halfWidthZ = m_padAreaWidthZ / 2.0;
    const double halfHeightY = m_padAreaHeightY / 2.0;

    // Calculate position of pad center within ERAM
    double localY = padY * (kPadHeightY + kPadGap) + kPadHeightY / 2.0;
    double localZ = padZ * (kPadWidthZ + kPadGap) + kPadWidthZ / 2.0;

    // Convert to global coordinates (ERAM-centered)
    y = eram.centerY + (localY - halfHeightY);
    z = eram.centerZ + (localZ - halfWidthZ);
}

void HATGeometry::GetPadBounds(int eramId, int padY, int padZ,
                               double& yMin, double& yMax,
                               double& zMin, double& zMax) const {
    if (eramId < 0 || eramId >= kNERAMs ||
        padY < 0 || padY >= kNPadsY || padZ < 0 || padZ >= kNPadsZ) {
        throw std::out_of_range("Pad indices out of range");
    }

    const ERAM& eram = m_erams[eramId];
    const double halfWidthZ = m_padAreaWidthZ / 2.0;
    const double halfHeightY = m_padAreaHeightY / 2.0;

    // Calculate local bounds within ERAM
    double localYMin = padY * (kPadHeightY + kPadGap);
    double localYMax = localYMin + kPadHeightY;
    double localZMin = padZ * (kPadWidthZ + kPadGap);
    double localZMax = localZMin + kPadWidthZ;

    // Convert to global coordinates
    yMin = eram.centerY + (localYMin - halfHeightY);
    yMax = eram.centerY + (localYMax - halfHeightY);
    zMin = eram.centerZ + (localZMin - halfWidthZ);
    zMax = eram.centerZ + (localZMax - halfWidthZ);
}

bool HATGeometry::IsInModule(double y, double z) const {
    const double halfWidth = kModuleWidthZ / 2.0;
    const double halfHeight = kModuleHeightY / 2.0;

    return (std::abs(y) <= halfHeight && std::abs(z) <= halfWidth);
}

bool HATGeometry::IsInPadArea(double y, double z) const {
    return GetERAMFromPosition(y, z) >= 0;
}

bool HATGeometry::IsInQuartzWindow(double y, double z) const {
    if (!m_quartzWindow) return false;

    return (y >= m_quartzWindow->windowYMin && y <= m_quartzWindow->windowYMax &&
            z >= m_quartzWindow->windowZMin && z <= m_quartzWindow->windowZMax);
}
