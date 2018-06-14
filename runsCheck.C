{
   TString rootFile;
   
   TFile * f = NULL;
   TTree * tree = NULL;
   
   const char* treeName="gen_tree";
   
   for( int i = 1; i <=46 ; i++){
      rootFile.Form("~/ANALYSIS/RUNs/H060_208Pb/gen_run%02d.root", i);
      
      f = new TFile (rootFile, "read"); 
      tree = (TTree*)f->Get(treeName);
      
      double size = f->GetSize(); // in byte
      printf("=====> /// %15s //// is loaded. Total #Entry: %10d, size: %6.1f MB \n", rootFile.Data(),  tree->GetEntries(), size/1024/1024);
      
   }
   
}
