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
#include "../AutoCali/Cali_littleTree_trace.h"
#include "../AutoCali/Check_e_x.C"
#include "../AutoCali/Cali_compareF.C"
#include "../AutoCali/Cali_xf_xn.C"
#include "../AutoCali/Cali_xf_xn_to_e.C"
#include "../AutoCali/Cali_e_trace.h"
#include "../AutoCali/GetCoinTimeCorrectionCutG.C"

//==========================================
//         This file show the steps for calibration 
//         //TODO detect branch te_t and trdt_t exist or not, if exist, calibrate with coinTime
//==========================================

double eThreshold = 400;

void AutoCalibrationTrace(){
   
   int option;
   printf(" ========= Auto Calibration w/ Trace ============== \n");
   printf(" ================================================== \n");
   printf(" --------- GeneralSortTrace.C --> sorted.root------  \n");
   printf(" ================================================== \n");
   printf(" 0 = alpha source calibration for xf - xn.\n");
   printf(" 1 = xf+xn to e calibration. \n");
   printf(" 2 = Generate smaller root file with e, x, z, detID, coinTimeUC (aware of Gate)\n");
   printf(" 3 = coinTimeUC calibration. (MANUAL) \n");
   printf(" --------- transfer.root require below ------------\n");
   printf(" 4 = e calibration by compare with simulation.\n");
   printf(" 5 = Generate new root with calibrated data. \n");
   printf(" ================================================== \n");
   printf(" Choose action : ");
   int temp = scanf("%d", &option);
   
//==================================================== data files
   
   //======== alpha data
   TChain * chainAlpha = new TChain("gen_tree");
   //chainAlpha->Add("data/gen_run11.root");
   //chainAlpha->Add("data/gen_run19.root"); //22Ne
   chainAlpha->Add("data/gen_run34.root"); //28Mg
   
   //======== experimental sorted data
   TChain * chain = new TChain("gen_tree");

   chain->Add("data/gen_run60_69.root");
   chain->Add("data/gen_run70_74.root");
   chain->Add("data/gen_run75_87.root");
   //chain->Add("data/gen_run54.root");
   //chain->Add("data/gen_run52.root");
   //chain->Add("data/gen_run50.root");

/*=============== 208Pb   
   chain->Add("../H060/data/gen_run11.root");  //01
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

/**///=========================================== Calibration
   if( option > 5 || option < 0 ) return;
   
   if( option == 0 ) {
      printf(" ================ alpha source files :  \n");
      chainAlpha->GetListOfFiles()->Print();
      printf(" ================================================== \n");
      Cali_xf_xn(chainAlpha);
      return ;
   }
   
      
   printf(" ================ files :  \n");
   chain->GetListOfFiles()->Print();
   printf(" ================================================== \n");

   
   if( option == 1 ) Cali_xf_xn_to_e(chain);
   
   TString rootfileSim="transfer.root";
      
   if( option == 2 ) {
      printf("=============== creating smaller tree.\n");
      chain->Process("../AutoCali/Cali_littleTree_trace.C+");
      Check_e_x("temp.root", eThreshold);
   }
   
   if( option == 3 ) {
      int det = -1; 
      printf(" Choose detID (-1 for all): ");
      temp = scanf("%d", &det);
      if( det > -1 ){
         GetCoinTimeCorrectionCutG("temp.root", det);
      }else{
         for(int iDet = 0; iDet < 24; iDet ++){
            GetCoinTimeCorrectionCutG("temp.root", iDet);
         }
      }
      gROOT->ProcessLine(".q");
   }
   
   if( option == 4 ) {
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
            chain->Process("../AutoCali/Cali_e_trace.C+");
            gROOT->ProcessLine(".q");
         }
      }else{
         printf("!!!!! cannot open transfer.root !!!!! \n");
         return;
      }
   }
   
   if( option == 5 ) {
      chain->Process("../AutoCali/Cali_e_trace.C+");
      gROOT->ProcessLine(".q");
   }
   
}

