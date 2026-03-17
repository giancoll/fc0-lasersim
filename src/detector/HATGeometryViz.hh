#pragma once

#include "detector/HATGeometry.hh"
#include "config/Config.hh"

// Forward declarations to avoid ROOT dependency in main header
class TCanvas;
class TPad;
class TH2F;

/// Visualization utilities for HAT geometry using ROOT TBox
class HATGeometryViz {
public:
    /// Draw Y-Z projection (vertical vs left-right)
    /// Shows pad geometry, ERAM layout, quartz window, and laser/mirror
    static TCanvas* DrawGeometryYZ(const HATGeometry& geom,
                                    const Config* cfg = nullptr,
                                    const char* title = "HAT Geometry - Y-Z View",
                                    bool drawPads = false);

    /// Draw X-Z projection (drift axis vs left-right)
    /// Shows laser track, mirror, and TPC boundaries
    static TCanvas* DrawGeometryXZ(const HATGeometry& geom,
                                    const Config* cfg = nullptr,
                                    const char* title = "HAT Geometry - X-Z View");

    /// Legacy method - now calls DrawGeometryYZ
    static TCanvas* DrawGeometry(const HATGeometry& geom,
                                  const char* title = "HAT Detector Geometry",
                                  bool drawPads = false) {
        return DrawGeometryYZ(geom, nullptr, title, drawPads);
    }

    /// Draw module frame outline
    static void DrawModuleFrame(const HATGeometry& geom);

    /// Draw quartz window aperture
    static void DrawQuartzWindow(const HATGeometry& geom);

    /// Draw laser track on Y-Z view
    static void DrawLaserYZ(const Config& cfg);

    /// Draw mirror on Y-Z view
    static void DrawMirrorYZ(const Config& cfg);

    /// Draw laser track on X-Z view
    static void DrawLaserXZ(const Config& cfg);

    /// Draw mirror on X-Z view (tilted line representation)
    static void DrawMirrorXZ(const Config& cfg);

    /// Draw TPC bounds on X-Z view
    static void DrawTPCBoundsXZ(const Config& cfg);

    /// Draw individual pads for all ERAMs (creates 9216 boxes - very slow!)
    static void DrawAllPads(const HATGeometry& geom);

    /// Draw a single pad
    static void DrawPad(const HATGeometry& geom, int eramId, int padY, int padZ);

    /// Create a 2D histogram showing pad layout
    /// Useful for plotting charge distributions
    static TH2F* CreatePadHistogram(const char* name = "HATPads");
};
