#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TH2F.h>
#include <TH1F.h>
#include <TF1.h>
#include <TMath.h>
#include <TSpectrum.h>
#include <TGraph.h>
#include <fstream>

void AngularDist(int mode, double Ex, double dEx = 0.2, int bin = 80) {   
   //mode == 0; all detector
   //mode == 1; individual detector
   
/**///======================================================== initial input
   
   const char* rootfile0="~/ANALYSIS/H060_ana/C_gen_run11.root"; const char* treeName0="tree";
   const char* rootfile1="~/ANALYSIS/Simulation/transfer.root"; const char* treeName1="tree";
   
   double zRange[3] = {400, -450, -100};
   
   //setting for simulation
   double thetaCMLow = 10; //deg
   double dEx_sim = 0.05; // MeV
   
    
/**///========================================================  load tree

   TFile *f0 = new TFile (rootfile0, "read"); 
   TTree *tree0 = (TTree*)f0->Get(treeName0);
   printf("=====> /// %20s //// is loaded. Total #Entry: %10d \n", rootfile0,  tree0->GetEntries());
   
   TFile *f1 = new TFile (rootfile1, "read"); 
   TTree *tree1 = (TTree*)f1->Get(treeName1);
   printf("=====> /// %20s //// is loaded. Total #Entry: %10d \n", rootfile1,  tree1->GetEntries());
   
/**///======================================================== Browser or Canvas

   //TBrowser B ;   
   Int_t Div[2] = {1,1};  //x,y
   Int_t size[2] = {800,600}; //x,y
   
   TCanvas * cAngularDist = new TCanvas("cAngularDist", "cAngularDist", 0, 0, size[0]*Div[0], size[1]*Div[1]);
   cAngularDist->Divide(Div[0],Div[1]);
   for( int i = 1; i <= Div[0]*Div[1] ; i++){
      cAngularDist->cd(i)->SetGrid();
   }
   cAngularDist->cd(1);
   gStyle->SetOptStat(1111111);
   gStyle->SetStatY(0.8);
   gStyle->SetStatX(0.99);
   gStyle->SetStatW(0.2);
   gStyle->SetStatH(0.1);
   
/**///========================================================= load files

/**///========================================================= Analysis

   TString expression, gate_sim, gate_exp, gate_det;
   gate_sim.Form("loop == 1 && TMath::Abs(Ex-%f) < %f && thetaCM > %f && hit == 1", Ex, dEx_sim, thetaCMLow);      

   printf("============================ Ex : %f\n", Ex);
   
   TH1F* h = new TH1F("h", "thetaCM", 500, 0, 50);
   TH2F* h2 = new TH2F("h2", "thetaCMvZ", zRange[0], zRange[1], zRange[2], 500, 0, 50); 
   TH1F* w = new TH1F("w", "Z_th", zRange[0], zRange[1], zRange[2]);
   TH1F* k = new TH1F("k", "Z_exp", bin, zRange[1], zRange[2]); 

   vector<double> angle;
   vector<double> meanAngle;
   vector<double> dCos; 
   
   for( int i = 0; i < 6; i++){
      //printf("--------- detID == %d \n", i);
      gate_det.Form("&& detID%6 == %d", i);
      
      //============= get the acceptance from simulation
      tree1->Draw("thetaCM >> h", gate_sim + gate_det);
      tree1->Draw("thetaCM : z >> h2", gate_sim + gate_det);
      
      if( h->GetEntries() == 0){
         printf(" no data for detID == %d \n", i);
         continue;
      }
      //============== find the acceptance, the angle the count drop
      angle.clear();
      for(int j = 1 ; j < 500; j++){
         int a = h->GetBinContent(j);
         int b = h->GetBinContent(j+1);
         
         if( a == 0 && b > 0) angle.push_back(h->GetBinLowEdge(j+1));
         if( a > 0 && b == 0) angle.push_back(h->GetBinLowEdge(j+1));
      }
      
      if( angle.size() == 4){
         angle.erase(angle.begin(), angle.begin()+2);
      }
            
      //============ print angle
      meanAngle.clear();
      dCos.clear();
      for( int j = 0; j < angle.size()/2; j++){
         double delta = angle[2*j+1] - angle[2*j];
         meanAngle = (angle[2*j+1] + angle[2*j])/2;
         if ( delta < 2. ) continue;
         dCos.push_back(TMath::Sin(mean*TMath::DegToRad())*(delta*TMath::DegToRad()));
         printf("%d, %10.5f - %10.5f = %10.5f | %10.5f, %10.5f \n", 
                    j,
                    angle[2*j], 
                    angle[2*j+1], 
                    delta,
                    mean,
                    1./TMath::Sin(mean*TMath::DegToRad())*(delta*TMath::DegToRad()));
      }
      
   }
   
   //====== get Ex -z gate from simulation
   
   
   //========== for experimental data
   gROOT->ProcessLine(".L /home/ttang/ANALYSIS/Calibration/Count.C+");
   
   gate_exp.Form("TMath::Abs(Ex-%4.1f)<%f", Ex, dEx);
   
   
   //========== if Xsec exist.
   
   
}

