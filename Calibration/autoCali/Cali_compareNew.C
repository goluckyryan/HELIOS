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

// this macro aims to align same position dets with saem scale factor but difference offset. 

void Cali_compareNew(TTree *tree, TTree *rTree, int det = -1){
/**///======================================================== User Input

   double a1Range[2] = {250, 320};
   double a0Range[2] = {-0.7, 0.7}; // the energy gap
   
   double eThreshold = 300;
   double minDistThreshold   = 0.02;

   int hitMode = 1; // 0 = only both xn, xf valid, 1 = any
   
   int ExIDMax = 3;
   
   int nTrial = 60 ;

   int scaleDownTo = 600;
   
   printf("======================== \n");
   printf("distant Threshold : %f \n", minDistThreshold);
   
/**///======================================================== display tree
   int totnumEntry = tree->GetEntries();
   
   printf("============== Total #Entry: %10d \n", tree->GetEntries());
   printf("==== reference Total #Entry: %10d \n", rTree->GetEntries());
   
/**///======================================================== histogram
   
   TH2F * exPlot  = new TH2F("exPlot" , "exPlot" , 200, -1.3, 1.3, 200, 0, 2500);
   TH2F * exPlotC = new TH2F("exPlotC", "exPlotC", 200, -1.3, 1.3, 200, 0, 10);
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

   jDet = 2;

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

   Float_t  e[100]; TBranch * b_Energy; //!
   Float_t xf[100]; TBranch * b_XF;     //!
   Float_t xn[100]; TBranch * b_XN;     //!

   tree->SetBranchAddress("e", e, &b_Energy);
   tree->SetBranchAddress("xf", xf, &b_XF);
   tree->SetBranchAddress("xn", xn, &b_XN);
   
   double      eR;   TBranch * b_e;     //!
   double      xR;   TBranch * b_x;     //!
   int      detID;   TBranch * b_detID; //!
   int       loop;   TBranch * b_loop;  //!
   int        hit;   TBranch * b_hit;   //!
   int       ExID;   TBranch * b_ExID;  //!

   rTree->SetBranchAddress("e", &eR, &b_e);
   rTree->SetBranchAddress("x", &xR, &b_x);
   rTree->SetBranchAddress("detID", &detID, &b_detID);
   rTree->SetBranchAddress("loop", &loop, &b_loop);
   rTree->SetBranchAddress("hit", &hit, &b_hit);
   rTree->SetBranchAddress("ExID", &ExID, &b_ExID);
   
/**///======================================================== Extract tree entry, create new smaller trees, use that tree to speed up
   double B1 [numDet]; // best a1 of iDet
   double B0 [numDet]; // best a0 of iDet

   TBenchmark clock;  
   
   int sDet = 0;
   int eDet = iDet;
   if(det >= 0 ) {
      sDet = det%iDet;
      eDet = sDet + 1;
   }
   
   for( int idet = sDet; idet < eDet ; idet ++){
      
      bool shown = false; clock.Reset(); clock.Start("timer");
      TString title; title.Form("detID%%%d=%d", iDet, idet);      
      printf("========== creating a smaller trees for detID%%%d = %d \n", iDet, idet);
         
      /**///========================================= TODO is save as an array faster?
      
      TFile * temp1 = new TFile("temp1.root", "recreate");
      TTree * tTree1 = new TTree("tree", "tree");
      
      double e1, x1;
      int detID1;
      tTree1->Branch("e1", &e1, "e1/D");
      tTree1->Branch("x1", &x1, "x1/D");
      tTree1->Branch("detID1", &detID1, "detID1/I");

      for( int eventID = 0 ; eventID < totnumEntry; eventID ++ ){
         tree->GetEntry(eventID);
         bool eventFlag = false;
         for( int jdet = idet ; jdet < numDet ; jdet += iDet){
           if( e[jdet] <  eThreshold ) continue;
           if( hitMode == 0 && ( xf[jdet] == 0 || xn[jdet] == 0 ) ) continue;
           if( hitMode == 1 && ( xf[jdet] == 0 && xn[jdet] == 0 ) ) continue;

           eventFlag = true;
           
           detID1 = jdet;
           e1 = e[jdet];

           double xfC = xf[jdet] * xfxneCorr[jdet][1] + xfxneCorr[jdet][0] ;
           double xnC = xn[jdet] * xnCorr[jdet] * xfxneCorr[jdet][1] + xfxneCorr[jdet][0];
         
           if( hitMode == 0 ) x1 = (xfC - xnC)/(xfC + xnC);
         
           if( hitMode == 1 ){
             if( xf[jdet] > 0 && xn[jdet] > 0 ){
               x1 = (xfC - xnC)/(xfC+xnC);
             }else if(xf[jdet] == 0 && xn[jdet] > 0 ){
               x1 = (1-2*xnC/e[jdet]);
             }else if(xf[jdet] > 0 && xn[jdet] == 0 ){
               x1 = (2*xfC/e[jdet]-1);
             }else{
               x1 = TMath::QuietNaN();
             }
           }  

         }
         if( eventFlag == false) continue;         
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
   /**///======================================================== open the tree and plot
   
      const char* tempfile="temp1.root";
      TFile *f0 = new TFile (tempfile, "read"); 
      TTree * sTree = (TTree*)f0->Get("tree"); // s for seleced
      printf("========== number of event : %d \n", sTree->GetEntries()); 
      
      double  eS; TBranch * b_eS;   //!
      double  xS; TBranch * b_xS;   //!
      int detIDS; TBranch * b_detIDS; //!

      sTree->SetBranchAddress("e1", &eS, &b_eS);
      sTree->SetBranchAddress("x1", &xS, &b_xS);
      sTree->SetBranchAddress("detID1", &detIDS, &b_detIDS);
      
      if( det >= 0 ){
         cScript->cd(1);
         exPlot->Reset();
         exPlot->SetTitle(title + "(exp)");
         for( int i = 0; i < sTree->GetEntries() ; i++){
            sTree->GetEntry(i);
            exPlot->Fill(xS, eS);
         }
         exPlot->Draw();
         cScript->Update();
         
         cScript->cd(2);
         exPlotR->Reset();
         exPlotR->SetTitle(title + "(sim)");
         for( int i = 0; i < rTree->GetEntries() ; i++){
            rTree->GetEntry(i);
            if( detID != idet%iDet ) continue;
            if( loop  != 1 ) continue;
            if( hit   != 1 ) continue;
            if( ExID  >  ExIDMax ) continue;

            exPlotR->Fill(xR, eR);
         }
         exPlotR->Draw("colz");            
         cScript->Update();
         
      }
   
   /**///======================================================== Get tree entry and cal dist
      
      clock.Reset(); clock.Start("timer");
      
      double A0[jDet];
      double A1 = 270.;
      for( int j = 0; j < jDet ; j++){
        A0[j] = 0;
      }
      
      double minTotalMinDist = 99.;
      
      gDist->SetTitle("Total min-dist; a1; a0; min-dist");
      gDist->Clear();
      
      int eventIDStepSize = sTree->GetEntries()/scaleDownTo ; // restrict number of point be around 200 to 300
      int eventjStepSize = 2;
      
      if( eventIDStepSize == 0 ) eventIDStepSize = 1;
      if( eventjStepSize == 0 ) eventjStepSize = 1;
      
      TRandom * r1 = new TRandom();
      TDatime time;
      r1->SetSeed(time.GetTime());

      //calculate number of event will be used.
      int countEvent = 0;
      for( int eventID = 0 ; eventID < sTree->GetEntries(); eventID += eventIDStepSize ){
        sTree->GetEntry(eventID);
        countEvent++;
      }

      int countMax = 0; 
      printf("======================= find fit by Monle Carlo. #Trial: %d, #event: %d\n", nTrial, countEvent);
      for( int iTrial = 0; iTrial < nTrial; iTrial ++){ 
         
         //TODO better method, not pure random
         double a1;
         double a0[jDet];
         
         a1 = a1Range[0] + (a1Range[1] - a1Range[0])*r1->Rndm();
         printf("%3d | %5.1f, ", iTrial, a1);
         
         for( int j = 0;j < jDet; j++){
           a0[j] = a0Range[0] + (a0Range[1] - a0Range[0])*r1->Rndm();
           if( j < jDet - 1) printf("%6.2f, ", a0[j]);
         }
         printf("%6.2f | ", a0[jDet-1]);

         double totalMinDist    = 0.;
         int count = 0;
         for( int eventID = 0 ; eventID < sTree->GetEntries(); eventID += eventIDStepSize ){
            sTree->GetEntry(eventID);
            
            double minDist = 99;
            double eC, xC;
            //convert detIDS to j = 0,1,2,.. jDet
            int jdetIDS = (detIDS - det)/iDet;

            //printf("==================== %d| %f, %f \n", eventID, eS/a1 - a0, xS);
            for( int eventj = 0; eventj < rTree->GetEntries(); eventj += eventjStepSize){
               rTree->GetEntry(eventj);
               if( detID != idet%6 ) continue;
               if( loop  != 1 ) continue;
               if( hit   != 1 ) continue;
               if( ExID  >  ExIDMax ) continue;
               
               //calculate dist
               double tempDist = TMath::Power(xS - xR,2) + TMath::Power( (eS/a1 + a0[jdetIDS]) - eR,2);
               if( tempDist < minDist ) {
                  minDist = tempDist;
                  eC = eR;
                  xC = xR;
               }
               //if( eventj%100 == 0) printf("%d, %8.4f, %8.4f| %8.4f < %8.4f \n", eventj, eR, xR, tempDist, minDist);               
                       
            }
            if( minDist < minDistThreshold ) {
               count ++;  
               totalMinDist += minDist;
               //printf("%8.4f, %8.4f|%8.4f, %8.4f|%8.4f \n", eS/a1+a0, xS, eC, xC, minDist);       
            }else{
               totalMinDist += minDistThreshold;
            }
         }
         
         if( count == 0 ) totalMinDist = minTotalMinDist + 0.2; // to avoid no fill
         
         gDist->SetPoint(iTrial, a1, a0[0], totalMinDist);
         
         printf("%7.3f < %6.3f (%3d, %3d) ", totalMinDist, minTotalMinDist, count, countMax);   

         //======== time
         clock.Stop("timer");
         Double_t time = clock.GetRealTime("timer");
         clock.Start("timer");
         printf( "|%5.2f min| ", time/60.);
         
         if( totalMinDist < minTotalMinDist && count > countEvent/4.  && count >= countMax) {
            countMax = count; // next best fit must have more data points
            minTotalMinDist = totalMinDist;
            A1 = a1;
            printf("%5.1f, ", A1);
            for( int j = 0; j < jDet; j++){
              A0[j] = a0[j];
              if( j < jDet-1 ) printf("%5.3f, ", A0[j]);
            } 
            printf("%5.3f\n",A0[jDet-1]);
         }else{
            printf("\n");
         }
            
      }// end of loop
      
      //After founded the best fit, plot the result
            
      for( int j = 0; j < jDet; j++){
        int i = j*iDet + det ;
        B1[i] = A1;
        B0[i] = A0[j];
        printf("==========> %2d: (B1, B0):(%f\t%f)\n",i,  B1[i], B0[i]);
      } 
      
      exPlotC->Reset();
      for( int eventID = 0 ; eventID < sTree->GetEntries(); eventID ++ ){
         sTree->GetEntry(eventID);
         exPlotC->Fill(xS, eS/B1[detIDS] + B0[detIDS]);
      }
      
      if( det == -1 ){
         cScript->cd(1);
         exPlot->Reset();
         exPlot->SetTitle(title + "(exp)");
         for( int i = 0; i < sTree->GetEntries() ; i++){
            sTree->GetEntry(i);
            exPlot->Fill(xS, eS);
         }
         exPlot->Draw();
         cScript->Update();
         
         cScript->cd(2);
         exPlotR->Reset();
         exPlotR->SetTitle(title + "(sim)");
         for( int i = 0; i < rTree->GetEntries() ; i++){
            rTree->GetEntry(i);
            if( detID != idet%6 ) continue;
            if( loop  != 1 ) continue;
            if( hit   != 1 ) continue;
            if( ExID  >  ExIDMax ) continue;

            exPlotR->Fill(xR, eR);
         }
         exPlotR->Draw("colz");            
         cScript->Update();
         
      }
      
      cScript->cd(3);
      exPlotC->Draw("");
      
      //cScript->cd(3);
      //gDist->Draw("tri1");
      
      //cScript->cd(3)->SetTheta(90);
      //cScript->cd(3)->SetPhi(0);
    
      cScript->Update();
   
   } // end of loop idet  

/**///======================================================== save result
      
   if( det < 0 ){
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

/**///========= end of program
}
