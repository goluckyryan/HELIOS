#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TROOT.h>
#include <TObjArray.h>
#include <TStyle.h>
#include <TH2F.h>
#include <TH1F.h>
#include <TF1.h>
#include <TMath.h>
#include <TSpectrum.h>
#include <TGraph.h>
#include <TLegend.h>
#include <TObjArray.h>
#include <fstream>

double xRej[2] = {100, 100};

Double_t fline(Double_t *x, Double_t *par)
{
    if ( xRej[0] < x[0] && x[0] < xRej[1] ) {
      TF1::RejectPoint();
      return 0;
   }
   //return par[0] + par[1]*x[0];
   return par[0]*TMath::Exp(-par[1]*x[0]);
}

void TargetDegrade(){

   const char* rootfile="A_gen_run30.root";
   TFile *f0 = new TFile (rootfile, "read"); 
   TTree * tree = (TTree*)f0->Get("tree");
   
   gStyle->SetOptStat(0);
   
   TCutG * cutC = new TCutG("cutC", 4);
   cutC->SetVarX("tac");
   cutC->SetVarY("-ddt");
   cutC->SetPoint(0, 2100, 3000 );
   cutC->SetPoint(1, 3480, 3000 );
   cutC->SetPoint(2, 3480, 7500 );
   cutC->SetPoint(3, 2100, 7500 );
   cutC->SetPoint(4, 2100, 3000 );
   cutC->SetLineColor(2);
   //cutC->Draw("same");
   
   TCutG * cutDP = new TCutG("cutDP", 4);
   cutDP->SetVarX("tac");
   cutDP->SetVarY("-ddt");
   cutDP->SetPoint(0, 2100,  900 );
   cutDP->SetPoint(1, 3480,  900 );
   cutDP->SetPoint(2, 3480, 1200 );
   cutDP->SetPoint(3, 2100, 1200 );
   cutDP->SetPoint(4, 2100,  900 );
   cutDP->SetLineColor(4);
   //cutDP->Draw("same");
   
   double xRangeP[2] ={2950, 3410};
   
   //for 225ug
   //xRangeP[0] = 2228;
   //xRangeP[1] = 2642;
   
   TCutG * cutD = new TCutG("cutD", 4);
   cutD->SetVarX("tac");
   cutD->SetVarY("-ddt");
   cutD->SetPoint(0, xRangeP[0],  990 );
   cutD->SetPoint(1, xRangeP[1],  990 );
   cutD->SetPoint(2, xRangeP[1], 1100 );
   cutD->SetPoint(3, xRangeP[0], 1100 );
   cutD->SetPoint(4, xRangeP[0],  990 );
   cutD->SetLineColor(6);
   //cutD->Draw("same");
   
   TCanvas * cDegrade = new TCanvas("cDegrade", "cDegarde", 0, 0, 1200, 800);
   cDegrade->Divide(2,2);
   for(int i = 1; i <= 4 ; i++){
		cDegrade->cd(i)->SetGrid();
	}
   
   cDegrade->cd(1);
   TH2F * pid = new TH2F("pid", "pid; tac [ch], ddt [ch]", 500, 2000, 3500, 500, 0, 8000);
   tree->Draw("-ddt:tac >> pid", "6 <= run && run <=14", "colz");
   cutC->Draw("same");
   cutD->Draw("same");
   
   //==============================================================
	cDegrade->cd(2);
	
   double fitRange[2] = {10, 80};
   
	TH1F * kc = new TH1F("kc", "carbon"  , 110, -10, 100);
	TH1F * kd = new TH1F("kd", "deuteron", 110, -10, 100);
		
   tree->Draw("(eventID/1e3 - 3350)*120/218 >> kc", "cutC && 11 <= run && run <= 14"); //167 ug/cm2 
   tree->Draw("(eventID/1e3 - 3350)*120/218 >> kd", "cutD && 11 <= run && run <= 14"); //167 ug/cm2 
   
   //tree->Draw("(eventID/1e3 - 3117)*120/218. >> kc", "cutC && 6 <= run && run <= 10"); //178 ug/cm2
   //tree->Draw("(eventID/1e3 - 3117)*120/218. >> kd", "cutD && 6 <= run && run <= 10"); //178 ug/cm2
   
   //tree->Draw("(eventID/1e3 - 3500)*120/218. >> kc", "cutC && 15 <= run && run <= 17"); //225 ug/cm2
   //tree->Draw("(eventID/1e3 - 3500)*120/218. >> kd", "cutD && 15 <= run && run <= 17"); //225 ug/cm2
   
   kc->SetLineColor(2);
   
   kd->SetXTitle("time [min]");
   kd->SetYTitle("count / 1 min");
   kd->SetTitle("Target Degradation");
   kc->SetXTitle("time [min]");
   
   TF1 * f1 = new TF1("f1", fline, fitRange[0], fitRange[1], 2);
   TF1 * f2 = new TF1("f2", fline, fitRange[0], fitRange[1], 2);
   
   kd->Fit(f1, "0"); // use "R" for fixed range
   kc->Fit(f2, "0");
      
   double p0 = f1->GetParameter(0);
   double p1 = f1->GetParameter(1);
   
   double q0 = f2->GetParameter(0);
   double q1 = f2->GetParameter(1);
   
   kc->Scale(p0/q0);
   kc->Fit(f2, "0");
   kc->Draw("same");

	//unbroken line
   //TF1 * f1n = new TF1("f1n", "[0]+[1]*x", fitRange[0], fitRange[1]);
   //TF1 * f2n = new TF1("f2n", "[0]+[1]*x", fitRange[0], fitRange[1]);
	TF1 * f1n = new TF1("f1n", "[0]*TMath::Exp(-[1]*x)", fitRange[0], fitRange[1]);
   TF1 * f2n = new TF1("f2n", "[0]*TMath::Exp(-[1]*x)", fitRange[0], fitRange[1]);
   f1n->SetParameters(f1->GetParameters());
	f2n->SetParameters(f2->GetParameters());
   
   f1n->SetLineColor(4);
   f1n->Draw("same");
   f2n->Draw("same");
   
   printf("=========================\n");
   //printf("p0 : %f , p1: %f , rate : %f%%/hr \n", p0, p1, p1/p0*60*100.);
   //printf("q0 : %f , q1: %f , rate : %f%%/hr \n", q0, q1, q1/q0*60*100.);
   printf("p0 : %f , p1: %f , rate : %f%%/hr \n", p0, p1, p1*60*100.);
   printf("q0 : %f , q1: %f , rate : %f%%/hr \n", q0, q1, q1*60*100.);
   
   
	TLegend * legend = new TLegend(0.5,0.1,0.9,0.35);
   legend->SetName("legend");
   TString lTitle;
   lTitle.Form("(d,d') rate: %5.1f%%/hr", p1/p0*60*100.);
	legend->AddEntry(kd,lTitle, "l");
	lTitle.Form("(12C,12C') rate: %5.1f%%/hr", q1/q0*60*100.);
	legend->AddEntry(kc,lTitle, "l");
	legend->Draw("same");
   
   //==================================================
   cDegrade->cd(3);
   tree->Draw("ddt_t/60","cutD && 6 <= run && run <= 10");
   
   cDegrade->cd(4);
   //tree->Draw("ddt_t/60 : eventID","cutD && 6 <= run && run <= 10");
   
   TH1F * kdp = new TH1F("kdp", "(d,p)", 55, -10, 100);
   tree->Draw("(eventID/1e3 - 3350)*120/218 >> kdp", "11 <= run && run <= 14 && detID < 18 && -0.5 < Ex && Ex < 2.5"); //167 ug/cm2 
   //tree->Draw("(eventID/1e3 - 3117)*120/218.>> kdp", "6 <= run && run <= 10 && detID < 18 && -0.5 < Ex && Ex < 2.5"); // 178 ug
   
   kdp->SetXTitle("Time [min]");
   kdp->SetYTitle("count / 2 min");
   
   TF1 * g1 = new TF1("g1", "[0]+[1]*x", fitRange[0], fitRange[1]);
   kdp->Fit("g1", "R");

   double w0 = g1->GetParameter(0);
   double w1 = g1->GetParameter(1);
		
	printf("========================= (d,p) \n");
   printf("w0 : %f , w1: %f , rate : %f%%/hr \n", w0, w1, w1/w0*60*100.);
   
   /**/
}
