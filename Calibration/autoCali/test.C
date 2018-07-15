//#include "TH1.h"
//#include "TF1.h"
//#include "TList.h"
//#include <fstream>

//void test(){
{  
   const char* rootfileAlpha="../../H060_ana/data/gen_run11.root";
   const char* rootfileSim="../../Simulation/transfer.root";
   
   TFile *fa = new TFile (rootfileAlpha, "read"); 
   TTree * atree = (TTree*)fa->Get("gen_tree");

   TFile *fs = new TFile (rootfileSim, "read"); 
   TTree * sTree = (TTree*)fs->Get("tree");
   
   gROOT->ProcessLine(".L Cali_compare2.C");
   
   Cali_compare2(atree, sTree);
   


/*
  const char* tempfile="temp.root";
      TFile *f0 = new TFile (tempfile, "read"); 
      TTree * sTree = (TTree*)f0->Get("tree"); // s for seleced
      printf("========== number of event : %d \n", sTree->GetEntries()); 
      
      double  eS; TBranch * b_eS;   //!
      double  xS; TBranch * b_xS;   //!
      //int detIDS; TBranch * b_detIDS; //!

      sTree->SetBranchAddress("e1", &eS, &b_eS);
      sTree->SetBranchAddress("x1", &xS, &b_xS);
      //sTree->SetBranchAddress("detID1", &detIDS, &b_detIDS);

   TH2F * exPlot  = new TH2F("exPlot" , "exPlot" , 200, -1.3, 1.3, 200, 0, 10);
   TH2F * exPlotC = new TH2F("exPlotC", "exPlotC", 200, -1.3, 1.3, 200, 0, 10);

  Int_t Div[2] = {2,1};  //x,y
   Int_t size[2] = {400,400}; //x,y
   
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

   double B1[24];
   double B0[24];

   B1[2]  = 275.228615;
   B1[2]  = 254.6;
   B1[8]  = B1[2];
   B1[14] = B1[2];
   B1[20] = B1[2];

   B0[2]  = -0.17964;
   B0[8]  =  0.240221-0.1;
   B0[14] = -0.661694;
   B0[20] = -0.036444;

   B0[2] = -0.516;
   B0[8] = -0.173;


         cScript->cd(1);
         exPlot->Reset();
         for( int i = 0; i < sTree->GetEntries() ; i++){
            sTree->GetEntry(i);
            if( detIDS > 8 ) continue;
            exPlot->Fill(xS, eS/B1[detIDS] + B0[detIDS]);
         }
         exPlot->Draw();
         cScript->Update();

         B0[8] -= 0.05;

         cScript->cd(2);
         exPlotC->Reset();
         for( int i = 0; i < sTree->GetEntries() ; i++){
            sTree->GetEntry(i);
            if( detIDS > 8 ) continue;
            exPlotC->Fill(xS, eS/B1[detIDS] + B0[detIDS]);
         }
         exPlotC->Draw("");            
         cScript->Update();
         
     
  /*
   TCanvas *c = new TCanvas("c","Graph2D example",0,0,600,400);
   Double_t x, y, z, P = 6.;
   Int_t np = 300;
   TGraph2D *dt = new TGraph2D();
   dt->SetTitle("Graph title; X axis title; Y axis title; Z axis title");
   TRandom *r = new TRandom();
   for (Int_t N=0; N<np; N++) {
      x = 2*P*(r->Rndm(N))-P;
      y = 2*P*(r->Rndm(N))-P;
      z = (sin(x-1.0)/(x-1.0))*(sin(y)/y)+0.2;
      dt->SetPoint(N,x,y,z);
      //dt->Draw("TRI1");
      //c->Update();
   }
   //gStyle->SetPalette(1);
   dt->Draw("TRI1");
  */


   
}
