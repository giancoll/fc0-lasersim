#include <TFile.h>
#include <TTree.h>
#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <string>
#include <TGraph2D.h>
#include <TGraphErrors.h>
#include <TCanvas.h>
#include "TH1F.h"
#include "TF1.h"
#include <string>

#include <Math/Functor.h> 
#include <Fit/Fitter.h>
#include "TLegend.h"

	    using matf = std::vector<std::vector<float>>;
		using mati = std::vector<std::vector<int>>;

        constexpr int kHatPadsZPerERAM = 36;
        constexpr int kHatPadsYPerERAM = 32;
        constexpr int kHatERAMsPerRow = 4;
        constexpr int kHatRows = 2;
        constexpr int kHatGlobalPadsZ = kHatPadsZPerERAM * kHatERAMsPerRow;
        constexpr int kHatGlobalPadsY = kHatPadsYPerERAM * kHatRows;
        constexpr float kHatPadWidthZMm = 11.16;
        constexpr float kHatPadHeightYMm = 10.05;

        inline bool globalHatPadIndex(int eram, int localPadZ, int localPadY,
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
	
ROOT::Fit::FitResult fit (std::vector<float> z1, std::vector<float> y1, std::vector<float> err1, float val, float iniz) {
    ROOT::Fit::Fitter fitter;
	std::function<double(const double*)> chi2 = [&] (const double* p) {
		double a=p[0]; double b=p[1]; double c=p[2];
		double res = 0;
		int dim = z1.size();
		for (int i = 0; i<dim; i++){
			res += (a*y1[i] + b*z1[i] +c )*(a*y1[i] + b*z1[i] +c )/((a*a + b*b)*(err1[i]*err1[i])); //
		}
		return res;
	};

	//fitter.SetFunction(ROOT::Math::WrappedMultiTF1(chi2, chi2.GetNdim()), False);
	ROOT::Math::Functor f(chi2, 3);
	fitter.SetFCN(f);
	fitter.Config().ParSettings(0).SetName("a");
    fitter.Config().ParSettings(1).SetName("b");
    fitter.Config().ParSettings(2).SetName("c");
    if (iniz == 0) {
		fitter.Config().ParSettings(0).SetValue(0);
		fitter.Config().ParSettings(1).SetValue(1);
		fitter.Config().ParSettings(2).SetValue(val);}
    if (iniz == 1) {
    	TGraph gr(z1.size(), z1.data(), y1.data());
		TF1 ff("f", "pol1");
		int status = gr.Fit(&ff, "Q");
		if (status != 0) {
   	 		std::cout << "Fit failed"<<std::endl;
		}
		double aa = ff.GetParameter(0);
		double bb = ff.GetParameter(1);
		fitter.Config().ParSettings(0).SetValue(1);
		fitter.Config().ParSettings(1).SetValue(-bb);
		fitter.Config().ParSettings(2).SetValue(-aa);}
    if (iniz == 2) {
		fitter.Config().ParSettings(0).SetValue(1);
		fitter.Config().ParSettings(1).SetValue(0);
		fitter.Config().ParSettings(2).SetValue(val);}
	fitter.Config().MinimizerOptions().SetMinimizerType("Minuit2");
	fitter.FitFCN();
	ROOT::Fit::FitResult result = fitter.Result();
	return result;
}


//funzione per fare una media sulle righe di pad. In input viene data le varie matrici con i dati ottenuti dalla media per pad
//e i vettori per i risultati finali
//vengono calcolati anche gli errori
void compute1 (int ny, int nz, const std::vector<std::vector<float>>& y, const std::vector<std::vector<float>>& z, const std::vector<std::vector<float>>& t, const std::vector<std::vector<int>>& count, std::vector<float>& y1, std::vector<float>& z1, std::vector<float>& t1, std::vector<float>& ey, std::vector<float>& ez, std::vector<float>& et, std::vector<float>& ex, float vd){
	for (int j = 1; j < (ny-1); j++) {

		float sy = 0; float sz = 0; float n=0; float st = 0;
		float sy2 = 0; float sz2 = 0; float st2 = 0;
		int u = 0;
        for (int i = 0; i < nz; i++) {
            if (count[i][j] != 0) { //controllo per sommare solo pad colpiti
            	sy += (y[i][j]*count[i][j]);
            	sz += (z[i][j]*count[i][j]);
            	st += (t[i][j]*count[i][j]);
            	n += count[i][j];	
            	u +=1;
            	}
        }
        float meanz = sz/n; float meany = sy/n; float meant = st/n;
        //calcolo della dev standard
        for (int i = 0; i < nz; i++) {
            if (count[i][j] != 0) {
            	sy2 += (y[i][j]-meany)*(y[i][j]-meany);
            	sz2 += (z[i][j]-meanz)*(z[i][j]-meanz);
            	st2 += (t[i][j]-meant)*(t[i][j]-meant);
            	}
        }
        if (sz != 0&& u!=1 ){
        z1.push_back(meanz); y1.push_back(meany); t1.push_back(meant);
        float a = sqrt(sz2/((u-1)*u)); ez.push_back(a);
        float b = sqrt(sy2/((u-1)*u)); ey.push_back(b);
        float c = sqrt(st2/((u-1)*u)); et.push_back(c); ex.push_back(c*vd);}
        
	        if (sz != 0 && u==1 ){
	        z1.push_back(meanz); y1.push_back(meany); t1.push_back(meant);
	        float a = kHatPadWidthZMm/sqrt(12); ez.push_back(a);
	        float b = kHatPadHeightYMm/sqrt(12); ey.push_back(b);
	        float c = sqrt(st2/((n-1)*n)); et.push_back(c); ex.push_back(c*vd);}
    }
}
//conto analogo ma in questo caso faccio la media per colonna
void compute2 (int ny, int nz, const std::vector<std::vector<float>>& y, const std::vector<std::vector<float>>& z, const std::vector<std::vector<float>>& t, const std::vector<std::vector<int>>& count, std::vector<float>& y1, std::vector<float>& z1, std::vector<float>& t1, std::vector<float>& ey, std::vector<float>& ez, std::vector<float>& et, std::vector<float>& ex, float vd){
	for (int j = 1; j < (nz-1); j++) { 
		float sy = 0; float sz = 0; float n=0; float st = 0;
		float sy2 = 0; float sz2 = 0; float st2 = 0;
		int u = 0;
        for (int i = 0; i < ny; i++) {
            if (count[j][i] != 0) {
            	sy += (y[j][i]*count[j][i]);
            	sz += (z[j][i]*count[j][i]);
            	st += (t[j][i]*count[j][i]);
            	n += count[j][i];
            	u += 1;
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
        if (sz != 0 && u!=1 ){
        z1.push_back(meanz); y1.push_back(meany); t1.push_back(meant);
        float a = sqrt(sz2/((u-1)*u)); ez.push_back(a);
        float b = sqrt(sy2/((u-1)*u)); ey.push_back(b);
        float c = sqrt(st2/((u-1)*u)); et.push_back(c); ex.push_back(c*vd);}
        
	        if (sz != 0 && u==1){
	        z1.push_back(meanz); y1.push_back(meany); t1.push_back(meant);
	        float a = kHatPadWidthZMm/sqrt(12); ez.push_back(a);
	        float b = kHatPadHeightYMm/sqrt(12); ey.push_back(b);
	        float c = sqrt(st2/((n-1)*n)); et.push_back(c); ex.push_back(c*vd);}
    }
}

std::vector<double> tlsv2 (std::string filename,int par1, int par2, int par3, int par4) {
    //TFile *file = TFile::Open("dati_matteo/ev22.root");
    //TFile *file = TFile::Open("filt/ev22t.root");
    
    TFile *file = TFile::Open(("filt/"+filename+".root").c_str());
    TTree *tree = (TTree*)file->Get("anode");
	//std::ofstream outfile("val_matrici.txt");
	
	int medpes = par1; 
	int iniz = par2; 
	int val = par3;
	int piano = par4;
    
 
    //per non ripetere spesso parti di codici, ho creato questa dichiarazione
    using matf = std::vector<std::vector<float>>;
	using mati = std::vector<std::vector<int>>;

	//matrici per sommare per pad le 3 variabili z y t e una per tenere il conteggio degli elettroni per singolo evento
	    int ny = kHatGlobalPadsY; int nz = kHatGlobalPadsZ; //numero di pad in y e in z 
    matf sz(nz, std::vector<float>(ny, 0));
    matf sy(nz, std::vector<float>(ny, 0));
    matf st(nz, std::vector<float>(ny, 0));
    mati count(nz, std::vector<int>(ny, 0));
    //matrici dati finali per fit per i 100 eventi diversi
    matf all_y1, all_z1, all_t1, all_x1;
    matf all_ey, all_ez, all_et, all_ex;
    
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
	
	//n = 1; 
	
	//loop sugli eventi
	for (int ev=0; ev<(n); ev++) {
		//riazzero i vettori somma
		for (int i = 0; i < nz; i++) {
            for (int j = 0; j < ny; j++) {
                sz[i][j] = 0;
                sy[i][j] = 0;
                st[i][j] = 0;
                count[i][j] = 0;
            }
        }
        //riempio le matrici
        tree->GetEntry(ev);
	        int m = valuez->size();
	        for (int j=0; j<m; j++) {
	            int zi = -1;
	            int yi = -1;
	            if (!globalHatPadIndex(eram->at(j), padz->at(j), pady->at(j), zi, yi)) continue;
	            //std::cout << zi << " ";
            sz[zi][yi] += valuez->at(j);
            sy[zi][yi] += valuey->at(j);
            st[zi][yi] += valuet->at(j);
            count[zi][yi] += 1;
        }

    //matrici con la media di ogni singolo pad
    matf z(nz, std::vector<float>(ny, 0));
	matf x(nz, std::vector<float>(ny, 0));
	matf y(nz, std::vector<float>(ny, 0));
	matf t(nz, std::vector<float>(ny, 0));
	
	//calcolo della media per pad
	for (int i = 0; i < (nz); i++) {
    	for (int j = 0; j < ny; j++) {
        	if (count[i][j] != 0) {
        		z[i][j] = sz[i][j] / count[i][j];
        		y[i][j] = sy[i][j] / count[i][j];
        		t[i][j] = st[i][j] / count[i][j];
        		//std::cout << y[i][j] << " ";
        	}
    	}
	}
	
	//vettori ausiliari dove inserisco il risultato della media pesata
	std::vector<float> z1, y1 , ez, ey, t1, et, x1, ex;

	//richiamo delle funzione per la media pesata a seconda se per riga o per colona
	if (medpes == 0){
		compute1 (ny, nz, y, z, t,count,y1,z1,t1,ey,ez,et,ex,vd);}
    
    if (medpes == 1){
    	compute2 (ny, nz, y, z, t,count,y1,z1,t1,ey,ez,et,ex,vd);}
    
	int k = t1.size();
	for (int i = 0; i<(k); i++){
		x1.push_back(t1[i]*vd);
	}
	
    all_y1.push_back(y1); all_z1.push_back(z1); all_t1.push_back(t1); all_x1.push_back(x1);
    all_ey.push_back(ey); all_ez.push_back(ez); all_et.push_back(et); all_ex.push_back(ex);
	}
	

	//vettori per l'itercetta e i residui
    std::vector<float> intercetta,res, chi2rid,sigres,numev;
	//std::cout << std::endl << " eventi non fittati " << std::endl;
	
	float numb=4;
	
	
	std::vector<float> residuals;
	
	//calcolo per i 100 eventi del fit
	for (int ev = 0; ev < (n); ev++) {
		ROOT::Fit::FitResult fit1;
		if (piano == 0){
		if (medpes==0){
			fit1 = fit(all_z1[ev],all_y1[ev],all_ez[ev], val, iniz);}
		if (medpes==1){
			fit1 = fit(all_z1[ev],all_y1[ev],all_ey[ev], val, iniz);}	}
		if (piano == 1) {
			fit1 = fit(all_x1[ev],all_y1[ev],all_ex[ev], val, iniz);
		}
		float a = fit1.Parameter(0); float b = fit1.Parameter(1); float c = fit1.Parameter(2);
		int dim = all_z1[ev].size();
		int enter = 0;
		for (int i=0; i<dim; i++){
			float r;
			if (piano == 0) {
				r = (a*all_y1[ev][i] + b*all_z1[ev][i] +c )/sqrt(a*a + b*b);}
			if (piano == 1){
				r = (a*all_y1[ev][i] + b*all_x1[ev][i] +c )/sqrt(a*a + b*b);}
			residuals.push_back(r);
		}	
	}
	
//	for (auto x : res) {
//		outfile << x << "  ";
//	}

	//std::cout << residuals.size()<< std::endl;

	float max1 = residuals[0];
	float min1 =residuals[0];
	for (float x : residuals) {
    	if (x > max1) max1 = x;
    	if (x < min1) min1 = x;
	}
	
	//min1=-1.5; max1=1.5;
	//std::cout << min1 << " " << max1;
   
    //riempimento dell'istogramma
    TCanvas *c2 = new TCanvas("c2", "Residui", 600, 500);
	c2->cd();
	//c2->SetLogy();
	TH1F *h1 = new TH1F("h1", "Residui", 100,min1,max1);
	h1->GetXaxis()->SetTitle("Residuo [mm]");
	h1->GetYaxis()->SetTitle("Conteggi");
	for (float x : residuals) { h1->Fill(x);}
	h1->Draw();
	double std_res = h1->GetRMS();
	//std::cout << std::endl;
	
	h1->Fit("gaus","Q");
	TF1 *g = h1->GetFunction("gaus");/*
	double A     = g->GetParameter(0);
	double mean  = g->GetParameter(1);
	double sigma = g->GetParameter(2);
	TF1 *f = new TF1("f", "[0]*exp(-0.5*((x-[1])/[2])^2) + " "[3]*exp(-0.5*((x-[1])/[4])^2)", min1, max1);
	f->SetParameters(A,mean,sigma,A*0.2,sigma*3);
	
	h1->Fit(f,"rQ");
	TF1 *g1 = new TF1("g1", "[0]*exp(-0.5*((x-[1])/[2])^2)", min1, max1);
	g1->SetParameters( f->GetParameter(0), f->GetParameter(1), f->GetParameter(2) );
	// Wide Gaussian
	TF1 *g2 = new TF1("g2", "[0]*exp(-0.5*((x-[1])/[2])^2)", min1, max1);
	g2->SetParameters( f->GetParameter(3), f->GetParameter(1), f->GetParameter(4) );*/

	//-----------------------file-----------------------------
	std::string basePath = "res_plot/" + filename;
	
	
	// =====================================================
	// Style
	// =====================================================

	g->SetLineColor(kRed);
	g->SetLineWidth(2);
/*
	g1->SetLineColor(kBlue);
	g1->SetLineStyle(2);
	g1->SetLineWidth(2);

	g2->SetLineColor(kGreen+2);
	g2->SetLineStyle(2);
	g2->SetLineWidth(2);*/

	// =====================================================
	// Draw everything
	// =====================================================

	h1->Draw();

	g->Draw("same");
	//g1->Draw("same");
	//g2->Draw("same");

	// =====================================================
	// Legend
	// =====================================================
/*
	TLegend *leg = new TLegend(0.12,0.70,0.40,0.88);

	leg->AddEntry(h1,  "Data", "l");
	leg->AddEntry(f,  "Total fit", "l");
	leg->AddEntry(g1, "Narrow Gaussian", "l");
	leg->AddEntry(g2, "Wide Gaussian", "l");

	leg->Draw();
	*/
	c2->Update();
	c2->SaveAs((basePath + ".png").c_str());
	
	double sigma_err = g->GetParError(2);
	double sigma_val = g->GetParameter(2);
	std::vector<double> results;
	results.push_back(sigma_val); results.push_back(sigma_err);
	delete c2;
	return results;
	
	
}
