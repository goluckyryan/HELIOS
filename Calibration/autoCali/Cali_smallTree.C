#include <TROOT.h>
#include <TStyle.h>
#include <TTree.h>
#include <TFile.h>
#include <TSelector.h>
#include <TMath.h>
#include <TBenchmark.h>
#include <TH2F.h>
#include <TH1F.h>
#include <TF1.h>
#include <TGraph2D.h>
#include <TRandom.h>
#include <string>
#include <fstream>
#include <math.h>
#include <TRandom.h>
#include <TDatime.h>

void Cali_smallTree(TTree *expTree, TString saveFileName = "temp.root"){
/**///======================================================== User Input

/**///======================================================== display tree
   int totnumEntry = expTree->GetEntries();
   
   printf("====================== make smaller tree.\n"); 
   printf("============== Total #Entry: %10lld \n", expTree->GetEntries());
   
/**///========================================================= load correction
//========================================= detector Geometry
   string detGeoFileName = "detectorGeo_upstream.txt";
   int numDet;
   int rDet = 6; // number of detector at different position, row-Det
   int cDet = 4; // number of detector at same position, column-Det
   vector<double> pos;
   double length = 50.5;
   double firstPos = -110;
   double xnCorr[24]; // xn correction for xn = xf
   double xfxneCorr[24][2]; //xf, xn correction for e = xf + xn
   
   printf("----- loading detector geometery : %s.", detGeoFileName.c_str());
   ifstream file;
   file.open(detGeoFileName.c_str());
   int i = 0;
   if( file.is_open() ){
      string x;
      while( file >> x){
         //printf("%d, %s \n", i,  x.c_str());
         if( x.substr(0,2) == "//" )  continue;
         if( i == 6 ) length   = atof(x.c_str());
         if( i == 8 ) firstPos = atof(x.c_str());
         if( i == 9 ) cDet = atoi(x.c_str());
         if( i >= 10 ) {
            pos.push_back(atof(x.c_str()));
         }
         i = i + 1;
      }
      
      rDet = pos.size();
      file.close();
      printf("... done.\n");
      
      for(int id = 0; id < rDet; id++){
         pos[id] = firstPos + pos[id];
      }
      
      for(int i = 0; i < rDet ; i++){
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
   
   numDet = rDet * cDet;
   
   //========================================= xf = xn correction
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
      return;
   }
   file.close();
   
   //========================================= e = xf + xn correction

   printf("----- loading xf/xn-e correction.");
   file.open("correction_xfxn_e.dat");
   if( file.is_open() ){
      double a, b;
      int i = 0;
      while( file >> a >> b){
         if( i >= numDet) break;
         xfxneCorr[i][0] = a;
         xfxneCorr[i][1] = b;
         i = i + 1;
      }
      printf("... done.\n");
   }else{
      printf("... fail.\n");
      return;
   }
   file.close();

/**///======================================================== setup tree

   Float_t  e[100]; TBranch * b_Energy; //!
   Float_t xf[100]; TBranch * b_XF;     //!
   Float_t xn[100]; TBranch * b_XN;     //!

   expTree->SetBranchAddress("e", e, &b_Energy);
   expTree->SetBranchAddress("xf", xf, &b_XF);
   expTree->SetBranchAddress("xn", xn, &b_XN);
   
/**///======================================================== Extract tree entry, create new smaller trees, use that tree to speed up

   TBenchmark clock;  bool shown = false; 

   TFile * tempF1 = new TFile(saveFileName, "recreate");
   TTree * tempTree = new TTree("tree", "tree");
   
   double eTemp, xTemp, zTemp;
   int detIDTemp, multi;
   tempTree->Branch("e", &eTemp, "eTemp/D");
   tempTree->Branch("z", &zTemp, "zTemp/D");
   tempTree->Branch("x", &xTemp, "xTemp/D");
   tempTree->Branch("detID", &detIDTemp, "detIDTemp/I");
   tempTree->Branch("multi", &multi, "multi/I");

   for( int eventT = 0 ; eventT < totnumEntry; eventT ++ ){
      expTree->GetEntry(eventT);
      
      multi = 0; //multiplicity
      for( int idet = 0 ; idet < numDet; idet ++){
         if( e[idet] == 0 ) continue;
         if( xf[idet] == 0 && xn[idet] == 0 ) continue;
         
         detIDTemp = idet;
         eTemp = e[idet];
         
         double xfC = xf[idet] * xfxneCorr[idet][1] + xfxneCorr[idet][0] ;
         double xnC = xn[idet] * xnCorr[idet] * xfxneCorr[idet][1] + xfxneCorr[idet][0];
         
         if( xf[idet] > 0 && xn[idet] > 0 ){
            xTemp = (xfC - xnC)/(xfC+xnC);
            multi++;
         }else if(xf[idet] == 0 && xn[idet] > 0 ){
            xTemp = (1-2*xnC/e[idet]);
            multi++;
         }else if(xf[idet] > 0 && xn[idet] == 0 ){
            xTemp = (2*xfC/e[idet]-1);
            multi++;
         }else{
            xTemp = TMath::QuietNaN();
         }
         
         int detID = idet%rDet;
         if( pos[detID] < 0 ){
            zTemp = pos[detID] - (-xTemp + 1.)*length/2 ; 
         }else{
            zTemp = pos[detID] + (xTemp + 1.)*length/2 ; 
         }
         
      }
      if( multi == 0 ) continue;
      tempTree->Fill();
      
      clock.Stop("timer");
      Double_t time = clock.GetRealTime("timer");
      clock.Start("timer");

      if ( !shown ) {
         if (fmod(time, 10.) < 1 ){
            printf( "%10d[%2d%%]|%3.0f min %5.2f sec | expect:%5.2f min\n", 
                  eventT, 
                  TMath::Nint((eventT+1)*100./totnumEntry),
                  TMath::Floor(time/60.), time - TMath::Floor(time/60.)*60.,
                  totnumEntry*time/(eventT+1.)/60.);
                  shown = 1;
         }
      }else{
         if (fmod(time, 10) > 9 ){
            shown = 0;
         }
      }
      
      
   }
   
   tempTree->Write();
   int totalEventNum = tempTree->GetEntries();
   printf("========== saved to %s , event : %d \n", saveFileName.Data(), totalEventNum); 
   tempF1->Close();
   
}
