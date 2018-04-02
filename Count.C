{   

   gROOT->ProcessLine(".L fpeaks.C");

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
   double nearPos[6];
   double length;
   printf("----- loading sensor position.");
   ifstream file;
   file.open("nearPos.dat");
   if( file.is_open() ){
      double a;
      int i = 0;
      while( file >> a ){
         if( i >= 7) break;
         if( i == 6) length = a;
         nearPos[i] = a;
         i = i + 1;
      }
      file.close();
      printf("... done.\n");
      for(int i = 0; i < 5 ; i++){
         printf("%6.2f mm, ", nearPos[i]);
      }
      printf("%6.2f mm || length : %6.2f mm \n", nearPos[5], length);
   }else{
       printf("... fail\n");
   }
   
   double xnCorr[24];
   printf("----- loading xf-xn correction.");
   file.open("correction_xf_xn.dat");
   if( file.is_open() ){
      double a;
      int i = 0;
      while( file >> a ){
         if( i >= numDet) break;
         xnCorr[i] = a;
         //xnCorr[i] = 1;
         i = i + 1;
      }
      
      printf("... done.\n");
   }else{
      printf("... fail.\n");
   }
   file.close();
   
   double c1[6][4];
   double c0[6][4];
   double m[6]  ;
   printf("----- loading energy calibration for same position. \n");
   for( int i = 0; i < 6; i++){
      TString filename;
      filename.Form("e_correction_%d.dat", i);
      printf("        %s", filename.Data());
      file.open(filename.Data());
      if( file.is_open() ){
         double a, b;
         int j = 0;
         while( file >> a >> b ){
            c0[i][j] = a;
            c1[i][j] = b;
            j = j + 1;
            if( j >= 4) break;
         }
         file >> a;
         m[i] = a;
         
         printf("... done.\n");
         //printf("                 c0 : %f, m : %f \n", c0[i][2], m[i]);
      }else{
         printf("... fail.\n");
      }
      file.close();
   }
   
   double p0[6];
   double p1[6];
   printf("----- loading energy calibration for different position.");
   file.open("e_correction_diff.dat");
   if( file.is_open() ){
      double a, b;
      int i = 0;
      while( file >> a >> b ){
         p0[i] = a;
         p1[i] = b;
         i = i+ 1;
      }
      file.close();
      printf("... ok.\n");
      //for(int i = 0; i < 6 ; i++){
      //   printf("                    p0: %f, p1: %f \n", p0[i], p1[i]);
      //}
   }else{
      printf("... fail.\n");
   }
   file.close();
   
   double mean[24];
   printf("----- loading tac (mean) calibration.");
   file.open("tac_correction_mean.dat");
   if( file.is_open() ){
      double a;
      int i = 0;
      while( file >> a ){
         if( i > 24) break;
         mean[i] = a;
         i = i + 1;
      }
      printf("... done.\n");
   }else{
      printf("... fail.\n");
   }
   file.close();
   
/**///========================================================= Analysis

   TH1F * spec  = new TH1F("spec" , "spec"  , 400, -1, 10);

   spec ->SetXTitle("Ex [MeV]");
   
   TString gate, gateB;
   
   gate  = "good == 1 && det%6 != 5 && TMath::Abs(t4)<1000";
   gateB = "good == 0 && TMath::Abs(energy_t)<20 && det%6 != 5 && TMath::Abs(t4)<1000";
   
   tree->Draw("Ex>>spec ", gate, "");
  
   spec ->Draw();
   
   TSpectrum * peak = new TSpectrum(20);
   peak->Search(spec, 1, "", 0.05);
   TH1F * h1 = peak->Background(spec,10);
   //h1->Sumw2();
   
   TH1F * specS = (TH1F*) spec->Clone();
   specS->SetName("specS");
   specS->Add(h1, -1.);
   specS->Sumw2();
   specS->Draw();
   // Fitting 
   
   int  nPeaks  = peak->Search(specS, 1, "", 0.05);
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
   
   double para[3 * (nPeaks + nEPeaks)]; 
   for(int i = 0; i < nPeaks ; i++){
      para[3*i+0] = height[i] * 0.05 * TMath::Sqrt(TMath::TwoPi());
      para[3*i+1] = energy[i];
      para[3*i+2] = 0.05;
   }
   
   for( int i = nPeaks ; i < nPeaks + nEPeaks; i++){
      para[3*i+0] = 20.; 
      para[3*i+0] = 3.5; 
      para[3*i+0] = 0.05;
   }
   
   //nPeaks = 16;
   TF1 * fit = new TF1("fit",fpeaks, -1 , 10, 3*( nPeaks + nEPeaks ));
   fit->SetParameters(para);
   fit->SetNpx(1000);
   
   //fit->Draw("same");   
   specS->Fit("fit", "q");
   
   double* paraE = fit->GetParErrors();
   double* paraA = fit->GetParameters();
   
   double bw = specS->GetBinWidth(1);
   
   double ExPos[nPeaks];
   
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
   
   printf("============================= 2 sigma\n");
   for(int i = 0; i < nPeaks + nEPeaks; i++){
      printf("%2d , Ex: (%8.4f, %8.4f) \n", 
              i,  
              paraA[3*i+1] - 2*paraA[3*i+2],
              paraA[3*i+1] + 2*paraA[3*i+2]);
   }
   
   
   // theta CM distribution
   
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
   
   
}

