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
   
/**///========================================================= Analysis
   
   
   TH2F ** k = new TH2F*[24];
   //TProfile ** px = new TProfile*[24];
   
   for( int i = 0; i < 24; i++){
      TString name;
      name.Form("k%d", i);
      k[i] = new TH2F(name, name, 300, -1, 1, 300, 1500, 4500);
      TString expression;
      //expression.Form("tac[4]:(xf[%d]-xn[%d])/(xf[%d]+xn[%d]) >> k%d", i, i, i, i, i);
      expression.Form("tt:(xf[%d]-xn[%d])/(xf[%d]+xn[%d]) >> k%d", i, i, i, i, i);
      //expression.Form("tac[4]>2400?tac[4]:tac[4]+1250:(xf[%d]-xn[%d])/(xf[%d]+xn[%d]) >> k%d", i, i, i, i, i);
            
      TString gate;
      gate.Form("tac[4] > 2000 && e[%d] > 100 && xf[%d] !=0 && xn[%d] !=0", i, i, i);
      
      cScript->cd(i+1);
      tree->Draw(expression, gate, "");
   
   }
   
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
   
   TH2F ** q = new TH2F*[24];
   TProfile ** px = new TProfile*[24];
   
   double a0[24], a1[24], a2[24], a3[24], a4[24];
   
   TF1 * fit = new TF1("fit", "pol4", -1,1);
   
   for( int i = 0; i < 24; i++){
      TString name;
      name.Form("q%d", i);
      q[i] = new TH2F(name, name, 300, -1, 1, 300, 1500, 4500);
      TString expression;
      expression.Form("tt:(xf[%d]-xn[%d])/(xf[%d]+xn[%d]) >> q%d", i, i, i, i, i);
            
      TString gate;
      gate.Form("tac[4] > 2000 && e[%d] > 100 && xf[%d] !=0 && xn[%d] !=0", i, i, i);
      
      cScript->cd(i+1);
      tree->Draw(expression, gate, "");

      name.Form("k%dpx", i);
      px[i] = new TProfile(name, name, 300, -1,1);
      q[i]->ProfileX(name);
      px[i]->Draw("same");
      px[i]->Fit("fit", "q");
      
      a0[i] = fit->GetParameter(0);
      a1[i] = fit->GetParameter(1);
      a2[i] = fit->GetParameter(2);
      a3[i] = fit->GetParameter(3);
      a4[i] = fit->GetParameter(4);
      
   }
   
   cScript->Update();
   printf("0 for end, 1 for saving parameters: ");
   int dummy;
   scanf("%d", &dummy);
   if( dummy == 0 ) return;
   if( dummy == 1 ){
      FILE * paraOut;
      TString filename;
      filename.Form("tac_correction.dat", i);
      paraOut = fopen (filename.Data(), "w+");
      
      printf("=========== save parameters to %s \n", filename.Data());
      for( int i = 0; i < 24; i++){
         fprintf(paraOut, "%9.6f  %9.6f %9.6f  %9.6f %9.6f\n", a0[i], a1[i], a2[i], a3[i], a4[i]);
      }
      
      fflush(paraOut);
      fclose(paraOut);
   }
   
  
}

