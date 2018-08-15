#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TH2F.h>
#include <TCutG.h>
#include <TProfile.h>
#include <TF1.h>
#include <TSpectrum.h>
#include <TMath.h>

void GetCoinTimeCorrectionCutG(){

   //int detID = 1;

   TFile * f1 = new TFile("A_trace.root", "READ");
   TTree * tree = (TTree*) f1->Get("tree");
   
   int totnumEntry = tree->GetEntries();
   printf( "========== total Entry : %d \n", totnumEntry);

   int Div[2] = {1,1};
   int size[2] = {600,600}; //x,y
   TCanvas * cAna = new TCanvas("cAna", "cAna", 0, 0, size[0]*Div[0], size[1]*Div[1]);
   if( cAna->GetShowEditor() )  cAna->ToggleEditor();
   if( cAna->GetShowToolBar() ) cAna->ToggleToolBar();
   cAna->Divide(Div[0],Div[1]);
   for( int i = 1; i <= Div[0]*Div[1] ; i++){
      cAna->cd(i)->SetGrid();
   }
   
   gStyle->SetOptStat(0);
   if(cAna->GetShowEditor() )   cAna->ToggleEditor();
   if(!cAna->GetShowToolBar() ) cAna->ToggleToolBar();
   
   TString name, expression, gate;

   TH2F * hTX   = new TH2F("hTX",   "time vs X; x; coinTime [ch]", 300, -1.5, 1.5, 300, -10, 40);
   TH2F * hTXg  = new TH2F("hTXg",  "time vs X; x; coinTime [ch]", 300, -1.5, 1.5, 300, -10, 40);
   TH2F * hTXc2 = new TH2F("hTXc2", "time vs X; x; coinTime [ch]", 300, -1.5, 1.5, 300, -10, 40);
   TH1F * hT = new TH1F("hT", "", 100, 0, 20);
   TSpectrum * spec = new TSpectrum(5);
   
   TCutG * cut = NULL;
   
   FILE * paraOut;
   TString filename;
   filename.Form("correction_coinTime.dat");
   paraOut = fopen (filename.Data(), "w+");

   for( int detID = 0; detID < 24; detID ++ ){   
      
      printf("============ detID: %d\n", detID);
   
      expression.Form("coinTime:x>>hTX");   
      gate.Form("Iteration$==%d", detID);
      name.Form("time vs X (detID-%d); x; coinTime [ch]", detID);
      hTX->SetTitle(name);
      tree->Draw(expression, gate, "colz");
      
      //==== create cut and draw
      cAna->WaitPrimitive("CUTG", "TCutG");
      cut = (TCutG*) gROOT->FindObjectAny("CUTG");
      cut->SetName("cut1");
      cut->SetVarX("x");
      cut->SetVarY("coinTime");
      gate.Form("cut1 && Iteration$==%d", detID);
      
      expression.Form("coinTime:x>>hTXg");   
      tree->Draw(expression, gate, "colz");
      cut->Draw("same");
      
      //==== create profile and fit
      name.Form("hTXp%d", detID);
      TProfile * hp = new TProfile(name, name, 400, -1.5,1.5);
      hTXg->ProfileX(name);
      hp->Draw("same");
      
      TF1 * fit7 = new TF1("fit7", "pol7", -2, 2);
      hp->Fit("fit7", "");
      
      cAna->WaitPrimitive();
      
      //==== create substract
      double q[8];
      q[0] = fit7->GetParameter(0);
      q[1] = fit7->GetParameter(1);
      q[2] = fit7->GetParameter(2);
      q[3] = fit7->GetParameter(3);
      q[4] = fit7->GetParameter(4);
      q[5] = fit7->GetParameter(5);
      q[6] = fit7->GetParameter(6);
      q[7] = fit7->GetParameter(7);
    
      gate.Form("Iteration$==%d", detID);  
      expression.Form("coinTime - %f*x - %f*TMath::Power(x,2) - %f*TMath::Power(x,3)- %f*TMath::Power(x,4)- %f*TMath::Power(x,5)- %f*TMath::Power(x,6)- %f*TMath::Power(x,7) :x>>hTXc2", q[1], q[2], q[3], q[4], q[5], q[6], q[7]);
      tree->Draw(expression, gate, "colz");
      
      cAna->WaitPrimitive();
      
      //==== 1-D plot
      expression.Form("coinTime - %f*x - %f*TMath::Power(x,2) - %f*TMath::Power(x,3)- %f*TMath::Power(x,4)- %f*TMath::Power(x,5)- %f*TMath::Power(x,6)- %f*TMath::Power(x,7) >>hT", q[1], q[2], q[3], q[4], q[5], q[6], q[7]);
      tree->Draw(expression, gate, "colz");
      
      int nPeak = spec->Search(hT);
      printf("find %d peaks\n", nPeak);
      
      float * xpos =  spec->GetPositionX();
      float * ypos =  spec->GetPositionY();
      
      TF1* fit2g = new TF1("fit2g", "gaus(0) + gaus(3)");
      fit2g->SetParameter(0, ypos[0]);
      fit2g->SetParameter(1, xpos[0]);
      fit2g->SetParameter(2, 0.5);
      fit2g->SetParameter(3, ypos[1]);
      fit2g->SetParameter(4, xpos[1]);
      fit2g->SetParameter(5, 0.5);
      
      hT->Fit("fit2g");
      
      double time[2], eTime[2];
      double resol[2], eResol[2];
      
      printf("================================= in nano-sec\n");
      
      for(int i = 0; i < 2; i ++){
         time[i]   = fit2g->GetParameter(3*i+1) * 10.;
         eTime[i]  = fit2g->GetParError(3*i+1) * 10.;
         resol[i]  = fit2g->GetParameter(3*i+2) * 10.;
         eResol[i] = fit2g->GetParError(3*i+2) * 10.;
         
         printf("Peak: %6.2f(%4.2f), Sigma: %6.4f(%6.4f)\n", time[i], eTime[i], resol[i], eResol[i]);   
      }
      
      printf("==== time difference: %6.2f(%5.2f) ns \n", 
                        (time[1] - time[0]), 
                        TMath::Sqrt( eTime[0]*eTime[0] + eTime[1]*eTime[1]));
      
      
      cAna->WaitPrimitive();
      

      printf("=========== save parameters to %s \n", filename.Data());
      fprintf(paraOut, "%d\t", detID);
      for( int i = 0; i < 7; i++){
         fprintf(paraOut, "%11.6f\t", q[i]);
      }
      fprintf(paraOut, "%11.6f\n", q[7]);
      
   }
   
   fflush(paraOut);
   fclose(paraOut);
}
