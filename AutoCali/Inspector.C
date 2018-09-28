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
#include <TObjArray.h>
#include <TCutG.h>
#include <fstream>

TH2F ** hdEE  = NULL;
TH2F ** hdEEG = NULL;
TH2F *  hRDTa = NULL;
TH1F ** hRDT = NULL;
TH2F *  hArrayRDT = NULL;
TH1F *  hTDiff = NULL;
TH2F ** hEX = NULL;
TH2F *  hEZ = NULL;
TH2F *  hS1RDT = NULL;
TH2F *  hElum = NULL;


TString openFileName;

TObjArray * cutList = NULL;
TCutG ** cut = NULL;

string detGeoFileName = "detectorGeo.txt";
int numDet;
int rDet = 6; // number of detector at different position, row-Det
int cDet = 4; // number of detector at same position, column-Det
vector<double> pos;
double length = 50.5;
double firstPos = -110;
double zRange[3];

double xnCorr[24]; // xn correction for xn = xf
double xfxneCorr[24][2]; //xf, xn correction for e = xf + xn
double eCorr[24][2]; // e-correction

void Inspector::Begin(TTree * tree)
{
   TString option = GetOption();
   
   openFileName = tree->GetDirectory()->GetName();
   int findslat = openFileName.Last('/');
   openFileName.Remove(0, findslat+1);
   
//========================================= detector Geometry   
   printf("----- loading detector geometery : %s.", detGeoFileName.c_str());
   ifstream file;
   file.open(detGeoFileName.c_str());
   int i = 0;
   if( file.is_open() ){
      string x;
      while( file >> x){
         //printf("%d, %s \n", i,  x.c_str());
         if( x.substr(0,2) == "//" )  continue;
         if( i == 6 ) length   = atof(x.c_str());
         if( i == 8 ) firstPos = atof(x.c_str());
         if( i == 9 ) cDet = atoi(x.c_str());
         if( i >= 10 ) {
            pos.push_back(atof(x.c_str()));
         }
         i = i + 1;
      }
      
      rDet = pos.size();
      file.close();
      printf("... done.\n");
      
      for(int id = 0; id < rDet; id++){
         pos[id] = firstPos + pos[id];
      }
      
      for(int i = 0; i < rDet ; i++){
         if( firstPos > 0 ){
            printf("%d, %6.2f mm - %6.2f mm \n", i, pos[i], pos[i] + length);
         }else{
            printf("%d, %6.2f mm - %6.2f mm \n", i, pos[i] - length , pos[i]);
         }
      }
      printf("=======================\n");
      
   }else{
       printf("... fail\n");
       return;
   }
   
   numDet = rDet * cDet;
   
   
   zRange[0] = 400; // number of bin
   
   if( firstPos > 0 ){
      zRange[1] = pos[0]-50;
      zRange[2] = pos[rDet-1] + length + 50;
   }else{
      zRange[1] = pos[0]- length - 50;
      zRange[2] = pos[rDet-1] + 50;
   }
   
   
      //========================================= xf = xn correction
   printf("----- loading xf-xn correction.");
   file.open("correction_xf_xn.dat");
   if( file.is_open() ){
      double a;
      int i = 0;
      while( file >> a ){
         if( i >= numDet) break;
         xnCorr[i] = a;
         i = i + 1;
      }
      
      printf("... done.\n");
   }else{
      printf("... fail.\n");
      
      for(int i = 0; i < numDet; i++){
         xnCorr[i] = 1;
      }
   }
   file.close();
   
   //========================================= e = xf + xn correction
   
   printf("----- loading xf/xn-e correction.");
   file.open("correction_xfxn_e.dat");
   if( file.is_open() ){
      double a, b;
      int i = 0;
      while( file >> a >> b){
         if( i >= numDet) break;
         xfxneCorr[i][0] = a;
         xfxneCorr[i][1] = b;
         i = i + 1;
      }
      printf("... done.\n");
   }else{
      printf("... fail.\n");
      for(int i = 0; i < numDet; i++){
         xfxneCorr[i][0] = 0;
         xfxneCorr[i][1] = 1;
      }
   }
   file.close();

   //========================================= e correction
   
   printf("----- loading e correction.");
   file.open("correction_e.dat");
   if( file.is_open() ){
      double a, b;
      int i = 0;
      while( file >> a >> b){
         if( i >= numDet) break;
         eCorr[i][0] = a;  // 1/a1
         eCorr[i][1] = b;  //  a0 , e' = e * a1 + a0
         //printf("\n%2d, e0: %9.4f, e1: %9.4f", i, eCorr[i][0], eCorr[i][1]);
         i = i + 1;
      }
      printf("... done.\n");
      
   }else{
      printf("... fail.\n");
      for( int i = 0; i < 24 ; i++){
         eCorr[i][0] = 1.;
         eCorr[i][1] = 0.;
      }
      //return;
   }
   file.close();

   
   //===================== open cut
   TFile * fileCut = new TFile("rdtCuts.root");
   if( fileCut->IsOpen() ){
      TObjArray * cutList = (TObjArray*) fileCut->FindObjectAny("cutList");
      
      const int numCut = cutList->GetEntries();
      cut = new TCutG * [numCut];
      
      for( int i = 0 ; i < numCut; i++){
         cut[i] = (TCutG* ) cutList->At(i);
         printf("cut name: %s , VarX: %s, VarY: %s\n", cut[i]->GetName(), cut[i]->GetVarX(), cut[i]->GetVarY()); 
      }
   }

   TString name, title;
   hdEE  = new TH2F* [4]; 
   hdEEG = new TH2F* [4]; 
   for ( int i = 0; i < 4; i++){
      
      name.Form("hdEE%d", i);
      title.Form("hdEE - %d (raw) ; E ; dE", i);
      hdEE[i] = new TH2F(name, title, 200, 0, 8000, 200, 0, 5000);
      
      name.Form("hdEEG%d", i);
      title.Form("hdEEG - %d (raw, gated) ; E ; dE", i);
      hdEEG[i] = new TH2F(name, title, 200, 0, 8000, 200, 0, 5000);
   }

   hRDTa = new TH2F("hRDTa", "hRDTa (raw); rdt; id", 200, 0, 8000, 8, 0, 8 );
   
   hRDT = new TH1F* [8];
   for( int i = 0; i < 8 ; i++){
      name.Form("hRDT%d", i);
      title.Form("RDT-%d (raw)", i);
      hRDT[i] = new TH1F(name, title, 200, 0, 8000);
   }

   hArrayRDT = new TH2F("hArrayRDT", "RDT - Arrat Hit", 24, 0, 24, 8, 0, 8);
   hS1RDT = new TH2F("hS1RDT", "RDT - S1 Hit", 4, 4, 8, 8, 0, 8);
   
   hTDiff = new TH1F("hTDiff", "TDiff; time diff [ch]", 400, -200, 200);

   hEX = new TH2F*[24];
   for( int i = 0; i < 24 ; i ++){
      name.Form("hEX%d", i);
      title.Form("e - x (det-%d); e ; x ", i);
      hEX[i] = new TH2F(name, title, 300, -2, 2, 300, 0, 10);
   }
   
   hEZ = new TH2F("hEZ", "E- Z; Z [mm]; E", zRange[0], zRange[1], zRange[2], 300, 0, 10);

   hElum = new TH2F("hElum", "Elum; energy; ID", 500, 0, 1200, 4, 4, 8);
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
   b_ELUM->GetEntry(entry);
   
   
   //============== coincident time
   int nCT = 8 * 24;
   int coinTime[nCT];
   for( int i = 0 ; i < nCT; i ++){
      coinTime[i] = -10000;
   }
   
   bool refCoinTime = true;
   
   for( int i = 0; i < 8; i++){
      if( !TMath::IsNaN(rdt[i]) ) {
         
         int rdtTime = (int) rdt_t[i];
         
         for( int j = 0; j < 24; j++){
            
            if( !TMath::IsNaN(e[j]) ) {
               int eTime = (int) e_t[j];
               coinTime[8*j+i] = eTime-rdtTime;
               
               if( TMath::Abs(coinTime[8*j+i]) < 20 ) refCoinTime = false;
               
            }
         }
      }
   }
   
   //if( refCoinTime ) return kTRUE;  //######### coinTime gate
   
   for( int i = 0; i < nCT; i++){
      hTDiff->Fill(coinTime[i]);
   }
   
   //============== check pass RDT gate
   int rdtMultiHit = 0;
   for( int i = 0; i < 8; i++){
      if( !TMath::IsNaN(rdt[i]) ) rdtMultiHit ++;
   }
   
   //if( rdtMultiHit != 2 ) return kTRUE; //######### multiHit gate
   
   bool refRDT1 = true; if( cut[0]->IsInside( rdt[4], rdt[0] )) refRDT1 = false;
   bool refRDT2 = true; if( cut[1]->IsInside( rdt[5], rdt[1] )) refRDT2 = false;
   bool refRDT3 = true; if( cut[2]->IsInside( rdt[6], rdt[2] )) refRDT3 = false;
   bool refRDT4 = true; if( cut[3]->IsInside( rdt[7], rdt[3] )) refRDT4 = false;
   
   //if( refRDT1 && refRDT2 && refRDT3 && refRDT4) return kTRUE; //######### rdt gate
   
   //============= coincdient with recoil and array
   bool rejArrayRDT = true;
   for( int i = 0; i < 8; i++){
      if( !TMath::IsNaN(rdt[i]) ) {
         
         for( int j = 0; j < 24; j++){
            if (( i == 0 || i == 4) && 6  <= j && j < 12 && !TMath::IsNaN(e[j]) ) rejArrayRDT = false; 
            if (( i == 1 || i == 5) && 12 <= j && j < 18 && !TMath::IsNaN(e[j]) ) rejArrayRDT = false; 
            if (( i == 2 || i == 6) && 18 <= j && j < 24 && !TMath::IsNaN(e[j]) ) rejArrayRDT = false; 
            if (( i == 3 || i == 7) && 0  <= j && j < 6  && !TMath::IsNaN(e[j]) ) rejArrayRDT = false; 
         }
         
      }
   }
   
   //if( rejArrayRDT ) return kTRUE; //######### Array -rdt gate
   
   for( int i = 0; i < 8; i++){
      if( !TMath::IsNaN(rdt[i]) ) {
         hRDTa->Fill(rdt[i], i); // x, y
   
         for( int j = 0; j < 24; j++){
            if( !TMath::IsNaN(e[j]) ) hArrayRDT->Fill(j, i);
         }
      }
   }
   
   
   //============= recoil
   for( int i = 0; i < 4; i++){
      if(!TMath::IsNaN(rdt[i]) && !TMath::IsNaN(rdt[i+4])) {
         hdEE[i]->Fill(rdt[i+4], rdt[i]); //x, y
         if( cut[i]->IsInside( rdt[i+4], rdt[i] ) ) hdEEG[i]->Fill(rdt[i+4], rdt[i]);
      }
   }
   
   for( int i = 0; i < 8; i++){
      hRDT[i]->Fill(rdt[i]);
   }
   
   //============= S1 and recoil
   bool rejS1RDT = true;
   for( int i = 0; i < 8; i++){
      if( !TMath::IsNaN(rdt[i]) ) {
         hRDTa->Fill(rdt[i], i); // x, y
   
         for( int j = 4; j < 8; j++){
            if( !TMath::IsNaN(elum[j]) ) {
               hS1RDT->Fill(j, i);
               rejS1RDT = false;
            }
         }
      }
   }
   
   if( rejS1RDT ) return kTRUE; //######### S1 -rdt gate
   
   //============ ELUM
   for( int i = 4; i < 8 ; i ++){
      if( !TMath::IsNaN(elum[i]))hElum->Fill( elum[i], i );
   }
   


   //============= e and x
   for( int i = 0 ; i < 24; i++){
      
      if( i == 11 || i == 12 ) continue;
      
      if( TMath::IsNaN(e[i]) ) continue;
      if( TMath::IsNaN(xf[i])  && TMath::IsNaN(xn[i])  ) continue;
      if( xf[i] == 0  && xn[i] == 0) continue;
      
      //printf("%2d| %f, %f, %f \n",i , e[i], xf[i], xn[i]);
      
      double eC = TMath::QuietNaN();
      eC   = e[i]/eCorr[i][0] + eCorr[i][1];  
      
      
      bool xfQ = TMath::IsNaN(xf[i]) || xf[i] == 0;
      bool xnQ = TMath::IsNaN(xn[i]) || xn[i] == 0; 
       
      double xfC = TMath::QuietNaN();
      double xnC = TMath::QuietNaN();
       
      if( !xfQ) xfC = xf[i] * xfxneCorr[i][1] + xfxneCorr[i][0] ;
      if( !xnQ) xnC = xn[i] * xnCorr[i] * xfxneCorr[i][1] + xfxneCorr[i][0];
      
      //mapping correction 
      if( 12 <= i && i <= 17 ) {
         float temp = xnC;
         xnC = xfC;
         xfC = temp;
      } 
      
      Float_t x = TMath::QuietNaN();
      if(!xfQ  && !xnQ ) {
         x = (xfC-xnC)/(xfC+xnC);
      }   
      if( xfQ && !xnQ ){
         x = 1.0-2.0*xnC/e[i];
      }   
      if( !xfQ && xnQ ){
         x = 2.0*xfC/e[i]-1.0;
      }
            
      //printf("         %f, %f | %f, %f \n", x, e[i], 2*xf[i]/e[i]-1, 1-2*xn[i]/e[i]);
      hEX[i]->Fill(x, eC);
      
      double z = 0;
      int detID = i%rDet;
      if( pos[detID] < 0 ){
         z = pos[detID] - (-x + 1.)*length/2 ; 
      }else{
         z = pos[detID] + (x + 1.)*length/2 ; 
      }
      
      hEZ->Fill( z, eC);
      
   }
   
   return kTRUE;
}

void Inspector::SlaveTerminate()
{

}

void Inspector::Terminate()
{
   printf("=========== done. \n");
   //gStyle->SetOptStat(0);
   gStyle->SetOptStat(111111111);
   gStyle->SetLabelSize(0.04, "XY");
   gStyle->SetTitleFontSize(0.1);
   
   //Int_t Div[2] = {4,4};
   Int_t Div[2] = {2,1};
   Int_t size[2] = {400,300}; //x,y
   TCanvas * cInspector = new TCanvas("cInspector", openFileName, 0, 0, size[0]*Div[0], size[1]*Div[1]);
   if(cInspector->GetShowEditor() )cInspector->ToggleEditor();
   if(cInspector->GetShowToolBar() )cInspector->ToggleToolBar();
   cInspector->Divide(Div[0],Div[1]);
   for( int i = 1; i <= Div[0]*Div[1] ; i++){
      cInspector->cd(i)->SetGrid();
      //cInspector->cd(i)->SetLogz();
   }
   
   cInspector->cd(1); hS1RDT->Draw("colz");
   cInspector->cd(2); hElum->Draw("colz");
   
   //cInspector->cd(1); hdEE[0]->Draw("colz"); cut[0]->Draw("same");
   //cInspector->cd(2); hdEE[1]->Draw("colz"); cut[1]->Draw("same");
   //cInspector->cd(3); hdEE[2]->Draw("colz"); cut[2]->Draw("same");
   //cInspector->cd(4); hdEE[3]->Draw("colz"); cut[3]->Draw("same");
   
   //cInspector->cd(5); hdEEG[0]->Draw("colz");
   //cInspector->cd(6); hdEEG[1]->Draw("colz");
   //cInspector->cd(7); hdEEG[2]->Draw("colz");
   //cInspector->cd(8); hdEEG[3]->Draw("colz");
   
   
   //cInspector->cd(1) ; hRDT[0] ->Draw();
   //cInspector->cd(5) ; hRDT[1] ->Draw();
   //cInspector->cd(9) ; hRDT[2] ->Draw();
   //cInspector->cd(13); hRDT[3] ->Draw();
   //cInspector->cd(2); hRDT[4] ->Draw();
   //cInspector->cd(6); hRDT[5] ->Draw();
   //cInspector->cd(10); hRDT[6] ->Draw();
   //cInspector->cd(14); hRDT[7] ->Draw();

   
   //cInspector->cd(5); hRDTa->Draw("colz");
   //cInspector->cd(6); hArrayRDT->Draw("colz");
   //cInspector->cd(7); hTDiff->Draw("colz");
   //cInspector->cd(8); hEZ->Draw("scat");
   
   /*
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
   */
}
