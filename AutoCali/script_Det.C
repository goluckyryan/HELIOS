#include "TFile.h"
#include "TTree.h"
#include "TObjArray.h"
#include "TH2F.h"
#include "TROOT.h"
#include "TStyle.h"
#include "TChain.h"
#include "TCutG.h"
#include "TLegend.h"


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

void script_Det(){
   
   double ExRange[3] = {140, -1, 6};
   double eRange[3]  = {400, 0, 10};
   double thetaCMRange[3]  = {40, 0, 40};

   
   TFile * file0 = new TFile("A_gen_run56_100.root");
   TFile * file1 = new TFile("transfer.root");
   
   TFile * fileCut = new TFile("rdtCuts.root");

   TTree * tree0 = (TTree*) file0->FindObjectAny("tree");
   TTree * tree1 = (TTree*) file1->FindObjectAny("tree");
   
   TObjArray * fxList = (TObjArray*) file1->FindObjectAny("fxList");
   TObjArray * gList = (TObjArray*) file1->FindObjectAny("gList");
   
   TObjArray * cutList = NULL;
   TCutG ** cut = NULL;
   if( fileCut->IsOpen() ){
      TObjArray * cutList = (TObjArray*) fileCut->FindObjectAny("cutList");
      
      const int numCut = cutList->GetEntries();
      cut = new TCutG * [numCut];
      
      printf(" ======== found %d cuts in %s \n", numCut, fileCut->GetName());
      for( int i = 0 ; i < numCut; i++){
         cut[i] = (TCutG* ) cutList->At(i);
         printf("cut name: %s , VarX: %s, VarY: %s\n", cut[i]->GetName(), cut[i]->GetVarX(), cut[i]->GetVarY()); 
      }
   }
   
   //============= Gate
   //TString gate_RDT = "(cut0 || cut1 || cut2 || cut3) && -20 < coin_t && coin_t < 40 && rdtMultiHit == 2 && arrayRDT == 0 && multiHit == 1 ";
   
   TString gate_RDT = "(cut0 || cut1 || cut2 || cut3) && -20 < coin_t && coin_t < 40";
   
   //TString gate_RDT = "cut3 && -20 < coin_t && coin_t < 40 && rdtMultiHit == 2 && arrayRDT == 0 && multiHit == 1 ";
   
   TString detGate = "&& detID != 11 ";
   
   
   //============== Plot
   TCanvas * cScript = new TCanvas("cScript", "cScript", 0, 0, 2400, 1200);
   cScript->Divide(6,4);
   for( int i = 1; i <= 24 ; i++){
      cScript->cd(i)->SetGrid();
   }
   
   gStyle->SetOptStat(0);
   //gStyle->SetOptStat("neiou");
   if(cScript->GetShowEditor() )cScript->ToggleEditor();
   if(cScript->GetShowToolBar())cScript->ToggleToolBar();
   
   
   TH1F ** hEx  = new TH1F*[24];
   TH1F ** specS = new TH1F*[24];
   TH1F ** hBg  = new TH1F*[24];
   double gsSigma[24];
   for( int i = 0; i < 24 ; i++){
      cScript->cd(i+1);
      
      TString titleH;
      titleH.Form("det-%d; Ex [MeV]; Count / %3.0f keV", i,  (ExRange[2] - ExRange[1])*1000/ExRange[0]);
      hEx[i] = new TH1F(Form("hEx%d",i), titleH, ExRange[0], ExRange[1], ExRange[2]);
      hEx[i]->GetXaxis()->SetTitleSize(0.06);
      hEx[i]->GetYaxis()->SetTitleSize(0.06);
      hEx[i]->GetXaxis()->SetTitleOffset(0.7);
      hEx[i]->GetYaxis()->SetTitleOffset(0.6);
      detGate.Form("&& detID == %d", i);
      tree0->Draw(Form("Ex >> hEx%d", i), gate_RDT + detGate); 
      
      
      specS[i] = (TH1F*) hEx[i]->Clone();
      titleH.Form("det-%d; Ex [MeV]; Count / %3.0f keV", i, (ExRange[2] - ExRange[1])*1000/ExRange[0]);
      specS[i]->SetTitle(titleH);   
      specS[i]->SetName("specS");
      specS[i]->GetXaxis()->SetTitleSize(0.06);
      specS[i]->GetYaxis()->SetTitleSize(0.06);
      specS[i]->GetXaxis()->SetTitleOffset(0.7);
      specS[i]->GetYaxis()->SetTitleOffset(0.6);
      
      //=================== find peak and fit
      printf("============= estimate background and find peak\n");
      TSpectrum * peak = new TSpectrum(50);
      nPeaks = peak->Search(hEx[i], 1, "", 0.1);
      hBg[i] = (TH1F * )peak->Background(hEx[i],10);
      hBg[i]->Draw("same");
      
      specS[i]->Add(hBg[i], -1.);
      specS[i]->Sumw2();
      specS[i]->Draw();
      
      //========== Fitting 
      printf("============= Fit.....");
      printf(" found %d peaks \n", nPeaks);
      
      double * xpos = peak->GetPositionX();
      double * ypos = peak->GetPositionY();
      
      int * inX = new int[nPeaks];
      TMath::Sort(nPeaks, xpos, inX, 0 );
      vector<double> energy, height;
      for( int j = 0; j < nPeaks; j++){
         energy.push_back(xpos[inX[j]]);
         height.push_back(ypos[inX[j]]);
      }
      
      const int  n = 3 * nPeaks;
      double * para = new double[n]; 
      for(int i = 0; i < nPeaks ; i++){
         para[3*i+0] = height[i] * 0.05 * TMath::Sqrt(TMath::TwoPi());
         para[3*i+1] = energy[i];
         para[3*i+2] = 0.08;
      }

      TF1 * fit = new TF1("fit", fpeaks, ExRange[1], ExRange[2], 3* nPeaks );
      fit->SetLineWidth(1);
      fit->SetNpx(1000);
      fit->SetParameters(para);
      specS[i]->Fit("fit", "q");

      printf("============= Det-%d \n", i);   
      const Double_t* paraE = fit->GetParErrors();
      const Double_t* paraA = fit->GetParameters();
      
      double bw = specS[i]->GetBinWidth(1);
      
      double * ExPos = new double[nPeaks];
      double * ExSigma = new double[nPeaks];   
      for(int i = 0; i < nPeaks ; i++){
         ExPos[i] = paraA[3*i+1];
         ExSigma[i] = paraA[3*i+2];
         printf("%2d , count: %8.0f(%3.0f), mean: %8.4f(%8.4f), sigma: %8.4f(%8.4f) \n", 
                 i, 
                 paraA[3*i] / bw,   paraE[3*i] /bw, 
                 paraA[3*i+1], paraE[3*i+1],
                 paraA[3*i+2], paraE[3*i+2]);
      }
      cScript->Update();
      
      gsSigma[i] = ExSigma[0];
      
      gSystem->ProcessEvents();
      
   }
   
   printf("========= Ground state Sigma. \n");
   for( int i = 0; i < 24; i++){
      printf("Det-%2d | sigma : %4.0f keV \n", i, gsSigma[i] * 1000.);
   }
   
}
