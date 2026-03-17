#include "detector/HATGeometryViz.hh"

#include <TCanvas.h>
#include <TFile.h>
#include <TTree.h>

#include <TBox.h>
#include <TPad.h>
#include <TLine.h>
#include <TAxis.h>
#include <TH2F.h>
#include <TStyle.h>
#include <iostream>
#include <cmath>

TCanvas* HATGeometryViz::DrawGeometryYZ(const HATGeometry& geom,
                                         const Config* cfg,
                                         const char* title,
                                         bool drawPads) {
    TCanvas* canvas = new TCanvas("hatCanvasYZ", title, 900, 600);
    canvas->cd();

    // Create a histogram as background for axis labels
    TH2F* frame = new TH2F("frameYZ", title,
                           100, -1000, 1000,
                           100, -500, 500);
    frame->SetXTitle("Z [mm] (horizontal, left-right)");
    frame->SetYTitle("Y [mm] (vertical, top-bottom)");
    frame->Draw();

    // Draw geometry elements
    DrawModuleFrame(geom);
    DrawQuartzWindow(geom);

    if (drawPads) {
        std::cout << "[HATGeometryViz] Drawing all " << geom.kNPadsTotal
                  << " pads... (this may take a moment)" << std::endl;
        DrawAllPads(geom);
    }

    // Draw laser and mirror if config provided
    if (cfg) {
        DrawLaserYZ(*cfg);
        DrawMirrorYZ(*cfg);
    }

    canvas->Modified();
    canvas->Update();

    return canvas;
}

TCanvas* HATGeometryViz::DrawGeometryXZ(const HATGeometry& geom,
                                         const Config* cfg,
                                         const char* title) {
    TCanvas* canvas = new TCanvas("hatCanvasXZ", title, 900, 600);
    canvas->cd();

    // TPC bounds from config (or defaults)
    double zMin = -89.0, zMax = 89.0;    // Full detector Z extent [cm]
    double xMin = 0.0, xMax = 100.0;     // Drift range [cm]

    if (cfg) {
        zMin = cfg->detector.tpcZminMm;
        zMax = cfg->detector.tpcZmaxMm;
        xMin = cfg->detector.tpcXminMm;
        xMax = cfg->detector.tpcXmaxMm;
    }

    // Create a histogram as background for axis labels (bounds already in mm)
    TH2F* frame = new TH2F("frameXZ", title,
                           100, zMin, zMax,
                           100, xMin, xMax);
    frame->SetXTitle("Z [mm] (left-right)");
    frame->SetYTitle("X [mm] (drift axis)");
    frame->Draw();

    // Draw TPC boundaries
    if (cfg) {
        DrawTPCBoundsXZ(*cfg);
        DrawLaserXZ(*cfg);
        DrawMirrorXZ(*cfg);
    }

    canvas->Modified();
    canvas->Update();

    return canvas;
}

void HATGeometryViz::DrawModuleFrame(const HATGeometry& geom) {
    // Module frame outline
    double halfZ = geom.kModuleWidthZ / 2.0;
    double halfY = geom.kModuleHeightY / 2.0;

    TBox* frame = new TBox(-halfZ, -halfY, halfZ, halfY);
    frame->SetLineColor(1);
    frame->SetLineWidth(3);
    frame->SetFillStyle(0);
    frame->Draw();

}


void HATGeometryViz::DrawQuartzWindow(const HATGeometry& geom) {
    const HATGeometry::QuartzWindow* qwin = geom.GetQuartzWindow();
    if (!qwin) return;

    // Draw aperture window (inner rectangle)
    TBox* aperture = new TBox(qwin->windowZMin, qwin->windowYMin,
                              qwin->windowZMax, qwin->windowYMax);
    aperture->SetLineColor(2);
    aperture->SetLineWidth(2);
    aperture->SetLineStyle(2);  // Dashed
    aperture->SetFillStyle(0);  // No fill
    aperture->Draw();

}

void HATGeometryViz::DrawAllPads(const HATGeometry& geom) {
    // WARNING: This draws 9216 boxes (8 ERAMs × 1152 pads each) - very slow!
    // Only use for detailed inspection of specific regions
    for (int eramId = 0; eramId < geom.kNERAMs; ++eramId) {
        if(eramId == 3) continue;  // Skip ERAM 3 (quartz window)
        for (int padY = 0; padY < geom.kNPadsY; ++padY) {
            for (int padZ = 0; padZ < geom.kNPadsZ; ++padZ) {
                DrawPad(geom, eramId, padY, padZ);
            }
        }
    }
}

void HATGeometryViz::DrawPad(const HATGeometry& geom, int eramId, int padY, int padZ) {
    double yMin, yMax, zMin, zMax;
    geom.GetPadBounds(eramId, padY, padZ, yMin, yMax, zMin, zMax);

    TBox* pad = new TBox(zMin, yMin, zMax, yMax);
    pad->SetLineColor(3);
    pad->SetLineWidth(1);
    pad->SetLineStyle(1);
    pad->SetFillStyle(0);
    pad->Draw();
}

void HATGeometryViz::DrawLaserYZ(const Config& cfg) {
    const LaserConfig& laser = cfg.generator.laser;

    // Laser appears as a point in Y-Z plane (perpendicular to drift axis X)
    double laserZ = laser.zMm;
    double laserY = laser.yMm;

    // Draw laser position as a small circle
    TBox* laserPt = new TBox(laserZ - 3, laserY - 3, laserZ + 3, laserY + 3);
    laserPt->SetLineColor(2);   // Red
    laserPt->SetLineWidth(2);
    laserPt->SetFillColor(7);
    laserPt->SetLineWidth(2);
    laserPt->Draw();

}

void HATGeometryViz::DrawMirrorYZ(const Config& cfg) {
    const MirrorConfig& mirror = cfg.generator.laser.mirror;
    if (!mirror.enabled) return;

    // Mirror appears as a circle in Y-Z plane
    double mirrorZ = mirror.zMm;
    double mirrorY = mirror.yMm;
    double mirrorR = mirror.radiusMm;

    TBox* mirrorBox = new TBox(mirrorZ - mirrorR, mirrorY - mirrorR,
                               mirrorZ + mirrorR, mirrorY + mirrorR);
    mirrorBox->SetLineColor(6);   // Magenta
    mirrorBox->SetLineWidth(2);
    mirrorBox->SetFillColor(6);
    mirrorBox->SetFillStyle(3003);  // Sparse dots
    mirrorBox->Draw();

}

void HATGeometryViz::DrawLaserXZ(const Config& cfg) {
    const LaserConfig& laser = cfg.generator.laser;

    // All coordinates in mm
    double laserZ  = laser.zMm;
    double laserX0 = laser.xStartMm;

    // Initial beam direction (normalised in X-Z plane)
    double dX = laser.dx, dZ = laser.dz;
    double norm = std::sqrt(dX*dX + dZ*dZ);
    dX /= norm; dZ /= norm;

    if (laser.mirror.enabled) {
        // Segment 1: entry → mirror
        double mirrorX = laser.mirror.xMm;
        double mirrorZ = laser.mirror.zMm;

        TLine* seg1 = new TLine(laserZ, laserX0, mirrorZ, mirrorX);
        seg1->SetLineColor(2);
        seg1->SetLineWidth(2);
        seg1->Draw();

        // Reflect direction off mirror normal n=(cos θ, 0, sin θ)
        double theta = laser.mirror.tiltThetaDeg * M_PI / 180.0;
        double nX = std::cos(theta), nZ = std::sin(theta);
        double dot = dX*nX + dZ*nZ;
        double rX = dX - 2.0*dot*nX;
        double rZ = dZ - 2.0*dot*nZ;

        // Segment 2: mirror → TPC wall (clip to bounds)
        double zMin = cfg.detector.tpcZminMm;
        double zMax = cfg.detector.tpcZmaxMm;
        double xMin = cfg.detector.tpcXminMm;
        double xMax = cfg.detector.tpcXmaxMm;

        // Find smallest positive t to reach any wall
        double tExit = 1e9;
        if (std::abs(rX) > 1e-9) {
            double t = (rX > 0 ? xMax - mirrorX : xMin - mirrorX) / rX;
            if (t > 0) tExit = std::min(tExit, t);
        }
        if (std::abs(rZ) > 1e-9) {
            double t = (rZ > 0 ? zMax - mirrorZ : zMin - mirrorZ) / rZ;
            if (t > 0) tExit = std::min(tExit, t);
        }

        double exitZ = mirrorZ + rZ * tExit;
        double exitX = mirrorX + rX * tExit;

        TLine* seg2 = new TLine(mirrorZ, mirrorX, exitZ, exitX);
        seg2->SetLineColor(2);
        seg2->SetLineWidth(2);
        seg2->SetLineStyle(2);  // Dashed for reflected segment
        seg2->Draw();

    } else {
        // Straight beam: entry → xEnd
        double xEnd = laser.xEndMm;
        double endZ = laserZ + dZ * (xEnd - laserX0) / dX;
        TLine* seg = new TLine(laserZ, laserX0, endZ, xEnd);
        seg->SetLineColor(2);
        seg->SetLineWidth(2);
        seg->Draw();
    }
}

void HATGeometryViz::DrawMirrorXZ(const Config& cfg) {
    const MirrorConfig& mirror = cfg.generator.laser.mirror;
    if (!mirror.enabled) return;

    double mirrorX = mirror.xMm;
    double mirrorZ = mirror.zMm;
    double mirrorLength = mirror.radiusMm * 2.0;  // Diameter in mm
    double tiltTheta = mirror.tiltThetaDeg * M_PI / 180.0;

    // Mirror orientation in X-Z plane:
    // - At theta=0°: normal along +X, mirror perpendicular to X (parallel to anode)
    //   In X-Z view: appears as horizontal line in Z direction
    // - At theta>0°: mirror tilts away from anode
    //   In X-Z view: appears as tilted line

    // Mirror edge endpoints in X-Z plane
    // Normal n = (cos θ, sin θ) in (X,Z); perpendicular is (-sin θ, cos θ)
    double perpZ = std::cos(tiltTheta);    // Z component of perpendicular
    double perpX = -std::sin(tiltTheta);  // X component of perpendicular

    // Mirror extent along perpendicular direction
    double z1 = mirrorZ - perpZ * mirrorLength / 2.0;
    double x1 = mirrorX - perpX * mirrorLength / 2.0;
    double z2 = mirrorZ + perpZ * mirrorLength / 2.0;
    double x2 = mirrorX + perpX * mirrorLength / 2.0;

    TLine* mirrorLine = new TLine(z1, x1, z2, x2);
    mirrorLine->SetLineColor(4);    // Blue
    mirrorLine->SetLineWidth(2);
    mirrorLine->Draw();

}

void HATGeometryViz::DrawTPCBoundsXZ(const Config& cfg) {
    double zMin = cfg.detector.tpcZminMm;
    double zMax = cfg.detector.tpcZmaxMm;
    double xMin = cfg.detector.tpcXminMm;
    double xMax = cfg.detector.tpcXmaxMm;

    // Draw TPC boundary
    TBox* bounds = new TBox(zMin, xMin, zMax, xMax);
    bounds->SetLineColor(1);
    bounds->SetLineWidth(2);
    bounds->SetFillStyle(0);
    bounds->Draw();

}

TH2F* HATGeometryViz::CreatePadHistogram(const char* name) {
    // Create a 2D histogram with pad granularity
    // Useful for plotting charge distributions or hit maps

    double padAreaZ = 36 * 11.16 + 35 * 0.12;  // 405.96 mm
    double padAreaY = 32 * 10.05 + 31 * 0.12;  // 325.32 mm

    // Create histogram with pad-sized bins
    TH2F* hist = new TH2F(name, "HAT Pad Response",
                          36, -padAreaZ/2.0, padAreaZ/2.0,
                          32, -padAreaY/2.0, padAreaY/2.0);

    hist->SetXTitle("Z [mm] (horizontal)");
    hist->SetYTitle("Y [mm] (vertical)");
    hist->SetZTitle("Charge [ADC]");

    // Set bin labels with pad indices (optional - can clutter)
    // for (int iz = 1; iz <= 36; ++iz) {
    //     hist->GetXaxis()->SetBinLabel(iz, Form("%d", iz-1));
    // }
    // for (int iy = 1; iy <= 32; ++iy) {
    //     hist->GetYaxis()->SetBinLabel(iy, Form("%d", iy-1));
    // }

    return hist;
}
