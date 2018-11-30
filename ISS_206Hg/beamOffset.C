{

   gROOT->ProcessLine(".!root -l ../Simulation/transfer.C+");
   
   TFile * f1 = new TFile("transfer.root");
   TTree * t1 = (TTree*) f1->FindObjectAny("tree");
   
   
   t1->Draw("detRowID>>k0(4,0,4)", "ExID == 0");
   
   double p0 = k0->GetBinContent(1);
   double p1 = k0->GetBinContent(2);
   double p2 = k0->GetBinContent(3);
   double p3 = k0->GetBinContent(4);
   

   printf("%5.0f, %5.0f, %5.0f, %5.0f\n", p0, p1, p2, p3);
   
   double dx = (p0-p2)/(p0+p2);
   double dy = (p1-p3)/(p1+p3);
   
   printf("dx : %f, dy : %f \n", dx, dy);

   gROOT->ProcessLine(".q");
}
