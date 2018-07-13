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
#include "Cali_xf_xn.C"
#include "Cali_xf_xn_to_e.C"
#include "Cali_e.h"

//==========================================
//         This file show the steps for calibration 
//
//==========================================

int eCdet = 4; // e-correction detID

void AutoCalibration(){
   
   int option;
   printf(" ========= Auto Calibration ================== \n");
   printf(" ============================================= \n");
   printf(" 0 = alpha source calibration for xf - xn.\n");
   printf(" 1 = xf+xn to e calibration. \n");
   printf(" 2 = e calibration by compare with simulation.\n");
   printf(" 3 = Generate new root with calibrated data. \n");
   printf(" ============================================= \n");
   printf(" Choose action (-1 for all): ");
   scanf("%d", &option);
   
//==================================================== data files
   //======== alpha data
   const char* rootfileAlpha="gen_run11.root";
   //const char* rootfileAlpha="~/ANALYSIS/H060_ana/data/gen_run09.root";
   
   //======== Simulation data
   const char* rootfileSim="~/Desktop/HELIOS/Simulation/transfer.root";
   //const char* rootfileSim="~/ANALYSIS/Simulation/transfer.root";

   //======== experimental data
   //const char* rootfile0="gen_run11.root";
   TChain * chain = new TChain("gen_tree");
//   chain->Add("~/ANALYSIS/H060_ana/data/gen_run11.root");  //01
   chain->Add("gen_run11.root");  //01
/*   chain->Add("data/gen_run12.root");  //02
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
*/         
   chain->GetListOfFiles()->Print();
      
   TFile *fa = new TFile (rootfileAlpha, "read"); 
   TTree * atree = (TTree*)fa->Get("gen_tree");

   TFile *fs = new TFile (rootfileSim, "read"); 
   TTree * sTree = (TTree*)fs->Get("tree");
   
/**///=========================================== Calibration
   if( option == 0 ) Cali_xf_xn(atree);
   if( option == 1 ) Cali_xf_xn_to_e(chain);
   if( option == 2 ) Cali_compare(chain, sTree, eCdet);
   if( option == 3 ) chain->Process("~/ANALYSIS/Calibration/Cali_e.C+");
   if( option == -1){
      Cali_xf_xn(atree);
      Cali_xf_xn_to_e(chain);
      Cali_compare(chain, sTree);
      chain->Process("~/ANALYSIS/Calibration/Cali_e.C+");
   }

   
}

