#include <TFile.h>
#include <TTree.h>
#include <iostream>
#include <cmath>
#include <vector>
#include <TGraph2D.h>
#include <TGraphErrors.h>
#include <TCanvas.h>
#include "TH1F.h"
#include "TF1.h"
#include <fstream>

constexpr int kHatPadsZPerERAM = 36;
constexpr int kHatPadsYPerERAM = 32;
constexpr int kHatERAMsPerRow = 4;
constexpr int kHatRows = 2;
constexpr int kHatGlobalPadsZ = kHatPadsZPerERAM * kHatERAMsPerRow;
constexpr int kHatGlobalPadsY = kHatPadsYPerERAM * kHatRows;

bool globalHatPadIndex(int eram, int localPadZ, int localPadY,
                       int& globalPadZ, int& globalPadY) {
    if (eram < 0 || eram >= 8) return false;
    if (localPadZ < 0 || localPadZ >= kHatPadsZPerERAM) return false;
    if (localPadY < 0 || localPadY >= kHatPadsYPerERAM) return false;

    const int row = eram / kHatERAMsPerRow;
    const int col = eram % kHatERAMsPerRow;
    globalPadZ = col * kHatPadsZPerERAM + localPadZ;
    globalPadY = row * kHatPadsYPerERAM + localPadY;
    return globalPadZ >= 0 && globalPadZ < kHatGlobalPadsZ &&
           globalPadY >= 0 && globalPadY < kHatGlobalPadsY;
}

void compute1 (int ny, int nz, const std::vector<std::vector<float>>& y, const std::vector<std::vector<float>>& z, const std::vector<std::vector<float>>& t, const std::vector<std::vector<int>>& count, std::vector<float>& y1, std::vector<float>& z1, std::vector<float>& t1, std::vector<float>& ey, std::vector<float>& ez, std::vector<float>& et, std::vector<float>& ex, float vd){
	for (int j = 0; j < ny; j++) { 
		float sy = 0; float sz = 0; float n=0; float st = 0;
		float sy2 = 0; float sz2 = 0; float st2 = 0;	
        for (int i = 0; i < nz; i++) {
            if (count[i][j] != 0) {
            	sy += (y[i][j]*count[i][j]);
            	sz += (z[i][j]*count[i][j]);
            	st += (t[i][j]*count[i][j]);
            	n += count[i][j];	
            	}
        }
        float meanz = sz/n; float meany = sy/n; float meant = st/n;
        for (int i = 0; i < nz; i++) {
            if (count[i][j] != 0) {
            	sy2 += (y[i][j]-meany)*(y[i][j]-meany);
            	sz2 += (z[i][j]-meanz)*(z[i][j]-meanz);
            	st2 += (t[i][j]-meant)*(t[i][j]-meant);
            	}
        }
        if (sz != 0){
        z1.push_back(meanz); y1.push_back(meany); t1.push_back(meant);
        float a = sqrt(sz2/((n-1)*n)); ez.push_back(a);
        float b = sqrt(sy2/((n-1)*n)); ey.push_back(b);
        float c = sqrt(st2/((n-1)*n)); et.push_back(c); ex.push_back(c*vd);}
    }
}

void compute2 (int ny, int nz, const std::vector<std::vector<float>>& y, const std::vector<std::vector<float>>& z, const std::vector<std::vector<float>>& t, const std::vector<std::vector<int>>& count, std::vector<float>& y1, std::vector<float>& z1, std::vector<float>& t1, std::vector<float>& ey, std::vector<float>& ez, std::vector<float>& et, std::vector<float>& ex, float vd){
	for (int j = 0; j < nz; j++) { 
		float sy = 0; float sz = 0; float n=0; float st = 0;
		float sy2 = 0; float sz2 = 0; float st2 = 0;
        for (int i = 0; i < ny; i++) {
            if (count[j][i] != 0) {
            	sy += (y[j][i]*count[j][i]);
            	sz += (z[j][i]*count[j][i]);
            	st += (t[j][i]*count[j][i]);
            	n += count[j][i];	
            	}
        }
        float meanz = sz/n; float meany = sy/n; float meant = st/n;
        for (int i = 0; i < ny; i++) {
            if (count[j][i] != 0) {
            	sy2 += (y[j][i]-meany)*(y[j][i]-meany);
            	sz2 += (z[j][i]-meanz)*(z[j][i]-meanz);
            	st2 += (t[j][i]-meant)*(t[j][i]-meant);
            	}
        }
        if (sz != 0){
        z1.push_back(meanz); y1.push_back(meany); t1.push_back(meant);
        float a = sqrt(sz2/((n-1)*n)); ez.push_back(a);
        float b = sqrt(sy2/((n-1)*n)); ey.push_back(b);
        float c = sqrt(st2/((n-1)*n)); et.push_back(c); ex.push_back(c*vd);}
    }
}

void dedx() {

	//std::ofstream outfile("zi_matrix.txt");
    //TFile *file = TFile::Open("dati_matteo/ev22.root");
    TFile *file = TFile::Open("filt/ev29.root");
    TTree *tree = (TTree*)file->Get("anode");
    
    using matf = std::vector<std::vector<float>>;
	using mati = std::vector<std::vector<int>>;

    int ny = kHatGlobalPadsY; int nz = kHatGlobalPadsZ;
    matf sz(nz, std::vector<float>(ny, 0));
    matf sy(nz, std::vector<float>(ny, 0));
    matf st(nz, std::vector<float>(ny, 0));
    mati count(nz, std::vector<int>(ny, 0));
    
    std::vector<std::vector<float>> all_y1, all_z1, all_t1;
    std::vector<std::vector<float>> all_ey, all_ez, all_et;
    std::vector<std::vector<float>> all_x1, all_ex;
    
    //vettori dati finali per fit
    
    int medpes = 1; // va specificata se 0 media per righe, 1 media per colonne
    
    //vettori dove metterò i dati dal file root  
    //anode
    std::vector<float>* valuez = 0;
    tree->SetBranchAddress("anode_z", &valuez);
    std::vector<int>* padz = 0;
    tree->SetBranchAddress("anode_padZ", &padz);
    std::vector<int>* eram = 0;
    tree->SetBranchAddress("anode_eram", &eram);
    std::vector<float>* valuey = 0;
    tree->SetBranchAddress("anode_y", &valuey);
    std::vector<int>* pady = 0;
    tree->SetBranchAddress("anode_padY", &pady);
    std::vector<float>* valuet = 0;
    tree->SetBranchAddress("anode_t", &valuet);
    
    int n = tree->GetEntries();
	float vd = 0.0801; //velocità di deriva
	
	
	std::vector<float> dedx;
	
	//n = 1; 
	
	for (int ev=0; ev<(n); ev++) {
		for (int i = 0; i < nz; i++) {
            for (int j = 0; j < ny; j++) {
                sz[i][j] = 0;
                sy[i][j] = 0;
                st[i][j] = 0;
                count[i][j] = 0;
            }
        }
        tree->GetEntry(ev);
        int m = valuez->size();
        //std::cout<< m << endl;
        for (int j=0; j<m; j++) {
            int zi = -1;
    		int yi = -1;
            if (!globalHatPadIndex(eram->at(j), padz->at(j), pady->at(j), zi, yi)) continue;
            //std::cout << zi << " ";
            //if (zi <= 0 || zi >= 182 || yi <= 0 || yi >= 77) continue;    
        	//outfile << zi << " ";
            sz[zi][yi] += valuez->at(j);
            sy[zi][yi] += valuey->at(j);
            st[zi][yi] += valuet->at(j);
            count[zi][yi] += 1;
        }
    //outfile.close();


    //vettori con la media di ogni singolo pad
    matf z(nz, std::vector<float>(ny, 0));
	matf x(nz, std::vector<float>(ny, 0));
	matf y(nz, std::vector<float>(ny, 0));
	matf t(nz, std::vector<float>(ny, 0));
	
	 
	for (int i = 0; i < (nz); i++) {
		float sumde=0;
    	for (int j = 0; j < ny; j++) {
        	if (count[i][j] != 0) {
        		z[i][j] = sz[i][j] / count[i][j];
        		y[i][j] = sy[i][j] / count[i][j];
        		t[i][j] = st[i][j] / count[i][j];
        		//std::cout << y[i][j] << " ";
        		if (medpes == 1) sumde += count[i][j];
        	}
    	}
    	dedx.push_back(sumde); 
	}
	
	for (int j = 0; j < ny; j++) {
		float sumde=0;
    	for (int i = 0; i < (nz); i++) {
        	if (count[i][j] != 0) {
        		sumde += count[i][j];
        	}
    	}
    	if (medpes == 0) dedx.push_back(sumde);
	}

	std::vector<float> z1, y1 , ez, ey, t1, et, x1, ex;

	if (medpes == 0){
		compute1 (ny, nz, y, z, t,count,y1,z1,t1,ey,ez,et,ex,vd);}
    
    if (medpes == 1){
    	compute2 (ny, nz, y, z, t,count,y1,z1,t1,ey,ez,et,ex,vd);}
    
	int k = t1.size();
	for (int i = 0; i<k; i++){
		x1.push_back(t1[i]*vd);
	}

    all_y1.push_back(y1); all_z1.push_back(z1); all_t1.push_back(t1);all_x1.push_back(x1);
    all_ey.push_back(ey); all_ez.push_back(ez); all_et.push_back(et);all_ex.push_back(ex);

    
	}
	
	
	float min = *std::min_element(dedx.begin(), dedx.end());
	float max = *std::max_element(dedx.begin(), dedx.end());
	
	TH1F *h = new TH1F("h", "Istogramma conteggi per pad", 501, min,max);

	for (float x : dedx) {
    	if (x!=0){
    		h->Fill(x);}
	}
	
	h->SetTitle("Numero di conteggi per cm");
	h->GetXaxis()->SetTitle("de/dx");
	//h->GetYaxis()->SetTitle("");
	TF1 *fLandau = new TF1("fLandau", "landau", 20, 500);
	fLandau->SetParameter(0, 17000);
	fLandau->SetParameter(1, 50);
	fLandau->SetParameter(2, 10);
	h->Fit(fLandau);
	
	//gStyle->SetOptStat(0);
	//gStyle->SetOptFit(0);
	
	h->Draw();
	
	double A     = fLandau->GetParameter(0);
	double MPV   = fLandau->GetParameter(1);
	double sigma = fLandau->GetParameter(2);

	double MPVerr = fLandau->GetParError(1);
	double chi = fLandau->GetChisquare();
	double num = fLandau->GetNDF();
	std::cout << "MPV = " << MPV << " +/- " << MPVerr << std::endl;
	
	std::cout << "chirid = " << chi/num << std::endl;


}
