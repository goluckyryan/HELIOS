{
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

   Int_t Div[2] = {1,2};  //x,y
   Int_t size[2] = {800,400}; //x,y
   
   TCanvas * cScript = new TCanvas("cScript", "cScript", 0, 0, size[0]*Div[0], size[1]*Div[1]);
   cScript->Divide(Div[0],Div[1]);
   for( int i = 1; i <= Div[0]+Div[1] ; i++){
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
   
      
   
/**///========================================================= Analysis
   
   printf("========== plotting final spectrum \n");
   
   TH1F * k = new TH1F("k", "k" , 400, -2000, 1500);
   k->SetXTitle("Ex [a.u.]");
   TString expression;
   expression.Form("energy >> +k");        
   tree->Draw(expression, "TMath::Abs(energy_t)<20" , "");
   
   TH2F * h = new TH2F("h", "h" , 400, -2000, 1500, 400, -600, -200);
   h->SetXTitle("Ex [a.u.]");
   for( int i = 0 ; i < 6 ; i++){
      for(int j = 0; j < 4 ; j++){
   
         expression.Form("x[%d]: energy >> + h", i + 6*j , i +6*j);        
         tree->Draw(expression, "TMath::Abs(energy_t)<20" , "");
      }
   }
   
   cScript->cd(2);
   TSpectrum * spec = new TSpectrum(20);
   int nPeaks = spec->Search(k, 1 ,"", 0.01);
   float * xpos = spec->GetPositionX();
   
   int * inX = new int[nPeaks];
   TMath::Sort(nPeaks, xpos, inX, 0 );  
   vector<double> energy;   
   for( int j = 0; j < nPeaks; j++){
      printf(" %d , x: %8.3f \n", j, xpos[inX[j]]);
      //energy.push_back(xpos[inX[j]]-xpos[inX[0]]);
      energy.push_back(xpos[inX[j]]);
   }
   
   //TH1 *hb = spec->Background
   
   // fitting using gaussians
   
   
   vector<double> knownE;
   knownE.push_back(0);
   knownE.push_back(1808.74);
   knownE.push_back(2938.33);
   knownE.push_back(3941.57);
   knownE.push_back(4318.89);
   knownE.push_back(4896);
   knownE.push_back(5291);
   
   // convert to real energy 
   int numPeak = knownE.size();
   TGraph * ga = new TGraph(numPeak, &energy[0], &knownE[0] );
   ga->Draw("*ap");
   ga->Fit("pol1", "q");
   double eC0 = pol1->GetParameter(0);
   double eC1 = pol1->GetParameter(1);
   printf("====  eC0:%8.3f, eC1:%8.3f \n", eC0, eC1);
   
   vector<double> realEnergy;
   for( int j = 0; j < nPeaks; j++){
      realEnergy.push_back(energy[j] * eC1 + eC0);
      printf(" %d , e: %8.3f \n", j, realEnergy[j]);
   }
   
   
   TH1F * z = new TH1F("z", "z" , 500, -500, 10000);
   z->SetXTitle("Ex [keV]");
   expression.Form("energy*%f + %f >> +z", i + 6*j, eC1, eC0);
   tree->Draw(expression, "TMath::Abs(energy_t)<20" , "");
   
   cScript->cd(1);
   h->Draw("colz");
   
   cScript->cd(2);
   //k->Draw();
   TH2F * p = new TH2F("p", "p" , 400, -2000, 1500, 400, -600, -200);
   p->SetXTitle("Ex [a.u.]");
   for( int i = 0 ; i < 6 ; i++){
      for(int j = 0; j < 4 ; j++){
   
         expression.Form("x[%d]: energy >> + p", i + 6*j , i +6*j);        
         tree->Draw(expression, "TMath::Abs(energy_t)<20 && 8 > tac[0] && tac[0] > -4" , "");
      }
   }
   
   p->Draw("colz");
   
   
   
}
