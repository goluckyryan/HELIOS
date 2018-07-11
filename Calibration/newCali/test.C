//#include "TH1.h"
//#include "TF1.h"
//#include "TList.h"
//#include <fstream>

//void test(){
{  
   TTree *tree = NULL;
   TTree *cTree = NULL;
   const char* rootfile0="gen_run11.root";
   TFile *f0 = new TFile (rootfile0, "read"); 
   tree = (TTree*)f0->Get("gen_tree");
   
   const char* rootfile1="~/Desktop/HELIOS/Simulation/transfer.root";
   TFile *f1 = new TFile (rootfile1, "read"); 
   cTree = (TTree*)f1->Get("tree");
   
   Int_t Div[2] = {2,1};  //x,y
   Int_t size[2] = {400,400}; //x,y
   
   TCanvas * cScript = new TCanvas("cScript", "cScript", 0, 0, size[0]*Div[0], size[1]*Div[1]);
   cScript->Divide(Div[0],Div[1]);
   for( int i = 1; i <= Div[0]+Div[1] ; i++){
      cScript->cd(i)->SetGrid();
   }
   
   
   cScript->cd(1);
   
   tree->Draw("e[3]/250 -0.4 :(xf[3]-xn[3])/(xf[3]+xn[3]) >> h(400, -1.3, 1.3, 400, 0, 10)", "e[3]>0 && xf[3]>0 && xn[3]>0");
   
   cScript->cd(2);
   
   cTree->Draw("e:x>>hc(400, -1.3, 1.3, 400, 0, 10)", "detID == 3", "colz");
   h->Draw("same");
   
}
