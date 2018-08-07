{

	TFile * f1 = new TFile("knockout_resol.root", "read");
	TTree * tree = (TTree *) f1->Get("tree");
	
   gStyle->SetOptStat(000000);

   TH2F ** heSp = new TH2F*[4];
   
   TGraph ** ge = new TGraph*[4];

   TMultiGraph * gm = new TMultiGraph();
   
   int KEAList[4] = { 50, 100, 200, 300};
   
   for( int j = 0; j < 4; j++){
      
      TString name;
      name.Form("heSp%d", j);
      heSp[j] = new TH2F(name, "eResol vs SpDiff; eResol [MeV]; SpDiff [MeV]", 10, 0, 1000, 50, 0, 10);
      ge[j] = new TGraph();
      
      TString expression, gate;
      expression.Form("kb-fVR[3].P():eResol>>heSp%d",j);
      gate.Form("KEA == %d", KEAList[j]);
      tree->Draw(expression, gate);
      
      for(int i = 0; i < 10; i++){
         heSp[j]->ProjectionY("hy", i, i+1);
         
         double x = 50 + 100*i;
         double half = (hy->GetMaximum())/2;
         
         int SpHalfBin = hy->FindLastBinAbove(half);
         double fitCenter = hy->GetBinCenter(SpHalfBin);  
         
         hy->Fit("pol1", "Rq", "", fitCenter - 0.5*(1+i), fitCenter + 0.5 *(1+i));
         
         double a0 = pol2->GetParameter(0);
         double a1 = pol2->GetParameter(1);
         //double a2 = pol2->GetParameter(2);
         
         double SpHalf = (half - a0)/a1;
         
         //double SpHalf = (- a1  TMath::Sqrt(a1*a1 - 4*a2*(a0-half)))/(2*a2*(a0-half));
          
         ge[j]->SetPoint(i, x, 2*SpHalf);
         
      }
      
      gm->Add(ge[j]);
      
   }
   
   gm->Draw("AC");
   

}
