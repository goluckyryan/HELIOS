{   
/**///======================================================== initial input
   
   const char* rootfile0="test.root";        const char* treeName0="tree";
   const char* rootfile1="X_H052_Mg25.root"; const char* treeName1="tree";
   
/**///========================================================  load tree

   TFile *f0 = new TFile (rootfile0, "read"); 
   TTree *tree0 = (TTree*)f0->Get(treeName0);
   printf("=====> /// %15s //// is loaded. Total #Entry: %10d \n", rootfile0,  tree0->GetEntries());
   
   
   TFile *f1 = new TFile (rootfile1, "read"); 
   TTree *tree1 = (TTree*)f1->Get(treeName1);
   printf("=====> /// %15s //// is loaded. Total #Entry: %10d \n", rootfile1,  tree1->GetEntries());
   
/**///======================================================== Browser or Canvas

   //TBrowser B ;   
   Int_t Div[2] = {1,1};  //x,y
   Int_t size[2] = {800,600}; //x,y
   
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
   double l = 50.5; // length
   double support = 43.5;
   double firstPos = 218.5; // m 
   double pos[6] = {0. , 59., 118., 176.9, 236.2, 294.8}; // near position in m
/**///========================================================= Analysis
   
   TString expression, gate, gate_aux, gate_e;
   
   gate = "tag == 2 && TMath::Cos(thetaCM) < 0.985";
   
   expression.Form("thetaCM*TMath::RadToDeg() : Ex >> p");
   
   TH2F * p = new TH2F("p","p", 500, -1, 10, 500, 0, 50);
   tree0->Draw(expression, "tag == 2", "colz");
   
   TH2F * k = new TH2F("k","k", 200, -1, 10, 200, 0, 50);
   tree1->Draw("thetaCM:Ex>>k", "good == 1 && thetaCM > 0", "same");
      
   /**/
}

