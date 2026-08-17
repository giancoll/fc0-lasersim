#include <TCanvas.h>
#include <TFile.h>
#include <TF1.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TStyle.h>
#include <TTree.h>

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <map>
#include <numeric>
#include <string>
#include <vector>

namespace {

constexpr int kPadsZPerERAM = 36;
constexpr int kPadsYPerERAM = 32;
constexpr int kERAMsPerRow = 4;
constexpr int kRows = 2;
constexpr int kGlobalPadsZ = kPadsZPerERAM * kERAMsPerRow;
constexpr int kGlobalPadsY = kPadsYPerERAM * kRows;
constexpr double kPadPitchZMm = 11.16 + 0.12;
constexpr double kPadPitchYMm = 10.05 + 0.12;

struct EventPoints {
    std::vector<double> z;
    std::vector<double> y;
    std::vector<double> t;
    std::vector<int> globalPadZ;
    std::vector<int> globalPadY;
};

struct Summary {
    double meanEndpoints = 0.0;
    double meanDensityPerCm = 0.0;
    double residualSigmaMm = 0.0;
    double residualSigmaErrMm = 0.0;
    double residualRmsMm = 0.0;
    int nEvents = 0;
    int nFitPoints = 0;
};

bool globalPadIndex(int eram, int localPadZ, int localPadY, int& globalZ, int& globalY) {
    if (eram < 0 || eram >= 8) return false;
    if (localPadZ < 0 || localPadZ >= kPadsZPerERAM) return false;
    if (localPadY < 0 || localPadY >= kPadsYPerERAM) return false;

    const int row = eram / kERAMsPerRow;
    const int col = eram % kERAMsPerRow;
    globalZ = col * kPadsZPerERAM + localPadZ;
    globalY = row * kPadsYPerERAM + localPadY;
    return globalZ >= 0 && globalZ < kGlobalPadsZ && globalY >= 0 && globalY < kGlobalPadsY;
}

double mean(const std::vector<double>& values) {
    if (values.empty()) return 0.0;
    return std::accumulate(values.begin(), values.end(), 0.0) / values.size();
}

double rms(const std::vector<double>& values) {
    if (values.empty()) return 0.0;
    const double m = mean(values);
    double s2 = 0.0;
    for (const double x : values) s2 += (x - m) * (x - m);
    return std::sqrt(s2 / values.size());
}

std::vector<EventPoints> loadEvents(const std::string& rootPath) {
    TFile file(rootPath.c_str(), "READ");
    if (file.IsZombie()) {
        throw std::runtime_error("Cannot open ROOT file: " + rootPath);
    }

    auto* tree = dynamic_cast<TTree*>(file.Get("anode"));
    if (!tree) {
        throw std::runtime_error("ROOT file has no anode tree: " + rootPath);
    }

    std::vector<float>* anodeZ = nullptr;
    std::vector<float>* anodeY = nullptr;
    std::vector<float>* anodeT = nullptr;
    std::vector<int>* anodeEram = nullptr;
    std::vector<int>* anodePadZ = nullptr;
    std::vector<int>* anodePadY = nullptr;

    tree->SetBranchAddress("anode_z", &anodeZ);
    tree->SetBranchAddress("anode_y", &anodeY);
    tree->SetBranchAddress("anode_t", &anodeT);
    tree->SetBranchAddress("anode_eram", &anodeEram);
    tree->SetBranchAddress("anode_padZ", &anodePadZ);
    tree->SetBranchAddress("anode_padY", &anodePadY);

    std::vector<EventPoints> events;
    events.reserve(tree->GetEntries());

    for (Long64_t iev = 0; iev < tree->GetEntries(); ++iev) {
        tree->GetEntry(iev);
        EventPoints ev;
        const std::size_t n = anodeZ ? anodeZ->size() : 0;
        for (std::size_t i = 0; i < n; ++i) {
            int gZ = -1;
            int gY = -1;
            if (!globalPadIndex(anodeEram->at(i), anodePadZ->at(i), anodePadY->at(i), gZ, gY)) {
                continue;
            }
            ev.z.push_back(anodeZ->at(i));
            ev.y.push_back(anodeY->at(i));
            ev.t.push_back(anodeT->at(i));
            ev.globalPadZ.push_back(gZ);
            ev.globalPadY.push_back(gY);
        }
        events.push_back(std::move(ev));
    }

    return events;
}

void saveAveragePadMap(const std::vector<EventPoints>& events,
                       const std::string& outPrefix,
                       const std::string& title) {
    TH2F h("avg_pad_map", title.c_str(),
           kGlobalPadsZ, 0, kGlobalPadsZ,
           kGlobalPadsY, 0, kGlobalPadsY);
    h.GetXaxis()->SetTitle("global pad Z column");
    h.GetYaxis()->SetTitle("global pad Y row");
    h.GetZaxis()->SetTitle("mean drifted electrons / pad / event");

    for (const auto& ev : events) {
        for (std::size_t i = 0; i < ev.globalPadZ.size(); ++i) {
            h.Fill(ev.globalPadZ[i] + 0.5, ev.globalPadY[i] + 0.5, 1.0);
        }
    }
    if (!events.empty()) h.Scale(1.0 / static_cast<double>(events.size()));

    gStyle->SetOptStat(0);
    TCanvas c("c_avg_pad_map", "Average pad occupancy", 1200, 650);
    c.SetRightMargin(0.15);
    h.Draw("COLZ");
    c.SaveAs((outPrefix + "_avg_pad_map.png").c_str());
}

void saveDensityHist(const std::vector<EventPoints>& events,
                     const std::string& outPrefix,
                     int medpes) {
    std::vector<double> counts;
    for (const auto& ev : events) {
        std::map<int, int> byBin;
        for (std::size_t i = 0; i < ev.globalPadZ.size(); ++i) {
            const int key = (medpes == 1) ? ev.globalPadZ[i] : ev.globalPadY[i];
            byBin[key] += 1;
        }
        for (const auto& [_, count] : byBin) {
            if (count > 0) counts.push_back(count);
        }
    }
    if (counts.empty()) return;

    const double lo = std::max(0.0, *std::min_element(counts.begin(), counts.end()) - 1.0);
    const double hi = *std::max_element(counts.begin(), counts.end()) + 1.0;
    TH1F h("density_counts", "Drifted endpoint counts per occupied pad row/column",
           100, lo, hi);
    h.GetXaxis()->SetTitle(medpes == 1 ? "endpoints per global Z pad column" :
                                           "endpoints per global Y pad row");
    h.GetYaxis()->SetTitle("entries");
    for (const auto x : counts) h.Fill(x);

    TCanvas c("c_density", "Density", 800, 600);
    h.Draw();
    c.SaveAs((outPrefix + "_density_counts.png").c_str());
}

std::vector<double> buildFitPoints(const EventPoints& ev, int medpes,
                                   std::vector<double>& coord,
                                   std::vector<double>& y,
                                   std::vector<double>& err,
                                   double vdMmPerNs,
                                   int plane) {
    coord.clear();
    y.clear();
    err.clear();

    const int nBins = (medpes == 1) ? kGlobalPadsZ : kGlobalPadsY;
    std::vector<double> sumZ(nBins, 0.0);
    std::vector<double> sumY(nBins, 0.0);
    std::vector<double> sumT(nBins, 0.0);
    std::vector<int> count(nBins, 0);

    for (std::size_t i = 0; i < ev.z.size(); ++i) {
        const int bin = (medpes == 1) ? ev.globalPadZ[i] : ev.globalPadY[i];
        if (bin < 0 || bin >= nBins) continue;
        sumZ[bin] += ev.z[i];
        sumY[bin] += ev.y[i];
        sumT[bin] += ev.t[i];
        count[bin] += 1;
    }

    for (int bin = 0; bin < nBins; ++bin) {
        if (count[bin] <= 0) continue;
        const double mz = sumZ[bin] / count[bin];
        const double my = sumY[bin] / count[bin];
        const double mt = sumT[bin] / count[bin];
        const double x = mt * vdMmPerNs;
        coord.push_back(plane == 0 ? mz : x);
        y.push_back(my);
        err.push_back(medpes == 1 ? kPadPitchYMm / std::sqrt(12.0) :
                                    kPadPitchZMm / std::sqrt(12.0));
    }
    return coord;
}

bool fitLine(const std::vector<double>& x,
             const std::vector<double>& y,
             double& slope,
             double& intercept) {
    if (x.size() < 2 || x.size() != y.size()) return false;
    const double mx = mean(x);
    const double my = mean(y);
    double sxx = 0.0;
    double sxy = 0.0;
    for (std::size_t i = 0; i < x.size(); ++i) {
        sxx += (x[i] - mx) * (x[i] - mx);
        sxy += (x[i] - mx) * (y[i] - my);
    }
    if (sxx <= 0.0) return false;
    slope = sxy / sxx;
    intercept = my - slope * mx;
    return true;
}

Summary analyze(const std::vector<EventPoints>& events,
                const std::string& outPrefix,
                int medpes,
                int plane,
                double vdMmPerNs,
                double residualPlotRangeMm,
                double residualFitRangeMm) {
    Summary summary;
    summary.nEvents = static_cast<int>(events.size());

    std::vector<double> endpointsPerEvent;
    std::vector<double> densityPerEvent;
    std::vector<double> residuals;
    std::vector<double> coord;
    std::vector<double> y;
    std::vector<double> err;

    for (const auto& ev : events) {
        endpointsPerEvent.push_back(static_cast<double>(ev.z.size()));
        if (!ev.z.empty()) {
            const auto [zMin, zMax] = std::minmax_element(ev.z.begin(), ev.z.end());
            const double lengthCm = std::max(1.0e-9, (*zMax - *zMin) / 10.0);
            densityPerEvent.push_back(ev.z.size() / lengthCm);
        }

        buildFitPoints(ev, medpes, coord, y, err, vdMmPerNs, plane);
        double slope = 0.0;
        double intercept = 0.0;
        if (!fitLine(coord, y, slope, intercept)) continue;
        for (std::size_t i = 0; i < coord.size(); ++i) {
            const double r = (y[i] - (slope * coord[i] + intercept)) /
                             std::sqrt(1.0 + slope * slope);
            residuals.push_back(r);
        }
    }

    summary.meanEndpoints = mean(endpointsPerEvent);
    summary.meanDensityPerCm = mean(densityPerEvent);
    summary.nFitPoints = static_cast<int>(residuals.size());
    summary.residualRmsMm = rms(residuals);

    if (!residuals.empty()) {
        const auto [rMinIt, rMaxIt] = std::minmax_element(residuals.begin(), residuals.end());
        const double maxAbs = std::max(std::abs(*rMinIt), std::abs(*rMaxIt));
        const double fitRange = residualFitRangeMm > 0.0 ? residualFitRangeMm : std::max(1.0, maxAbs);
        const double plotRange = residualPlotRangeMm > 0.0 ? residualPlotRangeMm : 3.0;

        TH1F hFit("residuals_fit", "Track residuals;residual [mm];entries", 120, -fitRange, fitRange);
        for (const auto r : residuals) {
            if (std::abs(r) <= fitRange) hFit.Fill(r);
        }
        TF1 gaus("gaus", "gaus", -fitRange, fitRange);
        gaus.SetParameters(hFit.GetMaximum(), 0.0, std::max(1.0e-3, hFit.GetRMS()));
        hFit.Fit(&gaus, "Q0S");

        TF1* gFit = hFit.GetFunction("gaus");
        if (gFit) {
            auto* g = gFit;
            summary.residualSigmaMm = g->GetParameter(2);
            summary.residualSigmaErrMm = g->GetParError(2);
        }

        TH1F hPlot("residuals", "Track residuals;residual [mm];entries", 120, -plotRange, plotRange);
        for (const auto r : residuals) hPlot.Fill(r);
        TCanvas c("c_residuals", "Residuals", 800, 600);
        hPlot.Draw();
        if (gFit) {
            auto* gPlot = static_cast<TF1*>(gFit->Clone("gaus_full_residual_fit"));
            const double fitBinWidth = 2.0 * fitRange / 120.0;
            const double plotBinWidth = 2.0 * plotRange / 120.0;
            if (fitBinWidth > 0.0) {
                gPlot->SetParameter(0, gPlot->GetParameter(0) * plotBinWidth / fitBinWidth);
            }
            gPlot->SetRange(-plotRange, plotRange);
            gPlot->Draw("same");
        }
        c.SaveAs((outPrefix + "_residuals.png").c_str());
    }

    return summary;
}

} // namespace

int main(int argc, char** argv) {
    if (argc < 3) {
        std::cerr
            << "Usage: " << argv[0]
            << " input.root output_prefix [medpes=1] [plane=0] [vd_mm_per_ns=0.0801] [residual_plot_range_mm=3.0] [residual_fit_range_mm=0]\n"
            << "  medpes=1 averages by global Z pad columns, suitable for horizontal Z tracks.\n"
            << "  plane=0 fits y(z); plane=1 fits y(x=t*vd).\n"
            << "  residual_plot_range_mm fixes the residual histogram x axis to +/- this value.\n"
            << "  residual_fit_range_mm fixes the Gaussian fit range to +/- this value; 0 uses the full data range.\n";
        return 1;
    }

    const std::string input = argv[1];
    const std::string outPrefix = argv[2];
    const int medpes = (argc > 3) ? std::atoi(argv[3]) : 1;
    const int plane = (argc > 4) ? std::atoi(argv[4]) : 0;
    const double vdMmPerNs = (argc > 5) ? std::atof(argv[5]) : 0.0801;
    const double residualPlotRangeMm = (argc > 6) ? std::atof(argv[6]) : 3.0;
    const double residualFitRangeMm = (argc > 7) ? std::atof(argv[7]) : 0.0;

    try {
        auto events = loadEvents(input);
        auto summary = analyze(events, outPrefix, medpes, plane, vdMmPerNs,
                               residualPlotRangeMm, residualFitRangeMm);
        saveDensityHist(events, outPrefix, medpes);
        saveAveragePadMap(events, outPrefix, "Average drifted endpoints per HAT pad");

        std::cout << "Input: " << input << "\n";
        std::cout << "Events: " << summary.nEvents << "\n";
        std::cout << "Mean drifted endpoints/event: " << summary.meanEndpoints << "\n";
        std::cout << "Mean drifted endpoints/cm/event: " << summary.meanDensityPerCm << "\n";
        std::cout << "Fit points in residual histogram: " << summary.nFitPoints << "\n";
        std::cout << "Residual RMS [mm]: " << summary.residualRmsMm << "\n";
        std::cout << "Residual Gaussian sigma [mm]: " << summary.residualSigmaMm
                  << " +/- " << summary.residualSigmaErrMm << "\n";
        std::cout << "Wrote plots with prefix: " << outPrefix << "\n";
    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << "\n";
        return 2;
    }

    return 0;
}
