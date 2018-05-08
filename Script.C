{   

/**///======================================================== initial input
   
   //const char* rootfile="psd_run38.root"; const char* treeName="psd_tree";
   const char* rootfile="H052_Mg25.root"; const char* treeName="gen_tree";
   //const char* rootfile="X_H052_Mg25.root"; const char* treeName="tree";
   
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
         if( i >= 4) break;
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
   
   TH2F ** h = new TH2F**[24];
   
   TString expression, gate, name ;
   
   for( int i = 0; i < 24; i++){
      name.Form("h%d", i);
      h[i] = new TH2F(name, name, 100, -1, 1, 100, 0, 2000);
      
      expression.Form("e[%d]:(xf[%d]-xn[%d])/(xf[%d]+xn[%d]) >> h%d", i, i, i, i, i, i);
      
      gate.Form("e[%d] > 0 && xf[%d] > 0 && xn[%d] > 0", i, i, i);
      
      tree->Draw(expression, gate);
      
      cScript->Update();
      printf("===== %d ", i);
   }
   
   /*
   TString expression, gate, gateB;
   
   //gate  = "good == 1 && det%6 != 5 && TMath::Abs(t4)<1000";
   gate  = "good == 1 && TMath::Abs(t4)<1000";
   gateB = "good == 0 && TMath::Abs(energy_t)<20 && det%6 != 5 && TMath::Abs(t4)<1000";
   
   expression = "thetaCM : x >>j(400,-600, -200, 400, 0, 50) ";
   tree->Draw(expression, gate , "");
   /**/
   
   
}

