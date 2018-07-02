{

   const char* rootfile="~/ANALYSIS/H060_ana/C_gen_run11.root";
   TFile *f0 = new TFile (rootfile, "read"); 
   TTree * tree = (TTree*)f0->Get("tree");
   
   TH2F * h = new TH2F("h", "h", 500, 2000, 3500, 500, 50, 7550);
   
   tree->Draw("-ddt:tac >> h", "15 <= run && run <= 20", "colz");
   
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
   xRangeP[0] = 2228;
   xRangeP[1] = 2642;
   TCutG * cutP = new TCutG("cutP", 4);
   cutP->SetVarX("tac");
   cutP->SetVarY("-ddt");
   cutP->SetPoint(0, xRangeP[0],  990 );
   cutP->SetPoint(1, xRangeP[1],  990 );
   cutP->SetPoint(2, xRangeP[1], 1100 );
   cutP->SetPoint(3, xRangeP[0], 1100 );
   cutP->SetPoint(4, xRangeP[0],  990 );
   cutP->SetLineColor(6);
   cutP->Draw("same");

}
