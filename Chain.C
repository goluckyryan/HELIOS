//void Script(){   
{

   TChain * chain = new TChain("gen_tree");
   chain->Add("data/gen_run32.root");
   chain->Add("data/gen_run33.root");
   chain->Add("data/gen_run34.root");
   chain->Add("data/gen_run35.root");
   
   //TTree * tree = chain;
   chain.GetListOfFiles()->Print();
   
   chain.Process("~/ANALYSIS/Calibration/Cali_root.C+");


   //gROOT->ProcessLine(".x ~/ANALYSIS/Calibration/Cali_e_samePos.C(chain)");
   

/**///======================================================== initial input
/*   
   //const char* rootfile="psd_run38.root"; const char* treeName="psd_tree";
   //const char* rootfile="H052_Mg25.root"; const char* treeName="gen_tree";
   //const char* rootfile="X_H052_Mg25.root"; const char* treeName="tree";
   const char* rootfile="test_3.root"; const char* treeName="tree";
   
   const string detGeoFileName = "detectorGeo_upstream.txt";
   bool isLoadDetGeo = false;
   
/**///========================================================  load tree
/*
   TFile *f0 = new TFile (rootfile, "read"); 
   TTree *tree = (TTree*)f0->Get(treeName);
   printf("=====> /// %15s //// is loaded. Total #Entry: %10d \n", rootfile,  tree->GetEntries());
   
/**/   
}

