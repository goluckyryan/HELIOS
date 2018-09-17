#include "TROOT.h"
#include "TStyle.h"
#include "TFile.h"
#include "TTree.h"
#include "TH2F.h"
#include "TLine.h"
#include "TCanvas.h"
#include "TCutG.h"
#include "TRandom.h"
#include "TF1.h"

void xResol(){

   int numEvent = 100000;
   double resol = 0.5;

   //====================== build tree
   TString saveFileName = "xResol.root";
   TFile * saveFile = new TFile(saveFileName, "recreate");
   TTree * tree = new TTree("tree", "tree");
   
   double x;
   double x1, x2, x3, x4;
   double xf, xn, e;
   
   tree->Branch("xf", &xf, "xf/D");
   tree->Branch("xn", &xn, "xn/D");
   tree->Branch("e" ,  &e, "e/D");
   
   tree->Branch("x1", &x1, "x1/D");
   tree->Branch("x2", &x2, "x2/D");
   tree->Branch("x3", &x3, "x3/D");
   tree->Branch("x4", &x4, "x4/D");
   
   tree->Branch("x ",  &x, "x/D");
   
   for( int i = 0; i < numEvent; i++){
      
      e  = 50*gRandom->Rndm();
      x = 2*(gRandom->Rndm()-0.5); //(-1,1);
      
      xf = e*(x + 1.0)/2 + gRandom->Gaus(0, 1.0*resol);
      xn = e*(1.0 - x)/2 + gRandom->Gaus(0, 1.0*resol);
      e = e + gRandom->Gaus(0, resol);
      
      x1 = (xf-xn)/(xf+xn);
      x2 = 1 - 2* xn/e;
      x3 = 2* xf/e - 1;
      x4 = (xf-xn)/e;
      
      tree->Fill();
   }
   
   saveFile->Write();
   saveFile->Close();
   
   printf("=============== done. saved as %s.\n", saveFileName.Data());
   gROOT->ProcessLine(".q");

}
