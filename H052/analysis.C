{

   TFile * f1 = new TFile("A_trace.root", "READ");
   TTree * tree = (TTree*) f1->Get("tree");
   
   int totnumEntry = tree->GetEntries();
   printf( "========== total Entry : %d \n", totnumEntry);

   int Div[2] = {6,4};
   int size[2] = {230,230}; //x,y
   TCanvas * cAna = new TCanvas("cAna", "cAna", 0, 0, size[0]*Div[0], size[1]*Div[1]);
   if( cAna->GetShowEditor() )  cAna->ToggleEditor();
   if( cAna->GetShowToolBar() ) cAna->ToggleToolBar();
   cAna->Divide(Div[0],Div[1]);
   for( int i = 1; i <= Div[0]*Div[1] ; i++){
      cAna->cd(i)->SetGrid();
   }
   
   
   TH2F ** hTX = new TH2F*[24];
   
   TProfile ** hp = new TProfile * [24];
   
   TH2F ** hTXc = new TH2F*[24];
   
   for( int i = 0 ; i < 24; i++){
      TString name;
      name.Form("hTX%d", i);
      TString expression, gate;
      
      //hTX[i] = new TH2F(name, "time vs X; x; rise time", 400, -1.5, 1.5, 400, 40, 100);
      //expression.Form("te_t:x>>hTX%d", i);
      
      //hTX[i] = new TH2F(name, "rise time vs X; x; rise time", 400, -1.5, 1.5, 400, 0, 20);
      //expression.Form("te_r:x>>hTX%d", i);
      
      //hTX[i] = new TH2F(name, "rise time vs time; time; rise time", 400, 40, 100, 400, 0, 20);
      //expression.Form("te_r:te_t>>hTX%d", i);
      
      hTX[i] = new TH2F(name, "coincident vs X; x; rise time", 400, -1.5, 1.5, 400, -10, 50);
      expression.Form("coin_t+te_t-trdt_t:x>>hTX%d", i);
      
      gate.Form("Iteration$==%d", i);
      
      cAna->cd(i+1);
      tree->Draw(expression, gate, "colz");
      
      /*
      name.Form("hTXp%d", i);
      hp[i] = new TProfile(name, name, 400, -1.5,1.5);
      
      hTX[i]->ProfileX(name);
      
      hp[i]->Draw("same");
      
      /*
      TF1 * fit = new TF1("fit", "[0]+[1]*TMath::Power(x,2) +[2]*TMath::Power(x,4)", -2, 2);
      fit->SetParameter(0,1);
      fit->SetParameter(1,1);
      fit->SetParameter(2,1);
      
      hp[i]->Fit("fit", "");
      
      double p[3];
      p[0] = fit->GetParameter(0);
      p[1] = fit->GetParameter(1);
      p[2] = fit->GetParameter(2);
      
      //correction
      name.Form("hTXc%d", i);
      hTXc[i] = new TH2F(name, "time vs X; x; rise time", 400, -1.5, 1.5, 400, 40, 100);
      
      expression.Form("te_t - %f *TMath::Power(x,2) - %f *TMath::Power(x,4) :x>>hTXc%d", p[1], p[2], i);
      
      tree->Draw(expression, gate, "colz");
      /**/ 
   }

}
