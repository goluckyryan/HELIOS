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
#include "/home/ttang/ANALYSIS/Calibration/Cali_xf_xn_to_e.C"
#include "/home/ttang/ANALYSIS/Calibration/Cali_e_samePos.C"
#include "/home/ttang/ANALYSIS/Calibration/Cali_e_diffPos.C"
#include "/home/ttang/ANALYSIS/Calibration/Cali_physical.C"
#include "/home/ttang/ANALYSIS/Calibration/Cali_tac.C"
#include "/home/ttang/ANALYSIS/Calibration/Count.C"

//==========================================
//         This file show the steps for calibration 
//
//==========================================

void Analysis(){

   int option = 0;
   printf(" ========= Analysis ============== \n");
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

   if( option == 0 ){
      TChain * chain = new TChain("gen_tree");
      chain->Add("data/gen_run11.root");
      chain->Add("data/gen_run12.root");
      chain->Add("data/gen_run13.root");
      chain->Add("data/gen_run15.root");
      chain->Add("data/gen_run16.root");
      chain->Add("data/gen_run17.root");
      chain->Add("data/gen_run18.root");
      chain->Add("data/gen_run19.root");
      chain->Add("data/gen_run20.root");
      chain->Add("data/gen_run21.root");
      chain->Add("data/gen_run22.root");
      chain->Add("data/gen_run23.root");
      chain->Add("data/gen_run24.root");
      chain->Add("data/gen_run25.root");
      chain->Add("data/gen_run26.root");
      chain->Add("data/gen_run27.root");
      chain->Add("data/gen_run28.root");
      chain->Add("data/gen_run29.root");
      chain->Add("data/gen_run30.root");
      
      chain->GetListOfFiles()->Print();
      
      //============ remember to Set correction files
      //============ generating a new root file for next analysis
      chain->Process("~/ANALYSIS/Calibration/Cali_root.C+");
   }
/**///=========================================== load C_root
   
   TTree *tree = NULL;
   if( option > 0 ){
      const char* rootfile="~/ANALYSIS/H060_ana/C_gen_run11.root";
      TFile *f0 = new TFile (rootfile, "read"); 
      tree = (TTree*)f0->Get("tree");
   }
   
   //========= for xf+xn to e
   if(option == 1) Cali_xf_xn_to_e(tree);
   
   //========= for e_samePos
   // need to adjust the setting in the *.C
   if(option == 2) Cali_e_samePos(tree);
   
   //========= for e_diffPos
   // need to adjust the setting in the *.C
   if(option == 3) Cali_e_diffPos(tree);
   
   //========= for Ex
   // need to adjust the setting in the *.C
   if(option == 4) Cali_physical(tree);
   
   //========= for TAC 
   // need to adjust the setting in the *.C
   if(option == 5) Cali_tac(tree);
   
//============================= Fitting and Count
   // need to adjust the setting in the *.C
   if(option == 6) Count(tree, -1, 0, 0.1);   

/**/
   
}

