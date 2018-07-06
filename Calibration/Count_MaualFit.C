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

TString gate, gate_cm, gate_Aux;

int nPeaks = 16;

Double_t fpeaks(Double_t *x, Double_t *par) {
   Double_t result = 0;
   for (Int_t p=0;p<nPeaks;p++) {
      Double_t norm  = par[3*p+0];
      Double_t mean  = par[3*p+1];
      Double_t sigma = par[3*p+2];
      result += norm * TMath::Gaus(x[0],mean,sigma, 1);
   }
   return result;
}

void Count_MaualFit(TTree * tree, int detID = -1, int splitCtrl = 0, double threshold = 0.1){   

/**///======================================================== initial input
   
   double ExRange[3] = {200, -1, 6};
   
   /*// for H052
   gate_Aux = " && z > -629 + 32 * Ex + 0.67 * Ex * Ex ";
   gate_cm = "&& 50 > thetaCM && thetaCM > 0 ";
   */
   bool isH052 = false;
   
   // for H060 
   gate = "hitID >= 0 && e > 200 && 5 > detID%6 && detID%6 > 0";
   gate_cm = "";
   gate_Aux = "";
   
   bool isFixMean = false ;
   
   vector<double> knownE;
   knownE.push_back(0);
   knownE.push_back(0.779);
   knownE.push_back(1.5671);
   knownE.push_back(2.0322);
   knownE.push_back(2.5000);
   
   double avgPeakHeigh = 100.;
   
   double backgroundFactor = 11; // smaller = more curve 
   
/**///========================================================  load tree

   printf("=============>  Total #Entry: %10d \n", tree->GetEntries());

/**///======================================================== Browser or Canvas

   //TBrowser B ;   
   Int_t Div[2] = {1,2};  //x,y
   Int_t size[2] = {800,300}; //x,y
   
   TCanvas * cCount = new TCanvas("cCount", "cCount", 0, 0, size[0]*Div[0], size[1]*Div[1]);
   cCount->Divide(Div[0],Div[1]);
   for( int i = 1; i <= Div[0]*Div[1] ; i++){
      cCount->cd(i)->SetGrid();
   }
   cCount->cd(1);

   gStyle->SetOptStat(1111111);
   gStyle->SetStatY(0.8);
   gStyle->SetStatX(0.99);
   gStyle->SetStatW(0.2);
   gStyle->SetStatH(0.1);
   
/**///========================================================= load files
   
/**///========================================================= Analysis

   TH1F * spec  = new TH1F("spec" , "spec"  , ExRange[0], ExRange[1], ExRange[2]);
   spec ->SetXTitle("Ex [MeV]");   
   TString ytitle;
   ytitle.Form("Count / %6.2f keV", (ExRange[2] - ExRange[1])/ExRange[0] * 1000.);
   spec ->SetYTitle(ytitle);
   
   //========define gate_x
   TString gate_x;
   if( splitCtrl == 0 ) {
      gate_x.Form("&& -1.5 < x && x < 1.5"); 
   }else if( splitCtrl == 1) {
      gate_x.Form("&& x < 0");
   }else if( splitCtrl == 2) {
      gate_x.Form("&& x > 0");
   }else if( splitCtrl == 3) {
      gate_x.Form("&& x < -1./3.");
   }else if( splitCtrl == 4) {
      gate_x.Form("&& x > 1./3.");
   }
   
   //===== exclude defected detectors
   if( isH052 ){
      if( detID == 0){
         gate.Form("good == 1 && det%6 == %d && det != 18 && TMath::Abs(t4)<1000", detID);
      }
      if( detID == 5){
         gate.Form("good == 1 && det%6 == %d && det != 11 && TMath::Abs(t4)<1000", detID);
      }
   }   
   
   if( detID >= 0 ){
      gate_Aux.Form("&& detID%6 == %d", detID);  
   }
   
   printf("gate        : %s\n", gate.Data());
   printf("gate_thetCM : %s\n", gate_cm.Data());
   printf("gate_x      : %s\n", gate_x.Data());
   printf("gate_Aux    : %s\n", gate_Aux.Data());
   printf("threshold   : %f\n", threshold);
   
   //================= plot Ex
   printf("============= plot Ex with gates\n");    
   tree->Draw("Ex>>spec ", gate + gate_x +  gate_cm + gate_Aux, "");
  
   cCount->cd(1);
   spec ->Draw();
   
   //=================== estimate BG
   TH1F * specS = (TH1F*) spec->Clone();
   
   printf("============= estimate background \n");
   TSpectrum * peak = new TSpectrum(50);
   peak->Search(spec, 1, "", threshold);
   TH1 * h1 = peak->Background(spec,backgroundFactor);
   h1->Draw("same");
   
   cCount->cd(2);
   TString title;
   specS->SetName("spectrum");
   specS->Add(h1, -1.);
   specS->Sumw2();
   specS->Draw();
   
   
   //========== Fitting 
   printf("============= Fit..... \n");

   const int nPeaks = knownE.size();
   const int  n = 3*nPeaks;
   double * para = new double[n];
   
   for(int i = 0; i < nPeaks ; i++){
      para[3*i+0] = avgPeakHeigh;
      para[3*i+1] = knownE[i];
      para[3*i+2] = 0.1;
   }

   TF1 * fit = new TF1("fit", fpeaks, ExRange[1], ExRange[2], 3* nPeaks );
   fit->SetNpx(1000);
   fit->SetParameters(para);
   //===fix mean, set limits
   for(int i = 0; i < nPeaks ; i++){
      //fit->SetParLimits(3*i+0, 0, 20000);
      if( isFixMean ) fit->FixParameter(3*i+1, knownE[i]);
      fit->SetParLimits(3*i+2, 0, 3);
   }
   
   specS->Fit("fit", "q");

   printf("============= display\n");   
   const Double_t* paraE = fit->GetParErrors();
   const Double_t* paraA = fit->GetParameters();
   
   double bw = specS->GetBinWidth(1);
   
   double * ExPos = new double[nPeaks];
   
   for(int i = 0; i < nPeaks ; i++){
      ExPos[i] = paraA[3*i+1];
   }
   
   for(int i = 0; i < nPeaks ; i++){
      ExPos[i] = paraA[3*i+1];
      printf("%2d , count: %8.0f(%3.0f), mean: %8.4f(%8.4f), sigma: %8.4f(%8.4f) \n", 
              i, 
              paraA[3*i] / bw,   paraE[3*i] /bw, 
              paraA[3*i+1], paraE[3*i+1],
              paraA[3*i+2], paraE[3*i+2]);
   }
   /**/
}
