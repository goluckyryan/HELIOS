#include <TFile.h>
#include <TChain.h>
#include <TTree.h>
#include <TProof.h>
#include <TFileCollection.h>

void Proof_onGeneralSortTrace(){

   //TFileCollection * fc = new TFileCollection();
   //fc->Add("data/trace_run107.root");
   

   TChain * chain = new TChain("tree");
   chain->Add("data/trace_run107.root"); 
   
   TProof * p = TProof::Open("", "workers=4");
   
   chain->SetProof();
   chain->Process("GeneralSortTraceProof.C+");
   chain->SetProof(0);
   
   //p->AddChain(chain);
   //p->Process("GeneralSortTraceProof.C+");

   //p->Process(fc, "GeneralSortTraceProof.C+");
   
   
   
}
