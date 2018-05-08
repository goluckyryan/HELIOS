void AngularDist(int mode, double Ex, int bin = 80, double dEx = 0.02) {   

/**///======================================================== initial input
   
   const char* rootfile0="X_H052_Mg25.root"; const char* treeName0="tree";
   const char* rootfile1="test.root";        const char* treeName1="tree";
   
/**///========================================================  load tree

   TFile *f0 = new TFile (rootfile0, "read"); 
   TTree *tree0 = (TTree*)f0->Get(treeName0);
   printf("=====> /// %20s //// is loaded. Total #Entry: %10d \n", rootfile0,  tree0->GetEntries());
   
   TFile *f1 = new TFile (rootfile1, "read"); 
   TTree *tree1 = (TTree*)f1->Get(treeName1);
   printf("=====> /// %20s //// is loaded. Total #Entry: %10d \n", rootfile1,  tree1->GetEntries());
   
/**///======================================================== Browser or Canvas

   //TBrowser B ;   
   Int_t Div[2] = {1,1};  //x,y
   Int_t size[2] = {800,600}; //x,y
   
   TCanvas * cAngularDist = new TCanvas("cAngularDist", "cAngularDist", 0, 0, size[0]*Div[0], size[1]*Div[1]);
   cAngularDist->Divide(Div[0],Div[1]);
   for( int i = 1; i <= Div[0]*Div[1] ; i++){
      cAngularDist->cd(i)->SetGrid();
   }
   cAngularDist->cd(1);
   gStyle->SetOptStat(1111111);
   gStyle->SetStatY(0.8);
   gStyle->SetStatX(0.99);
   gStyle->SetStatW(0.2);
   gStyle->SetStatH(0.1);
   
/**///========================================================= load files

/**///========================================================= Analysis
   
   //double Ex = 4.3;
   
   printf("============================ Ex : %f\n", Ex);
   TString expression, gate_e, gate, gate_det;
   
   TH1F* h = new TH1F("h", "h", 500, 0, 50);
   TH2F* h2 = new TH2F("h2", "h2", 400, -600, -200, 500, 0, 50); 
   TH1F* w = new TH1F("w", "Z_th", 400, -600, -200);
   TH1F* k = new TH1F("k", "Z_exp", bin, -600, -200); 
   
   if( mode == 1 ){
   
      for( int i = 0; i < 6; i++){
         //printf("--------- detID == %d \n", i);
         gate_det.Form("&& detID%6 == %d", i);
         gate_e.Form("tag == 2 && TMath::Abs(Ex-%f) < 0.01 && thetaCM > 9", Ex);      
      
         //============= get the acceptance
         
         tree1->Draw("thetaCM >> h", gate_e + gate_det);
           
         tree1->Draw("thetaCM : z >> h2", gate_e + gate_det);
         
         if( h->GetEntries() == 0){
            printf(" no data for detID == %d \n", i);
            continue;
         }
         //============== find the acceptance, the angle the count drop
         vector<double> angle;
         angle.clear();
         for(int j = 1 ; j < 500; j++){
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
         
         if( angle.size() == 4){
            angle.erase(angle.begin(), angle.begin()+2);
         }
               
         vector<double> dCos;
         dCos.clear();
         for( int j = 0; j < angle.size()/2; j++){
            double delta = angle[2*j+1] - angle[2*j];
            double mean = (angle[2*j+1] + angle[2*j])/2;
            if ( delta < 2. ) continue;
            dCos.push_back(TMath::Sin(mean*TMath::DegToRad())*(delta*TMath::DegToRad()));
            printf(" %10.5f - %10.5f = %10.5f | %10.5f, %10.5f \n", 
                       angle[2*j], 
                       angle[2*j+1], 
                       delta,
                       mean,
                       1./TMath::Sin(mean*TMath::DegToRad())*(delta*TMath::DegToRad()));
         }
         
         tree1->Draw("z >> w", gate_e + gate_det);
         double wMax = w->GetMaximum(); 
         
         
         gate.Form("good == 1 && TMath::Abs(t4)<1000 && TMath::Abs(Ex-%4.1f)<%f", Ex, dEx);
         tree0->Draw("z >> k", gate + gate_det );
         
         w->Scale(50./wMax);
         w->Draw("");
         k->SetLineColor(2);
         k->Draw("same");
         h2->Draw("same");
         
         cAngularDist->Update();
         //printf("0 for stop, 1 for continous : ");
         //int dummy;
         //scanf("%d", &dummy);
         //if( dummy == 0 ) break;
      }
      
   }else if (mode == 0) {
      gate_e.Form("tag == 2 && TMath::Abs(Ex-%f) < 0.1", Ex);  
      gate.Form("good == 1 && TMath::Abs(t4)<1000 && TMath::Abs(Ex-%4.1f)<%3f", Ex, dEx);
   }
    
   tree1->Draw("thetaCM : z >> h2", gate_e );
   tree1->Draw("z >> w", gate_e);
   tree0->Draw("z >> k", gate  );
   double wMax = w->GetMaximum(); 
   
   w->Scale(50./wMax);
   w->Draw("");
   
   k->SetLineColor(2);
   double kMax = k->GetMaximum();
   k->Scale(50./kMax);
   k->Draw("same");
   h2->Draw("same");
   
   /**/
   
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
     
      TSpectrum * peak = new TSpectrum(20);
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
      double * xpos = peak->GetPositionX();
      double * ypos = peak->GetPositionY();
      
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

