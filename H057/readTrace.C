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
#include <TLine.h>

void readTrace(){
   
/**///==============================================================   
   TFile * f1 = new TFile ("trace.root", "read");
   TTree * tree = (TTree *) f1->Get("tree");
   
	int totnumEntry = tree->GetEntries();
   printf( "========== total Entry : %d \n", totnumEntry);
   
   TCanvas * cRead = new TCanvas("cRead", "Read Trace", 0, 0, 800, 300);
   cRead->Divide(1,1);
   for( int i = 1; i <= 2 ; i++){
      cRead->cd(i)->SetGrid();
   }
   cRead->SetGrid();
   
/**///==============================================================   
   TClonesArray * arr = new TClonesArray("TGraph");
   tree->GetBranch("trace")->SetAutoDelete(kFALSE);
   tree->SetBranchAddress("trace", &arr);
   
   int eventID;
   float e[24], xf[24], xn[24];
   
   tree->SetBranchAddress("eventID",   &eventID);
   tree->SetBranchAddress("e",         e);
   tree->SetBranchAddress("xf",        xf);
   tree->SetBranchAddress("xn",        xn);

	char s [1] ;
	char b [1] ;
	b[0] = '1';

   TLine timeVLine;

   bool breakFlag = false;
   for( int ev = 0; ev < totnumEntry; ev++){
      arr->Clear();
      tree->GetEntry(ev);
      
      for( int i = 0; i < 24; i++){
         if( TMath::IsNaN(e[i]) ) continue;    
         printf("========= ev: %d, #trace: %d , (e, xf, xn) = (%7.2f, %7.2f, %7.2f)\n", 
                    eventID, arr->GetEntriesFast(), e[i], xf[i], xn[i]);
      }

      for( int j = 0; j < arr->GetEntriesFast() ; j++){

         TGraph * g  = (TGraph*) arr->At(j);
         TF1 * gFit = (TF1 *) g->FindObject("gFit");
         double base, time, riseTime, energy;
         energy   = gFit->GetParameter(0);
         time     = gFit->GetParameter(1);
         riseTime = gFit->GetParameter(2);
         base     = gFit->GetParameter(3);
         int kind = gFit->GetLineColor();
         
         TString gTitle;
         gTitle.Form("kind: %d, base: %5.1f, rise: %5.3f, time: %5.2f, energy: %6.1f ",
                  kind, base, time, riseTime, energy);
         
         printf("%s", gTitle.Data());
         g->SetTitle(gTitle);
        
         timeVLine.SetX1(time);
         timeVLine.SetX2(time);
         timeVLine.SetY1(-1000);
         timeVLine.SetY2(20000);
         timeVLine.SetLineColor(4);
         
         cRead->cd(1);
         cRead->Clear();
         g->Draw("");
         timeVLine.Draw("same");
         cRead->Update();
         
         gets(s);    
         if( s[0] == b[0] ) {
            breakFlag = true;
            break;
         }
      }
      if( breakFlag ) break;
   }

}
