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

   //TBrowser B ;   
   Int_t Div[2] = {6,4};  //x,y
   Int_t size[2] = {230,230}; //x,y
   
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
   
   TString gate_et = "&& TMath::Abs(energy_t)<20";
//   TString gate_e = "&& ( TMath::Abs(energy+1059)<50 || TMath::Abs(energy + 744) < 50 || TMath::Abs(energy + 464)<50 || TMath::Abs(energy + 351)<50 || TMath::Abs(energy - 174 )<50)"; 
   TString gate_e = "&& ( TMath::Abs(energy+1059)<50 || TMath::Abs(energy + 744) < 50 || TMath::Abs(energy + 464)<50 || TMath::Abs(energy + 351)<50)"; 

//   TString gate_e = "&& ( TMath::Abs(energy + 744) < 50)"; 

   
   double cut[24];
   cut[0] = 2500;
   cut[1] = 2000;
   cut[2] = 2000;
   cut[3] = 2870;
   cut[4] = 3000;
   cut[5] = 2281;
   cut[6] = 2500;
   cut[7] = 2000;
   cut[8] = 2400;
   cut[9] = 2000;
   cut[10] = 2000;
   cut[11] = 1000;
   cut[12] = 2500;
   cut[13] = 2500;
   cut[14] = 2500;
   cut[15] = 2300;
   cut[16] = 1800;
   cut[17] = 2000;
   cut[18] = 1500;
   cut[19] = 2200;
   cut[20] = 2000;
   cut[21] = 2000;
   cut[22] = 2000;
   cut[23] = 1800;
   
   int polDeg[24];
   for(int i = 0; i < 24; i++){
      polDeg[i] = 1;
   }
   
   
                                   polDeg[3]  = 6;                 polDeg[5]  = 3;
                   polDeg[8]  = 6; polDeg[9]  = 6; polDeg[10] = 8;
   polDeg[13] = 6; polDeg[14] = 8; polDeg[15] = 5; polDeg[16] = 4; polDeg[17] = 4;
                   polDeg[20] = 5; polDeg[21] = 6; polDeg[22] = 4; polDeg[23] = 4;
   
   
   
   // find Max polDeg
   int maxPolDeg = 0;
   for(int i = 0; i < 24; i++){
      if( polDeg[i] > maxPolDeg ) maxPolDeg = polDeg[i];
   }
   
   printf("max Pol deg : %d \n", maxPolDeg);
   
   TH2F ** q = new TH2F*[24];
   TProfile ** px = new TProfile*[24];
   
   double tc[24][maxPolDeg + 1];
   for( int i = 0; i < 24; i++){
      for(int j = 0; j <= maxPolDeg; j++){
         tc[i][j] = 0.;
      }
   }
   
   TH1F ** k = new TH1F*[24];
   double mean[24];
   
   for( int i = 0; i < 24; i++){
      TString name;
      name.Form("k%d", i);
      k[i] = new TH1F(name, name, 300, 1500, 4500);
      TString expression;
      expression.Form("tt>> k%d", i, i);
            
      TString gate;
      gate.Form("tac[4] > 2000 && e[%d] > 100 && xf[%d] !=0 && xn[%d] !=0", i, i, i);
      
      cScript->cd(i+1);
      tree->Draw(expression, gate, "");
      
      mean[i] = k[i]->GetMean();
      printf("%d,  mean: %f \n", i, mean[i]);
      
   } 
       
   
   for( int i = 0; i < 24; i++){
      printf("---------------- %d\n", i);
      
      TString polyName;
      polyName.Form("pol%d", polDeg[i]);
   
      TF1 * fit = new TF1("fit", polyName, -1,1);
      
      TString name;
      name.Form("q%d", i);
      
      q[i] = new TH2F(name, name, 50, -1, 1, 50, 1500, 4500);
      TString expression;
      expression.Form("tt:(xf[%d]-xn[%d])/(xf[%d]+xn[%d]) >> q%d", i, i, i, i, i);
            
      TString gate;
      gate.Form("tac[4] > 2000 && e[%d] > 100 && xf[%d] !=0 && xn[%d] !=0", i, i, i);
      
      cScript->cd(i+1);
      tree->Draw(expression, gate + gate_e + gate_et, "");
      //tree->Draw(expression, gate, "");
         
      name.Form("q%dpx", i);
      px[i] = new TProfile(name, name, 300, -1,1);
      q[i]->ProfileX(name);
      px[i]->Draw("same");

      px[i]->Fit("fit", "q");

      for( int j = 0; j <= polDeg[i]; j ++){
         tc[i][j] = fit->GetParameter(j);
         printf("        (%d,%d), %f \n", i, j, tc[i][j]);
      }
      
   }
   
   cScript->Update();
   printf("0 for end, 1 for saving parameters: ");
   int dummy;
   scanf("%d", &dummy);
   if( dummy == 0 ) return;
   if( dummy == 1 ){
      FILE * paraOut;
      
      paraOut = fopen ("tac_correction_mean.dat", "w+");
      printf("=========== save mean parameters to %s \n", "tac_correction_mean.dat");
      for( int i = 0; i < 24; i++){
         fprintf(paraOut, "%20.8f\n", mean[i]);
      }
      fflush(paraOut);
      fclose(paraOut);
      
      paraOut = fopen ("tac_correction_deg.dat", "w+");
      printf("=========== save deg parameters to %s \n", "tac_correction_deg.dat");
      for( int i = 0; i < 24; i++){
         fprintf(paraOut, "%d\n", polDeg[i]);
      }
      fflush(paraOut);
      fclose(paraOut);
      
      paraOut = fopen ("tac_correction.dat", "w+");
      printf("=========== save parameters to %s \n", "tac_correction.dat");
      for( int i = 0; i < 24; i++){
         for(int j = 0; j <= polDeg[i]; j++){
            fprintf(paraOut, "%20.6f", tc[i][j]);
         }
         fprintf(paraOut, "\n");
      }
      
      fflush(paraOut);
      fclose(paraOut);
   }
   
  
}

