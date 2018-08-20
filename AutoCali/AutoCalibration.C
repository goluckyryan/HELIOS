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
#include "../AutoCali/Cali_littleTree.h"
#include "../AutoCali/Check_e_x.C"
#include "../AutoCali/Cali_compareF.C"
#include "../AutoCali/Cali_xf_xn.C"
#include "../AutoCali/Cali_xf_xn_to_e.C"
#include "../AutoCali/Cali_e.h"

//==========================================
//         This file show the steps for calibration 
//
//==========================================

double eThreshold = 400;

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
   TString rootfileAlpha="../H060/data/gen_run09.root";
   
   //======== experimental data
   TChain * chain = new TChain("gen_tree");

   chain->Add("../H052/data/gen_run107.root"); 

   
/*   chain->Add("../H060/data/gen_run11.root");  //01
   chain->Add("../H060/data/gen_run12.root");  //02
   chain->Add("../H060/data/gen_run13.root");  //03
   chain->Add("../H060/data/gen_run15.root");  //04
   chain->Add("../H060/data/gen_run16.root");  //05
   chain->Add("../H060/data/gen_run17.root");  //06
   chain->Add("../H060/data/gen_run18.root");  //07
   chain->Add("../H060/data/gen_run19.root");  //08
   chain->Add("../H060/data/gen_run20.root");  //09
   chain->Add("../H060/data/gen_run21.root");  //10
   chain->Add("../H060/data/gen_run22.root");  //11
   chain->Add("../H060/data/gen_run23.root");  //12
   chain->Add("../H060/data/gen_run24.root");  //13
   chain->Add("../H060/data/gen_run25.root");  //14
   chain->Add("../H060/data/gen_run27.root");  //15
   chain->Add("../H060/data/gen_run28.root");  //16
   chain->Add("../H060/data/gen_run29.root");  //17
   chain->Add("../H060/data/gen_run30.root");  //18
*/
//   chain->Add("../H052/data/H052_Mg25.root");
   
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
   
   TString rootfileSim="transfer.root";
      
   if( option == 2 ) {
      printf("=============== creating smaller tree.\n");
      chain->Process("../AutoCali/Cali_littleTree.C+");
      Check_e_x("temp.root", eThreshold);
   }
   
   if( option == 3 ) {
      TFile *caliFile = new TFile ("temp.root", "read");
      if( !caliFile->IsOpen() ){
			printf("!!!!!!!!!!! no temp.root, please run step 2.!!!!!!!\n");
			return;
		}
      TTree * caliTree = (TTree*) caliFile->Get("tree");
      
      TFile *fs = new TFile (rootfileSim, "read"); 
      
      if(fs->IsOpen()){
         
         int eCdet = -1; 
         printf(" Choose detID (-1 for all & make new root): ");
         temp = scanf("%d", &eCdet);
         Cali_compareF(caliTree, fs, eCdet, eThreshold);
         
         if( eCdet == -1) {
            chain->Process("../AutoCali/Cali_e.C+");
            gROOT->ProcessLine(".q");
         }
      }else{
         printf("!!!!! cannot open transfer.root !!!!! \n");
         return;
      }
   }
   
   if( option == 4 ) {
      chain->Process("../AutoCali/Cali_e.C+");
      gROOT->ProcessLine(".q");
   }
   
   if( option == -1){
      Cali_xf_xn(atree);
      Cali_xf_xn_to_e(chain);
    
      printf("=============== creating smaller tree.\n");
      chain->Process("../AutoCali/Cali_littleTree.C+");
      
      TFile *caliFile = new TFile ("temp.root", "read"); 
      if( !caliFile->IsOpen() ){
			printf("!!!!!!!!!!! no temp.root, please run step 2.!!!!!!!\n");
			return;
		}
      TTree * caliTree = (TTree*) caliFile->Get("tree");
      
      TFile *fs = new TFile (rootfileSim, "read"); 
      
      if(fs->IsOpen()){
         Cali_compareF(caliTree, fs, -1, eThreshold);
      }else{
         printf("!!!!! cannot open transfer.root !!!!! \n");
         return;
      }   
      
      chain->Process("../AutoCali/Cali_e.C+");
   
      gROOT->ProcessLine(".q");
   
   }

   
}

