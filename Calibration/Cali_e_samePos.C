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

void Cali_e_samePos(){
/**///======================================================== initial input
   const char* rootfile="~/ANALYSIS/H060_ana/C_gen_run11.root"; const char* treeName="tree";
   
   // pause
   int detID;
   printf("which detector ? ");
   scanf("%d", &detID);

   const int numDet = 4;
   int eRange[3] = {150, -300, 1000}; //the range of Ex

   double cutSlope = 4.0; //redline, for cut
   double cutIntep[4] = {920, 900, 880, 920}; 
   double slope = 4.0; //blueline, for fit
   double yIntep = 2400;
   int numPeak = 10;
   eRange[0]=  80;   
   
   if( detID == 0){
      cutIntep[0] = 2400;
      cutIntep[1] = 2400;
      cutIntep[2] = 2400;
      cutIntep[3] = 2400;
      slope = 4;
   }
   
   if( detID == 1 ){
      cutIntep[0] = 2000;
      cutIntep[1] = 2000;
      cutIntep[2] = 2000;
      cutIntep[3] = 2000; 
      slope = 4.2;
   }
   
   if( detID == 2) {
      cutIntep[0] = 1800;
      cutIntep[1] = 1800;
      cutIntep[2] = 1800;
      cutIntep[3] = 1900;
      slope = 4.5;
      
      eRange[0]=  60;   
   }
   
   if( detID == 3){
      cutIntep[0] = 1600;
      cutIntep[1] = 1600;
      cutIntep[2] = 1600;
      cutIntep[3] = 1600;
      slope = 4.3;
      
      eRange[0]=  100;
   }
   
   if( detID == 4){
      cutIntep[0] = 1600;
      cutIntep[1] = 1600;
      cutIntep[2] = 1600;
      cutIntep[3] = 1650;
      slope = 4.0; // the slope can be difference.
   }
   
   if( detID == 5){
      cutIntep[0] = 1600;
      cutIntep[1] = 1600;
      cutIntep[2] = 1600;
      cutIntep[3] = 1600;
      slope = 3.7;
   }
   
/**///========================================================  load tree

   TFile *f0 = new TFile (rootfile, "read"); 
   TTree *tree = (TTree*)f0->Get(treeName);
   printf("=====> /// %15s //// is loaded. Total #Entry: %10d \n", rootfile,  tree->GetEntries());
   
/**///======================================================== Browser or Canvas

   //TBrowser B ;   
   Int_t Div[2] = {2,2};  //x,y
   Int_t size[2] = {400,400}; //x,y
   
   TCanvas * cScript = new TCanvas("cScript", "cScript", 0, 0, size[0]*Div[0], size[1]*Div[1]);
   cScript->Divide(Div[0],Div[1]);
   cScript->cd(1);
   for( int i = 1; i <= Div[0]+Div[1] ; i++){
      cScript->cd(i)->SetGrid();
   }

   gStyle->SetOptStat(1111111);
   gStyle->SetStatY(1.0);
   gStyle->SetStatX(0.99);
   gStyle->SetStatW(0.2);
   gStyle->SetStatH(0.1);
   
/**///========================================================= load files
   
   vector<double> pos;
   double length = 50.5;
   double firstPos = 0;

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
         if( i >= 9 ) {
            pos.push_back(atof(x.c_str()));
         }
         i = i + 1;
      }
      
      int nDet = pos.size();
      file.close();
      printf("... done.\n");
      
      for(int id = 0; id < nDet; id++){
         pos[id] = firstPos + pos[id];
      }
      
      for(int i = 0; i < nDet ; i++){
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

   
/**///========================================================= Analysis
   
   double xrange[2];
   if( firstPos > 0 ){
      xrange[0] = pos[detID] - 2;
      xrange[1] = pos[detID] + length + 2;   
   }else{
      xrange[0] = pos[detID] - length - 2;
      xrange[1] = pos[detID] + 2;
   }
   
   printf("======= showing cut line(red) and slope (blue)\n");
   
   TH2F ** b = new TH2F*[numDet];
   for( int i = 0; i < numDet; i ++){
      TString name;
      name.Form("b%d", i);
      b[i] = new TH2F(name, name , 200, xrange[0], xrange[1], 200, -100 , 2500);
      b[i]->SetXTitle("z");
      b[i]->SetYTitle("e");
      
      TString expression;
      expression.Form("e[%d]:z[%d]>> b%d" , detID + 6*i, detID + 6*i ,i);
      
      //printf("%s \n", expression.Data());
      
      TString gate;
      gate.Form("hitID == 0");
      
      cScript->cd(i+1);
      
      tree->Draw(expression, gate , "");
   }
   
   TF1 ** line = new TF1*[numDet];
   for( int i = 0; i < numDet; i++){
      TString name;
      name.Form("line%d", i);
      line[i] = new TF1(name, "[0] + x* [1]", xrange[0] , xrange[1]);
      line[i]->SetParameter(1, cutSlope);
      line[i]->SetLineColor(2);
      line[i]->SetParameter(0, cutIntep[i]);
      cScript->cd(i+1); 
      line[i]->Draw("same");
   }
   
   TF1 * traj = new TF1("traj", "[0] + x * [1]", xrange[0] , xrange[1]);
   traj->SetLineColor(4); // blue
   traj->SetParameter(1, slope);
   traj->SetParameter(0, yIntep);
   
   for( int i = 0; i < numDet; i++){
      cScript->cd(i+1); 
      traj->Draw("same");
   }
  
   //------ pause
   cScript->Update();
   printf("0 for stop, 1 for continous : ");
   int dummy;
   scanf("%d", &dummy);
   if( dummy == 0 ) return;
   
   
   printf("======= showing flattened energy vs pos \n");
   //============================== create 2-D histogram of projection
   eRange[1] = 300 - slope * pos[detID];
   eRange[2] = 2500 - slope * pos[detID];
   
   TH2F ** p = new TH2F*[numDet];
   for( int i = 0; i < numDet; i ++){
      TString name;
      name.Form("p%d", i);
      p[i] = new TH2F(name, name ,200, xrange[0] , xrange[1], eRange[0], eRange[1], eRange[2]);
      p[i]->SetYTitle("Ex [a.u.]");
      p[i]->SetXTitle("z");
      
      TString expression;
      expression.Form("e[%d] - %f * z[%d] : z[%d] >> p%d" , detID + 6*i, slope ,detID + 6*i ,detID + 6*i,  i);
      TString gate;
      gate.Form("e[%d] > %f + %f * z[%d] && hitID == 0 ", detID + 6*i, cutIntep[i], cutSlope, detID + 6*i);
      
      cScript->cd(i + 1);
      tree->Draw(expression, gate , "");
   }
   
      
   //pause
   cScript->Update();
   printf("0 for stop, 1 for projection and peak-finding, 2 for manual : ");
   scanf("%d", &dummy);
   if( dummy == 0 ) return;
   
   
   printf("======= showing energy spectrum and find peak\n");
   
   //================================ create 1-D histogram of projection
   TH1F ** g = new TH1F*[numDet];
   for( int i = 0; i < numDet; i ++){
      TString name;
      name.Form("g%d", i);
      g[i] = new TH1F(name, name , eRange[0], eRange[1], eRange[2]);
      g[i]->SetXTitle("Ex [a.u.]");
      
      TString expression;
      expression.Form("e[%d] - %f * z[%d]>> g%d" , detID + 6*i, slope ,detID + 6*i , i);
      TString gate;
      gate.Form("e[%d] > %f + %f * z[%d] && hitID == 0", detID + 6*i,cutIntep[i], cutSlope, detID + 6*i);
      
      cScript->cd(i + 1);
      
      tree->Draw(expression, gate , "");
   }
   
   cScript->Update();
   printf("================= find peak\n");
   
   vector<double> * peak = new vector<double>[numDet];
   int numCommonPeaks = 10;
   
   int ** index = new int*[numDet];
   
   if( dummy == 1){
      //======== find peak using TSpectrum
      TSpectrum * spec = new TSpectrum(10);
   
      for( int i = 0; i < numDet; i ++){
         //printf("------------- %d\n", i);
         cScript->cd(i+1);
         int nPeaks = spec->Search(g[i], 1 ,"", 0.10);
         if( nPeaks < numCommonPeaks ) numCommonPeaks = nPeaks;

         float * xpos = spec->GetPositionX();
         // sorting and save 
         index[i] = new int[nPeaks];
         TMath::Sort(nPeaks, xpos, index[i], 0 );  
         for( int j = 0; j < nPeaks; j++){
            peak[i].push_back(xpos[index[i][j]]);
         }
      }
   
   }else{
      
      if( detID == 0){
         numCommonPeaks = 3;
         peak[0].push_back(746); peak[0].push_back(1057); peak[0].push_back(1514);
         peak[1].push_back(804); peak[1].push_back(1128); peak[1].push_back(1607);
         peak[2].push_back(604); peak[2].push_back( 860); peak[2].push_back(1240);
         peak[3].push_back(717); peak[3].push_back( 994); peak[3].push_back(1428);
      }
      
      if( detID == 5){
         numCommonPeaks = 4;
         peak[0].push_back(-543); peak[0].push_back(-476); peak[0].push_back(-335); peak[0].push_back(-82);
         peak[1].push_back(-543); peak[1].push_back(-476); peak[1].push_back(-335); peak[1].push_back(-82);
         peak[2].push_back(-456); peak[2].push_back(-386); peak[2].push_back(-217); peak[2].push_back( 66); 
         peak[3].push_back(-537); peak[3].push_back(-478); peak[3].push_back(-332); peak[3].push_back(-79); 
      }   
      
   }

   vector<double> * Upeak = new vector<double>[numDet];  
   if( dummy == 1){
      cScript->Update();
      printf("0 for choosing lowest %d-peak, 1 for manual-select, 2 for end: ", numCommonPeaks);
      scanf("%d", &dummy);
      
      if( dummy == 2) return;
      
      // select peaks
      if( dummy == 0){
         
         for( int i = 0; i < numDet; i ++){
            for( int j = 0; j < numCommonPeaks; j++){
                  Upeak[i].push_back(peak[i][j]);
            }
         }
      
      
      }else if(dummy == 1){
      
         for( int i = 0; i < numDet; i ++){
            printf("======== for g%d ( 0 = reject, 1 = accept )\n", i);
            for( int j = 0; j < (int) peak[i].size(); j++){
               double temp = peak[i][j];
               printf(" %8.3f ? ", temp);
               int ok;
               scanf("%d", &ok);
               if( ok == 1){
                  Upeak[i].push_back(temp);
               }  
            }
         }
         
         numCommonPeaks = Upeak[0].size(); // assume user input is OK.
      
      }
   }else if(dummy == 2){
   
      for( int i = 0; i < numDet; i ++){
         for( int j = 0; j < numCommonPeaks; j++){
           Upeak[i].push_back(peak[i][j]);
         }
      }
   }
   
   numPeak = numCommonPeaks;   
   for( int i = 0; i < numDet; i ++){
      if( Upeak[i].size() > 0 ) {      
         printf("-------------------------- g%d \n", i);
         for(int j = 0; j < numCommonPeaks ; j ++){
            printf("%d, x: %8.3f \n", j, Upeak[i][j]);
         }
      }
   }
   
   cScript->Update();
   printf("0 for stop, 1 for scaling respect to g0: ");
   scanf("%d", &dummy);
   if( dummy == 0 ) return;
   
   //================================ find the scaling paramter respect to d0;
   double c0[numDet], c1[numDet];
   
   if( numPeak == 1 ){
      printf(" numEvent : %d \n", numPeak);
      for( int i = 0; i < numDet; i++){
         if( Upeak[i].size() > 0 ){
            c1[i] = 1.;
            c0[i] = - Upeak[i][0] + Upeak[0][0];
         }else{
            c1[i] = 1.;
            c0[i] = 0.;
         }   
         printf("==== %d | c0:%8.3f, c1:%8.3f \n", i, c0[i], c1[i]);
      }
   }else{
      printf(" numEvent : %d \n", numPeak);
      TGraph ** ga = new TGraph*[numDet];
      
      TF1 * fit = new TF1("fit", "pol1" );
      
      for( int i = 0; i < numDet; i++){
         ga[i] = new TGraph(numPeak, &Upeak[i][0], &Upeak[0][0] );
         ga[i]->Draw("*ap");
         ga[i]->Fit("fit", "q");
         c0[i] = fit->GetParameter(0);
         c1[i] = fit->GetParameter(1);
         printf("==== %d | c0:%8.3f, c1:%8.3f \n", i, c0[i], c1[i]);
      }
   }
   
   //===================================== scale the energy
   
   printf("========= plot correted energy. \n");
   
   TH1F ** h = new TH1F*[numDet];
   for( int i = 0; i < numDet; i ++){
      TString name;
      name.Form("h%d", i);
      h[i] = new TH1F(name, name , eRange[0], eRange[1], eRange[2]);
      h[i]->SetXTitle("Ex [a.u.]");
      //printf("--------%d \n", i);
      TString expression;
      expression.Form("(e[%d] - %f * z[%d])*%f + %f>> h%d" , detID + 6*i , slope ,detID + 6*i , c1[i], c0[i], i);
      TString gate;
      gate.Form("e[%d] > %f + %f * z[%d] && hitID == 0", detID + 6*i ,cutIntep[i], cutSlope, detID + 6*i);
      
      cScript->cd(1);
      
      if( i == 0) tree->Draw(expression, gate , "");
      tree->Draw(expression, gate , "same");
   }
   
   // ======== d with correction  
   printf("========= plot new-e vs z. \n");
   TH2F ** k = new TH2F*[numDet];
   TH2F * kall = new TH2F("kall", "kall",   200, xrange[0] , xrange[1] , 200, 100 , 2000);
   for( int i = 0; i < numDet; i ++){
      TString name;
      name.Form("k%d", i);
      k[i] = new TH2F(name, name , 200, xrange[0] , xrange[1] , 200, 100 , 2000);
      k[i]->SetXTitle("pos(xf,xn)");
      k[i]->SetYTitle("e");
      
      TString expression;
      cScript->cd(2);
      expression.Form("e[%d] * %f + %f + %f * z[%d]  : z[%d] >> + kall" , detID + 6*i, c1[i], c0[i], (1.0 - c1[i])*slope, detID + 6*i, detID + 6*i);      
      tree->Draw(expression, "hitID == 0" , "");
      
      
      expression.Form("e[%d] * %f + %f + %f * z[%d]  : z[%d] >> k%d" , detID + 6*i, c1[i], c0[i], (1.0 - c1[i])*slope, detID + 6*i, detID + 6*i, i);
      TString gate;
      gate.Form("hitID == 0");
      
      cScript->cd(3);
      if( i == 0) tree->Draw(expression, gate , "");
      tree->Draw(expression, gate , "same");
   }
  
   
   //===== save correction parameter   
   cScript->Update();
   printf("0 for end, 1 for saving parameters: ");
   scanf("%d", &dummy);
   if( dummy == 0 ) return;
   if( dummy == 1 ){
      FILE * paraOut;
      TString filename;
      filename.Form("e_correction_%d.dat", detID);
      paraOut = fopen (filename.Data(), "w+");
      
      printf("=========== save parameters to %s \n", filename.Data());
      for( int i = 0; i < numDet; i++){
         fprintf(paraOut, "%9.6f  %9.6f\n", c0[i], c1[i]);
      }
      fprintf(paraOut, "%9.6f\n", slope);
      
      fflush(paraOut);
      fclose(paraOut);
   }
   /**/
}

