#define Compare_cxx
// The class definition in Compare.h has been generated automatically
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
// Root > T->Process("Compare.C")
// Root > T->Process("Compare.C","some options")
// Root > T->Process("Compare.C+")
//

#include "Compare.h"
#include <TH2.h>
#include <TStyle.h>


void Compare::Begin(TTree * /*tree*/)
{
   // The Begin() function is called at the start of the query.
   // When running with PROOF Begin() is only called on the client.
   // The tree argument is deprecated (on PROOF 0 is passed).

   TString option = GetOption();
}

void Compare::SlaveBegin(TTree * /*tree*/)
{
   // The SlaveBegin() function is called after the Begin() function.
   // When running with PROOF SlaveBegin() is called on each slave server.
   // The tree argument is deprecated (on PROOF 0 is passed).

   TString option = GetOption();

}

Bool_t Compare::Process(Long64_t entry)
{
   // The Process() function is called for each entry in the tree (or possibly
   // keyed object in the case of PROOF) to be processed. The entry argument
   // specifies which entry in the currently loaded tree is to be processed.
   // It can be passed to either Compare::GetEntry() or TBranch::GetEntry()
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
   
   e1 = TMath::QuietNaN();
   e2 = TMath::QuietNaN();
   e3 = TMath::QuietNaN();
   e0 = TMath::QuietNaN();
   
   xf1 = TMath::QuietNaN();
   xf2 = TMath::QuietNaN();
   xf3 = TMath::QuietNaN();
   xf0 = TMath::QuietNaN();
   
   xn1 = TMath::QuietNaN();
   xn2 = TMath::QuietNaN();
   xn3 = TMath::QuietNaN();
   xn0 = TMath::QuietNaN();
   
   x1 = TMath::QuietNaN();
   x2 = TMath::QuietNaN();
   x3 = TMath::QuietNaN();
   x0 = TMath::QuietNaN();
   
   for(int i = 0; i < 12; i++){
      reg[i] = -1;
   }
   
   //__________________________________________________
   eventID = entry;
   b_Energy->GetEntry(entry,0);
   //if( e[id] == 0 && e[id2] == 0) return kTRUE;
   if( e[id]    > 0 ) {e0 = e[id];    reg[0] = 0;}
   if( e[id+6]  > 0 ) {e1 = e[id+6];  reg[3] = 3;}
   if( e[id+12] > 0 ) {e2 = e[id+12]; reg[6] = 6;}
   if( e[id+18] > 0 ) {e3 = e[id+18]; reg[9] = 9;}

   b_XF->GetEntry(entry,0);
   if( xf[id]    > 0 ) {xf0 = slope[id]    * xf[id]    + intep[id];    reg[1]  = 1; }
   if( xf[id+6]  > 0 ) {xf1 = slope[id+6]  * xf[id+6]  + intep[id+6];  reg[4]  = 4; }
   if( xf[id+12] > 0 ) {xf2 = slope[id+12] * xf[id+12] + intep[id+12]; reg[7]  = 7; }
   if( xf[id+18] > 0 ) {xf3 = slope[id+18] * xf[id+18] + intep[id+18]; reg[10] = 10; }

   b_XN->GetEntry(entry,0);
   if( xn[id]    > 0 ) {xn0 = slope[id]    * xn[id]    + intep[id];    reg[2]  = 2;}
   if( xn[id+6]  > 0 ) {xn1 = slope[id+6]  * xn[id+6]  + intep[id+6];  reg[5]  = 5;}
   if( xn[id+12] > 0 ) {xn2 = slope[id+12] * xn[id+12] + intep[id+12]; reg[8]  = 8;}
   if( xn[id+18] > 0 ) {xn3 = slope[id+18] * xn[id+18] + intep[id+18]; reg[11] = 11;}
   count++;
   

   x0 = ((xf0-xn0)/(xf0+xn0) + 1)*length/2 + nearPos[id];
   x1 = ((xf1-xn1)/(xf1+xn1) + 1)*length/2 + nearPos[id];
   x2 = ((xf2-xn2)/(xf2+xn2) + 1)*length/2 + nearPos[id];
   x3 = ((xf3-xn3)/(xf3+xn3) + 1)*length/2 + nearPos[id];
//#################################################################### Timer  
   saveFile->cd(); //set focus on this file
   newTree->Fill();  

   clock.Stop("timer");
   Double_t time = clock.GetRealTime("timer");
   clock.Start("timer");

   if ( !shown ) {
      if (fmod(time, 10) < 1 ){
         printf( "%10d[%2d%%]|%3d min %5.2f sec | expect:%5.1fmin %10d\n", 
               entry, 
               TMath::Nint((entry+1)*100./totnumEntry),
               TMath::Floor(time/60), time - TMath::Floor(time/60)*60,
               totnumEntry*time/(entry+1)/60.,
               count);
               shown = 1;
      }
   }else{
      if (fmod(time, 10) > 9 ){
         shown = 0;
      }
   }
   

   return kTRUE;
}

void Compare::SlaveTerminate()
{
   // The SlaveTerminate() function is called after all entries or objects
   // have been processed. When running with PROOF SlaveTerminate() is called
   // on each slave server.

}

void Compare::Terminate()
{
   // The Terminate() function is the last function to be called during
   // a query. It always runs on the client, it can be used to present
   // the results graphically or save the results to file.

   saveFile->cd(); //set focus on this file
   newTree->Write(); 
   saveFile->Close();

   printf("-------------- done. %s, %d\n", saveFileName.Data(), count);
}
