int nPeaks = 16;

TString gate, gateB, gate_cm;

TTree * tree0;
TTree * tree1;

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


void ThetaCMDist(double Ex){   

/**///======================================================== initial input
   
   const char* rootfile0="X_H052_Mg25.root"; const char* treeName0="tree";
   const char* rootfile1="test.root";        const char* treeName1="tree";
   
/**///========================================================  load tree

   TFile *f0 = new TFile (rootfile0, "read"); 
   tree0 = (TTree*)f0->Get(treeName0);
   printf("=====> /// %20s //// is loaded. Total #Entry: %10d \n", rootfile0,  tree0->GetEntries());
   
   TFile *f1 = new TFile (rootfile1, "read"); 
   tree1 = (TTree*)f1->Get(treeName1);
   printf("=====> /// %20s //// is loaded. Total #Entry: %10d \n", rootfile1,  tree1->GetEntries());
   
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
   gStyle->SetStatY(0.8);
   gStyle->SetStatX(0.99);
   gStyle->SetStatW(0.2);
   gStyle->SetStatH(0.1);
   
/**///========================================================= load files

/**///========================================================= Analysis
   
   TString expression, gate_e;

   //============= get the acceptance
   int nBin = 2000;
   TH1F* h = new TH1F("h", "h", nBin, 0, 50);
   expression.Form("thetaCM*TMath::RadToDeg() >> h");
   gate_e.Form("tag == 2 && TMath::Abs(Ex-%f) < 0.1 && thetaCM*TMath::RadToDeg() > 8 ", Ex);      
   tree1->Draw(expression, gate_e);
   
   //find the acceptance, the angle the count drop
   vector<double> angle;
   printf(" ============================ Ex : %f\n", Ex);
   angle.clear();
   for(int j = 1 ; j < nBin; j++){
      int a = h->GetBinContent(j);
      int b = h->GetBinContent(j+1);
      
      if( a == 0 && b > 0) {
         angle.push_back(h->GetBinLowEdge(j+1));
         //printf(" boundary : %10.5f\n", h[i]->GetBinLowEdge(j+1) );
      }
      if( a > 0 && b == 0) {
         angle.push_back(h->GetBinLowEdge(j+1));
         //printf(" boundary : %10.5f\n", h[i]->GetBinLowEdge(j+1) );
      }
   }
   
   vector<double> dCos;
   dCos.clear();
   for( int j = 0; j < angle.size()/2; j++){
      double delta = angle[2*j+1] - angle[2*j];
      double mean = (angle[2*j+1] + angle[2*j])/2;
      dCos.push_back(TMath::Sin(mean*TMath::DegToRad())*(delta*TMath::DegToRad()));
      printf("%2d | %10.5f - %10.5f = %10.5f | %10.5f, %10.5f \n", 
                 j, 
                 angle[2*j], 
                 angle[2*j+1], 
                 delta,
                 mean,
                 1./dCos[j]);
   }
   
   
   //tree0->Draw("thetaCM : Ex >> k(200,0,8,400,0,50)", "good == 1 && det%6 == 0", "colz");

   /*
   TH1F* k = new TH1F("k", "k", 450, 5, 50);
   tree0->Draw("thetaCM >> k", "good == 1 && TMath::Abs(t4)<1000 && TMath::Abs(Ex-0)<0.2");
   
   h->Scale(0.05);
   h->Draw();
   k->SetLineColor(2);
   k->Draw("same");
   
   */
   /*
   //=========== get the background-free distribution
   
   //gate  = "good == 1 && det%6 != 5 && TMath::Abs(t4)<1000";
   //gate  = "good == 1 && det%6 == 0 && TMath::Abs(t4)<1000";
   //gateB = "good == 0 && TMath::Abs(energy_t)<20 && det%6 != 5 && TMath::Abs(t4)<1000";
   gate_cm = "&& 50 > thetaCM && thetaCM > 0 ";
   
   for( int i = 0; i < 6; i++){
      TH1F * spec  = new TH1F("spec" , "spec"  , 400, -1, 10);
      spec ->SetXTitle("Ex [MeV]");
      
      gate.Form("good == 1 && det%6 == %d && TMath::Abs(t4)<1000", i);
      tree0->Draw("Ex>>spec ", gate + gate_cm, "");
     
      TSpectrum * peak = new TSpectrum(30);
      peak->Search(spec, 1, "", 0.05);
      TH1 * h1 = peak->Background(spec,10);
      //h1->Sumw2();
      
      TH1F * specS = (TH1F*) spec->Clone();
      specS->SetTitle("t4-gate && |e_t| < 20 &&  det%6 != 5 && TMath::Abs(t4)<1000" + gate_cm);
      specS->SetName("specS");
      specS->Add(h1, -1.);
      specS->Sumw2();
      specS->Draw();
      
      
      //========== Fitting 
      nPeaks  = peak->Search(specS, 1, "", 0.05);
      float * xpos = peak->GetPositionX();
      float * ypos = peak->GetPositionY();
      
      int * inX = new int[nPeaks];
      TMath::Sort(nPeaks, xpos, inX, 0 );
      vector<double> energy, height;
      for( int j = 0; j < nPeaks; j++){
         energy.push_back(xpos[inX[j]]);
         height.push_back(ypos[inX[j]]);
      }
      
      int nEPeaks = 0;
      
      const int  n = 3 * (nPeaks + nEPeaks);
      double * para = new double[n]; 
      for(int i = 0; i < nPeaks ; i++){
         para[3*i+0] = height[i] * 0.05 * TMath::Sqrt(TMath::TwoPi());
         para[3*i+1] = energy[i];
         para[3*i+2] = 0.05;
         //printf("%2d, %f \n", i, para[3*i+1]);
      }
      
      for( int i = nPeaks ; i < nPeaks + nEPeaks; i++){
         para[3*i+0] = 20.; 
         para[3*i+0] = 3.5; 
         para[3*i+0] = 0.05;
      }
      
      TF1 * fit = new TF1("fit", fpeaks, -1 , 10, 3*( nPeaks + nEPeaks ));
      fit->SetNpx(1000);
      fit->SetParameters(para);
      //fit->Draw("same");   
      specS->Fit("fit", "q");
      
      const Double_t* paraE = fit->GetParErrors();
      const Double_t* paraA = fit->GetParameters();
      
      double bw = specS->GetBinWidth(1);
      
      double * ExPos = new double[nPeaks];
      
      for(int i = 0; i < nPeaks + nEPeaks; i++){
         ExPos[i] = paraA[3*i+1];
      }
      //sort ExPos

      
      for(int i = 0; i < nPeaks + nEPeaks; i++){
         ExPos[i] = paraA[3*i+1];
         printf("%2d , count: %8.0f(%3.0f), mean: %8.4f(%8.4f), sigma: %8.4f(%8.4f) \n", 
                 i, 
                 paraA[3*i] / bw,   paraE[3*i] /bw, 
                 paraA[3*i+1], paraE[3*i+1],
                 paraA[3*i+2], paraE[3*i+2]);
      }
   
   }
   
   /**/
}

