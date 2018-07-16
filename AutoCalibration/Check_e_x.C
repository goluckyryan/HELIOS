#include "TROOT.h"
#include "TStyle.h"
#include "TFile.h"
#include "TTree.h"
#include "TH2F.h"
#include "TLine.h"
#include "TCanvas.h"
#include "TF1.h"

//void Check_e_x(){
{/**///======================================================== User input
   const char* rootfile="../H060_ana/data/gen_run11.root"; const char* treeName="gen_tree";
   
   double eThreshold = 300;
   
   int numDet = 24;
   double eRange[3]  = {400, 0, 2000};

/**///======================================================== read tree and create Canvas

   TFile *file0 = new TFile (rootfile, "read"); 
   TTree *tree = (TTree*)file0->Get(treeName);
   printf("=====> /// %15s //// is loaded. Total #Entry: %10lld \n", rootfile,  tree->GetEntries());
   
   Int_t Div[2] = {6,4};  //x,y
   Int_t size[2] = {200,230}; //x,y
   
   TCanvas * cCheck = new TCanvas("cCheck", "cCheck", 0, 0, size[0]*Div[0], size[1]*Div[1]);
   cCheck->Divide(Div[0],Div[1]);
   for( int i = 1; i <= Div[0]*Div[1] ; i++){
      cCheck->cd(i)->SetGrid();
   }

   gStyle->SetOptStat(0);
   gStyle->SetStatY(1.0);
   gStyle->SetStatX(0.99);
   gStyle->SetStatW(0.2);
   gStyle->SetStatH(0.1);
   gStyle->SetLabelSize(0.05, "X");
   gStyle->SetLabelSize(0.05, "Y");
   gStyle->SetTitleFontSize(0.1);
   

/**///======================================================== Analysis
   
   TLine * line = new TLine(-1, eThreshold, 1, eThreshold);
   line->SetLineColor(2);
   
   TH2F ** hEX = new TH2F*[numDet];
   TString expression, name, gate;
   for( int idet = 0 ; idet < numDet ; idet ++){
      cCheck->cd(idet+1);  
      name.Form("hEX%02d", idet);
      hEX[idet] = new TH2F(name, name, 400, -1.3, 1.3, eRange[0], eRange[1], eRange[2]);
      expression.Form("e[%d]:(xf[%d]-xn[%d])/(xf[%d]+xn[%d])>>hEX%02d", idet, idet, idet, idet, idet, idet);
      
      gate.Form("e[%d] > 0 && (xf[%d] > 0 || xn[%d] > 0 )", idet, idet, idet);

      tree->Draw(expression, gate);
      line->Draw("same");
      cCheck->Update();
   }
}
