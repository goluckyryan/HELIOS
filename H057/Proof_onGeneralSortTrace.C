#include <TFile.h>
#include <TChain.h>
#include <TTree.h>
#include <TProof.h>

void Proof_onGeneralSortTrace(){

   TChain * chain = new TChain("tree");
   chain->Add("/lcrc/project/HELIOS/h057_he6/root_data/run200.root"); 
   
   TProof * p = TProof::Open("", "workers=16");
   
   chain->SetProof();
   chain->Process("GeneralSortTraceProof.C");
   //chain->SetProof(0);
   
   
   delete chain;
   
}
