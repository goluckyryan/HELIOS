#include "TFile.h"
#include "TTree.h"
#include "TObjArray.h"
#include "TH2F.h"
#include "TROOT.h"
#include "TStyle.h"
#include "TChain.h"
#include "TCutG.h"


void script_Inspector(){

   TChain * chain = new TChain("tree");
   chain->Add("run60.root");
   chain->Add("run61.root");
   chain->Add("run62.root");
   chain->Add("run63.root");
   chain->Add("run64.root");
   chain->Add("run65.root");
   chain->Add("run66.root");
   chain->Add("run67.root");
   chain->Add("run68.root");
   chain->Add("run69.root");
   
   chain->GetListOfFiles()->Print();
   
   chain->Process("../../ISS_22Ne/sort_code/GeneralSort.C+");
}
