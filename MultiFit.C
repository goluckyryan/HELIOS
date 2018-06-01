int nPeaks = 16;

TString gate, gateB, gate_cm;

Double_t fpeaks(Double_t *x, Double_t *par) {
   Double_t result = 0;
   for (Int_t p=0;p<nPeaks;p++) {
      Double_t norm  = par[3*p+0];
      Double_t mean  = par[3*p+1];
      Double_t sigma = par[3*p+2];
      result += norm * TMath::Gaus(x[0],mean,sigma, 1);
   }
   return result;
}

void MultiFit(){   

/**///======================================================== initial input
   
   //const char* rootfile="psd_run38.root"; const char* treeName="psd_tree";
   //const char* rootfile="H052_Mg25.root"; const char* treeName="gen_tree";
   //const char* rootfile="X_H052_Mg25.root"; const char* treeName="tree";
   const char* rootfile="test_3.root"; const char* treeName="tree";
   
/**///========================================================  load tree

   TFile *f0 = new TFile (rootfile, "read"); 
   TTree *tree = (TTree*)f0->Get(treeName);
   printf("=====> /// %15s //// is loaded. Total #Entry: %10d \n", rootfile,  tree->GetEntries());
   
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
   
/**///========================================================= Analysis
   
   tree->Draw("0.03815* z - 14.76 - e >> spec(400, -1, 9)", "loop==1 && detID == 5", "colz");

   TSpectrum * specPeak = new TSpectrum(20);
   int nPeaks = specPeak->Search(spec, 1 ,"", 0.05);
   float * xpos = specPeak->GetPositionX();
   
   int * inX = new int[nPeaks];
   TMath::Sort(nPeaks, xpos, inX, 0 );  
   vector<double> energy;   
   for( int j = 0; j < nPeaks; j++){
      printf(" %d , x: %8.3f \n", j, xpos[inX[j]]);
      energy.push_back(xpos[inX[j]]);
   }
   
   vector<double> knownE;
   knownE.push_back(0);
   knownE.push_back(0.74);
   knownE.push_back(4.78);
   knownE.push_back(5.83);
   knownE.push_back(6.36);
   knownE.push_back(8.0);
   knownE.push_back(9.0);
   
   // convert to real energy 
   int numPeak = knownE.size();
   TGraph * ga = new TGraph(numPeak, &energy[0], &knownE[0] );
   ga->Draw("*ap");
   ga->Fit("pol1", "");
   double eC0 = pol1->GetParameter(0);
   double eC1 = pol1->GetParameter(1);
   printf("====  eC0:%8.3f, eC1:%8.3f \n", eC0, eC1);
   
   vector<double> realEnergy;
   for( int j = 0; j < nPeaks; j++){
      realEnergy.push_back(energy[j] * eC1 + eC0);
      printf(" %d , e: %8.3f \n", j, realEnergy[j]);
   }
   
   tree->Draw("(0.03815* z - 14.76 - e)*1.3164 +0.0056 >> spec2(400, -1, 10)", "loop==1 && detID == 5", "colz");

   //========== Fitting 
   TSpectrum * peak = new TSpectrum(20);
   double threshold = 0.1;
   nPeaks  = peak->Search(spec2, 1, "", threshold);
   printf("======== found %d peaks \n", nPeaks);
   float * xpos = peak->GetPositionX();
   float * ypos = peak->GetPositionY();
   
   int * inX = new int[nPeaks];
   TMath::Sort(nPeaks, xpos, inX, 0 );
   vector<double> energy, height;
   for( int j = 0; j < nPeaks; j++){
      energy.push_back(xpos[inX[j]]);
      height.push_back(ypos[inX[j]]);
   }
   
   const int  n = 3 * nPeaks;
   double * para = new double[n]; 
   for(int i = 0; i < nPeaks ; i++){
      para[3*i+0] = height[i] * 0.05 * TMath::Sqrt(TMath::TwoPi());
      para[3*i+1] = energy[i];
      para[3*i+2] = 0.05;
   }
   
   TF1 * fit = new TF1("fit", fpeaks, -1 , 10, 3* nPeaks );
   fit->SetNpx(1000);
   fit->SetParameters(para);
   spec2->Fit("fit", "q");
   
   const Double_t* paraE = fit->GetParErrors();
   const Double_t* paraA = fit->GetParameters();
   
   double bw = spec2->GetBinWidth(1);
   
   double * ExPos = new double[nPeaks];
   
   //for(int i = 0; i < nPeaks ; i++){
   //   ExPos[i] = paraA[3*i+1];
   //}
   
   double meanSigma = 0;
   
   for(int i = 0; i < nPeaks ; i++){
      ExPos[i] = paraA[3*i+1];
      printf("%2d , count: %8.0f(%3.0f), mean: %8.4f(%8.4f), sigma: %8.4f(%8.4f) \n", 
              i, 
              paraA[3*i] / bw,   paraE[3*i] /bw, 
              paraA[3*i+1], paraE[3*i+1],
              paraA[3*i+2], paraE[3*i+2]);
              
     meanSigma += paraA[3*i+2];
   }
   
   printf("======= mean Sigma : %f \n", meanSigma/nPeaks);
   
   
   
}

