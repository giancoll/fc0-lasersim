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

#include "tlsv2.h"
#include "delta.h"

int main () {
	
	//std::vector<std::string> names = {"ev75grad","ev60grad","ev45grad","ev30grad","ev15grad"};
	//std::vector<std::string> names = {"ev31","ev29","ev30","ev12", "ev13","ev15"};
	//std::vector<std::string> names = {"ev22", "ev18", "ev17", "ev6", "ev16", "ev19", "ev20"};
	std::vector<std::string> names = { "ev6", "ev5","ev3", "ev1", "ev2", "ev4", "ev7"};
	
	
	std::vector<int> medpes (7,0) ; // 0 media per righe, 1 media per colonne
	std::vector<float> iniz(7,0); // 0 verticali, 1 angolate, 2 orizzontali
	std::vector<float> piano(7,1); // 0 piano (zy), 1 piano (xy)
	
	
	//std::vector<float> val = {800, 400, 100, 0, -100, -400, -600}; // corrisponde a c, quindi - la mia retta, come su tlsv2.cc
	//std::vector<float> val = {300, 200, 100, 0, -100, -200};
	std::vector<float> val = {-187, -287, -387, -487, -587, -687,-787};
	//std::vector<float> val= {-200,-800};//,-387,-487,-587,-687,-787};
	
	
	std::vector<double> ris; std::vector<double> err;
	/*
	for (int i=0; i<names.size();i++){
		delta(names[i],medpes[i],val[i]);} \\filtro geometrico
	*/
	for (int i=0; i<names.size();i++){
		std::vector<double> v = tlsv2(names[i],medpes[i], iniz[i], val[i],piano[i]);
		ris.push_back(v[0]);
		err.push_back(v[1]);
	}
	
	for (auto x: ris) std::cout << x <<", ";
	std::cout << std::endl;
	for (auto x: err) std::cout << x <<", ";
	
	return 1;
}
