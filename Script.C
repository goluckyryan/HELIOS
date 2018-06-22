//====================== this script is for specific analysis
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


int nPeaks = 16;

Double_t fpeaks(Double_t *x, Double_t *par) {
   Double_t result = 0;
   for (Int_t p=0;p<nPeaks;p++) {
      Double_t norm  = par[3*p+0];
      Double_t mean  = par[3*p+1];
      Double_t sigma = par[3*p+2];
      result += norm * TMath::Gaus(x[0],mean,sigma, 1);
   }
   return result;
}

void Script(){   

/**///======================================================== initial input
   
   const char* rootfile="C_gen_run11.root"; const char* treeName="tree";
   
   string detGeoFileName = "/home/ttang/ANALYSIS/H060_ana/detectorGeo_upstream.txt";
   bool isLoadDetGeo = true;
   
   TString gate;


   
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
   double Bfield, bore;
   double a, w, length, support, firstPos;
   double posRecoil, rhoRecoil;
   vector<double> pos;
   int iDet, jDet;
   
   if( isLoadDetGeo ){
      printf("----- loading detector geometery : %s.", detGeoFileName.c_str());
      ifstream file;
      file.open(detGeoFileName.c_str());
      string line;
      int i = 0;
      if( file.is_open() ){
         string x;
         while( file >> x){
            //printf("%d, %s \n", i,  x.c_str());
            if( x.substr(0,2) == "//" )  continue;
            
            if( i == 0 ) Bfield = atof(x.c_str());
            if( i == 1 ) bore = atof(x.c_str());
            if( i == 2 ) a    = atof(x.c_str());
            if( i == 3 ) w    = atof(x.c_str());
            if( i == 4 ) posRecoil = atof(x.c_str());
            if( i == 5 ) rhoRecoil   = atof(x.c_str());
            if( i == 6 ) length   = atof(x.c_str());
            if( i == 7 ) support = atof(x.c_str());
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
         
         printf("========== B-field: %6.2f T \n", Bfield);
         printf("========== Recoil detector: %6.2f mm, radius: %6.2f mm \n", posRecoil, rhoRecoil);
         printf("========== gap of multi-loop: %6.2f mm \n", firstPos > 0 ? firstPos - support : firstPos + support );
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
          
      }
   }
   
/**///========================================================= Analysis
   
   //tree->Draw("eventID/1000.>>k(100, 3100, 3400)"," -5300 < ddt && ddt < -4000 && 6 <= run && run <= 10", "colz" );
   //tree->Draw("eventID/1000.>>h(100, 3100, 3400)"," -1 < Ex && Ex < 1 && 6 <= run && run <= 10", "colz" );
   
   int detID = 3;
   
   int eRange[2] = {600,1800};
   
   double xrange[2];
   if( firstPos > 0 ){
      xrange[0] = pos[detID] - 2;
      xrange[1] = pos[detID] + length + 2;   
   }else{
      xrange[0] = pos[detID] - length - 2;
      xrange[1] = pos[detID] + 2;
   }
   
   i = 0;
   
//############################################################################
   cScript->cd(1);
   TString name = "b1";
   b1 = new TH2F(name, name , 200, xrange[0], xrange[1], 200, eRange[0], eRange[1]);
   b1->SetXTitle("z");
   b1->SetYTitle("e");
   
   TString expression;
   expression.Form("e[%d]:z[%d]>> b1" , detID + iDet*i, detID + iDet*i);
   
   //printf("%s \n", expression.Data());
   
   cScript->cd(i+1);
   
   gate.Form("hitID >= 0");
   
   tree->Draw(expression, gate , "");
   
   
   //== gate on 1/4 and 3/4 of x
   
   gate.Form("hitID >= 0 && -1 < x[%d] && x[%d] < -0.5", detID + iDet*i,detID + iDet*i );
   name = "b2";
   TH2F* b2 = new TH2F(name, name , 200, xrange[0], xrange[1], 100, eRange[0], eRange[1]);
   expression.Form("e[%d]:z[%d]>> b2" , detID + iDet*i, detID + iDet*i);
   tree->Draw(expression, gate, "same colz");
   
   gate.Form("hitID >= 0 && 0.5 < x[%d] && x[%d] < 1", detID + iDet*i,detID + iDet*i );
   name = "b3";
   TH2F* b3 = new TH2F(name, name , 200, xrange[0], xrange[1], 100, eRange[0], eRange[1]);
   expression.Form("e[%d]:z[%d]>> b3" , detID + iDet*i, detID + iDet*i);
   tree->Draw(expression, gate, "same colz");

   //== projection
   cScript->cd(2);
   gate.Form("hitID >= 0 && -1 < x[%d] && x[%d] < -0.5", detID + iDet*i,detID + iDet*i );
   name = "p2";
   
   TH1F* p2 = new TH1F(name, name , 80, eRange[0], eRange[1]);
   p2->SetLineColor(2);
   expression.Form("e[%d]>> p2" , detID + iDet*i);
   tree->Draw(expression, gate);
   
   
   cScript->cd(4);
   gate.Form("hitID >= 0 && 0.5 < x[%d] && x[%d] < 1", detID + iDet*i,detID + iDet*i );
   name = "p3";
   TH1F* p3 = new TH1F(name, name , 80, eRange[0], eRange[1]);
   p3->SetLineColor(4);
   expression.Form("e[%d]>> p3" , detID + iDet*i);
   tree->Draw(expression, gate, "");
   
   
   vector<double> peak2;
   TSpectrum * spec2 = new TSpectrum(6);
   int n2 = spec2->Search(p2, 1, "", 0.2);
   float * xpos2 = spec2->GetPositionX();
   {
      int * index = new int[n2];
      TMath::Sort(n2, xpos2, index, 0 );  
         for( int j = 0; j < n2; j++){
            peak2.push_back(xpos2[index[j]]);
      }
   }
   
   
   vector<double> peak3;
   TSpectrum * spec3 = new TSpectrum(6);
   int n3 = spec3->Search(p3, 1, "", 0.2);
   float * xpos3 = spec3->GetPositionX();
   {
      int * index = new int[n3];
      TMath::Sort(n3, xpos3, index, 0 );  
         for( int j = 0; j < n3; j++){
            peak3.push_back(xpos3[index[j]]);
      }
   }
   
   //=============== 
   
   
   
   
   
   
//############################################################################   
   /*
   
   TH2F ** b = new TH2F*[jDet];
   for( int i = 0; i < jDet; i ++){
      TString name;
      name.Form("b%d", i);
      b[i] = new TH2F(name, name , 200, xrange[0], xrange[1], 200, eRange[0], eRange[1]);
      b[i]->SetXTitle("z");
      b[i]->SetYTitle("e");
      
      TString expression;
      expression.Form("e[%d]:z[%d]>> b%d" , detID + iDet*i, detID + iDet*i ,i);
      
      //printf("%s \n", expression.Data());
      
      cScript->cd(i+1);
      
      tree->Draw(expression, gate , "colz");
   }
   
   /*

   double slope = 4.0;
   
   
   
   double eCRange[3];
   eCRange[0] = 100;
   eCRange[1] = eRange[0] - slope * pos[detID];
   eCRange[2] = eRange[1] - slope * pos[detID];
   
   
   TH1F ** b = new TH1F*[jDet];
   for( int i = 0; i < jDet; i ++){
      TString name;
      name.Form("b%d", i);
      b[i] = new TH1F(name, name , eCRange[0], eCRange[1], eCRange[2]);
      b[i]->SetXTitle("eC");
      
      TString expression;
      expression.Form("e[%d] - %f * z[%d] >> b%d" , detID + iDet*i, slope ,detID + iDet*i, i);
      
      //printf("%s \n", expression.Data());
      
      cScript->cd(i+1);
      
      tree->Draw(expression, gate , "");
   }
   
   //==== find peak and fit, get average sigma, change slope, get minumum 
   
   for( int i = 0; i < 1; i ++){
      cScript->cd(i+1);
      TSpectrum * spec = new TSpectrum(10);
      int nPeaks  = spec->Search(b[i], 1, "", 0.1);
      
      float * xpos = spec->GetPositionX();
      float * ypos = spec->GetPositionY();
      
      int n = 3 * nPeaks;
      double * para = new double[n]; 
      for(int k = 0; k < nPeaks ; k++){
         printf(" x: %f, y: %f \n", xpos[k], ypos[k]);
         para[3*k+0] = ypos[k]* TMath::Sqrt(TMath::TwoPi());
         para[3*k+1] = xpos[k];
         para[3*k+2] = 100.;
      }
      
      TF1 * fit = new TF1("fit", fpeaks, eCRange[1], eCRange[2], n );
      fit->SetNpx(1000);
      fit->SetParameters(para);
      b[i]->Fit("fit", "");
      
      Double_t* paraA = fit->GetParameters();
      
      for(int k = 0; k < nPeaks ; k++){
         printf("%2d , mag: %8.4f mean: %8.4f, sigma: %8.4f \n", 
                 k,
                 paraA[3*k+0], 
                 paraA[3*k+1],
                 paraA[3*k+2]);
      }
   }
   
   
   /***/
}

