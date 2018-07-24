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
#include <TCanvas.h>
#include <TGraph.h>
#include <TClonesArray.h>
#include <TBenchmark.h>
#include <TMath.h>
#include <TF1.h>

#define M 100 //M value for energy filter from digi setting

const Int_t idConst = 1010; //Temp value to get idDet
Int_t idDetMap[160] = {300, 301,400,  -1,  -1,  -1,  -1,  -1,  -1,  -1,//RF TIMING STUFF
                        -1,  -1, -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,//Empty
                       101, 102,103, 104, 105, 106, 107, 108,  -1,  -1,//Recoils
                        -1, -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,//Empty
                         1,  0,   5,   4,   3,   2,   1,   0,  -1,  -1,/*1*/
                         3,  2,   1,   0,   5,   4,   3,   2,  -1,  -1,/*2*/
                        11, 10,   9,   8,   7,   6,   5,   4,  -1,  -1,/*3*/
                        -1, -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,//Empty
                         7,  6,  11,  10,   9,   8,   7,   6,  -1,  -1,/*4*/
                        15, 14,  13,  12,  11,  10,   9,   8,  -1,  -1,/*5*/
                        17, 16,  15,  14,  13,  12,  17,  16,  -1,  -1,/*6*/
                        -1, -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,//Empty
                        19, 18,  17,  16,  15,  14,  13,  12,  -1,  -1,/*7*/
                        21, 20,  19,  18,  23,  22,  21,  20,  -1,  -1,/*8*/
                        23, 22,  21,  20,  19,  18,  23,  22,  -1,  -1,/*9*/		 
                        -1, -2,  -3,  -4,  -5,  -6,  -7,  -8,  -9, -10};///

Int_t idKindMap[160] = {-1, -1,  9, -1, -1, -1, -1, -1, -1, -1,
                        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                         1,  1,  0,  0,  0,  0,  0,  0, -1, -1,//1
                         2,  2,  2,  2,  1,  1,  1,  1, -1, -1,//2
                         0,  0,  0,  0,  0,  0,  2,  2, -1, -1,//3
                        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                         2,  2,  1,  1,  1,  1,  1,  1, -1, -1,//4
                         0,  0,  0,  0,  2,  2,  2,  2, -1, -1,//5
                         1,  1,  1,  1,  1,  1,  0,  0, -1, -1,//6
                        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                         0,  0,  2,  2,  2,  2,  2,  2, -1, -1,//7
                         1,  1,  1,  1,  0,  0,  0,  0, -1, -1,//8
                         2,  2,  2,  2,  2,  2,  1,  1, -1, -1,//9
                        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};

//===========================================
TFile * saveFile; //!
TTree * newTree; //!
int totnumEntry; // of original root

//gClock   
TBenchmark gClock;
Bool_t shown = 0;

//PSD struct
typedef struct {
   int eventID;
   float e [24];
   float xf[24];
   float xn[24];
   float tac;
   
   ULong64_t e_t[24];
   ULong64_t tac_t;
   
} PSD;

PSD psd; 

//trace
TClonesArray * arr ;//!
TGraph * gTrace; //!
float te[24]; //time of e by trace
float ttac; //time of tac by trace

TF1 * gFit; //!

int maxEntry = 10000;
int effEntry = 0;

int traceLength = 200;

void Ana_Trace::Begin(TTree * tree)
{
   // The Begin() function is called at the start of the query.
   // When running with PROOF Begin() is only called on the client.
   // The tree argument is deprecated (on PROOF 0 is passed).

   TString option = GetOption();   
   totnumEntry = tree->GetEntries();
	effEntry = TMath::Min(maxEntry, totnumEntry);
	
   printf( "========== Make a new tree with trace, total Entry : %d \n", totnumEntry);
	printf("==== use entry : %d \n", effEntry);
   
   saveFile = new TFile( "trace.root","recreate");
   newTree =  new TTree("tree","tree");

	newTree->Branch("eventID", &psd.eventID , "eventID/I");
   
	newTree->Branch("e",           psd.e, "e[24]/F");
   newTree->Branch("xf",         psd.xf, "xf[24]/F");
   newTree->Branch("xn",         psd.xn, "xn[24]/F");
   newTree->Branch("tac",      &psd.tac, "tac/F");
   newTree->Branch("e_t",       psd.e_t, "e_t[24]/l");
   newTree->Branch("tac_t",  &psd.tac_t, "tac_t/l");
   
	newTree->Branch("te",              te, "te[24]/F");
   newTree->Branch("ttac",         &ttac, "ttac/F");

   arr = new TClonesArray("TGraph");
   newTree->Branch("trace", arr, 256000);
   arr->BypassStreamer();
   
	gFit = new TF1("gFit", "[0]/(1+TMath::Exp(-(x-[1])/[2]))+[3]", 0, 140);
   
   gClock.Reset();
   gClock.Start("timer");   
   printf("====================== started \n");
   
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
	
   if( entry >= maxEntry)  return kTRUE;
   
   b_NumHits->GetEntry(entry);

	if( NumHits < 4 ) return kTRUE; // e, xn, xf, tac
   
/**///===================================================== initialization
	psd.eventID = entry;
	
   for(int i = 0; i < 24; i++){
      psd.e[i]   = TMath::QuietNaN();
      psd.xf[i]  = TMath::QuietNaN();
      psd.xn[i]  = TMath::QuietNaN();
      psd.e_t[i]   = 0;
      
      te[i]   = TMath::QuietNaN();
   }
   
   psd.tac = TMath::QuietNaN();
   psd.tac_t = 0;
   ttac = TMath::QuietNaN();
   
   arr->Clear();

/**///===================================================== extrac values

   b_id->GetEntry(entry);
   b_pre_rise_energy->GetEntry(entry);
   b_post_rise_energy->GetEntry(entry);
   b_event_timestamp->GetEntry(entry);

   int countE = 0;
   int countXF = 0;
   int countXN = 0;
   int countTAC = 0;
   
   int map, ch, kind;
   
   for( int i = 0 ; i < NumHits; i++){
		map = id[i]-idConst;
      ch = idDetMap[map];
      kind = idKindMap[map];
      
      switch(kind){
         case 0: /* Energy signal */
            psd.e[ch]   = ((float)(post_rise_energy[i])-(float)(pre_rise_energy[i]))/M;
            psd.e_t[ch] = event_timestamp[i];
            countE ++;
            break;
         case 1: // XF
            psd.xf[ch] = ((float)(post_rise_energy[i])-(float)(pre_rise_energy[i]))/M;
            countXF ++;
            break;
         case 2: // XN
            psd.xn[ch] = ((float)(post_rise_energy[i])-(float)(pre_rise_energy[i]))/M;
            countXN ++;
            break;
         case 9: // TAC
            psd.tac   = ((float)(post_rise_energy[i])-(float)(pre_rise_energy[i]))/M;
            psd.tac_t = event_timestamp[i];
            countTAC ++;
            break;
      }
   }
   
   if( !(countE > 0 && ( countXF > 0 || countXN > 0 ) && countTAC > 0) ) return kTRUE;
   
/**///===================================================== trace 
   b_trace->GetEntry(entry);
   int numTrace = 0;
   for( int i = 0; i < NumHits; i++){
      map = id[i]-idConst;
      ch = idDetMap[map];
      kind = idKindMap[map];
      
      if( kind != 0 && kind != 9 ) continue;
      
      gTrace = (TGraph*) arr->ConstructedAt(numTrace);
      gTrace->Clear();
      
      numTrace ++;
      
      //Set gTrace
      double base = 0;
      for( int j = 0; j < traceLength; j++){
         if( trace[i][j] < 16000){
            base = trace[i][j];
            gTrace->SetPoint(j, j, trace[i][j]);
         }else{
            gTrace->SetPoint(j, j, base);
         }
      }

	   gFit->SetLineColor(kind == 0 ? 2 : kind);
	   gFit->SetRange(0, traceLength);
	   
	   base = gTrace->Eval(1);
	   double temp = gTrace->Eval(80) - base;

      gFit->SetParameter(0, temp); //energy
      gFit->SetParameter(1, 50); // time
	   gFit->SetParameter(2, 1); //riseTime
	   gFit->SetParameter(3, base);

      if( gTrace->Eval(120) < base ) gFit->SetRange(0, 100); //sometimes, the trace will drop    
      if( gTrace->Eval(20) < base) gFit->SetParameter(1, 5); //sometimes, the trace drop after 5 ch
	
      gTrace->Fit("gFit", "qR");
      
		switch(kind){
			case 0: te[ch] = gFit->GetParameter(1);break;
			case 9: ttac   = gFit->GetParameter(1);break;
		}
   }
   
   //#################################################################### Timer  
   saveFile->cd(); //set focus on this file
   newTree->Fill();  

   gClock.Stop("timer");
   Double_t time = gClock.GetRealTime("timer");
   gClock.Start("timer");

   if ( !shown ) {
      if (fmod(time, 10) < 1 ){
         printf( "%10lld[%2d%%]|%3.0f min %5.2f sec | expect:%5.2f min\n", 
               entry, 
               TMath::Nint((entry+1)*100./effEntry),
               TMath::Floor(time/60.), 
               time - TMath::Floor(time/60.)*60.,
               effEntry*time/(entry+1.)/60.);
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
   int validCount = newTree->GetEntries();
   saveFile->Close();
   printf("=======================================================\n");
   printf("----- Total processed entries : %3.1f k\n",effEntry/1000.0);
   gClock.Stop("timer");
   Double_t time = gClock.GetRealTime("timer");
   printf("----- Total run time : %6.0f sec \n", time);
   printf("----- Rate for sort: %6.3f k/sec\n",effEntry/time/1000.0);
   printf("----- saved as %s.  Valid Count: %d ", "trace.root", validCount); 
}
