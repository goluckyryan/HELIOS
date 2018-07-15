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
#include <TProof.h>
#include "Cali_compare.C"
#include "Cali_compare2.C"
#include "Cali_compareF.C"
#include "Cali_compareNew.C"
#include "Cali_xf_xn.C"
#include "Cali_xf_xn_to_e.C"
#include "Cali_e.h"

//==========================================
//         This file show the steps for calibration 
//
//==========================================

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
   int temp = scanf("%d", &option);
   
//==================================================== data files
   //======== alpha data
   //const char* rootfileAlpha="gen_run11.root";
   const char* rootfileAlpha="../../H060_ana/data/gen_run09.root";
   
   //======== Simulation data
   //const char* rootfileSim="~/Desktop/HELIOS/Simulation/transfer.root";
   const char* rootfileSim="../../Simulation/transfer.root";

   //======== experimental data
   TChain * chain = new TChain("gen_tree");
   chain->Add("../../H060_ana/data/gen_run11.root");  //01
//   chain->Add("gen_run11.root");  //01
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
      
   //TProof::Open("");
   //chain->SetProof();        
   chain->GetListOfFiles()->Print();
      
   TFile *fa = new TFile (rootfileAlpha, "read"); 
   TTree * atree = (TTree*)fa->Get("gen_tree");

   TFile *fs = new TFile (rootfileSim, "read"); 
   TTree * sTree = NULL;
   if( fs->IsOpen() ){
      sTree = (TTree*)fs->Get("tree");
   }else{
      printf("!!!!!!!!!!! please generate simulation data\n");
   }
   
/**///=========================================== Calibration
   if( option == 0 ) Cali_xf_xn(atree);
   if( option == 1 ) Cali_xf_xn_to_e(chain);
   if( option == 2 ) {
         
      int method = 0;
      printf("=====================\n");
      printf("Method-0 : find e/a+b for each det.\n");
      printf("Method-1 : same position detector share same.\n");
      printf("Method-2 : use fx .\n");
      
      //TODO printf("Method-2 : method-0 + x-scaling factor.\n");
      printf("====== Pick a method: ");
      temp = scanf("%d", &method);
      
      if( method == 0 ){
         int eCdet = -1; 
         printf(" Choose detID (-1 for all): ");
         temp = scanf("%d", &eCdet);
         Cali_compare(chain, sTree, eCdet);
      }

      if( method == 1) Cali_compare2(chain, sTree);

      if( method == 2) {
         int eCdet = -1; 
         printf(" Choose detID (-1 for all): ");
         temp = scanf("%d", &eCdet);
         Cali_compareF(chain, fs, eCdet);
      }
      
      
      //Cali_compareNew(chain, sTree, eCdet);
   }
   if( option == 3 ) chain->Process("Cali_e.C+");

   if( option == -1){
      Cali_xf_xn(atree);
      Cali_xf_xn_to_e(chain);
      
      int method = 0;
      printf("=====================\n");
      printf("Method-0 : find e/a+b for each det.\n");
      printf("Method-1 : same position detector share same.\n");
      printf("====== Pick a method: ");
      temp = scanf("%d", &method);
      
      if( method == 0 ) Cali_compare(chain, sTree, -1);
      if( method == 1 ) Cali_compare2(chain, sTree);
      
      chain->Process("Cali_e.C+");
   }

   
}

