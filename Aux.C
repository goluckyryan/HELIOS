//#include "TH1.h"
//#include "TF1.h"
//#include "TList.h"

Double_t fline(Double_t *x, Double_t *par)
{
    if ( (80 <x[0] && x[0] < 125) ) {
      TF1::RejectPoint();
      return 0;
   }
   return par[0] + par[1]*x[0];
}

void Aux(){

   
   const char* rootfile="~/ANALYSIS/H060_ana/C_gen_run11.root";
   TFile *f0 = new TFile (rootfile, "read"); 
   TTree * tree = (TTree*)f0->Get("tree");
   
   /*
   TH2F * h = new TH2F("h", "h", 500, 0, 7, 500, -450, -100);
   
   tree->Draw("z:e >> h", "hit == 1 && ExID == 0 && loop ==1", "");
   
   TF1 * fit = new TF1("fit", "[0]+[1]*x +([2]+[3]*x)*TMath::ASin([4]/TMath::Sqrt([5]*x*x + [6]*x + [7]))");
   fit->SetParameter(0, -522);
   fit->SetParameter(1, 56.6);
   fit->SetParameter(2, 135.6);
   fit->SetParameter(3, -60);
   fit->SetParameter(4, 31);
   fit->SetParameter(5, 9822);
   fit->SetParameter(6, -3000);
   fit->SetParameter(7, 20000);

   h->Fit("fit", "", "",2 , 7);
   /**/
   
   TCutG * cutC = new TCutG("cutC", 4);
   cutC->SetVarX("tac");
   cutC->SetVarY("-ddt");
   cutC->SetPoint(0, 2100, 3000 );
   cutC->SetPoint(1, 3480, 3000 );
   cutC->SetPoint(2, 3480, 7500 );
   cutC->SetPoint(3, 2100, 7500 );
   cutC->SetPoint(4, 2100, 3000 );
   cutC->SetLineColor(2);
   cutC->Draw("same");
   
   TCutG * cutDP = new TCutG("cutDP", 4);
   cutDP->SetVarX("tac");
   cutDP->SetVarY("-ddt");
   cutDP->SetPoint(0, 2100,  900 );
   cutDP->SetPoint(1, 3480,  900 );
   cutDP->SetPoint(2, 3480, 1200 );
   cutDP->SetPoint(3, 2100, 1200 );
   cutDP->SetPoint(4, 2100,  900 );
   cutDP->SetLineColor(4);
   cutDP->Draw("same");
   
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
   cutD->Draw("same");
   
   tree->Draw("(eventID/1e3 - 3117)*120/160 >> kc(400, -10, 200)", "cutC && 6 <= run && run <= 10"); //178 ug/cm2
   tree->Draw("(eventID/1e3 - 3117)*120/160 >> kd(400, -10, 200)", "cutD && 6 <= run && run <= 10"); //178 ug/cm2
   
   //tree->Draw("(eventID/1e3 - 3350)*120/160 >> kc(200, -10, 150)", "cutC && 11 <= run && run <= 14"); //167 ug/cm2 
   //tree->Draw("(eventID/1e3 - 3350)*120/160 >> kd(200, -10, 150)", "cutD && 11 <= run && run <= 14"); //167 ug/cm2 
   
   //kc->Scale(1.28);
   kc->SetLineColor(2);
   
   kd->SetXTitle("time [min]");
   kc->SetXTitle("time [min]");
   
   TF1 * f1 = new TF1("f1", fline, 10, 170, 2);
   TF1 * f2 = new TF1("f2", fline, 10, 170, 2);
   
   kd->Fit(f1, "0");
   kc->Fit(f2, "0");
   
   double p0 = f1->GetParameter(0);
   double p1 = f1->GetParameter(1);
   
   double q0 = f2->GetParameter(0);
   double q1 = f2->GetParameter(1);
   
   kc->Scale(p0/q0);
   
   kc->Fit(f2, "0");
   
   kc->Draw("same");

   TF1 * f1n = new TF1("f1n", "[0]+[1]*x", 10, 170);
   f1n->SetParameters(f1->GetParameters());
   
   TF1 * f2n = new TF1("f2n", "[0]+[1]*x", 10, 170);
   f2n->SetParameters(f2->GetParameters());
   
   f1n->SetLineColor(4);
   f1n->Draw("same");
   
   f2n->Draw("same");
   
   printf("=========================\n");
   
   double q0 = f2->GetParameter(0);
   double q1 = f2->GetParameter(1);
   
   printf("p0 : %f , p1: %f , rate : %f%%/hr \n", p0, p1, p1/p0*60*100.);
   printf("q0 : %f , q1: %f , rate : %f%%/hr \n", q0, q1, q1/q0*60*100.);
   
   /**/
}
