#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TSelector.h>
#include <TCanvas.h>
#include <TGraph.h>
#include <TClonesArray.h>
#include <TBenchmark.h>
#include <TMath.h>
#include <TF1.h>

void readTrace(){

   TFile * f1 = new TFile ("trace.root", "read");
   TTree * tree = (TTree *) f1->Get("tree");
   
   TCanvas * cRead = new TCanvas("cRead", "Read Trace", 0, 0, 800, 600);
   cRead->Divide(1,2);
   for( int i = 1; i <= 2 ; i++){
      cRead->cd(i)->SetGrid();
   }
   cRead->SetGrid();
   
   char*s = new char[1];
   
   TClonesArray * arr = new TClonesArray("TGraph");
   tree->GetBranch("trace")->SetAutoDelete(kFALSE);
   tree->SetBranchAddress("trace", &arr);
   
   TClonesArray * arrC = new TClonesArray("TGraph");
   tree->GetBranch("convo")->SetAutoDelete(kFALSE);
   tree->SetBranchAddress("convo", &arrC);
   
   Long64_t nentries = tree->GetEntries();
//   Long64_t nentries = 10;

   for( Long64_t ev = 0; ev < nentries; ev++){
      arr->Clear();
      tree->GetEntry(ev);
      TGraph *g  = (TGraph*) arr->At(0);
      TGraph *gC = (TGraph*) arrC->At(0);
      cRead->cd(1);
      g->Draw("");
      //g->GetYaxis()->SetRangeUser(7500, 9000);
      cRead->cd(2);
      gC->Draw("");
      cRead->Update();
      //gPad->WaitPrimitive(); 
      gets(s);    
      
      if( s == "e" ) break;
   }

}
