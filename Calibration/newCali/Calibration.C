#include <TROOT.h>
#include <TChain.h>
#include <TTree.h>
#include <TFile.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TH2F.h>
#include <TH1F.h>
#include <TF1.h>
#include <TMath.h>
#include <TSpectrum.h>
#include <TGraph.h>
#include <fstream>
#include "Cali_compare.C"

//==========================================
//         This file show the steps for calibration 
//
//==========================================

void Calibration(){
/*
   int option = 0;
   printf(" ========= Analysis ============== \n");
   printf(" xf - xn calibration should be done before\n"); //TODO, include alpha-calibration
   printf(" ================================= \n");
   printf(" 0 = generate C_*.root, remember to Set correction files in advance.\n");
   printf(" 1 = xf+xn to e calibration \n");
   printf(" 2 = e - same position calibration \n");
   printf(" 3 = e - diff position calibration \n");
   printf(" 4 = Ex calibration \n");
   printf(" 5 = tac calibration \n");
   printf(" 6 = Counting and Fit Spectrum \n");
   printf(" ========= Choose thing to do: ");
   scanf("%d", &option);

//====================================================
/**///================= the data files
/*
   if( option == 0 ){
      TChain * chain = new TChain("gen_tree");
      chain->Add("data/gen_run11.root");  //01
      chain->Add("data/gen_run12.root");  //02
      chain->Add("data/gen_run13.root");  //03
      chain->Add("data/gen_run15.root");  //04
      chain->Add("data/gen_run16.root");  //05
      chain->Add("data/gen_run17.root");  //06
      chain->Add("data/gen_run18.root");  //07
      chain->Add("data/gen_run19.root");  //08
      chain->Add("data/gen_run20.root");  //09
      chain->Add("data/gen_run21.root");  //10
      chain->Add("data/gen_run22.root");  //11
      chain->Add("data/gen_run23.root");  //12
      chain->Add("data/gen_run24.root");  //13
      chain->Add("data/gen_run25.root");  //14
      chain->Add("data/gen_run27.root");  //15
      chain->Add("data/gen_run28.root");  //16
      chain->Add("data/gen_run29.root");  //17
      chain->Add("data/gen_run30.root");  //18      
      chain->GetListOfFiles()->Print();
      
      //============ remember to Set correction files
      //============ generating a new root file for next analysis
      chain->Process("~/ANALYSIS/Calibration/Cali_root.C+");
   }
*/
   TTree *tree = NULL;
   TTree *cTree = NULL;
   const char* rootfile0="gen_run11.root";
   TFile *f0 = new TFile (rootfile0, "read"); 
   tree = (TTree*)f0->Get("gen_tree");
   
   const char* rootfile1="~/Desktop/HELIOS/Simulation/transfer.root";
   TFile *f1 = new TFile (rootfile1, "read"); 
   cTree = (TTree*)f1->Get("tree");
   
   //gROOT->ProcessLine(".L ~/Desktop/HELIOS/Calibration/newCali/Cali_compare.C+");
   
   Cali_compare(tree, cTree, 19);

/**///=========================================== Calibration
   
}

