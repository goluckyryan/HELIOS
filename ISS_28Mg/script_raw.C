#include "TFile.h"
#include "TTree.h"
#include "TObjArray.h"
#include "TH2F.h"
#include "TROOT.h"
#include "TStyle.h"
#include "TChain.h"
#include "TCutG.h"


void script_raw(){

   TChain * chain = new TChain("gen_tree");
   chain->Add("data/gen_run60_69.root");
   chain->Add("data/gen_run70_74.root");
   chain->Add("data/gen_run75_87.root");
   
   chain->GetListOfFiles()->Print();
   
   chain->Process("../../ISS_22Ne/sort_code/GeneralSort.C+");
   
   
}
