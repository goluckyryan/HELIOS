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

void Cali_compare(TTree *tree, TTree *rTree){

   double a1Range[3] = {200, 300, 10};
   double a0Range[3] = {-0.7, 0.7, 0.1}; 


   int totnumEntry = tree->GetEntries();
   
   printf("============== Total #Entry: %10d \n", tree->GetEntries());
   printf("==== reference Total #Entry: %10d \n", rTree->GetEntries());
   
/**///======================================================== histogram
   
   TH2F * exPlot  = new TH2F("exPlot", "exPlot", 400, -1.3, 1.3, 400, 0, 2500);
   TH2F * exPlotC = new TH2F("exPlotC", "exPlotC", 400, -1.3, 1.3, 400, 0, 10);
   TH2F * exPlotR = new TH2F("exPlotR", "exPlotR", 400, -1.3, 1.3, 400, 0, 10);
   TGraph2D * gDist = new TGraph2D();   

/**///======================================================== Plot

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
   
/**///========================================================= load correction
//========================================= detector Geometry
   string detGeoFileName = "detectorGeo_upstream.txt";
   int numDet;
   int iDet = 6; // number of detector at different position
   int jDet = 4; // number of detector at same position
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

   Float_t  e[100]; TBranch * b_Energy;   //!
   Float_t xf[100]; TBranch * b_XF;   //!
   Float_t xn[100]; TBranch * b_XN;   //!

   tree->SetBranchAddress("e", e, &b_Energy);
   tree->SetBranchAddress("xf", xf, &b_XF);
   tree->SetBranchAddress("xn", xn, &b_XN);
   
   double      eR;   TBranch * b_e;   //!
   double      xR;   TBranch * b_x;   //!
   int      detID;   TBranch * b_detID;   //!
   int       loop;   TBranch * b_loop; //!
   int        hit;   TBranch * b_hit;

   rTree->SetBranchAddress("e", &eR, &b_e);
   rTree->SetBranchAddress("x", &xR, &b_x);
   rTree->SetBranchAddress("detID", &detID, &b_detID);
   rTree->SetBranchAddress("loop", &loop, &b_loop);
   rTree->SetBranchAddress("hit", &hit, &b_hit);
   
/**///======================================================== Extract tree entry, create new smaller trees, use that tree to speed up
   double B1 [numDet]; // best a1 of iDet
   double B0 [numDet]; // best a0 of iDet

   TBenchmark clock;  
   for( int idet = 0; idet < numDet; idet ++){
      
      bool shown = false; clock.Reset(); clock.Start("timer");
   
      //TODO is save as an array faster?
      printf("========== creating a smaller trees for detID = %d \n", idet);
      TString title; title.Form("detID-%d", idet);      
            
      TFile * temp1 = new TFile("temp1.root", "recreate");
      TTree * tTree1 = new TTree("tree", "tree");
      
      double e1, x1;
      tTree1->Branch("e1", &e1, "e1/D");
      tTree1->Branch("x1", &x1, "x1/D");

      for( int eventID = 0 ; eventID < totnumEntry; eventID ++ ){
         tree->GetEntry(eventID);
         if( e[idet] == 0 ) continue;
         if( xf[idet] == 0 ) continue;
         if( xn[idet] == 0 ) continue;
         
         e1 = e[idet];
         
         double xfC = xf[idet] * xfxneCorr[idet][1] + xfxneCorr[idet][0] ;
         double xnC = xn[idet] * xnCorr[idet] * xfxneCorr[idet][1] + xfxneCorr[idet][0];
         x1 = (xfC - xnC)/(xfC+xnC);
         
         tTree1->Fill();
         
         clock.Stop("timer");
         Double_t time = clock.GetRealTime("timer");
         clock.Start("timer");

         if ( !shown ) {
            if (fmod(time, 10.) < 1 ){
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
      }
      tTree1->Write();
      int totalEventNum = tTree1->GetEntries();
      printf("========== saved event : %d \n", totalEventNum); 
      temp1->Close();
      
   /**///======================================================== open the tree
      
      const char* tempfile="temp1.root";
      TFile *f0 = new TFile (tempfile, "read"); 
      TTree * sTree = (TTree*)f0->Get("tree"); // s for seleced
      
      double  eS; TBranch * b_eS;   //!
      double  xS; TBranch * b_xS;   //!

      sTree->SetBranchAddress("e1", &eS, &b_eS);
      sTree->SetBranchAddress("x1", &xS, &b_xS);
      
      
   /**///======================================================== Get tree entry

      clock.Reset(); clock.Start("timer");
      
      double A0 = 0.;
      double A1 = 200.;
      
      double minTotalMinDist = 99.;
      double distThreshold   = 0.2;
      
      bool fillFlag = false;
      exPlotR->Clear();
      exPlotR->SetTitle(title + "(sim)");

      gDist->SetTitle("Total min-dist; a1; a0; min-dist");
      
      int eventIDStepSize = sTree->GetEntries()/200 ; // restrict number of point be around 200 to 300
      int eventjStepSize = 3;
      
      if( eventIDStepSize == 0 ) eventIDStepSize = 1;
      if( eventjStepSize == 0 ) eventjStepSize = 1;
      
      int nPoint = 50;
      printf("======================= find fit by Monle Carlo. #Point: %d\n", nPoint);
      for( int iPoint = 0; iPoint < nPoint; iPoint ++){ 
         
         //TODO better method, not pure random
         double a1, a0;
         //if( iPoint == 0){
            a1 = a1Range[0] + (a1Range[1] - a1Range[0])*gRandom->Rndm();
            a0 = a0Range[0] + (a0Range[1] - a0Range[0])*gRandom->Rndm();
         //}else{
         //   a1 = A1 + 2*TMath::Abs(A1 - a
         //}
         
         
         printf("%3d | a1: %5.1f, a0:%6.2f | ", iPoint, a1, a0);
         
         double totalMinDist    = 0.;
         bool filledOnce = false;
         for( int eventID = 0 ; eventID < sTree->GetEntries(); eventID += eventIDStepSize ){
            sTree->GetEntry(eventID);
            
            double minDist = 99;
            
            //printf("==================== %d| %f, %f \n", eventID, eS/a1 - a0, xS);
            for( int eventj = 0; eventj < rTree->GetEntries(); eventj += eventjStepSize){
               rTree->GetEntry(eventj);
               if( detID != idet%6 ) continue;
               if( loop  != 1 ) continue;
               if( hit   != 1 ) continue;
               
               if(fillFlag == false) {
                  exPlotR->Fill(xR, eR);
               }
               //calculate dist
               double tempDist = TMath::Power(xS - xR,2) + TMath::Power( (eS/a1 - a0) - eR,2);
               if( tempDist < minDist ) minDist = tempDist;
               
               //if( eventj%100 == 0) printf("%d, %8.4f, %8.4f| %8.4f < %8.4f \n", eventj, eR, xR, tempDist, minDist);               
            }
            fillFlag = true; 

            if( minDist < distThreshold ) {
               totalMinDist += minDist;
               filledOnce = true;
            }
         }
         
         if( ! filledOnce ) totalMinDist = 10.; // to avoid no fill
         
         gDist->SetPoint(iPoint, a1, a0, totalMinDist);
         
         printf("totalMinDist: %6.3f < %6.3f ", totalMinDist, minTotalMinDist);   

         //======== time
         clock.Stop("timer");
         Double_t time = clock.GetRealTime("timer");
         clock.Start("timer");
         printf( "(%5.2f min)", time/60.);
         
         if( totalMinDist < minTotalMinDist ) {
            minTotalMinDist = totalMinDist;
            A0 = a0;
            A1 = a1;
            printf(" A1: %5.1f, A0: %5.3f \n", A1, A0);
         }else{
            printf("\n");
         }
            
      }// end of loop
      
      
      //After founded the best fit, plot the result
      printf("==========> A1: %f, A0: %f \n", A1, A0);
      exPlot->Clear();
      exPlot->SetTitle(title + "(exp)");
      exPlotC->Clear();
         
      for( int eventID = 0 ; eventID < sTree->GetEntries(); eventID ++ ){
         sTree->GetEntry(eventID);
         
         exPlot->Fill(xS, eS);
         exPlotC->Fill(xS, eS/A1 - A0);
         
         B1[idet] = A1;
         B0[idet] = A0;
            
      }
      
      cScript->cd(1);
      exPlot->Draw();
      cScript->cd(2);
      exPlotR->Draw("colz");
      exPlotC->Draw("same");
      cScript->cd(3);
      gDist->Draw("surf1");
    
      cScript->Update();
   
   } // end of loop idet  

/**///======================================================== save result

   FILE * paraOut;
   TString filename;
   filename.Form("correction_e.dat");
   paraOut = fopen (filename.Data(), "w+");

   printf("=========== save parameters to %s \n", filename.Data());
   for( int i = 0; i < numDet; i++){
   fprintf(paraOut, "%9.6f  %9.6f\n", B1[i], B0[i]);
   }

   fflush(paraOut);
   fclose(paraOut);

}
