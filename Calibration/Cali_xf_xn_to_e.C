#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TProfile.h>
#include <TH2F.h>
#include <TH1F.h>
#include <TF1.h>
#include <TMath.h>
#include <TSpectrum.h>
#include <TGraph.h>
#include <fstream>

void Cali_xf_xn_to_e(TTree *tree){
   gStyle->SetOptStat(11111111);
   
   printf("=========== Total #Entry: %10d \n", tree->GetEntries());

/**///======================================================== Browser or Canvas

   Int_t Div[2] = {6,4};  //x,y
   Int_t size[2] = {230,230}; //x,y
   TCanvas * cCali_xf_xn_e = new TCanvas("cCali_xf_xn_e", "cCali_xf_xn_e", 0, 0, size[0]*Div[0], size[1]*Div[1]);
   cCali_xf_xn_e->Divide(Div[0],Div[1]);
   
   for( int i = 1; i <= Div[0]*Div[1] ; i++){
      cCali_xf_xn_e->cd(i)->SetGrid();
   }
   
   gStyle->SetOptStat(1111111);
   gStyle->SetStatY(1.0);
   gStyle->SetStatX(0.99);
   gStyle->SetStatW(0.2);
   gStyle->SetStatH(0.1);
   
/**///========================================================= Analysis

   //======== create histogram for each detector
   printf("creating xf-xn histogram for each detector.... please wait.\n"); 
   const int nDet = 24;
   TH2F ** d = new TH2F*[nDet];
   for( int i = 0; i < nDet; i ++){
      TString name;
      name.Form("d%d", i);
      d[i] = new TH2F(name, name , 300, -100 , 3500 , 300, -100 , 3500);
      d[i]->SetXTitle("xf+xn");
      d[i]->SetYTitle("e");
      
      TString expression;
      expression.Form("e[%d]:(xf[%d]+xn[%d])>> d%d" , i,i,i,i);
      TString gate;
      gate.Form("e[%d]!=0 && xf[%d]!=0 && xn[%d]!=0",i,i,i);
      //gate.Form("xf[%d]!=0 && xn[%d]!=0",i,i);
      
      cCali_xf_xn_e->cd(i+1);
      tree->Draw(expression, gate , "");
      cCali_xf_xn_e->Update();  
   }
   
   //======== profileX
   printf("fitting slope.\n");
   Double_t* slope = new Double_t[nDet];
   Double_t* intep = new Double_t[nDet];
   
   TF1 *fit = new TF1("fit", "pol1"); 
      
   for( int i = 0; i < nDet; i ++){
   
      cCali_xf_xn_e->cd(i+1);
      d[i]->Fit("fit", "q");
      
      slope[i] = fit->GetParameter(1);
      intep[i] = fit->GetParameter(0);
      
   }
   
   cCali_xf_xn_e->Update();
   //===== save correction parameter
   int dummy = 0;
   printf("0 for end, 1 for save e-xf+xn correction: ");
   scanf("%d", &dummy);
   if( dummy == 0 ) return;
   if( dummy == 1 ){
      
      FILE * paraOut;
      paraOut = fopen ("correction_xfxn_e.dat", "w+");
      
      for( int i = 0; i < nDet; i++){
         printf("%2d, %9.6f, %9.6f \n", i, intep[i], slope[i]);
         fprintf(paraOut, "%9.6f  %9.6f\n", intep[i], slope[i]);
      }
   
      fflush(paraOut);
      fclose(paraOut);
      
      printf("=========== save xfxn-e-correction parameters to %s \n", "correction_xfxn_e.dat");
      
      //cCali_xf_xn_e->SaveAs("xfxn_e_correction.pdf");
      
      printf("=========== save canvas to %s \n", "xfxn_e.pdf");
      
   }
   /*
   // make correction
   TH2F ** dc = new TH2F[nDet];
   for( int i = 0; i < nDet; i ++){
      TString name;
      name.Form("dc%d", i);
      dc[i] = new TH2F(name, name , 300, -100 , 3500 , 300, -100 , 3500);
      dc[i]->SetXTitle("xf+xn");
      dc[i]->SetYTitle("e");
      
      TString expression;
      expression.Form("e[%d]:%f*(xf[%d]+xn[%d])+%f>> dc%d" , i, slope[i] , i,i, intep[i], i);
      TString gate;
      gate.Form("e[%d]!=0 && xf[%d]!=0 && xn[%d]!=0",i,i,i);
      //gate.Form("xf[%d]!=0 && xn[%d]!=0",i,i);
      tree->Draw(expression, gate , "");
      
      //printf("========  i = %d \n", i); 
   }
   */

}

