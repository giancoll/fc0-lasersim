/// Example: Visualize HAT detector geometry using ROOT
/// Compile with: g++ -o viz VisualizeHATGeometry.cc ../src/detector/HATGeometry.cc ../src/detector/HATGeometryViz.cc ../src/config/SimConfig.cc `root-config --cflags --libs` -I.. -lstdc++fs
///
/// Usage:
///   ./viz                    — geometry-only view (Y-Z and X-Z)
///   ./viz output.root        — also show anode hit map from simulation output

#include "../src/detector/HATGeometry.hh"
#include "../src/detector/HATGeometryViz.hh"
#include "../src/config/Config.hh"

#include <TStyle.h>
#include <TApplication.h>
#include <TCanvas.h>
#include <TFile.h>
#include <TTree.h>
#include <TH2F.h>
#include <TGraph.h>
#include <TBox.h>
#include <TColor.h>
#include <iostream>
#include <string>
#include <map>
#include <algorithm>

int main(int argc, char** argv) {
    // Extract optional ROOT file argument before TApplication consumes argv
    std::string rootFile;
    for (int i = 1; i < argc; ++i) {
        const std::string arg(argv[i]);
        if (arg.size() > 5 && arg.substr(arg.size() - 5) == ".root") {
            rootFile = arg;
            break;
        }
    }

    TApplication app("HATViz", &argc, argv);
    gStyle->SetOptStat(0);  // Disable histogram stats box

    // Create geometry
    HATGeometry geom;

    // Load simulation config
    Config simCfg;
    try {
        simCfg = Config::Load("../config/simulation.json");
    } catch (const std::exception& e) {
        std::cerr << "Warning: Could not load config: " << e.what() << std::endl;
        std::cerr << "Proceeding without laser/mirror visualization" << std::endl;
    }

    std::cout << "=== HAT Detector Geometry ===" << std::endl;
    std::cout << "Module frame: " << geom.kModuleWidthZ << " x "
              << geom.kModuleHeightY << " mm (Z x Y)" << std::endl;
    std::cout << "Pad area: " << geom.GetPadAreaWidthZ() << " x "
              << geom.GetPadAreaHeightY() << " mm" << std::endl;
    std::cout << "Total pads: " << geom.kNPadsZ << " x " << geom.kNPadsY
              << " = " << geom.kNPadsTotal << std::endl;
    std::cout << "Pads per ERAM: " << geom.kPadsPerERAM << std::endl;
    std::cout << std::endl;

    // Print ERAM positions
    std::cout << "ERAM Positions (center coordinates):" << std::endl;
    for (int id = 0; id < geom.kNERAMs; ++id) {
        const auto& eram = geom.GetERAM(id);
        std::cout << "  ERAM " << id << ": (Y=" << eram.centerY << ", Z="
                  << eram.centerZ << ") mm" << std::endl;
    }
    std::cout << std::endl;

    // Test coordinate conversion
    std::cout << "Coordinate conversion test:" << std::endl;
    double testY = 0.0, testZ = 0.0;  // center
    int eramId, padY, padZ;
    if (geom.GetPadIndices(testY, testZ, eramId, padY, padZ)) {
        std::cout << "  Position (Y=" << testY << ", Z=" << testZ
                  << ") → ERAM " << eramId << ", Pad (" << padY << ", " << padZ << ")" << std::endl;
    } else {
        std::cout << "  Position (Y=" << testY << ", Z=" << testZ
                  << ") is outside pad area" << std::endl;
    }
    std::cout << std::endl;

    // Draw Y-Z view (vertical vs left-right)
    std::cout << "Drawing Y-Z view..." << std::endl;
    TCanvas* canvasYZ = HATGeometryViz::DrawGeometryYZ(geom,
                                                        &simCfg,
                                                        "HAT Geometry - Y-Z View (vertical vs left-right)",
                                                        true);  // drawPads=false

    // Draw X-Z view (drift axis vs left-right)
    std::cout << "Drawing X-Z view..." << std::endl;
    TCanvas* canvasXZ = HATGeometryViz::DrawGeometryXZ(geom,
                                                        &simCfg,
                                                        "HAT Geometry - X-Z View (drift axis vs left-right)");

    // Save to files
    canvasYZ->SaveAs("/tmp/HAT_Geometry_YZ.png");
    canvasXZ->SaveAs("/tmp/HAT_Geometry_XZ.png");
    std::cout << "Saved to /tmp/HAT_Geometry_YZ.png and /tmp/HAT_Geometry_XZ.png" << std::endl;

    // ── Optional: anode hit map from simulation output ─────────────────────────
    if (!rootFile.empty()) {
        std::cout << "Reading anode hits from: " << rootFile << std::endl;
        TFile* f = TFile::Open(rootFile.c_str(), "READ");
        if (!f || f->IsZombie()) {
            std::cerr << "Warning: cannot open " << rootFile << " — skipping hit map\n";
        } else {
            TTree* tree = dynamic_cast<TTree*>(f->Get("clusters"));
            if (!tree) {
                std::cerr << "Warning: 'clusters ' tree not found in " << rootFile << "\n";
            } else {
                std::vector<float>* brZ = nullptr;
                std::vector<float>* brY = nullptr;
                tree->SetBranchAddress("cl_z", &brZ);
                tree->SetBranchAddress("cl_y", &brY);

                // Count hits per pad; key encodes (eramId, padY, padZ) as a single int
                TGraph *gr_cluster = new TGraph();
                const long long nEntries = tree->GetEntries();
                for (long long ev = 0; ev < nEntries; ++ev) {
                    tree->GetEntry(ev);
                    if (!brZ || !brY) continue;
                    for (size_t k = 0; k < brZ->size(); ++k) {
                        gr_cluster->SetPoint(gr_cluster->GetN(), (*brZ)[k], (*brY)[k]);
                    }
                }

                TCanvas* canvasHits = new TCanvas("cHits",
                    Form("Map — %lld events", nEntries), 1100, 600);
                canvasHits->SetRightMargin(0.05);

                // Empty frame for axes (same extent as the Y-Z geometry view)
                TH2F* frameHits = new TH2F("frameHits",
                    "Cluster  Map;Z [mm] (horizontal);Y [mm] (vertical)",
                    100, -HATGeometry::kModuleWidthZ / 2., HATGeometry::kModuleWidthZ / 2.,
                    100, -HATGeometry::kModuleHeightY / 2., HATGeometry::kModuleHeightY / 2.);
                frameHits->Draw();

                // Draw hits as points
                gr_cluster->SetMarkerStyle(20);
                gr_cluster->SetMarkerSize(0.5);
                gr_cluster->SetMarkerColor(kRed + 1);
                gr_cluster->Draw("P SAME");

                // Overlay geometry elements (same as the standard Y-Z view)
                HATGeometryViz::DrawModuleFrame(geom);
                HATGeometryViz::DrawQuartzWindow(geom);
                HATGeometryViz::DrawLaserYZ(simCfg);
                HATGeometryViz::DrawMirrorYZ(simCfg);

                canvasHits->Modified();
                canvasHits->Update();
                canvasHits->SaveAs("/tmp/HAT_Hits_YZ.png");
                std::cout << "Saved to /tmp/HAT_Hits_YZ.png\n";
            }
            // keep file open so ROOT doesn't delete the objects
        }
    }

    // Interactive mode
    std::cout << "Showing canvases (press Ctrl+C to exit)..." << std::endl;
    app.Run();

    return 0;
}
