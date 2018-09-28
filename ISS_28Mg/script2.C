#include "TFile.h"
#include "TTree.h"
#include "TObjArray.h"
#include "TH2F.h"
#include "TROOT.h"
#include "TStyle.h"
#include "TChain.h"
#include "TCutG.h"


void script2(){

   
   TFile * file0 = new TFile("A_gen_run51.root");
   TFile * file1 = new TFile("A_gen_run52.root");
   TFile * file2 = new TFile("transfer.root");
   
   TFile * fileCut = new TFile("rdtCuts.root");

   TTree * tree0 = (TTree*) file0->FindObjectAny("tree");
   TTree * tree1 = (TTree*) file1->FindObjectAny("tree");
   TTree * tree2 = (TTree*) file2->FindObjectAny("tree");
   
   TObjArray * fxList = (TObjArray*) file2->FindObjectAny("fxList");
   TObjArray * gList = (TObjArray*) file2->FindObjectAny("gList");
   
   TObjArray * cutList = NULL;
   TCutG ** cut = NULL;
   if( fileCut->IsOpen() ){
      TObjArray * cutList = (TObjArray*) fileCut->FindObjectAny("cutList");
      
      const int numCut = cutList->GetEntries();
      cut = new TCutG * [numCut];
      
      for( int i = 0 ; i < numCut; i++){
         cut[i] = (TCutG* ) cutList->At(i);
         printf("cut name: %s , VarX: %s, VarY: %s\n", cut[i]->GetName(), cut[i]->GetVarX(), cut[i]->GetVarY()); 
         
         //cut[i]->Print();
      }
   }
   
   TString gate_RDT = "(cut0 || cut1 || cut2 || cut3) && detID != 12 && -20 < coin_t && coin_t < 40";
   
   TCanvas * cScript = new TCanvas("cScript", "cScript", 0, 0, 1600, 900);
   cScript->Divide(4,3);
   for( int i = 1; i <= 4 ; i++){
      cScript->cd(i)->SetGrid();
      //cScript->cd(i)->SetLogz();
   }
   
   //gStyle->SetOptStat(0);
   if(cScript->GetShowEditor() )cScript->ToggleEditor();
   if(cScript->GetShowToolBar())cScript->ToggleToolBar();
   
   cScript->cd(1);
   tree0->Draw("rdt[0]:rdt[4]>>rdt1(300, 0, 8000, 300, 0, 4000)", "", "colz");
   cut[0]->Draw("same");
   
   cScript->cd(2);
   tree0->Draw("rdt[1]:rdt[5]>>rdt2(300, 0, 8000, 300, 0, 4000)", "", "colz");
   cut[1]->Draw("same");
   
   cScript->cd(3);
   tree0->Draw("rdt[2]:rdt[6]>>rdt3(300, 0, 8000, 300, 0, 4000)", "", "colz");
   cut[2]->Draw("same");
   
   cScript->cd(4);
   tree0->Draw("rdt[3]:rdt[7]>>rdt4(300, 0, 8000, 300, 0, 4000)", "", "colz");
   cut[3]->Draw("same");
   
   cScript->cd(5);
   tree0->Draw("e:z >> k1(400, -600,  0, 400, 0, 10)", gate_RDT + "&& detID < 6", "colz");
   
   cScript->cd(6);
   tree0->Draw("e:z >> k2(100, -600, 0, 400, 0, 10)", gate_RDT + "&& 6 <= detID && detID < 12", "colz");
   
   cScript->cd(7);
   tree0->Draw("e:z >> k3(100, -600, 0, 400, 0, 10)", gate_RDT + "&& 12 <= detID && detID < 18", "colz");
   
   cScript->cd(8);
   tree0->Draw("e:z >> k4(100, -600, 0, 400, 0, 10)", gate_RDT + "&& 18 <= detID ", "colz");


   cScript->cd(9);
   tree1->Draw("e:z >> p1(400, -600,  0, 400, 0, 10)", gate_RDT + "&& detID < 6", "colz");
   
   cScript->cd(10);
   tree1->Draw("e:z >> p2(100, -600, 0, 400, 0, 10)", gate_RDT + "&& 6 <= detID && detID < 12", "colz");
   
   cScript->cd(11);
   tree1->Draw("e:z >> p3(100, -600, 0, 400, 0, 10)", gate_RDT + "&& 12 <= detID && detID < 18", "colz");
   
   cScript->cd(12);
   tree1->Draw("e:z >> p4(100, -600, 0, 400, 0, 10)", gate_RDT + "&& 18 <= detID ", "colz");   

}
