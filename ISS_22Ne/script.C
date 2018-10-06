#include "TFile.h"
#include "TTree.h"
#include "TObjArray.h"
#include "TH2F.h"
#include "TROOT.h"
#include "TStyle.h"


void script(){

   TFile * file0 = new TFile("A_gen_run25.root");
   TFile * file1 = new TFile("transfer.root");

   TTree * tree0 = (TTree*) file0->FindObjectAny("tree");
   TTree * tree1 = (TTree*) file1->FindObjectAny("tree");
   
   TObjArray * fxList = (TObjArray*) file1->FindObjectAny("fxList");
   
   gStyle->SetOptStat(0);
   
   /*
   tree0->Draw("e:z >> h(400, -550, -100, 400, 0, 10)", "-20 < coin_t && coin_t < 50", "colz");
   
   fxList->At(0)->Draw("same"); // 0.000
   fxList->At(1)->Draw("same"); // 1.017
   //fxList->At(2)->Draw("same"); // 1.702
   //fxList->At(3)->Draw("same"); // 1.822
   //fxList->At(4)->Draw("same"); // 2.315
   //fxList->At(5)->Draw("same"); // 2.517
   fxList->At(6)->Draw("same"); // 3.432
   fxList->At(7)->Draw("same"); // 3.831
   
   */
   
   TCanvas * cScript = new TCanvas("cScript", "cScript", 0, 0, 1600, 300);
   cScript->Divide(4,1);
   
   TString gate_RDT = "-20 < coin_t && coin_t < 50 ";
   
   cScript->cd(1);
   tree0->Draw("e:z >> k1(400, -600,  0, 400, 0, 10)", gate_RDT + "&&  detID < 6", "colz");
   
   cScript->cd(2);
   tree0->Draw("e:z >> k2(100, -600, 0, 400, 0, 10)", gate_RDT + "&& 6 <= detID && detID < 12", "colz");

   cScript->cd(3);
   tree0->Draw("e:z >> k3(100, -600, 0, 400, 0, 10)", gate_RDT + "&& 12 <= detID && detID < 18", "colz");
   
   cScript->cd(4);
   tree0->Draw("e:z >> k4(100, -600, 0, 400, 0, 10)", gate_RDT + "&& 18 <= detID ", "colz");
   
}
