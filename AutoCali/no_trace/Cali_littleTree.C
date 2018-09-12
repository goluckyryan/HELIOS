#define Cali_littleTree_cxx
// The class definition in Cali_littleTree.h has been generated automatically
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
// root> T->Process("Cali_littleTree.C")
// root> T->Process("Cali_littleTree.C","some options")
// root> T->Process("Cali_littleTree.C+")
//


#include "Cali_littleTree.h"
#include <TH2.h>
#include <TStyle.h>

void Cali_littleTree::Begin(TTree * /*tree*/)
{
   // The Begin() function is called at the start of the query.
   // When running with PROOF Begin() is only called on the client.
   // The tree argument is deprecated (on PROOF 0 is passed).

   TString option = GetOption();
   
}

void Cali_littleTree::SlaveBegin(TTree * /*tree*/)
{
   // The SlaveBegin() function is called after the Begin() function.
   // When running with PROOF SlaveBegin() is called on each slave server.
   // The tree argument is deprecated (on PROOF 0 is passed).

   TString option = GetOption();

}

Bool_t Cali_littleTree::Process(Long64_t entry)
{
   // The Process() function is called for each entry in the tree (or possibly
   // keyed object in the case of PROOF) to be processed. The entry argument
   // specifies which entry in the currently loaded tree is to be processed.
   // When processing keyed objects with PROOF, the object is already loaded
   // and is available via the fObject pointer.
   //
   // This function should contain the \"body\" of the analysis. It can contain
   // simple or elaborate selection criteria, run algorithms on the data
   // of the event and typically fill histograms.
   //
   // The processing can be stopped by calling Abort().
   //
   // Use fStatus to set the return value of TTree::Process().
   //
   // The return value is currently not used.
   
   //#################################################################### initialization

   eTemp    = TMath::QuietNaN();
   xTemp    = TMath::QuietNaN();
   zTemp    = TMath::QuietNaN();
   
   detIDTemp = -4;
   hitID = -4;
   zMultiHit = 0;
   
   //#################################################################### Get Tree
   eventID += 1;
   if( entry == 1 ) run += 1;
   
   b_Energy->GetEntry(entry,0);
   b_XF->GetEntry(entry,0);
   b_XN->GetEntry(entry,0);
   b_RDT->GetEntry(entry,0);
   
   //========== gate
   bool rdt_energy = false;
   bool coincident_t = false;
   for( int rID = 0; rID < 8; rID ++){
      if( rdt[rID] > 5000 ) rdt_energy = true;
      for( int i = 0; i < numDet; i++){
         int dt = (int) e_t[i] - rdt_t[rID];
         if( e[i] > 0 &&  TMath::Abs(dt) < 5) coincident_t = true;
      }   
   }
   if( !rdt_energy ) return kTRUE;
   if( !coincident_t ) return kTRUE;
   
   //#################################################################### Get Tree
   for(int idet = 0 ; idet < numDet; idet++){
      
      if( e[idet] == 0 ) continue;
      if( xf[idet] == 0 && xn[idet] == 0 ) continue;
      
      detIDTemp = idet;
      eTemp = e[idet];
      
      double xfC = xf[idet] * xfxneCorr[idet][1] + xfxneCorr[idet][0] ;
      double xnC = xn[idet] * xnCorr[idet] * xfxneCorr[idet][1] + xfxneCorr[idet][0];
      
      //========= calculate x
      if( xf[idet] > 0 && xn[idet] > 0 ){
         xTemp = (xfC - xnC)/(xfC+xnC);
         zMultiHit++;
         hitID = 0;
      }else if(xf[idet] == 0 && xn[idet] > 0 ){
         xTemp = (1-2*xnC/e[idet]);
         zMultiHit++;
         hitID = 1;
      }else if(xf[idet] > 0 && xn[idet] == 0 ){
         xTemp = (2*xfC/e[idet]-1);
         zMultiHit++;
         hitID = 2;
      }else{
         xTemp = TMath::QuietNaN();
      }
      
      //========= calculate z
      int detID = idet%iDet;
      if( pos[detID] < 0 ){
         zTemp = pos[detID] - (-xTemp + 1.)*length/2 ; 
      }else{
         zTemp = pos[detID] + (xTemp + 1.)*length/2 ; 
      }
      
   }//end of idet-loop
   
   if( zMultiHit == 0 ) return kTRUE;
   
   //#################################################################### Timer  
   saveFile->cd(); //set focus on this file
   newTree->Fill();  

   clock.Stop("timer");
   Double_t time = clock.GetRealTime("timer");
   clock.Start("timer");

   if ( !shown ) {
      if (fmod(time, 10) < 1 ){
         printf( "%10d[%2d%%]|%3.0f min %5.2f sec | expect:%5.2f min\n", 
               eventID, 
               TMath::Nint((eventID+1)*100./totnumEntry),
               TMath::Floor(time/60.), time - TMath::Floor(time/60.)*60.,
               totnumEntry*time/(eventID+1.)/60.);
         shown = 1;
      }
   }else{
      if (fmod(time, 10) > 9 ){
         shown = 0;
      }
   }

   return kTRUE;
}

void Cali_littleTree::SlaveTerminate()
{
   // The SlaveTerminate() function is called after all entries or objects
   // have been processed. When running with PROOF SlaveTerminate() is called
   // on each slave server.

}

void Cali_littleTree::Terminate()
{
   // The Terminate() function is the last function to be called during
   // a query. It always runs on the client, it can be used to present
   // the results graphically or save the results to file.
   
   saveFile->cd(); //set focus on this file
   newTree->Write(); 

   int totalEventNum = newTree->GetEntries();
   printf("-------------- done. %s, event: %d\n", saveFileName.Data(), totalEventNum);

   saveFile->Close();

}
