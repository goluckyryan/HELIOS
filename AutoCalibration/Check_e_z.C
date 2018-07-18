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
#include <TLegend.h>
#include <fstream>

int nPeaks = 16;
TTree *tree = NULL;

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

void Check_e_z(){  
/**///======================================================== User input
   const char* rootfile="A_H052_Mg25.root"; const char* treeName="tree";
   const char* simfile="transfer.root"; const char* treeNameS="tree";

   int numDet = 24;
   int Div[2] = {6,4};  //x,y
   
   double eRange[3]  = {400, 0, 10};
   double zRange[3]  = {400, -600, -200};
   double ExRange[3] = {200, -1, 8};
   
   int numFx = 6;
   bool showFx = false;
   bool showTx = false;
   TString drawOption ="colz"; 

/**///======================================================== read tree 

   TFile *file0 = new TFile (rootfile, "read"); 
   tree = (TTree*)file0->Get(treeName);
   printf("=====> /// %15s //// is loaded. Total #Entry: %10lld \n", rootfile,  tree->GetEntries());
   
   TFile *file1 = new TFile (simfile, "read"); 
   TTree *sTree = (TTree*)file1->Get(treeNameS);
   printf("=====> /// %15s //// is loaded. Total #Entry: %10lld \n", simfile,  sTree->GetEntries());

   gStyle->SetOptStat(10001);
   gStyle->SetStatY(0.9);
   gStyle->SetStatX(0.9);
   gStyle->SetStatW(0.4);
   gStyle->SetStatH(0.2);
   gStyle->SetLabelSize(0.05, "XY");
   gStyle->SetTitleFontSize(0.1);

/**///======================================================== Analysis

/**///======================================================== e vs x
   Int_t size[2] = {230,230}; //x,y
   TCanvas * cCheck1 = new TCanvas("cCheck1", "cCheck1", 0, 0, size[0]*Div[0], size[1]*Div[1]);
   if(cCheck1->GetShowEditor() )cCheck1->ToggleEditor();
   if(cCheck1->GetShowToolBar() )cCheck1->ToggleToolBar();
   cCheck1->Divide(Div[0],Div[1]);
   for( int i = 1; i <= Div[0]*Div[1] ; i++){
      cCheck1->cd(i)->SetGrid();
   }
   
   TH2F ** hEX = new TH2F*[numDet];
   TString expression, name, gate;
   for( int idet = 0 ; idet < numDet ; idet ++){
      cCheck1->cd(idet+1);  
      name.Form("hEX%02d", idet);
      hEX[idet] = new TH2F(name, name, 400, -1.3, 1.3, eRange[0], eRange[1], eRange[2]);
      expression.Form("e:x>>hEX%02d", idet);
      gate.Form("detID == %d && hitID >= 0", idet);
      tree->Draw(expression, gate);
      cCheck1->Update();
   }
   
/**///======================================================== Ex and fit
   
   TCanvas * cCheck3 = new TCanvas("cCheck3", "cCheck3", 50, 50,  600, 400);
   if(cCheck3->GetShowEditor() )cCheck3->ToggleEditor();
   if(cCheck3->GetShowToolBar() )cCheck3->ToggleToolBar();
   cCheck3->Divide(1,2);
   cCheck3->cd(1)->SetGrid();
   cCheck3->cd(1);
   gStyle->SetOptStat(0);
   gStyle->SetLabelSize(0.05, "XY");
   TString titleH;
   titleH.Form("excitation energy; Ex [MeV]; Count / %3.0f keV", (ExRange[2] - ExRange[1])*1000/ExRange[0]);
   TH1F * hEx = new TH1F("hEx", titleH, ExRange[0], ExRange[1], ExRange[2]);
   hEx->GetXaxis()->SetTitleSize(0.06);
   hEx->GetYaxis()->SetTitleSize(0.06);
   hEx->GetXaxis()->SetTitleOffset(0.7);
   hEx->GetYaxis()->SetTitleOffset(0.6);
   tree->Draw("Ex >> hEx"); 
   
   
   TH1F * specS = (TH1F*) hEx->Clone();
   titleH.Form("fitted spectrum; Ex [MeV]; Count / %3.0f keV", (ExRange[2] - ExRange[1])*1000/ExRange[0]);
   specS->SetTitle(titleH);   
   specS->SetName("specS");
   specS->GetXaxis()->SetTitleSize(0.06);
   specS->GetYaxis()->SetTitleSize(0.06);
   specS->GetXaxis()->SetTitleOffset(0.7);
   specS->GetYaxis()->SetTitleOffset(0.6);
   //=================== find peak and fit
   printf("============= estimate background and find peak\n");
   TSpectrum * peak = new TSpectrum(50);
   nPeaks = peak->Search(hEx, 1, "", 0.1);
   TH1 * h1 = peak->Background(hEx,10);
   h1->Draw("same");
   
   cCheck3->cd(2)->SetGrid();
   cCheck3->cd(2);
   
   specS->Add(h1, -1.);
   specS->Sumw2();
   specS->Draw();
   
   //========== Fitting 
   printf("============= Fit.....");
   printf(" found %d peaks \n", nPeaks);
   float * xpos =  peak->GetPositionX();
   float * ypos =  peak->GetPositionY();
   
   //double * xpos = (double *) peak->GetPositionX();
   //double * ypos = (double *) peak->GetPositionY();
   
   int * inX = new int[nPeaks];
   TMath::Sort(nPeaks, xpos, inX, 0 );
   vector<double> energy, height;
   for( int j = 0; j < nPeaks; j++){
      energy.push_back(xpos[inX[j]]);
      height.push_back(ypos[inX[j]]);
   }
   
   const int  n = 3 * nPeaks;
   double * para = new double[n]; 
   for(int i = 0; i < nPeaks ; i++){
      para[3*i+0] = height[i] * 0.05 * TMath::Sqrt(TMath::TwoPi());
      para[3*i+1] = energy[i];
      para[3*i+2] = 0.08;
   }

   TF1 * fit = new TF1("fit", fpeaks, ExRange[1], ExRange[2], 3* nPeaks );
   fit->SetLineWidth(1);
   fit->SetNpx(1000);
   fit->SetParameters(para);
   specS->Fit("fit", "q");

   printf("============= display\n");   
   const Double_t* paraE = fit->GetParErrors();
   const Double_t* paraA = fit->GetParameters();
   
   double bw = specS->GetBinWidth(1);
   
   double * ExPos = new double[nPeaks];
   double * ExSigma = new double[nPeaks];   
   for(int i = 0; i < nPeaks ; i++){
      ExPos[i] = paraA[3*i+1];
      ExSigma[i] = paraA[3*i+2];
      printf("%2d , count: %8.0f(%3.0f), mean: %8.4f(%8.4f), sigma: %8.4f(%8.4f) \n", 
              i, 
              paraA[3*i] / bw,   paraE[3*i] /bw, 
              paraA[3*i+1], paraE[3*i+1],
              paraA[3*i+2], paraE[3*i+2]);
   }
   cCheck3->Update();

/**///======================================================== e vs z
   gStyle->SetOptStat(11111);
   gStyle->SetStatY(0.9);
   gStyle->SetStatX(0.35);
   gStyle->SetStatW(0.25);
   gStyle->SetStatH(0.10);
   gStyle->SetLabelSize(0.035, "XY");
   gStyle->SetTitleFontSize(0.035);
   
   TCanvas * cCheck2 = new TCanvas("cCheck2", "cCheck2", 700, 50,  600, 400);
   if(cCheck2->GetShowEditor() )cCheck2->ToggleEditor();
   if(cCheck2->GetShowToolBar() )cCheck2->ToggleToolBar();
   cCheck2->SetGrid();
   TH2F * hEZ = new TH2F("hEZ", "e:z; z [mm]; e [MeV]", zRange[0], zRange[1], zRange[2], eRange[0], eRange[1], eRange[2]);
   tree->Draw("e:z >> hEZ", "hitID >= 0 " , drawOption);
   
   if( showFx ) {
      TGraph ** fx = new TGraph*[numFx];
      for( int i = 0 ; i < numFx ; i++){
         TString name;
         name.Form("fx%d", i);
         fx[i] = (TGraph *) file1->FindObjectAny(name);
         if( fx[i] != NULL ){
            fx[i]->Draw("same");
         }else{
            break;
         }
      }
   }
   cCheck2->Update();

/**///======================================================== thetaCM vs z
   TCanvas * cCheck4 = new TCanvas("cCheck4", "cCheck4", 700, 500,  600, 400);
   if(cCheck4->GetShowEditor() )cCheck4->ToggleEditor();
   if(cCheck4->GetShowToolBar() )cCheck4->ToggleToolBar();
   cCheck4->SetGrid();
   TH2F * hThetaZ = new TH2F("hThetaZ", "thetaCM:z; z [mm]; thetaCM [deg]", zRange[0], zRange[1], zRange[2], 400, 0, 50);
   tree->Draw("thetaCM:z >> hThetaZ", "",  drawOption); 
   
   if( showFx ) {
      TGraph ** tx = new TGraph*[numFx];
      for( int i = 0 ; i < numFx ; i++){
         TString name;
         name.Form("tx%d", i);
         tx[i] = (TGraph *) file1->FindObjectAny(name);
         if( tx[i] != NULL ){
            tx[i]->Draw("same");
         }else{
            break;
         }
      }
   }
   cCheck4->Update();

/**///======================================================== Ex vs z 
   gStyle->SetStatY(0.35);
   gStyle->SetStatX(0.9);
   gStyle->SetStatW(0.25);
   gStyle->SetStatH(0.10);
   TCanvas * cCheck5 = new TCanvas("cCheck5", "cCheck5", 50, 500,  600, 400);
   if(cCheck5->GetShowEditor() )cCheck5->ToggleEditor();
   if(cCheck5->GetShowToolBar() )cCheck5->ToggleToolBar();
   cCheck5->SetGrid();
   TH2F * hExZ = new TH2F("hExZ", "z:Ex; Ex [MeV]; z [mm]", ExRange[0], ExRange[1], ExRange[2], zRange[0], zRange[1], zRange[2]);
   tree->Draw("z : Ex >> hExZ", "", drawOption); 
   cCheck5->Update();


/**///======================================================== thetaCM   
   TCanvas * cCheck6 = new TCanvas("cCheck6", "cCheck6", 1300, 50,  600, 400);
   if(cCheck6->GetShowEditor() )cCheck6->ToggleEditor();
   if(cCheck6->GetShowToolBar() )cCheck6->ToggleToolBar();
   cCheck6->SetGrid();
   gStyle->SetOptStat(0);
   
   TH1F ** hTheta = new TH1F *[nPeaks];
   double yMax = 0;
   
   TLegend * legend = new TLegend(0.6,0.1,0.9,0.35);
   
   for(int i = 0; i < nPeaks; i++){
      TString name, title, gate, expression;
      name.Form("hTheta%d", i);
      title.Form("thetaCM +-3Sigma; thetCM [deg]; count / 0.5 deg");
      hTheta[i] = new TH1F(name, title, 100, 0, 50);
      hTheta[i]->SetLineColor(2*i+2);
      expression.Form("thetaCM >> hTheta%d", i);
      gate.Form("%f > Ex && Ex > %f", ExPos[i] + 3*ExSigma[i], ExPos[i] - 3*ExSigma[i] );
      if( i == 0 )tree->Draw(expression, gate);
      if( i > 0 )tree->Draw(expression, gate, "same");
      
      TString lTitle;
      lTitle.Form("Ex = %6.2f MeV, %3.0f keV", ExPos[i], ExSigma[i]*1000.);
      legend->AddEntry(name,lTitle, "l");
   
      if( hTheta[i]->GetMaximum() > yMax ) yMax = hTheta[i]->GetMaximum();
      hTheta[0]->GetYaxis()->SetRangeUser(1, yMax*1.5);
      
      cCheck6->SetLogy();
      cCheck6->Update();
   }
   
   legend->Draw();
   cCheck6->Update();
   
   
   
   
   
}
