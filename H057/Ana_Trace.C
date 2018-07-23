#define Ana_Trace_cxx
// The class definition in Ana_Trace.h has been generated automatically
// by the ROOT utility TTree::MakeSelector(). This class is derived
// from the ROOT class TSelector. For more information on the TSelector
// framework see $ROOTSYS/README/README.SELECTOR or the ROOT User Manual.

// The following methods are defined in this file:
//    Begin():        called every time a loop on the tree starts,
//                    a convenient place to create your histograms.
//    SlaveBegin():   called after Begin(), when on PROOF called only on the
//                    slave servers.
//    Process():      called for each event, in this function you decide what
//                    to read and fill your histograms.
//    SlaveTerminate: called at the end of the loop on the tree, when on PROOF
//                    called only on the slave servers.
//    Terminate():    called at the end of the loop on the tree,
//                    a convenient place to draw/fit your histograms.
//
// To use this file, try the following session on your Tree T:
//
// Root > T->Process("Ana_Trace.C")
// Root > T->Process("Ana_Trace.C","some options")
// Root > T->Process("Ana_Trace.C+")
//

#include "Ana_Trace.h"
#include <TH2.h>
#include <TStyle.h>

void Ana_Trace::Begin(TTree * /*tree*/)
{
   // The Begin() function is called at the start of the query.
   // When running with PROOF Begin() is only called on the client.
   // The tree argument is deprecated (on PROOF 0 is passed).

   TString option = GetOption();
   
}

void Ana_Trace::SlaveBegin(TTree * /*tree*/)
{
   // The SlaveBegin() function is called after the Begin() function.
   // When running with PROOF SlaveBegin() is called on each slave server.
   // The tree argument is deprecated (on PROOF 0 is passed).

   TString option = GetOption();

}

Bool_t Ana_Trace::Process(Long64_t entry)
{
   // The Process() function is called for each entry in the tree (or possibly
   // keyed object in the case of PROOF) to be processed. The entry argument
   // specifies which entry in the currently loaded tree is to be processed.
   // It can be passed to either Ana_Trace::GetEntry() or TBranch::GetEntry()
   // to read either all or the required parts of the data. When processing
   // keyed objects with PROOF, the object is already loaded and is available
   // via the fObject pointer.
   //
   // This function should contain the "body" of the analysis. It can contain
   // simple or elaborate selection criteria, run algorithms on the data
   // of the event and typically fill histograms.
   //
   // The processing can be stopped by calling Abort().
   //
   // Use fStatus to set the return value of TTree::Process().
   //
   // The return value is currently not used.

   if( entry > 1000)  return kTRUE;

   arr->Clear();
   gTrace = (TGraph*) arr->ConstructedAt(0);
   
   
   /*
   arrF->Clear();
   gFit = new TF1("gFit", "[2]/(1+TMath::Exp(-(x-[0])/[1])) + [3]", 0, 100);
   gFit = (TF1*) arrF->ConstructedAt(0);
   gFit->SetParameter(0,   50);
   gFit->SetParameter(1,   10);
   gFit->SetParameter(2, 1000);
   gFit->SetParameter(3, 8000);
   */
   b_trace->GetEntry(entry, 0);
   
   int base = 0;
   for( int i = 0; i < 150; i++){
      if( trace[0][i] < 16000){
         base = trace[0][i];
         gTrace->SetPoint(i, i, trace[0][i]);
      }else{
         gTrace->SetPoint(i, i, base);
      }
   }

	base = gTrace->Eval(1);
	energy = gTrace->Eval(80) - base;
	gFit->SetParameter(3, base);
	gFit->SetParameter(1, 50);
	gFit->SetParameter(2, 1);
	gFit->SetParameter(0, energy);

   gTrace->Fit("gFit", "qR");
   
   energy   = gFit->GetParameter(0);
   time     = gFit->GetParameter(1);
   riseTime = gFit->GetParameter(2);
   base     = gFit->GetParameter(3);


   //#################################################################### Timer  
   saveFile->cd(); //set focus on this file
   newTree->Fill();  

   clock.Stop("timer");
   Double_t time = clock.GetRealTime("timer");
   clock.Start("timer");

   if ( !shown ) {
      if (fmod(time, 10) < 1 ){
         printf( "%10lld[%2d%%]|%3.0f min %5.2f sec | expect:%5.2f min\n", 
               entry, 
               TMath::Nint((entry+1)*100./totnumEntry),
               TMath::Floor(time/60.), 
               time - TMath::Floor(time/60.)*60.,
               (Float_t)totnumEntry*time/(entry+1.)/60.);
               shown = 1;
      }
   }else{
      if (fmod(time, 10) > 9 ){
         shown = 0;
      }
   }
   
   return kTRUE;
}

void Ana_Trace::SlaveTerminate()
{
   // The SlaveTerminate() function is called after all entries or objects
   // have been processed. When running with PROOF SlaveTerminate() is called
   // on each slave server.

}

void Ana_Trace::Terminate()
{
   // The Terminate() function is the last function to be called during
   // a query. It always runs on the client, it can be used to present
   // the results graphically or save the results to file.
   newTree->Write();
}
