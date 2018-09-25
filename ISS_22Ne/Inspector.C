#define Inspector_cxx
// The class definition in Inspector.h has been generated automatically
// by the ROOT utility TTree::MakeSelector(). This class is derived
// from the ROOT class TSelector. For more information on the TSelector
// framework see $ROOTSYS/README/README.SELECTOR or the ROOT User Manual.

// The following methods are defined in this file:
//    Begin():        called every time a loop on the tree starts,
//                    a convenient place to create your histograms.
//    SlaveBegin():   called after Begin(), when on PROOF called only on the
//                    slave servers.
//    Process():      called for each event, in this function you decide what
//                    to read and fill your histograms.
//    SlaveTerminate: called at the end of the loop on the tree, when on PROOF
//                    called only on the slave servers.
//    Terminate():    called at the end of the loop on the tree,
//                    a convenient place to draw/fit your histograms.
//
// To use this file, try the following session on your Tree T:
//
// root> T->Process("Inspector.C")
// root> T->Process("Inspector.C","some options")
// root> T->Process("Inspector.C+")
//

#include "Inspector.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TMath.h>

TH2F ** hRDT = NULL;
TH2F *  hRDTa = NULL;
TH2F *  hArrayRDT = NULL;
TH2F *  hETDiff = NULL;
TH2F ** hEX = NULL;


double rdtPeak[8];
double corrRDT[8];


void Inspector::Begin(TTree * /*tree*/)
{
   TString option = GetOption();
   
   rdtPeak[0] = 1475;
   rdtPeak[1] =  841;
   rdtPeak[2] =  864;
   rdtPeak[3] = 1498;
   rdtPeak[4] = 4648;
   rdtPeak[5] = 5488;
   rdtPeak[6] = 5420;
   rdtPeak[7] = 4671;

   
   corrRDT[0] = rdtPeak[0]/rdtPeak[0];
   corrRDT[1] = rdtPeak[0]/rdtPeak[1];
   corrRDT[2] = rdtPeak[0]/rdtPeak[2];
   corrRDT[3] = rdtPeak[0]/rdtPeak[3];

   corrRDT[4] = rdtPeak[4]/rdtPeak[4];
   corrRDT[5] = rdtPeak[4]/rdtPeak[5];
   corrRDT[6] = rdtPeak[4]/rdtPeak[6];
   corrRDT[7] = rdtPeak[4]/rdtPeak[7];

   TString name, title;
   hRDT = new TH2F* [4]; 
   for ( int i = 0; i < 4; i++){
      name.Form("hRDT%d", i);
      title.Form("hRDT - %d (corrected) ; E ; dE", i);
      hRDT[i] = new TH2F(name, title, 300, 0, 8000, 300, 0, 5000);
   }

   hRDTa = new TH2F("hRDTa", "hRDTa (raw); rdt; id", 300, 0, 8000, 8, 0, 8 );

   hArrayRDT = new TH2F("hArrayRDT", "RDT - Arrat Hit", 24, 0, 24, 8, 0, 8);
   
   hETDiff = new TH2F("hETDiff", "E - TDiff; time diff [ch]; E", 400, -200, 200, 400, 0, 8000);

   hEX = new TH2F*[24];
   for( int i = 0; i < 24 ; i ++){
      name.Form("hEX%d", i);
      title.Form("e - x (det-%d); e ; x ", i);
      hEX[i] = new TH2F(name, title, 300, -2, 2, 300, 0, 2000);
   }

}

void Inspector::SlaveBegin(TTree * /*tree*/)
{
   TString option = GetOption();
}

Bool_t Inspector::Process(Long64_t entry)
{
   
   b_Energy->GetEntry(entry);
   b_RDT->GetEntry(entry);
   b_RDTTimestamp->GetEntry(entry);
   b_EnergyTimestamp->GetEntry(entry);
   b_XF->GetEntry(entry);
   b_XN->GetEntry(entry);
   
   
   //============== coincident time
   int coinTime = -10000;
   for( int i = 4; i < 8; i++){
      if( !TMath::IsNaN(rdt[i]) ) {
         int rdtTime = (int) rdt_t[i];
         for( int j = 0; j < 24; j++){
            if( !TMath::IsNaN(e[j]) ) {
               int eTime = (int) e_t[j];
               coinTime = eTime-rdtTime;
               hETDiff->Fill(coinTime, corrRDT[i] * rdt[i]);
            }
         }
      }
   }
   
   if( TMath::Abs(coinTime) > 50 ) return kTRUE;  
   
   //============= recoil
   bool rej = true;
   for( int i = 0; i < 4; i++){
      if(!TMath::IsNaN(rdt[i]) && !TMath::IsNaN(rdt[i+4])) {
         hRDT[i]->Fill(corrRDT[i+4] * rdt[i+4], corrRDT[i] * rdt[i]); //x, y
         if( TMath::Abs(corrRDT[i+4] * rdt[i+4] - 4400) < 500 && TMath::Abs(corrRDT[i] * rdt[i] - 1500)< 300 ) rej = false;
      }
   }
   
   //if( rej) return kTRUE;
   
   //============= coincdient with recoil and array
   for( int i = 0; i < 8; i++){
      if( !TMath::IsNaN(rdt[i]) ) {
         hRDTa->Fill(rdt[i], i); // x, y

         for( int j = 0; j < 24; j++){
            if( !TMath::IsNaN(e[j]) ) hArrayRDT->Fill(j, i);
         }
      }
   }

   //============= e and x
   for( int i = 0 ; i < 24; i++){
      
      if( TMath::IsNaN(e[i]) ) continue;
      if( TMath::IsNaN(xf[i])  && TMath::IsNaN(xn[i])  ) continue;
      if( xf[i] == 0  && xn[i] == 0) continue;
      
      //printf("%2d| %f, %f, %f \n",i , e[i], xf[i], xn[i]);
      
      bool xfQ = TMath::IsNaN(xf[i]);
      bool xnQ = TMath::IsNaN(xn[i]);
      
      Float_t x = -10.;
      if(!xfQ  && !xnQ ) {
         x = (xf[i]-xn[i])/(xf[i]+xn[i]);
         
      }else if( xfQ && !xnQ ){
         x = 1.0-2.0*xn[i]/e[i];
         
      }else if( !xfQ && xnQ ){
         x = 2.0*xf[i]/e[i]-1.0;
      
      }
      
      /*
      if(xf[i] != 0  && xn[i] !=0 ) {
         x = (xf[i]-xn[i])/(xf[i]+xn[i]);
         
      }else if(xf[i] == 0 && xn[i] != 0 ){
         x = 1.0-2.0*xn[i]/e[i];
         
      }else if(xf[i] != 0 && xn[i] == 0 ){
         x = 2.0*xf[i]/e[i]-1.0;
      
      }*/
      
      //printf("         %f, %f | %f, %f \n", x, e[i], 2*xf[i]/e[i]-1, 1-2*xn[i]/e[i]);
      hEX[i]->Fill(x, e[i]);
   }
   
   return kTRUE;
}

void Inspector::SlaveTerminate()
{

}

void Inspector::Terminate()
{
   printf("=========== done. \n");
   gStyle->SetOptStat(0);
   gStyle->SetLabelSize(0.04, "XY");
   gStyle->SetTitleFontSize(0.1);
   
   Int_t Div[2] = {4,2};
   Int_t size[2] = {300,300}; //x,y
   TCanvas * cAna22Ne = new TCanvas("cAna22Ne", "cAna22Ne", 0, 0, size[0]*Div[0], size[1]*Div[1]);
   if(cAna22Ne->GetShowEditor() )cAna22Ne->ToggleEditor();
   if(cAna22Ne->GetShowToolBar() )cAna22Ne->ToggleToolBar();
   cAna22Ne->Divide(Div[0],Div[1]);
   for( int i = 1; i <= Div[0]*Div[1] ; i++){
      cAna22Ne->cd(i)->SetGrid();
      cAna22Ne->cd(i)->SetLogz();
   }
   
   cAna22Ne->cd(1); hRDT[0]->Draw("colz");
   cAna22Ne->cd(2); hRDT[1]->Draw("colz");
   cAna22Ne->cd(3); hRDT[2]->Draw("colz");
   cAna22Ne->cd(4); hRDT[3]->Draw("colz");
   
   cAna22Ne->cd(5); hRDTa->Draw("colz");
   cAna22Ne->cd(6); hArrayRDT->Draw("colz");
   cAna22Ne->cd(7); hETDiff->Draw("colz");
   
   //===================== 2nd canvas
   Div[0] = 6; Div[1] = 4;
   size[0] = 230; size[1] = 230;
   TCanvas * cAnaEX = new TCanvas("cAnaEX", "cAnaEX", 0, 0, size[0]*Div[0], size[1]*Div[1]);
   if(cAnaEX->GetShowEditor() )cAnaEX->ToggleEditor();
   if(cAnaEX->GetShowToolBar() )cAnaEX->ToggleToolBar();
   cAnaEX->Divide(Div[0],Div[1]);
   for( int i = 1; i <= Div[0]*Div[1] ; i++){
      cAnaEX->cd(i)->SetGrid();
      //cAnaEX->cd(i)->SetLogz();
      cAnaEX->cd(i); 
      hEX[i-1]->Draw("colz");
   }
   
}
