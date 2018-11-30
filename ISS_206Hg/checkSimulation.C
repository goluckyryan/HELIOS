{
  gROOT->Reset();
  
  TFile * f1 = new TFile("transfer.root");
  TFile * f2 = new TFile("../ISS_130Xe/transfer.root");
  
  TTree * t1 = (TTree*) f1->FindObjectAny("tree");
  TTree * t2 = (TTree*) f2->FindObjectAny("tree");

  TH2F * hEZ1 = new TH2F("hEZ1", "energy vs Z; z [mm]; e [MeV]", 400, -500, 0, 400, 0, 10);
  TH2F * hEZ2 = new TH2F("hEZ2", "energy vs Z; z [mm]; e [MeV]", 400, -500, 0, 400, 0, 10);

  TH1F * hEx1 = new TH1F("hEx1", "Ex; Ex [MeV]; count / 10 keV", 900, -4, 5);
  TH1F * hEx2 = new TH1F("hEx2", "Ex; Ex [MeV]; count / 10 keV", 900, -4, 5);

  hEx2->SetLineColor(6);
  
  TCanvas * cCanvas = new TCanvas("cCanvas", "plot", 800, 1000);
  cCanvas->Divide(1,2);
  gStyle->SetOptStat(0);

  cCanvas->cd(1);
  t1->Draw("e:z>>hEZ1", "hit==1", "colz");
  t2->Draw("e:z>>hEZ2", "hit==1", "same");

  cCanvas->cd(2);
  t1->Draw("0.014718*z-e + 8.61>>hEx1", "hit==1", "");
  t2->Draw("0.014718*z-e + 8.61>>hEx2", "hit==1", "same");

  double max1 = hEx1->GetMaximum();
  double max2 = hEx2->GetMaximum();

  hEx2->Scale(max1/max2);
  hEx1->Draw();
  hEx2->Draw("hist same");
}
