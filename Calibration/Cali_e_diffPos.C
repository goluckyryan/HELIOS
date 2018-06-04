{
/**///======================================================== initial input
   
   //const char* rootfile="psd_run38.root"; const char* treeName="psd_tree";
   //const char* rootfile="H052_Mg25.root"; const char* treeName="gen_tree";
   
   const char* rootfile="C_H052_Mg25.root"; const char* treeName="tree";
   
   int eRange[3] = {400, -2000, 1300};
   
/**///========================================================  load tree

   TFile *f0 = new TFile (rootfile, "read"); 
   TTree *tree = (TTree*)f0->Get(treeName);
   printf("=====> /// %15s //// is loaded. Total #Entry: %10d \n", rootfile,  tree->GetEntries());
   
/**///======================================================== Browser or Canvas

   Int_t Div[2] = {1,2};  //x,y
   Int_t size[2] = {800,300}; //x,y
   
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
   double a;
   int i = 0;
   while( file >> a ){
      if( i >= 7) break;
      if( i == 6) length = a;
      nearPos[i] = a;
      i = i + 1;
   }
   file.close();
   printf("... done.\n      ");
   for(int i = 0; i < 5 ; i++){
      printf("%6.2f mm, ", nearPos[i]);
   }
   printf("%6.2f mm || length : %6.2f mm \n", nearPos[5], length);
   
   double ** c0 = new double[6];
   double ** c1 = new double[6];
   double * m  = new double[6];
   for(int i = 0; i < 6; i ++){
      c0[i] = new double[4];
      c1[i] = new double[4];
   }
   printf("----- loading energy calibration. \n");
   for( int i = 0; i < 6; i++){
      TString filename;
      filename.Form("e_correction_%d.dat", i);
      printf("        %s", filename.Data());
      file.open(filename.Data());
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
      
      file.close();
      printf("... done.\n");
      
      //printf("                 c0 : %f, m : %f \n", c0[i][2], m[i]);
   }

/**///========================================================= Analysis

   TH1F ** h = new TH1F[6];
   for( int i = 0; i < 6; i++){
      TString name;
      name.Form("h%d", i);
      h[i] = new TH1F(name, name , eRange[0], eRange[1], eRange[2]);
      h[i]->SetXTitle("Ex [a.u.]");
      
      for( int j = 0; j < 4; j++){   
         TString expression;
         
         expression.Form("-(e[%d] - %f * x[%d])*%f - %f>>  + h%d" , i + 6*j ,  m[i] ,i + 6*j , c1[i][j], c0[i][j], i);
         
         tree->Draw(expression, "" , "");
      }
   }
   
   printf("========== find peaks\n");
   // find peaks
   vector<double> * peak = new vector<double>[6];
   
   TSpectrum * spec = new TSpectrum(20);
   for(int i = 0 ; i < 6; i++){
      int nPeaks = spec->Search(h[i], 1 ,"", 0.10);
      float * xpos = spec->GetPositionX();
      
      // sorting and save 
      int * index = new int[nPeaks];
      TMath::Sort(nPeaks, xpos, index, 0 );  
      for( int j = 0; j < nPeaks; j++){
         peak[i].push_back(xpos[index[j]]);
      }
      
   //   printf("-------------------------- h%d \n", i);
   //   for(int j = 0; j < peak[i].size() ; j ++){
   //      printf("%d, x: %8.3f \n", j, peak[i][j]);
   //   }
   
   }
   
   
   printf("============== peaks selections \n");
   vector<double> * Upeak = new vector<double>[6];  
   
   double * q0 = new double[6];
   double * q1 = new double[6];
   
   q0[0] = 0;
   q1[0] = 1;
   
   bool endFlag = false;

   for( int i = 1; i < 6; i ++){
      
      cScript->cd(1);
      h[i-1]->Draw();
      cScript->cd(2);
      h[i]->Draw();      
      cScript->Update();
      
      Upeak[i-1].clear();
      printf("======== for h%d (1 for accept, 8 for skip the rest, 9 for end)\n", i-1);
      for( int j = 0; j < peak[i-1].size(); j++){
         double temp = peak[i-1][j];
         printf(" %8.3f ? ", temp);
         int ok;
         scanf("%d", &ok);
         if( ok == 1){
            Upeak[i-1].push_back(temp);
         }else if( ok == 8){
            break;
         }else if( ok == 9){
            endFlag = true;
            break;
         }
      }
      
      if( endFlag ) break;
      
      Upeak[i].clear();
      printf("======== for h%d (1 for accept, 8 for skip the rest) \n", i);
      for( int j = 0; j < peak[i].size(); j++){
         double temp = peak[i][j];
         printf(" %8.3f ? ", temp);
         int ok;
         scanf("%d", &ok);
         if( ok == 1){
            Upeak[i].push_back(temp);
         }if( ok == 8){
            break;
         }
         
      }
      
      // fitting
      int numPeak = Upeak[i].size();
      TGraph * ga = new TGraph(numPeak, &Upeak[i][0], &Upeak[i-1][0] );
      ga->Draw("*ap");
      ga->Fit("pol1", "q");
      q0[i] = pol1->GetParameter(0);
      q1[i] = pol1->GetParameter(1);
      printf("==== %d | q0:%8.3f, q1:%8.3f \n", i, q0[i], q1[i]);
      /***/
   }
   
   if(endFlag) return;
   
   printf("========= recalculate the coefficients \n");
   double * j0 = new double[6];
   double * j1 = new double[6];
   
   j1[0] = q1[0];          j0[0] = q0[0]; 
   for( int i = 1 ; i < 6 ; i++){
      j1[i] = j1[i-1] * q1[i];  
      j0[i] = j0[i-1] + q0[i]*j1[i-1];
   }
   
   for( int i = 0; i < 6; i++){  
      printf("%d === j0: %f, j1: %f\n",i,  j0[i], j1[i]);
   }
   //=========== final spectrum
   
   printf("========== plotting final spectrum \n");
   
   TH1F * k = new TH1F("k", "k" , eRange[0], eRange[1], eRange[2]);
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
                            j1[i],
                            j0[i]);
        
         tree->Draw(expression, "" , "");
      }
   }
   
   cScript->cd(2);
   k->Draw();
   
   //===== save correction parameter   
   cScript->Update();
   printf("0 for end, 1 for saving parameters: ");
   int dummy;
   scanf("%d", &dummy);
   if( dummy == 0 ) return;
   if( dummy == 1 ){
      FILE * paraOut;
      TString filename;
      filename.Form("e_correction_diff.dat");
      paraOut = fopen (filename.Data(), "w+");
      
      printf("=========== save parameters to %s \n", filename.Data());
      for( int i = 0; i < 6; i++){
         fprintf(paraOut, "%9.6f  %9.6f\n", j0[i], j1[i]);
      }
      
      fflush(paraOut);
      fclose(paraOut);
   }
   
   
   /**/
}

