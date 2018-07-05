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

void Cali_physical(TTree *tree){
/**///======================================================== initial input
   
   double eRange[3] = {200, -3500, -500};
   
   vector<double> knownE;
   knownE.push_back(0);
   knownE.push_back(1567.1);
   knownE.push_back(2032.2);
   knownE.push_back(2500.0);
   
/**///========================================================  load tree

   printf("=====>  Total #Entry: %10d \n", tree->GetEntries());
   
/**///======================================================== Browser or Canvas

   Int_t Div[2] = {1,2};  //x,y
   Int_t size[2] = {800,400}; //x,y
   
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

   
/**///========================================================= Analysis
   
   double tcut[24][2];
   tcut[0][0] = -200 ; tcut[0][1] = 400 ;
   tcut[1][0] = -200 ; tcut[1][1] = 400 ;
   tcut[2][0] = -200 ; tcut[2][1] = 400 ;
   //tcut[3][0] = -100 ; tcut[3][1] = 400 ;
   tcut[3][0] = -600 ; tcut[3][1] = 400 ;
   tcut[4][0] =  100 ; tcut[4][1] = 500 ;
   tcut[5][0] = -400 ; tcut[5][1] = 400 ;
   
   tcut[6][0] = -200 ; tcut[6][1] = 400 ;
   tcut[7][0] = -200 ; tcut[7][1] = 400 ;
   tcut[8][0] = -200 ; tcut[8][1] = 400 ;
   //tcut[9][0] =    0 ; tcut[9][1] = 500 ;
   tcut[9][0] =  -500 ; tcut[9][1] = 500 ;
   tcut[10][0] =  100 ; tcut[10][1] = 500 ;
   tcut[11][0] = -200 ; tcut[11][1] = -200 ;
   
   tcut[12][0] = -300 ; tcut[12][1] = 400 ;
   tcut[13][0] = -300 ; tcut[13][1] = 400 ;
   tcut[14][0] = -200 ; tcut[14][1] = 400 ;
   tcut[15][0] = -200 ; tcut[15][1] = 400 ;
   tcut[16][0] =  100 ; tcut[16][1] = 500 ;
   tcut[17][0] =  100 ; tcut[17][1] = 800 ;
   
   tcut[18][0] = -200 ; tcut[18][1] = 400 ;
   tcut[19][0] = -300 ; tcut[19][1] = 400 ;
   tcut[20][0] = -200 ; tcut[20][1] = 600 ;
   //tcut[21][0] =   50 ; tcut[21][1] = 600 ;
   tcut[21][0] = -300 ; tcut[21][1] = 600 ;
   tcut[22][0] =   50 ; tcut[22][1] = 500 ;
   tcut[23][0] =  200 ; tcut[23][1] = 700 ;
   
   
   //=================== plot aligned energy   
   printf("========== plotting final spectrum \n");
   
   TString name, expression, gate;
   
   TH1F * spec = new TH1F("spec", "specG" , eRange[0], eRange[1], eRange[2]);
   spec->SetXTitle("Ex [a.u.]");
   
   tree->Draw("energy >> spec", "hitID == 0 && 5 > detID%6 && detID%6 > 0 ");
   
   //================== find peak
   
   TSpectrum * specPeak = new TSpectrum(20);
   int nPeaks = specPeak->Search(spec, 1 ,"", 0.05);
   float * xpos = specPeak->GetPositionX();
   
   int * inX = new int[nPeaks];
   TMath::Sort(nPeaks, xpos, inX, 0 );  
   vector<double> energy;   
   for( int j = 0; j < nPeaks; j++){
      printf(" %d , x: %8.3f \n", j, xpos[inX[j]]);
      //energy.push_back(xpos[inX[j]]-xpos[inX[0]]);
      energy.push_back(xpos[inX[j]]);
   }
   
   //=============== convert to real energy 
   int numPeak = knownE.size();
   TCanvas * cGa = new TCanvas("cGa", "cGa", 1000, 0, 500, 500);
   cGa->cd(1);
   TGraph * ga = new TGraph(numPeak, &energy[0], &knownE[0] );
   TF1 * fit = new TF1("fit", "pol1");
   ga->Draw("*ap");
   ga->Fit("fit", "");
   double eC0 = fit->GetParameter(0);
   double eC1 = fit->GetParameter(1);
   printf("====  eC0:%8.3f, eC1:%8.3f \n", eC0, eC1);
   
   vector<double> realEnergy;
   for( int j = 0; j < nPeaks; j++){
      realEnergy.push_back(energy[j] * eC1 + eC0);
      printf(" %d , e: %8.3f \n", j, realEnergy[j]);
   }
   
   //=================== plot corrected Ex
   cScript->cd(2);
   TH1F * z = new TH1F("z", "z" , 500, -500, 10000);
   z->SetXTitle("Ex [keV]");
   
   expression.Form("energy*%f + %f >> + z", eC1, eC0);
   tree->Draw(expression, "hitID == 0 && 5 > detID%6 && detID%6 > 0 ");

   //====== draw   
   cScript->cd(1);
   spec->Draw();
   
   cScript->cd(2);
   z->Draw();
   
   cScript->Update();
   cGa->Update();
   //===== save correction parameter   
   printf("0 for end, 1 for saving parameters: ");
   int dummy;
   scanf("%d", &dummy);
   if( dummy == 0 ) return;
   if( dummy == 1 ){
      FILE * paraOut;
      TString filename;
      filename.Form("correction_e_to_Ex.dat");
      paraOut = fopen (filename.Data(), "w+");
      
      printf("=========== save parameters to %s \n", filename.Data());
      fprintf(paraOut, "%9.6f  %9.6f\n", eC0, eC1);
      
      fflush(paraOut);
      fclose(paraOut);
   }
   
   
}
