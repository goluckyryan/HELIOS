{   

/**///======================================================== initial input
   
   const char* rootfile="test_2.root"; const char* treeName="tree";
   
/**///========================================================  load tree

   TFile *f0 = new TFile (rootfile, "read"); 
   TTree *tree = (TTree*)f0->Get(treeName);
   printf("=====> /// %15s //// is loaded. Total #Entry: %10d \n", rootfile,  tree->GetEntries());
   
/**///======================================================== Browser or Canvas

   //TBrowser B ;   
   Int_t Div[2] = {3,1};  //x,y
   Int_t size[2] = {500,500}; //x,y
   
   TCanvas * cScript = new TCanvas("cScript", "cScript", 0, 0, size[0]*Div[0], size[1]*Div[1]);
   cScript->Divide(Div[0],Div[1]);
   for( int i = 1; i <= Div[0]*Div[1] ; i++){
      cScript->cd(i)->SetGrid();
   }
   cScript->cd(1);

   gStyle->SetOptStat(1111111);
   gStyle->SetStatY(1.0);
   gStyle->SetStatX(0.99);
   gStyle->SetStatW(0.2);
   gStyle->SetStatH(0.1);
   
/**///========================================================= load files
   
/**///========================================================= Analysis

   cScript->cd(1);
   tree->Draw("e:z>>k(400, -550, -150, 400, -1, 12) ", "", "colz");
   
   cScript->cd(2);
   tree->Draw("Ex>>h(400, -1, 8)");

   cScript->cd(3);
   tree->Draw("thetaCM:z>>p(400, -550, -150, 400, 0, 50)", "tag == 2", "colz");
}

