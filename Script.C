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
   
   printf("========== plotting final spectrum \n");
   
   TH1F * k = new TH1F("k", "k" , 400, -2000, 1500);
   k->SetXTitle("Ex [a.u.]");
   for( int i = 0; i < 6; i++){  
      for( int j = 0; j < 4; j++){   
         TString expression;
         expression.Form("(-(e[%d] - %f * x[%d])*%f - %f)*%f + %f >>  + k" , 
                            i + 6*j ,  
                            m[i] ,
                            i + 6*j , 
                            c1[i][j], 
                            c0[i][j],
                            p1[i],
                            p0[i]);
        
         tree->Draw(expression, "" , "");
      }
   }
   
   cScript->cd(2);
   TH2F * h = new TH2F("h", "h" , 400, -2000, 1500, 400, 0, 350);
   h->SetXTitle("Ex [a.u.]");
   for( int i = 0; i < 6; i++){  
      for( int j = 0; j < 4; j++){   
         TString expression;
         expression.Form("x[%d] : (-(e[%d] - %f * x[%d])*%f - %f)*%f + %f >>  + h" , 
                            i + 6*j ,
                            i + 6*j ,  
                            m[i] ,
                            i + 6*j , 
                            c1[i][j], 
                            c0[i][j],
                            p1[i],
                            p0[i]);
                            
         TString gate;
        
         tree->Draw(expression, "" , "");
      }
   }
   
   /*
   TH2F ** h = new TH2F[numDet];
   TString * gate = new TString[numDet];
   for( int i = 0; i < numDet; i ++){
      TString name;
      name.Form("h%d", i);
      h[i] = new TH2F(name, name, 200, 55 , 115, 200, 0, 2000);
      name.Form("e[%d]", i); h[i]->SetYTitle(name);
      name.Form("x[%d]", i); h[i]->SetXTitle(name);
      
      TString expression;
      expression.Form("e[%d]:(xf[%d]-xn[%d]) >> h%d" ,1 + 6*i , 1 + 6*i ,i);
      //gate[i].Form("xf[%d]!=0 && xn[%d]!=0 && e[%d]!=0", i, i, i);
      gate[i] = "";
      //cScript->cd(i+1);
      tree->Draw(expression, gate[i] , "");
   }
   /**/
   
   
   /**/
}

