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
   TFile * f1 = new TFile ("trace_2.root", "read");
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
   
   double tBase[20];
   double tChisq[20];
   double tEnergy[20];
   double tTime[20];
   double tRiseTime[20];
   int ch[20];
   int kind[20];
   int numHit;
   
   tree->SetBranchAddress("tBase",     tBase);  
   tree->SetBranchAddress("tEnergy",   tEnergy);  
   tree->SetBranchAddress("tTime",     tTime);
   tree->SetBranchAddress("tRiseTime", tRiseTime);    
   tree->SetBranchAddress("tChiSq",    tChisq);   
   tree->SetBranchAddress("ch",        ch);    
   tree->SetBranchAddress("kind",      kind);   
   tree->SetBranchAddress("NumHits",   &numHit);

	char s [1] ;
	char b [1] ;
	b[0] = '1';

	//TF1 * fit = new TF1("fit", "[0]/(1+TMath::Exp(-(x-[1])/[2]))+[3]", 0, 100);

   TLine timeVLine;

   bool breakFlag = false;
   for( int ev = 0; ev < totnumEntry; ev++){
      arr->Clear();
      tree->GetEntry(ev);
      
      printf("========= ev: %d, #trace: %d , numHit: %d\n", ev, arr->GetEntriesFast(), numHit);
      for( int j = 0; j < numHit ; j++){

         TGraph * g  = (TGraph*) arr->At(j);
         //TF1 * gFit = (TF1 *) g->FindObject("gFit");
         //double base, time, riseTime, energy;
         //energy   = gFit->GetParameter(0);
         //time     = gFit->GetParameter(1);
         //riseTime = gFit->GetParameter(2);
         //base     = gFit->GetParameter(3);
         
         TString gTitle;
         gTitle.Form("event : %d[%d],ch:%d, kind:%d, base: %5.1f, time: %5.2f ch, rise: %5.3f ch, energy: %6.1f | chi2: %6.2f", 
                       ev, j, ch[j], kind[j], tBase[j], tTime[j], tRiseTime[j], tEnergy[j], tChisq[j]);
         printf("%s", gTitle.Data());
         g->SetTitle(gTitle);
        
         timeVLine.SetX1(tTime[j]);
         timeVLine.SetX2(tTime[j]);
         timeVLine.SetY1(-1000);
         timeVLine.SetY2(20000);
         timeVLine.SetLineColor(4);
         timeVLine.Draw("same");
         
         //double base = g->Eval(1);
         //double height = g->Eval(80) - base;
         //fit->SetParameter(3, base);
         //fit->SetParameter(1, 50);
         //fit->SetParameter(2, 1);
         //fit->SetParameter(0, height);
         //
         //g->Fit("fit", "R");
         
         cRead->cd(1);
         cRead->Clear();
         g->Draw("");
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
