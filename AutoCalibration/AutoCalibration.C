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
#include "../Simulation/transfer.C"
#include "Cali_smallTree.C"
#include "Cali_compareF.C"
#include "Cali_xf_xn.C"
#include "Cali_xf_xn_to_e.C"
#include "Cali_e.h"

//==========================================
//         This file show the steps for calibration 
//
//==========================================

void AutoCalibration(){
   
   int option;
   printf(" ========= Auto Calibration ======================= \n");
   printf(" ================================================== \n");
   printf(" 0 = alpha source calibration for xf - xn.\n");
   printf(" 1 = xf+xn to e calibration. \n");
   printf(" 2 = Generate root file with e, x, z, detID, multi.\n");
   printf(" --------- transfer.root reqaure below ------------\n");
   printf(" 3 = e calibration by compare with simulation.\n");
   printf(" 4 = Generate new root with calibrated data. \n");
   printf(" ================================================== \n");
   printf(" Choose action (-1 for all): ");
   int temp = scanf("%d", &option);
   
//==================================================== data files
   
   //======== alpha data
   TString rootfileAlpha="../H060_ana/data/gen_run09.root";
   

   //======== experimental data
   TChain * chain = new TChain("gen_tree");
   chain->Add("../H060_ana/data/gen_run11.root");  //01
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
   printf(" ================ files :  \n");
   chain->GetListOfFiles()->Print();
   printf(" ================================================== \n");
      
   TFile *fa = new TFile (rootfileAlpha, "read"); 
   TTree * atree = (TTree*)fa->Get("gen_tree");

   
/**///=========================================== Calibration
   if( option == 0 ) Cali_xf_xn(atree);
   if( option == 1 ) Cali_xf_xn_to_e(chain);
   
   TString tempFileName = "temp.root";
   TString rootfileSim="../Simulation/transfer.root";
      
   if( option == 2 ) {
      printf("=============== creating smaller tree.\n");
      Cali_smallTree(chain, tempFileName);   
   }
   
   if( option == 3 ) {
      TFile *caliFile = new TFile (tempFileName, "read"); 
      TTree * caliTree = (TTree*) caliFile->Get("tree");
      
      TFile *fs = new TFile (rootfileSim, "read"); 
      
      if(fs->IsOpen()){
         
         int eCdet = -1; 
         printf(" Choose detID (-1 for all): ");
         temp = scanf("%d", &eCdet);
         Cali_compareF(caliTree, fs, eCdet);
         
      }else{
         printf("!!!!! cannot open transfer.root !!!!! \n");
         return;
      }
   }
   
   if( option == 4 ) chain->Process("Cali_e.C+");

   if( option == -1){
      Cali_xf_xn(atree);
      Cali_xf_xn_to_e(chain);
    
      printf("=============== creating smaller tree.\n");
      TString tempFileName = "temp.root";
      Cali_smallTree(chain, tempFileName);   
      
      TFile *caliFile = new TFile (tempFileName, "read"); 
      TTree * caliTree = (TTree*) caliFile->Get("tree");
      
      TFile *fs = new TFile (rootfileSim, "read"); 
      
      if(fs->IsOpen()){
         Cali_compareF(caliTree, fs, -1);
      }else{
         printf("!!!!! cannot open transfer.root !!!!! \n");
         return;
      }   
      
      chain->Process("Cali_e.C+");
   }

   
}

