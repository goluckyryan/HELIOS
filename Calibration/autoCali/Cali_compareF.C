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

//use the fx in refTree, use fx->Eval(x) as e

void Cali_compareF(TTree *expTree, TFile *refFile, int option = -1){
/**///======================================================== User Input

   double a1Range[2] = {250, 320};
   double a0Range[2] = {-0.7, 0.7}; // the energy gap
   
   double eThreshold = 300;
   double distThreshold   = 0.01;

   int hitMode = 1; // 0 = only both xn, xf valid, 1 = any
   
   int ExIDMax = 3;
   
   int nTrial = 1000;
   
   
   printf("======================== \n");
   printf("distant Threshold : %f \n", distThreshold);
   
/**///======================================================== display tree
   int totnumEntry = expTree->GetEntries();
   
   printf("============== Total #Entry: %10lld \n", expTree->GetEntries());
   
/**///======================================================== histogram
   
   TH2F * exPlot  = new TH2F("exPlot" , "exPlot" , 200, -1.3, 1.3, 200, 0, 2500);
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
   
   int numFx = 4;
   TGraph ** fx = new TGraph *[numFx];
   TString fxName;
   for( int i = 0; i < numFx ; i++){
      fxName.Form("fx%d", i);
      fx[i] = (TGraph*) refFile->FindObjectAny(fxName);
   }
   
/**///======================================================== Extract tree entry, create new smaller trees, use that tree to speed up
   double B1 [numDet]; // best a1 of rDet
   double B0 [numDet]; // best a0 of rDet

   TBenchmark clock;  
   
   int startDet = 0;
   if(option >= 0 ) {
      startDet = option;
      numDet = option + 1;
   }
   
   for( int idet = startDet; idet < numDet; idet ++){
      
      double iDet = idet%rDet;
      double zRange[2];
      if( firstPos > 0 ){
         zRange[0] = pos[iDet] - 10;
         zRange[1] = pos[iDet] + length +10;
      }else{
         zRange[0] = pos[iDet] - length -10;
         zRange[1] = pos[iDet] + 10;
      }
      TH2F * exPlotC = new TH2F("exPlotC", "exPlotC", 200, zRange[0], zRange[1], 200, 0, 10);
      
      bool shown = false; clock.Reset(); clock.Start("timer");
      TString title; title.Form("detID-%d", idet);      
      printf("========== creating a smaller trees for detID = %d \n", idet);
         
      /**///========================================= TODO is save as an array faster?
      
      TFile * tempF1 = new TFile("temp.root", "recreate");
      TTree * tempTree = new TTree("tree", "tree");
      
      double eTemp, xTemp;
      tempTree->Branch("e", &eTemp, "eTemp/D");
      tempTree->Branch("x", &xTemp, "xTemp/D");

      for( int eventT = 0 ; eventT < totnumEntry; eventT ++ ){
         expTree->GetEntry(eventT);
         if( e[idet] <  eThreshold ) continue;
         if( hitMode == 0 && ( xf[idet] == 0 || xn[idet] == 0) ) continue;
         if( hitMode == 1 && ( xf[idet] == 0 && xn[idet] == 0) ) continue;
         
         eTemp = e[idet];
         
         double xfC = xf[idet] * xfxneCorr[idet][1] + xfxneCorr[idet][0] ;
         double xnC = xn[idet] * xnCorr[idet] * xfxneCorr[idet][1] + xfxneCorr[idet][0];
         
         if( hitMode == 0 ) xTemp = (xfC - xnC)/(xfC + xnC);
         
         if( hitMode == 1 ){
            if( xf[idet] > 0 && xn[idet] > 0 ){
               xTemp = (xfC - xnC)/(xfC+xnC);
            }else if(xf[idet] == 0 && xn[idet] > 0 ){
               xTemp = (1-2*xnC/e[idet]);
            }else if(xf[idet] > 0 && xn[idet] == 0 ){
               xTemp = (2*xfC/e[idet]-1);
            }else{
               xTemp = TMath::QuietNaN();
            }
         }
         
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
      printf("=================== saved event : %d \n", totalEventNum); 
      tempF1->Close();
      
   /**///======================================================== open the tree and plot
      
      const char* tempfile="temp.root";
      TFile *f0 = new TFile (tempfile, "read"); 
      TTree * smallTree = (TTree*)f0->Get("tree"); // s for seleced
      //printf("========== number of event : %lld \n", smallTree->GetEntries()); 
      
      double  eS; TBranch * b_eS;   //!
      double  xS; TBranch * b_xS;   //!

      smallTree->SetBranchAddress("e", &eS, &b_eS);
      smallTree->SetBranchAddress("x", &xS, &b_xS);
      
      if( option >= 0 ){
         cScript->cd(1);
         exPlot->Reset();
         exPlot->SetTitle(title + "(exp)");
         for( int i = 0; i < smallTree->GetEntries() ; i++){
            smallTree->GetEntry(i);
            exPlot->Fill(xS, eS);
         }
         exPlot->Draw();
         cScript->Update();
         
         cScript->cd(2);
         fx[0]->Draw();
         fx[0]->GetYaxis()->SetRangeUser(0, 10);
         fx[0]->GetXaxis()->SetRangeUser(zRange[0], zRange[1] );
         for( int i = 1; i < numFx; i++){
            fx[i]->Draw("same");
         }         
         cScript->Update();
         
      }
   /**///======================================================== Get tree entry

      clock.Reset(); clock.Start("timer");
      
      double A0 = 0.;
      double A1 = 200.;
      
      double minTotalMinDist = 99.;
      
      gDist->SetTitle("Total min-dist; a1; a0; min-dist");
      gDist->Clear();
      
      TRandom * ranGen = new TRandom();
      TDatime time;
      ranGen->SetSeed(time.GetTime());
      
      //calculate number of event will be used.
      int countEvent = 0;
      for( int eventS = 0 ; eventS < smallTree->GetEntries(); eventS ++ ){
        smallTree->GetEntry(eventS);
        countEvent++;
      }
      
      int countMax = 0;
      printf("======================= find fit by Monle Carlo. #Point: %d, #event: %d\n", nTrial, countEvent);
      for( int iTrial = 0; iTrial < nTrial; iTrial ++){ 
         
         //TODO better method, not pure random
         double a1, a0;
         a1 = a1Range[0] + (a1Range[1] - a1Range[0])*ranGen->Rndm();
         a0 = a0Range[0] + (a0Range[1] - a0Range[0])*ranGen->Rndm();
         
         //printf("%3d | %5.1f, %6.2f | ", iTrial, a1, a0);
         
         double totalMinDist    = 0.;
         int count = 0; 
         for( int eventS = 0 ; eventS < smallTree->GetEntries(); eventS ++ ){
            smallTree->GetEntry(eventS);
            double minDist = 99;
            double eC, xC;
            //printf("==================== %d| %f, %f \n", eventS, eS/a1 - a0, xS);
            for( int i = 0; i < numFx; i++){

               double zR;
               
               if( firstPos < 0 ){
                  zR = pos[iDet] - (-xS + 1.)*length/2 ; 
               }else{
                  zR = pos[iDet] + (xS + 1.)*length/2 ;
               }
               
               double eR = fx[i]->Eval(zR);               
               
               //calculate dist
               double tempDist = TMath::Power( (eS/a1 + a0) - eR,2);
               if( tempDist < minDist ) {
                  minDist = tempDist;
                  eC = eR;
                  xC = xS;
               }
               //if( eventS%100 == 0) printf("%d, %d, %8.4f, %8.4f| %8.4f < %8.4f \n", eventS, i, eR, eS/a1+a0, tempDist, minDist);               
                       
            }
            if( minDist < distThreshold ) {
               count ++;  
               totalMinDist += minDist;
               //printf("%8.4f, %8.4f|%8.4f, %8.4f|%8.4f \n", eS/a1+a0, xS, eC, xC, minDist);       
            }else{
               totalMinDist += distThreshold;
            }
         }
         
         if( count == 0 ) totalMinDist = minTotalMinDist + 0.2; // to avoid no fill
         
         gDist->SetPoint(iTrial, a1, a0, totalMinDist);
         
         
         if( totalMinDist < minTotalMinDist && count >= countMax ) {
            countMax = count; // next best fit must have more data points
            minTotalMinDist = totalMinDist;
            A0 = a0;
            A1 = a1;
            
            //======== time
            clock.Stop("timer");
            Double_t time = clock.GetRealTime("timer");
            clock.Start("timer");
            //display
            printf("%d | %7.3f < %6.3f [%3d, %3d(%2.0f%%)] ", iTrial, totalMinDist, minTotalMinDist, count, countMax, countMax*100./countEvent);   
            printf( "|%5.2f min| ", time/60.);
            printf("%5.1f, %5.3f \n", A1, A0);
            
            if( option >= 0 ){
               exPlotC->Reset();
               for( int eventS = 0 ; eventS < smallTree->GetEntries(); eventS ++ ){
                  smallTree->GetEntry(eventS);
                  exPlotC->Fill(xS, eS/A1 + A0);
               }
               
               cScript->cd(2);
               exPlotC->Draw("same");
               cScript->Update();      
            }            

         }
            
      }// end of loop
      
      //After founded the best fit, plot the result
      printf("==========> (A1, A0):(%f\t%f) \n", A1, A0);
      B1[idet] = A1;
      B0[idet] = A0;

      if( option == -1 ){
         cScript->cd(1);
         exPlot->Reset();
         exPlot->SetTitle(title + "(exp)");
         for( int i = 0; i < smallTree->GetEntries() ; i++){
            smallTree->GetEntry(i);
            exPlot->Fill(xS, eS);
         }
         exPlot->Draw();
         cScript->Update();
         
         cScript->cd(2);
         fx[0]->Draw();
         fx[0]->GetYaxis()->SetRangeUser(0, 10);
         fx[0]->GetXaxis()->SetRangeUser(zRange[0], zRange[1] );
         for( int i = 1; i < numFx; i++){
            fx[i]->Draw("same");
         }  
         cScript->Update();
         
      }

      cScript->cd(2);      
      exPlotC->Reset();

      for( int eventS = 0 ; eventS < smallTree->GetEntries(); eventS ++ ){
         smallTree->GetEntry(eventS);
         double zS;
         
         if( pos[iDet] < 0 ){
            zS = pos[iDet] - (-xS + 1.)*length/2 ; 
         }else{
            zS = pos[iDet] + (xS + 1.)*length/2 ; 
         }
         exPlotC->Fill(zS, eS/A1 + A0);
      } 
      exPlotC->Draw("same");
      cScript->cd(3);
      gDist->Draw("tri1");
      
      cScript->cd(3)->SetTheta(90);
      cScript->cd(3)->SetPhi(0);
      cScript->Update();
   
   } // end of loop idet  

/**///======================================================== save result
   
   if( option < 0 ){
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
}
