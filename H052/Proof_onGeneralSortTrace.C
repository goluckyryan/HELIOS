#include <TFile.h>
#include <TChain.h>
#include <TTree.h>
#include <TProof.h>

void Proof_onGeneralSortTrace(){

   TChain * chain = new TChain("tree");
   chain->Add("data/trace_run107.root"); 
   
   TProof * p = TProof::Open("", "workers=4");
   
   chain->SetProof();
   chain->Process("GeneralSortTraceProof.C+");
   //chain->SetProof(0);
   
   
   delete chain;
   
}
