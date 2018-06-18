#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TH2F.h>
#include <TH1F.h>
#include <TF1.h>
#include <TMath.h>
#include <TSpectrum.h>
#include <TGraph.h>
#include <fstream>

void Cali_e_diffPos(){
/**///======================================================== User initial input
   
   const char* rootfile="~/ANALYSIS/H060_ana/C_gen_run32.root"; const char* treeName="tree";
   
   //setting the range of the flatted energy;
   int eRange[3] = {100, -4000, -1000};
   
/**///========================================================  load tree

   TFile *f0 = new TFile (rootfile, "read"); 
   TTree *tree = (TTree*)f0->Get(treeName);
   printf("=====> /// %15s //// is loaded. Total #Entry: %10d \n", rootfile,  tree->GetEntries());
   //printf("=====>  Total #Entry: %10d \n", tree->GetEntries());
   
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
   
   //=================== load detector geometery
   vector<double> pos;
   double length = 50.5;
   double firstPos = 0;
   int iDet = 6; // number of detector at different position
   int jDet = 4; // number of detector at same position

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
      
      iDet = pos.size();
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
       return;
   }
   
   //=================== load energy for same pos
   double ** c0 = new double*[iDet];
   double ** c1 = new double*[iDet];
   double * m  = new double[iDet];
   for(int i = 0; i < iDet; i ++){
      c0[i] = new double[jDet];
      c1[i] = new double[jDet];
   }
   printf("----- loading energy calibration. \n");
   for( int i = 0; i < iDet; i++){
      TString filename;
      filename.Form("correction_e%d.dat", i);
      printf("        %s", filename.Data());
      file.open(filename.Data());
      double a, b;
      int j = 0;
      while( file >> a >> b ){
         c0[i][j] = a;
         c1[i][j] = b;
         j = j + 1;
         if( j >= jDet) break;
      }
      file >> a;
      m[i] = a;
      
      file.close();
      printf("... done.\n");
      printf("    --------- detID : %d \n", i );
      for( int k = 0; k < jDet; k++ ){
         printf(" %d ---- c0 : %7.2f, c1 : %7.2f, m : %7.2f \n", k,  c0[i][k], c1[i][k],  m[i]);
      }
   }

/**///========================================================= Analysis

   TH1F ** h = new TH1F*[iDet];
   for( int i = 0; i < iDet; i++){
      TString name;
      name.Form("h%d", i);
      h[i] = new TH1F(name, name , eRange[0], eRange[1], eRange[2]);
      h[i]->SetXTitle("Ex [a.u.]");
      
      for( int j = 0; j < jDet; j++){   
         TString expression;
         
         expression.Form("-(e[%d] - %f * z[%d])*%f - %f>>  + h%d" , i + 6*j ,  m[i] ,i + 6*j , c1[i][j], c0[i][j], i);
         
         tree->Draw(expression, "hitID == 0" , "");
      }
   }
   
   printf("========== find peaks\n");
   // find peaks
   vector<double> * peak = new vector<double>[6];
   
   TSpectrum * spec = new TSpectrum(20);
   for(int i = 0 ; i < iDet; i++){
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
   
   double * q0 = new double[iDet];
   double * q1 = new double[iDet];
   
   q0[0] = 0;
   q1[0] = 1;
   
   bool endFlag = false;

   for( int i = 1; i < iDet; i ++){
      
      cScript->cd(1);
      h[i-1]->Draw();
      cScript->cd(2);
      h[i]->Draw();      
      cScript->Update();
      
      Upeak[i-1].clear();
      printf("======== for h%d (0 for reject, 1 for accept, 8 for skip the rest, 9 to exit)\n", i-1);
      for( int j = 0; j < (int) peak[i-1].size(); j++){
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
      printf("======== for h%d (0 for reject, 1 for accept, 8 for skip the rest) \n", i);
      for( int j = 0; j < (int) peak[i].size(); j++){
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
      
      //================================= fitting
      int numPeak = Upeak[i].size();
      if( numPeak > 1 ){
         TGraph * ga = new TGraph(numPeak, &Upeak[i][0], &Upeak[i-1][0] );
         TF1 * fit = new TF1("fit", "pol1");
         ga->Draw("*ap");
         ga->Fit("fit", "q");
         q0[i] = fit->GetParameter(0);
         q1[i] = fit->GetParameter(1);
      }else if(numPeak == 1){ // simply shift      
         q0[i] = Upeak[i][0] - Upeak[i-1][0];
         q1[i] = 1;
      }else{
         q0[i] = 0;
         q1[i] = 1;
      }
      printf("==== %d | q0:%8.3f, q1:%8.3f \n", i, q0[i], q1[i]);
      /***/
   }
   
   if(endFlag) return;
   
   printf("========= recalculate the coefficients \n");
   double * j0 = new double[iDet];
   double * j1 = new double[iDet];
   
   j1[0] = q1[0];          
   j0[0] = q0[0]; 
   for( int i = 1 ; i < iDet ; i++){
      j1[i] = j1[i-1] * q1[i];  
      j0[i] = j0[i-1] + q0[i]*j1[i-1];
   }
   
   for( int i = 0; i < iDet; i++){  
      printf("%d === j0: %f, j1: %f\n",i,  j0[i], j1[i]);
   }
   //=========== final spectrum
   
   printf("========== plotting final spectrum \n");
   
   TH1F * k = new TH1F("k", "k" , eRange[0], eRange[1], eRange[2]);
   k->SetXTitle("Ex [a.u.]");
   for( int i = 0; i < iDet; i++){  
      for( int j = 0; j < jDet; j++){   
         TString expression;
         expression.Form("(-(e[%d] - %f * z[%d])*%f - %f)*%f + %f >>  + k" , 
                            i + iDet*j ,  
                            m[i] ,
                            i + iDet*j , 
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
      filename.Form("correction_e_diff.dat");
      paraOut = fopen (filename.Data(), "w+");
      
      printf("=========== save parameters to %s \n", filename.Data());
      for( int i = 0; i < iDet; i++){
         fprintf(paraOut, "%9.6f  %9.6f\n", j0[i], j1[i]);
      }
      
      fflush(paraOut);
      fclose(paraOut);
   }

}

