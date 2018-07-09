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

Double_t fsquare(Double_t *x, Double_t *par){
   Double_t result = 0;
   
   if( par[0] < x[0] && x[0] < par[1] ){
      result = par[2]; 
   }else{
      result = 0;
   }

   return result;
}

TString expression, gate_sim, gate_exp, gate_det, gate_aux;

void AngularDist(int ExID, double dEx = 0.2, int bin = 80) {   
   
/**///======================================================== initial input
   
   const char* rootfile0="~/ANALYSIS/H060_ana/C_gen_run11.root"; const char* treeName0="tree";
   const char* rootfile1="~/ANALYSIS/Simulation/transfer.root"; const char* treeName1="tree";
   
   double zRange[3] = {400, -500, -100};
   
   //setting for simulation
   double thetaCMLow = 10; //deg
   double dEx_sim = 0.05; // MeV
   int detID[2] = {1, 4}; // start, end detID
   
   double ExRange[3] = {100, -1, 4};
   int nSplit = 3;
   double threshold = 0.1;
   
   bool isIgnoreEdge = true;
    
/**///========================================================  load tree
   
   TFile *f0 = new TFile (rootfile0, "read"); 
   TTree *tree0 = (TTree*)f0->Get(treeName0);
   printf("=====> /// %20s //// is loaded. Total #Entry: %10d \n", rootfile0,  tree0->GetEntries());
   
   TFile *f1 = new TFile (rootfile1, "read"); 
   TTree *tree1 = (TTree*)f1->Get(treeName1);
   printf("=====> /// %20s //// is loaded. Total #Entry: %10d \n", rootfile1,  tree1->GetEntries());
   
/**///======================================================== Browser or Canvas
   
   //TBrowser B ;   
   Int_t Div[2] = {detID[1]-detID[0]+1,2};  //x,y
   Int_t size[2] = {300,300}; //x,y // for each division
   
   TCanvas * cAngularDist = new TCanvas("cAngularDist", "cAngularDist", 0, 0, size[0]*Div[0], size[1]*Div[1]);
   cAngularDist->Divide(Div[0],Div[1]);
   for( int i = 1; i <= Div[0]*Div[1] ; i++){
      cAngularDist->cd(i)->SetGrid();
   }
   cAngularDist->cd(1);
   gStyle->SetOptStat(1111111);
   gStyle->SetStatY(0.8);
   gStyle->SetStatX(0.99);
   gStyle->SetStatW(0.2);
   gStyle->SetStatH(0.1);
   
/**///========================================================= load files

/**///========================================================= Analysis

   TString gate_x;
   
   gate_aux = "&& e > 200 ";
   
   //====== get Ex -z gate from simulation
   TH2F* w = new TH2F("w", "Ex_Z_th", zRange[0], zRange[1], zRange[2], 100, -1, 4);
   gate_sim.Form("loop == 1 && TMath::Abs(thetaCM-%f) < 1", thetaCMLow);         
   tree1->Draw("Ex:z >> w", gate_sim, "colz");
   TF1 * fit = new TF1("fit", "pol1");
   w->Fit("fit", "q");
   double p0 = fit->GetParameter(0);
   double p1 = fit->GetParameter(1);
   
   gate_exp.Form("hitID >= 0 && %f + %f * z > Ex", p0, p1);
   
   //TH2F* k = new TH2F("k", "Ex_Z_exp", bin, zRange[1], zRange[2], 100, -1, 4); 
   //tree0->Draw("Ex:z >> k", gate_exp, "");
   
   TH1F * spec  = new TH1F("spec" , "spec"  , ExRange[0], ExRange[1], ExRange[2]);
   spec ->SetXTitle("Ex [MeV]");   
   
   vector<double> ExAdapted;
   vector<double> countAdapted;
   vector<double> countErrAdapted;
   vector<int> detIDAdapted;
   vector<double> meanAngle;
   ExAdapted.clear();
   countAdapted.clear();
   countErrAdapted.clear();
   detIDAdapted.clear();
   meanAngle.clear();
   vector<double> dCos; 
   dCos.clear(); 
   
   TH1F* h = new TH1F("h", "thetaCM", 200, 0, 50);
   TH2F* h2 = new TH2F("h2", "thetaCMvZ", zRange[0], zRange[1], zRange[2], 500, 0, 50); 

   gate_sim.Form("loop == 1 && ExID == %d && thetaCM > %f && hit == 1", ExID, thetaCMLow);      

   printf("============================== gates:\n");
   printf("gate_sim    : %s\n", gate_sim.Data());
   printf("gate_exp    : %s\n", gate_exp.Data());
   printf("gate_aux    : %s\n", gate_aux.Data());
   printf("threshold   : %f\n", threshold);
   if( isIgnoreEdge ){
      printf("isIgnoreEdge: true \n");
   }else{
      printf("isIgnoreEdge: false \n");
   }
   for( int idet = detID[0]; idet <= detID[1]; idet++){
      
      printf("============================== detID : %d\n", idet);
      gate_det.Form("&& detID%6 == %d", idet);  
      
      for(int iSplit = 0; iSplit < nSplit+1; iSplit ++){
            
         //========define gate_x
         double split[2] = {-1.5, 1.5};
         split[0] = -1. + 2./ (nSplit+1) * (iSplit);
         split[1] = -1. + 2./ (nSplit+1) * (iSplit+1);
 
         if( split[0] < -1.0 + 0.01 ) split[0] = -1.5;
         if( split[1] > +1.0 - 0.01 ) split[1] = +1.5;

         if( isIgnoreEdge && split[0] < -1 || split[1] > 1) continue;

         gate_x.Form("&& %f < x && x < %f", split[0], split[1]);
         
         printf("gate_x      : %s\n", gate_x.Data());
               
         detIDAdapted.push_back(idet);
         
         //########################################################## for simulation data
         cAngularDist->cd(idet);
         tree1->Draw("thetaCM >> h", gate_sim + gate_det + gate_x);
         tree1->Draw("thetaCM : z >> h2", gate_sim + gate_det+gate_x);

         cAngularDist->Update();

         if( h->GetEntries() == 0){
            printf(" no data for detID == %d \n", idet);
            continue;
         }
         //============== find the acceptance, the angle the count drop
         vector<double> angle;
         angle.clear();
         for(int j = 1 ; j < 500; j++){
            int a = h->GetBinContent(j);
            int b = h->GetBinContent(j+1);
            
            if( a == 0 && b > 0) angle.push_back(h->GetBinLowEdge(j+1));
            if( a > 0 && b == 0) angle.push_back(h->GetBinLowEdge(j+1));
         }
         
         int sizeOfAngle = angle.size();
         if( sizeOfAngle > 2){
            angle.erase(angle.begin(), angle.begin()+sizeOfAngle-2);
         }
             
         //============ print angle
         for( int j = 0; j < (int) angle.size()/2; j++){
            double delta = angle[2*j+1] - angle[2*j];
            meanAngle.push_back((angle[2*j+1] + angle[2*j])/2);
            
            //if ( delta < 1. ) continue;
            dCos.push_back(TMath::Sin(meanAngle.back()*TMath::DegToRad())*(delta*TMath::DegToRad()));
            
            printf("%10.5f - %10.5f = %10.5f | %10.5f, %10.5f \n", 
                       angle[2*j], 
                       angle[2*j+1], 
                       delta,
                       meanAngle.back(),
                       1./TMath::Sin(meanAngle.back()*TMath::DegToRad())*(delta*TMath::DegToRad()));
         }
         
         //########################################################## for experimental data
         cAngularDist->cd(4+idet);
         
         //================= plot Ex
         //printf("============= plot Ex with gates\n");    
         tree0->Draw("Ex>>spec ", gate_exp + gate_x + gate_det + gate_aux, "");
         
         spec ->Draw();
         //gPad->WaitPrimitive();
         
         TH1F * specS = (TH1F*) spec->Clone();
         
         //=================== find peak and fit
         //printf("============= estimate background and find peak\n");
         TSpectrum * peak = new TSpectrum(10);
         peak->Search(spec, 1, "", threshold);
         TH1 * h1 = peak->Background(spec,10);
         h1->Draw("same");
         
         TString title;
         specS->SetName("specS");
         specS->Add(h1, -1.);
         specS->Sumw2();
         specS->Draw();
         
         cAngularDist->Update();
         
         //========== Fitting 
         //printf("============= Fit.....");
         nPeaks  = peak->Search(specS, 1, "", threshold);
         printf("========== found %d peaks \n", nPeaks);
         float * xpos = peak->GetPositionX();
         float * ypos = peak->GetPositionY();
         
         //Double_t * xpos = peak->GetPositionX();
         //Double_t * ypos = peak->GetPositionY();
         
         int * inX = new int[nPeaks];
         TMath::Sort(nPeaks, xpos, inX, 0 );
         vector<double> energy, height;
         for( int j = 0; j < nPeaks; j++){
            energy.push_back(xpos[inX[j]]);
            height.push_back(ypos[inX[j]]);
         }
         
         const int  n = 3 * nPeaks;
         double * para = new double[n]; 
         for(int j = 0; j < nPeaks ; j++){
            para[3*j+0] = height[j] * 0.05 * TMath::Sqrt(TMath::TwoPi());
            para[3*j+1] = energy[j];
            para[3*j+2] = 0.05;
         }

         TF1 * fit2 = new TF1("fit2", fpeaks, ExRange[1], ExRange[2], 3* nPeaks );
         fit2->SetNpx(1000);
         fit2->SetParameters(para);
         specS->Fit("fit2", "q");
          
         const Double_t* paraE = fit2->GetParErrors();
         const Double_t* paraA = fit2->GetParameters();
         
         double bw = specS->GetBinWidth(1);
         
         double * ExPos = new double[nPeaks];
         double * count = new double[nPeaks];
         double * countErr = new double[nPeaks];
         for(int j = 0; j < nPeaks ; j++){
            ExPos[j] = paraA[3*j+1];
            count[j] = paraA[3*j] / bw;
            countErr[j] = paraE[3*j] / bw; 
            printf("%2d , count: %8.0f(%3.0f), mean: %8.4f(%8.4f), sigma: %8.4f(%8.4f) \n", 
                    idet, 
                    paraA[3*j] / bw,   paraE[3*j] /bw, 
                    paraA[3*j+1], paraE[3*j+1],
                    paraA[3*j+2], paraE[3*j+2]);
         }
         
         //pick sutable count, using ExID
         //TODO, check Ex is correct, or change to Ex, not ExID
         ExAdapted.push_back(ExPos[ExID]);
         countAdapted.push_back(count[ExID]);
         countErrAdapted.push_back(countErr[ExID]);
      
      }
      
   }
   
   printf("================ summary \n");
   int nDet = (int) detIDAdapted.size();
   double countMax = 0;
   double countMin = 10000;
   for( int idet = 0; idet < nDet; idet++){
      
      if( countAdapted[idet] > countMax) countMax = countAdapted[idet];
      if( countAdapted[idet] < countMin) countMin = countAdapted[idet];
      
      printf("detID : %d, Mean-Angle: %6.2f, Count: %6.0f(%3.0f), Ex-Fit:%6.2f \n", 
               detIDAdapted[idet], 
               meanAngle[idet], countAdapted[idet], countErrAdapted[idet], ExAdapted[idet]); 
      
   }
   
   double yMax = TMath::Power(10,TMath::Ceil(TMath::Log10(countMax)));
   double yMin = TMath::Power(10,TMath::Floor(TMath::Log10(countMin)));
   
   TGraph * g = new TGraph(nDet, &meanAngle[0], &countAdapted[0]);
   g->GetYaxis()->SetRangeUser(yMin, yMax);
   g->GetXaxis()->SetLimits(10,40);
   TCanvas * cG = new TCanvas("cG", "cG", 200, 200, 500, 500);
   cG->SetGrid();
   cG->cd();
   cG->SetLogy();
   g->Draw("A*");
   
   
   //========== if Xsec exist.

}

