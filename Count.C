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


void Count(int detID, double threshold = 0.1){   

/**///======================================================== initial input
   
   //const char* rootfile="psd_run38.root"; const char* treeName="psd_tree";
   //const char* rootfile="H052_Mg25.root"; const char* treeName="gen_tree";
   const char* rootfile="X_H052_Mg25.root"; const char* treeName="tree";
   
   const int numDet = 4;
   
/**///========================================================  load tree

   TFile *f0 = new TFile (rootfile, "read"); 
   TTree *tree = (TTree*)f0->Get(treeName);
   printf("=====> /// %15s //// is loaded. Total #Entry: %10d \n", rootfile,  tree->GetEntries());
   
/**///======================================================== Browser or Canvas

   //TBrowser B ;   
   Int_t Div[2] = {1,2};  //x,y
   Int_t size[2] = {800,300}; //x,y
   
   TCanvas * cCount = new TCanvas("cCount", "cCount", 0, 0, size[0]*Div[0], size[1]*Div[1]);
   cCount->Divide(Div[0],Div[1]);
   for( int i = 1; i <= Div[0]*Div[1] ; i++){
      cCount->cd(i)->SetGrid();
   }
   cCount->cd(1);

   gStyle->SetOptStat(1111111);
   gStyle->SetStatY(0.8);
   gStyle->SetStatX(0.99);
   gStyle->SetStatW(0.2);
   gStyle->SetStatH(0.1);
   
/**///========================================================= load files

/**///========================================================= Analysis

   TH1F * spec  = new TH1F("spec" , "spec"  , 400, -1, 10);

   spec ->SetXTitle("Ex [MeV]");
   
   TString gate, gateB, gate_cm;
   
   //gate  = "good == 1 && det%6 != 5 && TMath::Abs(t4)<1000";
   gate.Form("good == 1 && det%6 == %d && TMath::Abs(t4)<1000", detID);
   
   gateB = "good == 0 && TMath::Abs(energy_t)<20 && det%6 != 5 && TMath::Abs(t4)<1000";
   gate_cm = "&& 50 > thetaCM && thetaCM > 0 ";
   
   tree->Draw("Ex>>spec ", gate + gate_cm, "");
  
   cCount->cd(1);
   spec ->Draw();
   
   
   TSpectrum * peak = new TSpectrum(50);
   peak->Search(spec, 1, "", threshold);
   TH1 * h1 = peak->Background(spec,10);
   //h1->Sumw2();
   h1->Draw("same");
   
   cCount->cd(2);
   TH1F * specS = (TH1F*) spec->Clone();
   TString title;
   title.Form("t4-gate && |e_t| < 20 && det%6 == %d && TMath::Abs(t4)<1000", detID);
   specS->SetTitle(title + gate_cm);
   specS->SetName("specS");
   specS->Add(h1, -1.);
   specS->Sumw2();
   specS->Draw();
   
   
   //========== Fitting 
   nPeaks  = peak->Search(specS, 1, "", threshold);
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
   
   //int nEPeaks = 0;
   
   //const int  n = 3 * (nPeaks + nEPeaks);
   const int  n = 3 * nPeaks;
   double * para = new double[n]; 
   for(int i = 0; i < nPeaks ; i++){
      para[3*i+0] = height[i] * 0.05 * TMath::Sqrt(TMath::TwoPi());
      para[3*i+1] = energy[i];
      para[3*i+2] = 0.05;
      //printf("%2d, %f \n", i, para[3*i+1]);
   }
   
   //for( int i = nPeaks ; i < nPeaks + nEPeaks; i++){
   //   para[3*i+0] = 20.; 
   //   para[3*i+0] = 3.5; 
   //   para[3*i+0] = 0.05;
   //}
   
   
   //nPeaks = 16;
   //TF1 * fit = new TF1("fit", fpeaks, -1 , 10, 3*( nPeaks + nEPeaks ));
   TF1 * fit = new TF1("fit", fpeaks, -1 , 10, 3* nPeaks );
   fit->SetNpx(1000);
   fit->SetParameters(para);
   //fit->Draw("same");   
   specS->Fit("fit", "q");
   
   
   const Double_t* paraE = fit->GetParErrors();
   const Double_t* paraA = fit->GetParameters();
   
   double bw = specS->GetBinWidth(1);
   
   double * ExPos = new double[nPeaks];
   
   //for(int i = 0; i < nPeaks + nEPeaks; i++){
   for(int i = 0; i < nPeaks ; i++){
      ExPos[i] = paraA[3*i+1];
   }
   //sort ExPos

   //for(int i = 0; i < nPeaks + nEPeaks; i++){
   for(int i = 0; i < nPeaks ; i++){
      ExPos[i] = paraA[3*i+1];
      printf("%2d , count: %8.0f(%3.0f), mean: %8.4f(%8.4f), sigma: %8.4f(%8.4f) \n", 
              i, 
              paraA[3*i] / bw,   paraE[3*i] /bw, 
              paraA[3*i+1], paraE[3*i+1],
              paraA[3*i+2], paraE[3*i+2]);
   }
   
   /*
   printf("============================= 2 sigma\n");
   for(int i = 0; i < nPeaks + nEPeaks; i++){
      printf("%2d , Ex: (%8.4f, %8.4f) \n", 
              i,  
              paraA[3*i+1] - 2*paraA[3*i+2],
              paraA[3*i+1] + 2*paraA[3*i+2]);
   }
   
   
   // theta CM distribution
   /*
   int Div2[2] = {5,3};  //x,y
   int size2[2] = {300,300}; //x,y
   TCanvas * cAux = new TCanvas("cAux", "cAux", 0, 0, size2[0]*Div2[0], size2[1]*Div2[1]);
   cAux->Divide(Div2[0],Div2[1]);
   for( int i = 1; i <= Div2[0]*Div2[1] ; i++){
      cAux->cd(i)->SetGrid();
   }
   
   TH1F ** dist = new TH1F*[nPeaks];
   TString expression, name, title,  gate_e;
   for(int i = 0; i < nPeaks + nEPeaks; i++){
      
      name.Form("dist%d", i);
      title.Form("Ex = %f +- 0.1", paraA[3*i+1]);
      dist[i] = new TH1F(name, title, 45, 0, 45);
      dist[i]->SetXTitle("theta_CM [deg]");
      dist[i]->SetYTitle("count / 1 deg");
      
      expression.Form("thetaCM >> dist%d", i);
      gate_e.Form("&& thetaCM != 0 && TMath::Abs(Ex - %f) < 0.1", paraA[3*i+1]);
      cAux->cd(i+1);
      
      tree->Draw(expression, gate + gate_e);
      
   }
   
   cAux->cd(15);
   tree->Draw("thetaCM:Ex", gate + " && thetaCM != 0", "colz");
   
   */
}

