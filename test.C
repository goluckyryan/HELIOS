//#include "TH1.h"
//#include "TF1.h"
//#include "TList.h"
//#include <fstream>

//void test(){
{  
   TString rootFile;
   TFile * f = NULL;
   TTree * tree = NULL;
   
   const char* treeName="gen_tree";

   int i = 11;
   rootFile.Form("~/ANALYSIS/RUNs/H060_208Pb/gen_run%02d.root", i);
   
   f = new TFile (rootFile, "read"); 
   tree = (TTree*)f->Get(treeName);
      
   ULong64_t e_t[100];
   TBranch  *b_EnergyTimestamp;
   tree->SetBranchAddress("e_t", e_t, &b_EnergyTimestamp);
   
      
}
