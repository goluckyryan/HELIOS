{
   TCanvas * cAux = new TCanvas("cAux", "cAux", 0, 0, 500, 500);
   //cAux->Divide(1,2);
   //for( int i = 1; i <= 2 ; i++){
   //   cAux->cd(i)->SetGrid();
   //}
   cAux->cd(1);
   
   TH1F * spec = new TH1F("spec", "spec" , 400, -2000, 1500);
   spec->SetXTitle("Ex [a.u.]");
   TString expression;
   expression.Form("energy >> spec");        
   tree->Draw(expression, "TMath::Abs(energy_t)<20" , "");
   
   TH2F * h = new TH2F("h", "h" , 400, -2000, 1500, 400, -600, -200);
   h->SetXTitle("Ex [a.u.]");
   for( int i = 0 ; i < 6 ; i++){
      for(int j = 0; j < 4 ; j++){
   
         expression.Form("x[%d]: energy >> + h", i + 6*j);        
         tree->Draw(expression, "TMath::Abs(energy_t)<20" , "");
      }
   }
   
   /*
   cAux->cd(2);
   TH2F * ha = new TH2F("ha", "ha" , 400, -2000, 1500, 400, -600, -200);
   ha->SetXTitle("Ex [a.u.]");
   for( int i = 0 ; i < 6 ; i++){
      for(int j = 0; j < 4 ; j++){
   
         expression.Form("x[%d]: energy >> + ha", i + 6*j);
         TString gate;
         if( i >= 3){
            gate.Form("TMath::Abs(energy_t)<20 && det == %d && TMath::Abs(ttt-%f-250)<250", i + 6*j, mean[i+ 6*j] );
         }else{
            gate.Form("TMath::Abs(energy_t)<20");
         }        
         tree->Draw(expression, gate , "");
      }
   }
    
    
   /*
   double mean[24];
   TH2F ** y = new TH2F*[24];
   
   for( int i = 0; i < 24; i++){
      mean[i] = q[i]->GetMean(2);
      
      TString name;
      name.Form("y%d", i);
      y[i] = new TH2F(name, name, 300, -1, 1, 300, -1500, 1500);
      TString expression;
      expression.Form("ttt-%f:z[%d] >> y%d", mean[i], i, i);
            
      TString gate;
      gate.Form("tac[4] > 2000 && e[%d] > 100 && xf[%d] !=0 && xn[%d] !=0", i, i, i);
      
      cScript->cd(i+1);
      tree->Draw(expression, gate, "");
      
   }
   */
   /*
   TString gate;
   gate.Form("tac[4] > 2000 && e[%d] > 100 && xf[%d] !=0 && xn[%d] !=0", i, i, i);
      
   /**/
   /*
   TH1F * ty = new TH1F("ty", "ty", 300, -1500, 1500);
   
   for( int i = 0; i < 24; i++){
      TString expression;
      expression.Form("ttt-%f>> + ty", mean[i]);
            
      TString gate;
      gate.Form("tac[4] > 2000 && e[%d] > 100 && xf[%d] !=0 && xn[%d] !=0", i, i, i);
      
      cScript->cd(i+1);
      tree->Draw(expression, gate, "");
      
   }
   
   /**/
   
}


