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
//#include "/home/ttang/ANALYSIS/Calibration/Cali_xf_xn_to_e.C"
//#include "/home/ttang/ANALYSIS/Calibration/Cali_e_samePos.C"
//#include "/home/ttang/ANALYSIS/Calibration/Cali_e_diffPos.C"
//#include "/home/ttang/ANALYSIS/Calibration/Cali_physical.C"
//#include "/home/ttang/ANALYSIS/Calibration/Cali_tac.C"
//#include "/home/ttang/ANALYSIS/Calibration/Count.C"

//==========================================
//         This file show the steps for calibration 
//
//==========================================

void Calibration(){

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

   TTree *tree = NULL;
   if( option > 0 ){
      const char* rootfile="~/ANALYSIS/H060_ana/C_gen_run11.root";
      TFile *f0 = new TFile (rootfile, "read"); 
      tree = (TTree*)f0->Get("tree");
   }

/**///=========================================== Calibration
   
   //========= for xf+xn to e
   if(option == 1){
      gROOT->ProcessLine(".L /home/ttang/ANALYSIS/Calibration/Cali_xf_xn_to_e.C+");
      Cali_xf_xn_to_e(tree); // need to be gen_tree
   }
   
   //========= for e_samePos
   // need to adjust the setting in the *.C
   if(option == 2) {
      gROOT->ProcessLine(".L /home/ttang/ANALYSIS/Calibration/Cali_e_samePos.C+");
      Cali_e_samePos(tree);
   }
   //========= for e_diffPos
   // need to adjust the setting in the *.C
   if(option == 3) {
      gROOT->ProcessLine(".L /home/ttang/ANALYSIS/Calibration/Cali_e_diffPos.C+");
      Cali_e_diffPos(tree);
   }
   
   //========= for Ex
   // need to adjust the setting in the *.C
   if(option == 4) {
      gROOT->ProcessLine(".L /home/ttang/ANALYSIS/Calibration/Cali_physical.C+");
      Cali_physical(tree);
   }
   
   //========= for TAC 
   // need to adjust the setting in the *.C
   if(option == 5) {
      gROOT->ProcessLine(".L /home/ttang/ANALYSIS/Calibration/Cali_tac.C+");
      Cali_tac+(tree);
   }
//============================= Fitting and Count
   // need to adjust the setting in the *.C
   if(option == 6) {
      gROOT->ProcessLine(".L /home/ttang/ANALYSIS/Calibration/Count.C+");
      Count(tree, -1, 0, 0.1);   
   }
/**/
   
}

