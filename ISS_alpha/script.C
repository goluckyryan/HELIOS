{


   TGraph * g = new TGraph();
   g->SetPoint(0,  815.2, 3.18);
   g->SetPoint(1, 1321.3, 5.16);
   g->SetPoint(2, 1404.4, 5.49);
   g->SetPoint(3, 1481.7, 5.81);
   g->Draw("A*");
   g->Fit("pol1");

}
