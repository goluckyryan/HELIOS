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
   
   double tcut[24][2];
   tcut[0][0] = -200 ; tcut[0][1] = 400 ;
   tcut[1][0] = -200 ; tcut[1][1] = 400 ;
   tcut[2][0] = -200 ; tcut[2][1] = 400 ;
   //tcut[3][0] = -100 ; tcut[3][1] = 400 ;
   tcut[3][0] = -600 ; tcut[3][1] = 400 ;
   tcut[4][0] =  100 ; tcut[4][1] = 500 ;
   tcut[5][0] = -400 ; tcut[5][1] = 400 ;
   
   tcut[6][0] = -200 ; tcut[6][1] = 400 ;
   tcut[7][0] = -200 ; tcut[7][1] = 400 ;
   tcut[8][0] = -200 ; tcut[8][1] = 400 ;
   //tcut[9][0] =    0 ; tcut[9][1] = 500 ;
   tcut[9][0] =  -500 ; tcut[9][1] = 500 ;
   tcut[10][0] =  100 ; tcut[10][1] = 500 ;
   tcut[11][0] = -200 ; tcut[11][1] = -200 ;
   
   tcut[12][0] = -300 ; tcut[12][1] = 400 ;
   tcut[13][0] = -300 ; tcut[13][1] = 400 ;
   tcut[14][0] = -200 ; tcut[14][1] = 400 ;
   tcut[15][0] = -200 ; tcut[15][1] = 400 ;
   tcut[16][0] =  100 ; tcut[16][1] = 500 ;
   tcut[17][0] =  100 ; tcut[17][1] = 800 ;
   
   tcut[18][0] = -200 ; tcut[18][1] = 400 ;
   tcut[19][0] = -300 ; tcut[19][1] = 400 ;
   tcut[20][0] = -200 ; tcut[20][1] = 600 ;
   //tcut[21][0] =   50 ; tcut[21][1] = 600 ;
   tcut[21][0] = -300 ; tcut[21][1] = 600 ;
   tcut[22][0] =   50 ; tcut[22][1] = 500 ;
   tcut[23][0] =  200 ; tcut[23][1] = 700 ;
   
   printf("========== plotting final spectrum \n");
   
   TString name, expression, gate;
   
   TH1F * spec = new TH1F("spec", "specG" , 400, -2000, 1500);
   spec->SetXTitle("Ex [a.u.]");
   for( int i = 0 ; i < 6 ; i++){
      for(int j = 0; j < 4 ; j++){
         
         if( i == 5 ) continue;
         expression.Form("energy >> + spec");
         
         int id = i + 6*j ;
         gate.Form("det == %d && (%f > t4 && t4 > %f ) && TMath::Abs(t4) < 1000 && TMath::Abs(energy_t)<20 ", id, tcut[id][1], tcut[id][0] );
                
         tree->Draw(expression, gate , "");
      }
   }
   
   cScript->cd(2);
   TSpectrum * specPeak = new TSpectrum(20);
   int nPeaks = specPeak->Search(spec, 1 ,"", 0.05);
   float * xpos = specPeak->GetPositionX();
   
   int * inX = new int[nPeaks];
   TMath::Sort(nPeaks, xpos, inX, 0 );  
   vector<double> energy;   
   for( int j = 0; j < nPeaks; j++){
      printf(" %d , x: %8.3f \n", j, xpos[inX[j]]);
      //energy.push_back(xpos[inX[j]]-xpos[inX[0]]);
      energy.push_back(xpos[inX[j]]);
   }
   
   vector<double> knownE;
   knownE.push_back(0);
   knownE.push_back(1808.74);
   knownE.push_back(2938.33);
   knownE.push_back(3941.57);
   knownE.push_back(4318.89);
   knownE.push_back(4901);
   knownE.push_back(5291);
   knownE.push_back(5476);
   knownE.push_back(5691);
   knownE.push_back(6125);
   knownE.push_back(7099);
   
   
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
   
   
   TH1F * z = new TH1F("z", "z" , 500, -500, 10000);
   z->SetXTitle("Ex [keV]");
   for( int i = 0 ; i < 6 ; i++){
      for(int j = 0; j < 4 ; j++){
         
         if( i == 5 ) continue;
         expression.Form("energy*%f + %f >> + z", eC1, eC0);
         
         int id = i + 6*j ;
         gate.Form("det == %d && (%f > t4 && t4 > %f ) && TMath::Abs(t4) < 1000 && TMath::Abs(energy_t)<20", id, tcut[id][1], tcut[id][0] );
                
         tree->Draw(expression, gate , "");
      }
   }
   
   
   cScript->cd(1);
   spec->Draw();
   
   cScript->cd(2);
   z->Draw();
   
   
   
}