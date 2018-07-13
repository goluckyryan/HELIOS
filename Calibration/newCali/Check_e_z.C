//#include "TH1.h"
//#include "TF1.h"
//#include "TList.h"
//#include <fstream>

//void Check_e_z(){
{  
/**///======================================================== User input
   const char* rootfile="A_gen_run11.root"; const char* treeName="tree";

   const char* simfile="~/ANALYSIS/Simulation/transfer.root"; const char* treeName="tree";

   int numDet = 24;

   double eRange[3] = {400, 0, 10};

/**///======================================================== read tree and create Canvas

   TFile *f0 = new TFile (rootfile, "read"); 
   TTree *tree = (TTree*)f0->Get(treeName);
   printf("=====> /// %15s //// is loaded. Total #Entry: %10d \n", rootfile,  tree->GetEntries());
   
   TFile *f1 = new TFile (simfile, "read"); 
   TTree *sTree = (TTree*)f1->Get(treeName);
   printf("=====> /// %15s //// is loaded. Total #Entry: %10d \n", simfile,  sTree->GetEntries());

   Int_t Div[2] = {6,4};  //x,y
   Int_t size[2] = {230,230}; //x,y
   
   TCanvas * cScript = new TCanvas("cScript", "cScript", 0, 0, size[0]*Div[0], size[1]*Div[1]);
   cScript->Divide(Div[0],Div[1]);
   for( int i = 1; i <= Div[0]+Div[1] ; i++){
      cScript->cd(i)->SetGrid();
   }

   gStyle->SetOptStat(1111111);
   gStyle->SetStatY(1.0);
   gStyle->SetStatX(0.99);
   gStyle->SetStatW(0.2);
   gStyle->SetStatH(0.1);

/**///======================================================== Analysis
   
   /*
   cScript->cd(1);
   tree->Draw("e:x>>h1(400, -1.3, 1.3, 400, 0, 2000)", "hitID >= 0 && detID == 13");
   
   cScript->cd(2);
   sTree->Draw("e:x>>h2(400, -1.3, 1.3, 400, 0, 10)", "loop == 1 && detID == 13%6 && ExID < 2");
   */
   
   TH2F ** hEZ = new TH2F*[numDet];
   TString expression, name, gate;
   for( int idet = 0 ; idet < numDet ; idet ++){
      cScript->cd(idet+1);  
      name.Form("hEZ%02d", idet);
      hEZ[idet] = new TH2F(name, name, 400, -1.3, 1.3, eRange[0], eRange[1], eRange[2]);
      expression.Form("e:x>>hEZ%02d", idet);
      gate.Form("detID == %d && hitID == 0", idet);
      tree->Draw(expression, gate);
      cScript->Update();
   }
   
   TCanvas * cScript2 = new TCanvas("cScript2", "cScript2", 100, 100,  800, 600);
   tree->Draw("e:z >> h(400, -500, -100, 400, 0, 10)", "hitID > = 0 && detID >= 18 " ); 
   
   
}
