#include <TFile.h>
#include <TChain.h>
#include <TTree.h>
#include <TProof.h>

void Proof_onGeneralSortTrace(){
   
   //=============== process after GEBSortMerge
   TChain * chain = new TChain("tree");
   //filename should be in the format "runXXXX.root", otherwise, the runID will be incorrect
   //chain->Add("/lcrc/project/HELIOS/h057_he6/root_data/run200.root"); 
   chain->Add("data/trace_run107.root"); 
   
   //TProof * p = TProof::Open("", "workers=1");
   
   //chain->SetProof();
   chain->Process("GeneralSortTraceProof.C+");
   //chain->SetProof(0);
   
   delete chain;
   //delete p;
   
   
   /* some thing wrong....
   //=============== process for calibaration and coinTime
   
   TChain * chain2 = new TChain("tree");
   chain2->Add("data/sortedTrace107.root");
   
   TProof * p2 = TProof::Open("", "workers=1");
   
   chain2->SetProof();
   chain2->Process("Cali_e_trace_Proof.C+");
   
   delete chain2;
   delete p2;
   */
}
