#include <TMemoryUsage.hxx>
#include <TND280Log.hxx>
#include "TH1F.h"
#include <algorithm>

namespace {
    struct CmpMemResident {
        /// Comparison operator for selecting which ProcInfo_t has the
        /// large resident memory value.
        bool operator()(ProcInfo_t lhs, ProcInfo_t rhs) {
            return lhs.fMemResident < rhs.fMemResident;
        }
    };
    struct CmpMemVirtual {
        /// Comparison operator for selecting which ProcInfo_t has the
        /// large virtual memory value.
        bool operator()(ProcInfo_t lhs, ProcInfo_t rhs) {
            return lhs.fMemVirtual < rhs.fMemVirtual;
        }
    };
}

ND::TMemoryUsage::TMemoryUsage(): fEnabled(false) {}

ND::TMemoryUsage::~TMemoryUsage() {}

void ND::TMemoryUsage::Enable(bool enable) {
    if (enable) ND280NamedInfo("MEM","Enabling logging of memory usage");
    else ND280NamedInfo("MEM","Disabling logging of memory usage");
    fEnabled = enable;
}

void ND::TMemoryUsage::LogMemory() {
    if (!fEnabled) return;
    ProcInfo_t info;
    gSystem->GetProcInfo(&info);
    fEventMemory.push_back(info);
}

void ND::TMemoryUsage::Write(ND::TND280Output* output) {
    if (!fEnabled) return;

    if (output && output->IsOpen()) {

        ND280NamedInfo("MEM","Writing memory usage histograms to file");
        output->cd();
        
        // Create the histograms we need.
        int numBins = fEventMemory.size();
        TH1F hMemRes("memory_resident", "Resident memory usage", 
                     numBins, 0, numBins);
        hMemRes.GetXaxis()->SetTitle("Event");
        hMemRes.GetYaxis()->SetTitle("Resident memory (GB)");
        
        TH1F hMemVirt("memory_virtual", "Virtual memory usage", 
                      numBins, 0, numBins);
        hMemVirt.GetXaxis()->SetTitle("Event");
        hMemVirt.GetYaxis()->SetTitle("Virtual memory (GB)");
        
        // Convert the vector of memory usage to entries in the
        // histograms. Memory usage is converted from KB to GB
        // for easier human-based parsing of the numbers.
        int bin = 1;
        for (std::vector<ProcInfo_t>::iterator it = fEventMemory.begin(); 
             it != fEventMemory.end(); 
             it++, bin++) {
            ProcInfo_t info = (*it);
            hMemRes.SetBinContent(bin, info.fMemResident / (1024. * 1024.));
            hMemVirt.SetBinContent(bin, info.fMemVirtual / (1024. * 1024.));
        }
        
        hMemRes.Write();
        hMemVirt.Write();
    }
    
    // Log the peak memory usage.
    double maxMemResident = 
        (std::max_element(fEventMemory.begin(), 
                          fEventMemory.end(), 
                          CmpMemResident()))->fMemResident;
    double maxMemVirtual  = 
        (std::max_element(fEventMemory.begin(), 
                          fEventMemory.end(), 
                          CmpMemVirtual()))->fMemVirtual;
    
    ND280NamedLog("MEM","Maximum resident memory usage: " 
                  << (maxMemResident / (1024. * 1024.)) << " GB");
    ND280NamedLog("MEM", "Maximum virtual memory usage:  " 
                  << (maxMemVirtual / (1024. * 1024.))  << " GB");
}
