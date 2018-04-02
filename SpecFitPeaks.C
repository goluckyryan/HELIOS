int nPeaks = 8;;

Double_t fpeaks(Double_t *x, Double_t *par) {
   Double_t result = 0;
   for (Int_t p=0;p<nPeaks;p++) {
      Double_t norm  = par[3*p+0];
      Double_t mean  = par[3*p+1];
      Double_t sigma = par[3*p+2];
      result += norm*TMath::Gaus(x[0],mean,sigma);
   }
   return result;
}

void SpecFitPeaks(){
   TCanvas * cAux = new TCanvas("cAux", "cAux", 0, 0, 500, 700);
   
   for( int i = 1; i <= 1 ; i++){
      cAux->cd(i)->SetGrid();
   }
   cAux->cd(1);
   
   
   TString expression;

   TH1F * spec = new TH1F("spec", "spec" , 400, -1, 10);
   spec->SetXTitle("Ex [a.u.]");
   tree->Draw("Ex>>spec", "good == 1 && det%6 != 5 && TMath::Abs(t4)<1000", "");

   TSpectrum * specPeak = new TSpectrum(20);
   nPeaks = specPeak->Search(spec, 1 ,"", 0.05);
   float * xpos = specPeak->GetPositionX();
   float * ypos = specPeak->GetPositionY();

   
   //Fit
   const int nn = 3* nPeaks;
   double para[nn];
   for( int i = 0 ; i < nPeaks ; i++){
      para[3*i+0] = ypos[i];
      para[3*i+1] = xpos[i];
      para[3*i+2] = 0.1;
   }
   
   TF1 * fit = new TF1("fit", fpeaks, -1, 10, 3*nPeaks);
   fit->SetParameters(para);
   fit->SetNpx(1000);
   spec->Fit("fit");
   

      
}


