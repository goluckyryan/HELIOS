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

                    //   1,  2,  3,  4,  5,  6,  7,  8,  9, 10
Int_t idDetMap[160] =  {60, -1, -1, -1, -1, -1, -1, -1, -1, -1,//RF TIMING STUFF
                        40, 41, 42, 43, 44, 45, 46, 47, -1, -1,//ELUM
                        48, 49, 50, 51, 52, 53, 54, 55, -1, -1,//ELUM
                        30, 30, 31, 31, 32, 32, 33, 33, -1, -1,//Recoil Pairs have same detid
                         1,  0,  5,  4,  3,  2,  1,  0, -1, -1,/*1*/
                         3,  2,  1,  0,  5,  4,  3,  2, -1, -1,/*2*/
                        11, 10,  9,  8,  7,  6,  5,  4, -1, -1,/*3*/
                        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,//Empty
                         7,  6, 11, 10,  9,  8,  7,  6, -1, -1,/*4*/
                        15, 14, 13, 12, 11, 10,  9,  8, -1, -1,/*5*/
                        17, 16, 15, 14, 13, 12, 17, 16, -1, -1,/*6*/
                        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,//Empty
                        19, 18, 17, 16, 15, 14, 13, 12, -1, -1,/*7*/
                        21, 20, 19, 18, 23, 22, 21, 20, -1, -1,/*8*/
                        23, 22, 21, 20, 19, 18, 23, 22, -1, -1,/*9*/      
                        -1, -2, -3, -4, -5, -6, -7, -8, -9,-10};///
                        
                    //   1, 2, 3, 4, 5, 6, 7, 8, 9, 10
Int_t idKindMap[160] = { 6,-1,-1,-1,-1,-1,-1,-1,-1,-1,
                         5, 5, 5, 5, 5, 5, 5, 5,-1,-1,
                         5, 5, 5, 5, 5, 5, 5, 5,-1,-1,
                         3, 4, 3, 4, 3, 4, 3, 4,-1,-1,//3=DE,4=E
                         1, 1, 0, 0, 0, 0, 0, 0,-1,-1,//1 0==energy 1==xf 2==xn
                         2, 2, 2, 2, 1, 1, 1, 1,-1,-1,//2
                         0, 0, 0, 0, 0, 0, 2, 2,-1,-1,//3
                        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
                         2, 2, 1, 1, 1, 1, 1, 1,-1,-1,//4
                         0, 0, 0, 0, 2, 2, 2, 2,-1,-1,//5
                         2, 2, 2, 2, 2, 2, 0, 0,-1,-1,//6
                        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
                         0, 0, 1, 1, 1, 1, 1, 1,-1,-1,//7
                         1, 1, 1, 1, 0, 0, 0, 0,-1,-1,//8
                         2, 2, 2, 2, 2, 2, 1, 1,-1,-1,//9
                        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1};

//===========================================
TFile * saveFile; //!
TTree * newTree; //!
int totnumEntry; // of original root

//gClock   
TBenchmark gClock;
Bool_t shown;
Int_t psdValidCount;

//tree
TClonesArray * arr ;//!
TGraph * gTrace; //!

int eventID;
int ch[71];   
int kind[71];
//float energy[71];
//float xf[71];
//float xn[71];

float energy;
float xf;
float xn;

float te;
float txf;
float txn;

TF1 * gFit; //!

double tBase[71];
double tEnergy[71];
double tTime[71];
double tRiseTime[71];
double tChiSq[71];


int maxEntry = 100000;
int effEntry = 0;

int traceLength = 200;

void Ana_Trace::Begin(TTree * tree)
{
   // The Begin() function is called at the start of the query.
   // When running with PROOF Begin() is only called on the client.
   // The tree argument is deprecated (on PROOF 0 is passed).

   TString option = GetOption();   
   
	//============================================
   totnumEntry = tree->GetEntries();
	effEntry = TMath::Min(maxEntry, totnumEntry);
   printf( "========== Make a new tree with trace, total Entry : %d \n", totnumEntry);
	printf("==== use entry : %d \n", effEntry);
   
   saveFile = new TFile( "trace.root","recreate");
   newTree =  new TTree("tree","tree");

	newTree->Branch("eventID", &eventID , "eventID/I");
   newTree->Branch("NumHits", &NumHits , "NumHits/I");
   newTree->Branch("ch",             ch, "ch[NumHits]/I");
   newTree->Branch("kind",         kind, "kind[NumHits]/I");
   
   //newTree->Branch("e",          energy, "energy[NumHits]/F");
   //newTree->Branch("xf",             xf, "xf[NumHits]/F");
   //newTree->Branch("xn",             xn, "xn[NumHits]/F");
	
	newTree->Branch("e",         &energy, "energy/F");
   newTree->Branch("xf",            &xf, "xf/F");
   newTree->Branch("xn",            &xn, "xn/F");
   
	newTree->Branch("te",             &te, "te/F");
   newTree->Branch("txf",           &txf, "txf/F");
   newTree->Branch("txn",           &txn, "txn/F");

   arr = new TClonesArray("TGraph");
   newTree->Branch("trace", arr, 256000);
   arr->BypassStreamer();
   
	gFit = new TF1("gFit", "[0]/(1+TMath::Exp(-(x-[1])/[2]))+[3]", 0, 140);
   
   newTree->Branch("tBase",     tBase,     "tBase[NumHits]/D");
   newTree->Branch("tEnergy",   tEnergy,   "tEnergy[NumHits]/D");
   newTree->Branch("tTime",     tTime,     "tTime[NumHits]/D");
   newTree->Branch("tRiseTime", tRiseTime, "tRiseTime[NumHits]/D");
   newTree->Branch("tChiSq",    tChiSq,    "tChiSq[NumHits]/D");
   
   gClock.Reset();
   gClock.Start("timer");
   shown = 0;
   psdValidCount = 0;
   
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
/**///===================================================== initialization
	eventID = entry;
	
   for(int i = 0; i < 71; i++){
      ch[i] = -1;
      kind[i] = -1;
      //energy[i] = TMath::QuietNaN();
      //xf[i] = TMath::QuietNaN();
      //xn[i] = TMath::QuietNaN();
   }
   
	energy = TMath::QuietNaN();
	xf = TMath::QuietNaN();
	xn = TMath::QuietNaN();

   arr->Clear();

   for(int i = 0; i < 71; i++){
      tBase[i]     = TMath::QuietNaN();
      tEnergy[i]   = TMath::QuietNaN();
      tTime[i]     = TMath::QuietNaN();
      tRiseTime[i] = TMath::QuietNaN();
      tChiSq[i]    = TMath::QuietNaN();
   }

/**///===================================================== basic
   b_NumHits->GetEntry(entry);

	if( NumHits < 3 ) return kTRUE;

   b_id->GetEntry(entry);
   b_pre_rise_energy->GetEntry(entry);
   b_post_rise_energy->GetEntry(entry);

   bool okFlag = false;
   int countE = 0;
   int countXF = 0;
   int countXN = 0;
   for( int i = 0 ; i < NumHits; i++){
		int map = id[i]-1010;
      int psd8Chan = id[i]%10; 
      ch[i] = idDetMap[map];
      kind[i] = idKindMap[map];
      //energy[i]=(post_rise_energy[i]-pre_rise_energy[i])/100.;
      
      if( (id[i] > 1000 && id[i] < 2000) && psd8Chan<8 &&  ch[i]>-1 ) {
         okFlag = true;
         switch(kind[i]){
            case 0: /* Energy signal */
               //energy[i] = ((float)(post_rise_energy[i])-(float)(pre_rise_energy[i]))/100.;
               energy = ((float)(post_rise_energy[i])-(float)(pre_rise_energy[i]))/100.;
               countE ++;
               break;
            case 1: // XF
               //xf[i] = ((float)(post_rise_energy[i])-(float)(pre_rise_energy[i]))/100.;
               xf = ((float)(post_rise_energy[i])-(float)(pre_rise_energy[i]))/100.;
               countXF ++;
               break;
            case 2: // XN
               //xn[i] = ((float)(post_rise_energy[i])-(float)(pre_rise_energy[i]))/100.;
               xn = ((float)(post_rise_energy[i])-(float)(pre_rise_energy[i]))/100.;
               countXN ++;
               break;
         }
      }
   }
   
   if( countE != 1 || countXF != 1 || countXN != 1 ) return kTRUE;
   
   if( okFlag == false ) return kTRUE;
/**///===================================================== trace 
   b_trace->GetEntry(entry);
   
   int numTrace = 0;
   for( int i = 0; i < NumHits; i++){
      
      if( ch[i] < 0 || ch[i] > 2 ) break; 
      
      numTrace ++;
      gTrace = (TGraph*) arr->ConstructedAt(i);
      gTrace->Clear();
      //printf("============= %d \n", i);   
      
      double base = 0;
      for( int j = 0; j < traceLength; j++){
         //printf("%d, %f \n", j, trace[i][j]); 
         if( trace[i][j] < 16000){
            base = trace[i][j];
            gTrace->SetPoint(j, j, trace[i][j]);
         }else{
            gTrace->SetPoint(j, j, base);
         }
      }

	   base = gTrace->Eval(1);
	   double temp = gTrace->Eval(80) - base;
	
	   gFit->SetParameter(0, temp);
	   gFit->SetParameter(3, base);
      gFit->SetParameter(2, 1); //riseTime
	      
	   if( gTrace->Eval(120) < base ) {
	      gFit->SetRange(0, 100);
      }else{
         gFit->SetRange(0, traceLength);
      }
      
      if( gTrace->Eval(20) < tBase[i]){
         gFit->SetParameter(1, 5);
      }else{
	      gFit->SetParameter(1, 50);
	   }
	
      gTrace->Fit("gFit", "qR");
      
      tEnergy[i]   = gFit->GetParameter(0);
      tTime[i]     = gFit->GetParameter(1); 
      tRiseTime[i] = gFit->GetParameter(2);
      tBase[i]     = gFit->GetParameter(3);
      tChiSq[i]    = gFit->GetChisquare()/gFit->GetNDF();
      
		switch(kind[i]){
			case 0: /* Energy signal */
				te = tEnergy[i];
				break;
			case 1: // XF
				txf = tEnergy[i];
				break;
			case 2: // XN
				txn = tEnergy[i];
				break;
		}
      
   }
   
   if( numTrace == 0 ) return kTRUE;
   
   //#################################################################### Timer  
   psdValidCount++;
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
   saveFile->Close();
   printf("-------- saved as %s. psdValidCount: %d ", "trace.root", psdValidCount); 
}
