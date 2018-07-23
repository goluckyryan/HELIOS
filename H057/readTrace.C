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
   
	int totnumEntry = tree->GetEntries();
   printf( "========== total Entry : %d \n", totnumEntry);
   
   TCanvas * cRead = new TCanvas("cRead", "Read Trace", 0, 0, 800, 600);
   cRead->Divide(1,2);
   for( int i = 1; i <= 2 ; i++){
      cRead->cd(i)->SetGrid();
   }
   cRead->SetGrid();
   
   
   
   TClonesArray * arr = new TClonesArray("TGraph");
   tree->GetBranch("trace")->SetAutoDelete(kFALSE);
   tree->SetBranchAddress("trace", &arr);
   
   Long64_t nentries = tree->GetEntries();
//   Long64_t nentries = 10;

	char s [1] ;
	char b [1] ;
	b[0] = '1';

	TF1 * fit = new TF1("fit", "[0]/(1+TMath::Exp(-(x-[1])/[2]))+[3]", 0, 100);

   for( Long64_t ev = 0; ev < nentries; ev++){
      arr->Clear();
      tree->GetEntry(ev);
      TGraph *g  = (TGraph*) arr->At(0);
      
      TString gTitle;
      gTitle.Form("event : %d", ev);
      g->SetTitle(gTitle);
      
      //double base = g->Eval(1);
      //double height = g->Eval(80) - base;
      //fit->SetParameter(3, base);
      //fit->SetParameter(1, 50);
      //fit->SetParameter(2, 1);
      //fit->SetParameter(0, height);
      //
      //g->Fit("fit", "R");
      
      cRead->cd(1);
      g->Draw("");
      ////g->GetYaxis()->SetRangeUser(7500, 9000);
      cRead->Update();
      
      gets(s);    
      printf(" = %s : ", s);
      if( s[0] == b[0] ) break;
   }

}
