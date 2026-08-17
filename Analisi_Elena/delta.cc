#include <iostream>
#include <fstream>
#include <vector>
#include <TFile.h>
#include <TTree.h>

void delta() {
    TFile *file = TFile::Open("dati_matteo/ev15.root");
    //TFile *file = TFile::Open("ev15grad_1000.root");
    if (!file || file->IsZombie()) {
        std::cerr << "Errore nell'apertura del file originale!" << std::endl;
        return;
    }
    TTree *tree = (TTree*)file->Get("anode");
    TFile *newfile = TFile::Open("filt/ev15_2.root", "RECREATE");
    newfile->cd();
    TTree *newtree = new TTree("anode", "Tree con punti filtrati in Y");

    //std::ofstream outfile("val_matrici.txt");
    
    int medpes = 1; // 0 media per righe, 1 media per colonne
    float iniz = 2; 
    float val = 200; //qua va messo il valore della retta tipo y = 200, metti 200
    float vd = 0.0801; //0.0801 


    using matf = std::vector<std::vector<float>>;
    using mati = std::vector<std::vector<int>>;

    int ny = 76; int nz = 256; 
    matf sz(nz, std::vector<float>(ny, 0));
    matf sy(nz, std::vector<float>(ny, 0));
    matf st(nz, std::vector<float>(ny, 0));
    mati count(nz, std::vector<int>(ny, 0));
    
    std::vector<float>* valuez = 0;
    std::vector<int>* padz = 0;
    std::vector<float>* valuey = 0;
    std::vector<int>* pady = 0;
    std::vector<float>* valuet = 0;

    tree->SetBranchAddress("anode_z", &valuez);
    tree->SetBranchAddress("anode_padZ", &padz);
    tree->SetBranchAddress("anode_y", &valuey);
    tree->SetBranchAddress("anode_padY", &pady);
    tree->SetBranchAddress("anode_t", &valuet);

    std::vector<float> out_valuez;
    std::vector<int> out_padz;
    std::vector<float> out_valuey;
    std::vector<int> out_pady;
    std::vector<float> out_valuet;

    newtree->Branch("anode_z", &out_valuez);
    newtree->Branch("anode_padZ", &out_padz);
    newtree->Branch("anode_y", &out_valuey);
    newtree->Branch("anode_padY", &out_pady);
    newtree->Branch("anode_t", &out_valuet);

    Long64_t nentries = tree->GetEntries();
    for (Long64_t i = 0; i < nentries; i++) {
        tree->GetEntry(i);
        out_valuez.clear();
        out_padz.clear();
        out_valuey.clear();
        out_pady.clear();
        out_valuet.clear();

	//dentro questo ciclo si possono cambiare i due raggi dell'ellisse

        int n_elements = valuey->size();
        for (int j = 0; j < n_elements; j++) {
        		//float alpha = 75 * M_PI / 180.0;
		        float dist1 = valuet->at(j)-2335; float dist2;
		        if (medpes==1){
		        	dist2 = valuey->at(j)-val;}
		        if (medpes==0){
		        	dist2 = valuez->at(j)-val;
		        }
		        float dist = (dist1*dist1)/(45.0*45.0) + (dist2*dist2)/(5.0*5.0);
		       /*if (valuez->at(j)<= (-890.0+(valuey->at(j)+252.0)*std::tan(alpha)+5.0/std::cos(alpha))  && valuez->at(j)>= (-890.0+(valuey->at(j)+252.0)*std::tan(alpha)-5.0/std::cos(alpha)) &&  valuet->at(j) >= 2300 && valuet->at(j) <= 2370 ){*/
		        if (dist<= (1.0)){
		            out_valuey.push_back(valuey->at(j));
		            out_pady.push_back(pady->at(j));
		            out_valuez.push_back(valuez->at(j));
		            out_padz.push_back(padz->at(j));
		            out_valuet.push_back(valuet->at(j));
		        }
        }
        newtree->Fill();
    }

    newfile->cd();
    newtree->Write();
    
    newfile->Close();
    file->Close(); 

    std::cout << "Filtro geometrico sui punti completato con successo!" << std::endl;
}
