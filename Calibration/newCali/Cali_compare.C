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
#include <string>
#include <fstream>

void Cali_compare(TTree *tree, TTree *rTree){

   int totnumEntry = tree->GetEntries();
   
   printf("============== Total #Entry: %10d \n", tree->GetEntries());
   printf("==== reference Total #Entry: %10d \n", rTree->GetEntries());
   
/**///======================================================== histogram
   
   TH2F * exPlot  = new TH2F("exPlot", "exPlot", 400, -1.3, 1.3, 400, 0, 2500);
   TH2F * exPlotC = new TH2F("exPlotC", "exPlotC", 400, -1.3, 1.3, 400, 0, 10);
   TH2F * exPlotR = new TH2F("exPlotR", "exPlotR", 400, -1.3, 1.3, 400, 0, 10);
   
/**///========================================================= load correction
//========================================= detector Geometry
   string detGeoFileName = "detectorGeo_upstream.txt";
   int numDet;
   int iDet; // number of detector at different position
   int jDet; // number of detector at same position
   vector<double> pos;
   double length;
   double firstPos;
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
   
   numDet = iDet * jDet;
   
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

   Float_t         e[100];
   Float_t         xf[100];
   Float_t         xn[100];

   TBranch        *b_Energy;   //!
   TBranch        *b_XF;   //!
   TBranch        *b_XN;   //!

   tree->SetBranchAddress("e", e, &b_Energy);
   tree->SetBranchAddress("xf", xf, &b_XF);
   tree->SetBranchAddress("xn", xn, &b_XN);
   
   double         eR;
   double         xR;
   int         detID;
   
   TBranch        *b_e;   //!
   TBranch        *b_x;   //!
   TBranch        *b_detID;   //!

   rTree->SetBranchAddress("e", &eR, &b_e);
   rTree->SetBranchAddress("x", &xR, &b_x);
   rTree->SetBranchAddress("detID", &detID, &b_detID);
   
/**///======================================================== Extract tree entry, create two new smaller trees, use that tree to speed up
   
   
   
/**///======================================================== Get tree entry
   printf("=======================\n");
   /*
   TBenchmark clock;
   Bool_t shown;
   Int_t count;
   
   clock.Reset();
   clock.Start("timer");
   shown = 0;
   count = 0;
   */
   
   
   double A0 = 0.;
   double A1 = 200.;
   
   double minTotalMinDist = 10000000.;
   double distThreshold   = 1.;
   
   //=== test on det-3
   int idet = 3;
   bool fillFlag = false;
   
   for( double a1 = 250; a1 <= 270; a1 += 10){
      for( double a0 = 0.3; a0 <= 0.7 ; a0 += 0.1){
      
      printf("##########################========================== a1: %f, a0:%f \n", a1, a0);
      
      double totalMinDist    = 0.;
      
         for( int eventID = 0 ; eventID < tree->GetEntries(); eventID ++ ){
            tree->GetEntry(eventID);
            if( e[idet] == 0 ) continue;
            if( xf[idet] == 0 ) continue;
            if( xn[idet] == 0 ) continue;
            
            //calculate x
            double xfC = xf[idet] * xfxneCorr[idet][1] + xfxneCorr[idet][0] ;
            double xnC = xn[idet] * xnCorr[idet] * xfxneCorr[idet][1] + xfxneCorr[idet][0];
            double x = (xfC - xnC)/(xfC+xnC);
            
            
            double minDist = 100000000;
            
            
            //printf("==================== %d| %f, %f \n", eventID, e[idet]/a1 - a0, x);
            
            
            for( int eventj = 0; eventj < rTree->GetEntries(); eventj ++){
               rTree->GetEntry(eventj);
               if( detID != idet%6 ) continue;
               
               if(fillFlag == false) exPlotR->Fill(xR, eR);
               
               //calculate dist
               double tempDist = TMath::Power(x - xR,2) + TMath::Power( (e[idet]/a1 - a0) - eR,2);
               if( tempDist < minDist ) minDist = tempDist;
               
               //if( eventj%100 == 0) printf("%d, %8.4f, %8.4f| %8.4f < %8.4f \n", eventj, eR, xR, tempDist, minDist);
               
            }
            
            if( minDist < distThreshold ) {
               totalMinDist += minDist;
               exPlot->Fill(x, e[idet]);
            }
            
            
            //#################################################################### Timer  
            fillFlag = true;
            
            //if( eventID > 500000) break;
            /*
            clock.Stop("timer");
            Double_t time = clock.GetRealTime("timer");
            clock.Start("timer");

            if ( !shown ) {
               if (fmod(time, 10) < 1 ){
                  printf( "%10d[%2d%%]|%3.0f min %5.2f sec | expect:%5.2f min\n", 
                        eventID, 
                        TMath::Nint((eventID+1)*100./totnumEntry),
                        TMath::Floor(time/60.), time - TMath::Floor(time/60.)*60.,
                        totnumEntry*time/(eventID+1.)/60.);
                        shown = 1;
               }
            }else{
               if (fmod(time, 10) > 9 ){
                  shown = 0;
               }
            }
            */
            
         }
      
         printf("========== totalMinDist: %f < %f \n", totalMinDist, minTotalMinDist);   
         
         if( totalMinDist < minTotalMinDist ) {
            minTotalMinDist = totalMinDist;
            A0 = a0;
            A1 = a1;
            printf("******************************************** A1: %f, A0: %f \n", A1, A0);
         }
         
      } // end of loop a0
   
   }// end of loop a1
   
   
   //After founded the best fit, plot the result
   
   printf("******************************************** A1: %f, A0: %f \n", A1, A0);
   for( int eventID = 0 ; eventID < tree->GetEntries(); eventID ++ ){
      tree->GetEntry(eventID);
      if( e[idet] == 0 ) continue;
      if( xf[idet] == 0 ) continue;
      if( xn[idet] == 0 ) continue;
      
      //calculate x
      double xfC = xf[idet] * xfxneCorr[idet][1] + xfxneCorr[idet][0] ;
      double xnC = xn[idet] * xnCorr[idet] * xfxneCorr[idet][1] + xfxneCorr[idet][0];
      double x = (xfC - xnC)/(xfC+xnC);
      
      exPlotC->Fill(x, e[idet]/A1 - A0);
         
   }
   

/**///======================================================== Browser or Canvas

   Int_t Div[2] = {3,1};  //x,y
   Int_t size[2] = {400,400}; //x,y
   
   TCanvas * cScript = new TCanvas("cScript", "cScript", 0, 0, size[0]*Div[0], size[1]*Div[1]);
   cScript->Divide(Div[0],Div[1]);
   for( int i = 1; i <= Div[0]+Div[1] ; i++){
      cScript->cd(i)->SetGrid();
   }

   gStyle->SetOptStat(1111111);
   gStyle->SetStatY(1.0);
   gStyle->SetStatX(0.99);
   gStyle->SetStatW(0.2);
   gStyle->SetStatH(0.1);
   
   cScript->cd(1);
   exPlot->Draw();
   cScript->cd(2);
   exPlotR->Draw();
   
   cScript->cd(3);
   exPlotC->Draw();

}
