int nPeaks = 16;

TString gate, gate_cm, gate_Aux;

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


void Count(int detID, int splitCtrl = 0, double threshold = 0.1){   

/**///======================================================== initial input
   
   const char* rootfile="C_gen_run30.root"; const char* treeName="tree";
   
   double ExRange[2] = {-1, 6};
   
   /*// for H052
   gate_Aux = " && z > -629 + 32 * Ex + 0.67 * Ex * Ex ";
   gate_cm = "&& 50 > thetaCM && thetaCM > 0 ";
   */
   bool isH052 = false;
   
   // for H060 
   gate = "hitID == 0 && 5 > detID%6 && detID%6 > 0";
   gate_cm = "";
   gate_Aux = "";
   
/**///========================================================  load tree

   TFile *f0 = new TFile (rootfile, "read"); 
   TTree *tree = (TTree*)f0->Get(treeName);
   printf("=====> /// %15s //// is loaded. Total #Entry: %10d \n", rootfile,  tree->GetEntries());
   //printf("=====>  Total #Entry: %10d \n", tree->GetEntries());

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

   //=================== load detector geometery
   vector<double> pos;
   double length = 50.5;
   double firstPos = 0;
   int iDet = 6;
   int jDet = 4;

   string detGeoFileName = "detectorGeo_upstream.txt";
   printf("----- loading detector geometery : %s.", detGeoFileName.c_str());
   ifstream file(detGeoFileName.c_str(), std::ifstream::in);
   int i = 0;
   if( file.is_open() ){
      string x;
      while( file >> x){
         //printf("%d, %s \n", i,  x.c_str());
         if( x.substr(0,2) == "//" )  continue;
         if( i == 6 ) length   = atof(x.c_str());
         if( i == 8 ) firstPos = atof(x.c_str());
         if( i == 9 ) jDet = atoi(x.c_str());
         if( i >= 10 ) {
            pos.push_back(atof(x.c_str()));
         }
         i = i + 1;
      }
      
      int iDet = pos.size();
      file.close();
      printf("... done.\n");
      
      for(int id = 0; id < iDet; id++){
         pos[id] = firstPos + pos[id];
      }
      
      for(int i = 0; i < iDet ; i++){
         if( firstPos > 0 ){
            printf("%d, %6.2f mm - %6.2f mm \n", i, pos[i], pos[i] + length);
         }else{
            printf("%d, %6.2f mm - %6.2f mm \n", i, pos[i] - length , pos[i]);
         }
      }
      printf("=======================\n");
      
   }else{
       printf("... fail\n");
       
   }
   
   double zMPos[6];
   for(int i = 0; i < iDet ; i++){
      if(firstPos < 0 ){
         zMPos[i] = pos[i] - length/2;
      }else{
         zMPos[i] = pos[i] + length/2;
      }
   } 

/**///========================================================= Analysis

   TH1F * spec  = new TH1F("spec" , "spec"  , 400, ExRange[0], ExRange[1]);
   spec ->SetXTitle("Ex [MeV]");   
   
   //========define gate_z
   TString gate_z;
   if( detID >= 0 ){
      //gate.Form("good == 1 && det%6 == %d && TMath::Abs(t4)<1000", detID);
      if( splitCtrl == 0 ) {
         if( firstPos > 0 ){
            gate_z.Form("&& %f < z && z < %f", pos[0] - 10, pos[iDet-1] + length + 10);
         }else{
            gate_z.Form("&& %f < z && z < %f", pos[0] - length - 10, pos[iDet-1] + 10);
         }
      }
      if( splitCtrl == 1) {
         gate_z.Form("&& z < %f", zMPos[detID]);
      }
      if( splitCtrl == 2) {
         gate_z.Form("&& z > %f", zMPos[detID]);
      }
      //exclude defected detectors
      if( isH052 ){
         if( detID == 0){
            gate.Form("good == 1 && det%6 == %d && det != 18 && TMath::Abs(t4)<1000", detID);
         }
         if( detID == 5){
            gate.Form("good == 1 && det%6 == %d && det != 11 && TMath::Abs(t4)<1000", detID);
         }
      }   
   }else if (detID == -1){
      //gate.Form("good == 1 && det != 11 && det != 18 && TMath::Abs(t4)<1000");
      if( firstPos > 0 ){
         gate_z.Form("&& %f < z && z < %f", pos[0] - 10, pos[iDet-1] + length + 10);
      }else{
         gate_z.Form("&& %f < z && z < %f", pos[0] - length - 10, pos[iDet-1] + 10);
      }
   }
   
   printf("%s\n", gate.Data());
   printf("%s\n", gate_cm.Data());
   printf("%s\n", gate_z.Data());
   printf("%s\n", gate_Aux.Data());

   printf(" threshold : %f \n", threshold);
   
   //================= plot Ex
   printf("============= plot Ex with gates\n");    
   tree->Draw("Ex>>spec ", gate + gate_z +  gate_cm + gate_Aux, "");
  
   cCount->cd(1);
   spec ->Draw();
   
   //=================== find peak and fit
   printf("============= estimate background and find peak\n");
   TSpectrum * peak = new TSpectrum(50);
   peak->Search(spec, 1, "", threshold);
   TH1 * h1 = peak->Background(spec,10);
   //h1->Sumw2();
   h1->Draw("same");
   
   cCount->cd(2);
   TH1F * specS = (TH1F*) spec->Clone();
   TString title;
   //if( detID >= 0){
   //   title.Form("t4-gate && |e_t| < 20 && det%6 == %d && TMath::Abs(t4)<1000", detID);
   //}else if(detID == -1){
   //   title.Form("t4-gate && |e_t| < 20 && TMath::Abs(t4)<1000");
   //}
   //specS->SetTitle(title + gate_cm + gate_z + " && thetaCM > 8");
   specS->SetName("specS");
   specS->Add(h1, -1.);
   specS->Sumw2();
   specS->Draw();
   
   
   //========== Fitting 
   printf("============= Fit.....");
   nPeaks  = peak->Search(specS, 1, "", threshold);
   printf(" found %d peaks \n", nPeaks);
   float * xpos = peak->GetPositionX();
   float * ypos = peak->GetPositionY();
   
   //Double_t * xpos = peak->GetPositionX();
   //Double_t * ypos = peak->GetPositionY();
   
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
   specS->Fit("fit", "q");

   printf("============= display\n");   
   const Double_t* paraE = fit->GetParErrors();
   const Double_t* paraA = fit->GetParameters();
   
   double bw = specS->GetBinWidth(1);
   
   double * ExPos = new double[nPeaks];
   
   for(int i = 0; i < nPeaks ; i++){
      ExPos[i] = paraA[3*i+1];
   }
   
   for(int i = 0; i < nPeaks ; i++){
      ExPos[i] = paraA[3*i+1];
      printf("%2d , count: %8.0f(%3.0f), mean: %8.4f(%8.4f), sigma: %8.4f(%8.4f) \n", 
              i, 
              paraA[3*i] / bw,   paraE[3*i] /bw, 
              paraA[3*i+1], paraE[3*i+1],
              paraA[3*i+2], paraE[3*i+2]);
   }
   
}

