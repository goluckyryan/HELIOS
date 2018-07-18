#include "TROOT.h"
#include "TStyle.h"
#include "TFile.h"
#include "TTree.h"
#include "TH2F.h"
#include "TLine.h"
#include "TCanvas.h"
#include "TF1.h"

void Check_e_x(TString rootFile = "temp.root" , double eThreshold = 400){
/**///======================================================== User input
   
   const char* treeName="tree";
   
   int numDet = 24;
   double eRange[3]  = {400, 0, 2000};
   double zRange[3]  = {400, -600, -200};

/**///======================================================== read tree and create Canvas

   TFile *file0 = new TFile (rootFile, "read"); 
   TTree *tree = (TTree*)file0->Get(treeName);
   printf("=====> /// %15s //// is loaded. Total #Entry: %10lld \n", rootFile.Data(),  tree->GetEntries());
   
   Int_t Div[2] = {6,4};  //x,y
   Int_t size[2] = {200,230}; //x,y
   
   TCanvas * cCheck = new TCanvas("cCheck", "cCheck", 0, 0, size[0]*Div[0], size[1]*Div[1]);
   cCheck->Divide(Div[0],Div[1]);
   for( int i = 1; i <= Div[0]*Div[1] ; i++){
      cCheck->cd(i)->SetGrid();
   }

   gStyle->SetOptStat(0);
   gStyle->SetStatY(0.9);
   gStyle->SetStatX(0.9);
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
      expression.Form("e:x>>hEX%02d", idet);
      
      gate.Form("detID == %d", idet);

      tree->Draw(expression, gate);
      line->Draw("same");
      cCheck->Update();
   }
   
/**///======================================================== e vs z
   TCanvas * cCheck2 = new TCanvas("cCheck2", "cCheck2", 700, 50,  600, 400);
   cCheck2->SetGrid();
   
   gStyle->SetOptStat(11111);
   gStyle->SetStatY(0.9);
   gStyle->SetStatX(0.35);
   gStyle->SetStatW(0.25);
   gStyle->SetStatH(0.10);
   gStyle->SetLabelSize(0.035, "X");
   gStyle->SetLabelSize(0.035, "Y");
   gStyle->SetTitleFontSize(0.035);
   
   TH2F * hEZ = new TH2F("hEZ", "e:z; z [mm]; e [MeV]", zRange[0], zRange[1], zRange[2], eRange[0], eRange[1], eRange[2]);
   tree->Draw("e:z >> hEZ", "" );
   
   TLine * line2 = new TLine(zRange[1], eThreshold, zRange[2], eThreshold);
   line2->SetLineColor(2);
   line2->Draw("same");  
   cCheck2->Update();
   
/**///======================================================== multi
   TCanvas * cCheck3 = new TCanvas("cCheck3", "cCheck3", 700, 600,  600, 300);
   cCheck3->SetGrid();
   
   TH1F * hMulti = new TH1F("hMulti", "Multi", 20, 0, 20);
   tree->Draw("multi >> hMulti", "" );
   
   cCheck3->Update();   
}
