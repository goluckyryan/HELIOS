#define GeneralSortTrace_cxx

#include "GeneralSortTrace.h"
#include <TH2.h>
#include <TStyle.h>
#include <TF1.h>
#include <TGraph.h>
#include <TClonesArray.h>
#include <TMath.h>
#include <TBenchmark.h>

#define NUMPRINT 20 //>0
#define MAXNUMHITS 20 //Highest multiplicity
#define M 100 //M value for energy filter from digi setting

//Arrays for mapping things...
//With new mapping...1.15..starts with 1010 now...
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

TBenchmark gClock;
Bool_t shown = 0;

TFile *saveFile; //!
TTree *newTree; //!

ULong64_t MaxProcessedEntries=10000;
int EffEntries;
ULong64_t NumEntries = 0;
ULong64_t ProcessedEntries = 0;

bool isTraceON = true;
int traceMethod = 1; // 0 = no process, 1, fit, 2, constant fraction 
int traceLength = 200;

bool isTACRF = true;
bool isRecoil = false;
bool isElum = false;
bool isEZero = false;

//trace
TClonesArray * arr ;//!
TGraph * gTrace; //!
TF1 * gFit; //!

float te[24];
float te_t[24];
float ttac_t;

//PSD struct
typedef struct {
   Int_t   eventID;
   Float_t Energy[24];
   Float_t XF[24];
   Float_t XN[24];
   Float_t Ring[24];
   Float_t RDT[24];
   Float_t TAC[24];
   Float_t ELUM[32];
   Float_t EZERO[4];//0,1 - DE0,E0
   // 2,3 - DEX,EX

   ULong64_t EnergyTimestamp[24];
   ULong64_t XFTimestamp[24];
   ULong64_t XNTimestamp[24];
   ULong64_t RingTimestamp[24];
   ULong64_t RDTTimestamp[24];
   ULong64_t TACTimestamp[24];
   ULong64_t ELUMTimestamp[32];
   ULong64_t EZEROTimestamp[4];
} PSD;

PSD psd; 

void GeneralSortTrace::Begin(TTree * tree)
{
   
   TString option = GetOption();
   NumEntries = tree->GetEntries();
   EffEntries = TMath::Min(MaxProcessedEntries, NumEntries);
   printf( "=====================================================\n");
   printf( "==========  General Sort w/ Trace  ==================\n");
   printf( "=====================================================\n");
   printf( "========== Make a new tree with trace, total Entry : %d, use : %d\n", NumEntries, EffEntries);
   printf( "  TAC/RF : %s \n", isTACRF ?  "On" : "Off");
   printf( "  Recoil : %s \n", isRecoil ? "On" : "Off");
   printf( "  Elum   : %s \n", isElum ?   "On" : "Off");
   printf( "  EZero  : %s \n", isEZero ?  "On" : "Off");
   printf( "  Trace  : %s , Method: %d \n", isTraceON ?  "On" : "Off", traceMethod);

   saveFile = new TFile("trace.root","RECREATE");
   newTree = new TTree("tree","PSD Tree w/ trace");

   newTree->Branch("eventID", &psd.eventID, "eventID/I");

   newTree->Branch("e",   psd.Energy,          "Energy[24]/F");
   newTree->Branch("e_t", psd.EnergyTimestamp, "EnergyTimestamp[24]/l");

   newTree->Branch("xf",   psd.XF,          "XF[24]/F");
   newTree->Branch("xf_t", psd.XFTimestamp, "XFTimestamp[24]/l");

   newTree->Branch("xn",   psd.XN,          "XN[24]/F");
   newTree->Branch("xn_t", psd.XNTimestamp, "XNTimestamp[24]/l"); 

   if( isRecoil){
      newTree->Branch("rdt",   psd.RDT,          "RDT[24]/F");
      newTree->Branch("rdt_t", psd.RDTTimestamp, "RDTTimestamp[24]/l"); 
   }
   
   if( isTACRF ){
      newTree->Branch("tac",   psd.TAC,          "TAC[24]/F");
      newTree->Branch("tac_t", psd.TACTimestamp, "TACTimestamp[24]/l"); 
   }
   
   if( isElum ) {
      newTree->Branch("elum",   psd.ELUM,          "ELUM[32]/F");
      newTree->Branch("elum_t", psd.ELUMTimestamp, "ELUMTimestamp[32]/l"); 
   }
   
   if( isEZero ){
      newTree->Branch("ezero",   psd.EZERO,          "EZERO[4]/F");
      newTree->Branch("ezero_t", psd.EZEROTimestamp, "EZEROTimestamp[4]/l"); 
   }
   if( isTraceON ){
      arr = new TClonesArray("TGraph");
      newTree->Branch("trace", arr, 256000);
      arr->BypassStreamer();
      
	   gFit = new TF1("gFit", "[0]/(1+TMath::Exp(-(x-[1])/[2]))+[3]", 0, 140);
      newTree->Branch("te",             te,  "te[24]/F");
      newTree->Branch("te_t",         te_t,  "te_t[24]/F");
      newTree->Branch("ttac_t",    &ttac_t,  "ttac_t/F");
   }
   
   gClock.Reset();
   gClock.Start("timer");
   
   printf("====================== started \n");
}

void GeneralSortTrace::SlaveBegin(TTree * /*tree*/)
{
  
  TString option = GetOption();
  
}

Bool_t GeneralSortTrace::Process(Long64_t entry)
{ 
   psd.eventID = entry;
   ProcessedEntries++;
   if(ProcessedEntries >= MaxProcessedEntries) return kTRUE;
   
   b_NumHits->GetEntry(entry);
   if( NumHits < 4 ) return kTRUE; // e, xn, xf, tac

/**///======================================= Zero struct
   for (Int_t i=0;i<24;i++) {//num dets
      psd.Energy[i]  = TMath::QuietNaN();
      psd.XF[i]      = TMath::QuietNaN();
      psd.XN[i]      = TMath::QuietNaN();
      psd.Ring[i]    = TMath::QuietNaN();
      psd.RDT[i]     = TMath::QuietNaN();
      psd.TAC[i]     = TMath::QuietNaN();
      if (i<32) psd.ELUM[i] = TMath::QuietNaN();
      if (i<4) psd.EZERO[i] = TMath::QuietNaN();

      psd.EnergyTimestamp[i] = TMath::QuietNaN();
      psd.XFTimestamp[i]     = TMath::QuietNaN();
      psd.XNTimestamp[i]     = TMath::QuietNaN();
      psd.RingTimestamp[i]   = TMath::QuietNaN();
      psd.RDTTimestamp[i]    = TMath::QuietNaN();
      psd.TACTimestamp[i]    = TMath::QuietNaN();
      if (i<32) psd.ELUMTimestamp[i] = TMath::QuietNaN();
      if (i<4) psd.EZEROTimestamp[i] = TMath::QuietNaN();	    
   }
   
   if( isTraceON ){
      for(int i = 0; i < 24; i++){
         te[i]  = TMath::QuietNaN();
         te_t[i] = TMath::QuietNaN();
      }
      ttac_t = TMath::QuietNaN();
      arr->Clear();
   }

/**///======================================= Pull needed entries
   
   b_id->GetEntry(entry);
   b_pre_rise_energy->GetEntry(entry);
   b_post_rise_energy->GetEntry(entry);
   b_event_timestamp->GetEntry(entry);
   if( isTraceON ) b_trace->GetEntry(entry);

   //ID PSD Channels
   Int_t idKind  = -1;
   Int_t idDet   = -1; // Detector number
   
   bool isPSDValid = false; // when e, xf, xn are all valid
   bool isTACValid = false;
   
   int countE = 0;
   int countXF = 0;
   int countXN = 0;
   
   bool isTraced = false;

   /* -- Loop over NumHits -- */
   for(Int_t i=0;i<NumHits;i++) {
      Int_t psd8Chan = id[i]%10;     
      Int_t idTemp   = id[i] - idConst;
      idDet  = idDetMap[idTemp];
      idKind = idKindMap[idTemp];
      
      //PSD
      /***********************************************************************/
      if( (id[i] > 1000 && id[i] < 2000) && psd8Chan<8 &&  idDet>-1 ) {
         
         switch(idKind){
            case 0: /* Energy signal */
               psd.Energy[idDet] = ((float)(post_rise_energy[i])-(float)(pre_rise_energy[i]))/M;
               psd.EnergyTimestamp[idDet] = event_timestamp[i];
               countE++;
               break;
            case 1: // XF
               psd.XF[idDet] = ((float)(post_rise_energy[i])-(float)(pre_rise_energy[i]))/M;
               psd.XFTimestamp[idDet] = event_timestamp[i];
               countXF++;
               break;
            case 2: // XN
               psd.XN[idDet] = ((float)(post_rise_energy[i])-(float)(pre_rise_energy[i]))/M;
               psd.XNTimestamp[idDet] = event_timestamp[i];
               countXN++;
               break;
         }
      }
      
      //TAC & RF TIMING
      /***********************************************************************/
      if( isTACRF && (id[i]>1000&&id[i]<2000)&&(idDet>=400&&idDet<=450)) { //RF TIMING SWITCH
         //if (ProcessedEntries < NUMPRINT) printf("RF id %i, idDet %i\n",id[i],idDet);

         Int_t tacTemp = idDet-400;
         psd.TAC[tacTemp] = ((float)(post_rise_energy[i])-(float)(pre_rise_energy[i]))/M;
         psd.TACTimestamp[tacTemp] = event_timestamp[i];
         isTACValid = true;
      }

      //RECOIL
      /************************************************************************/
      if( isRecoil && (id[i]>1000&&id[i]<2000)&&(idDet>=100&&idDet<=110)) { //recOILS
         Int_t rdtTemp = idDet-101;
         psd.RDT[rdtTemp] = ((float)(pre_rise_energy[i])-(float)(post_rise_energy[i]))/M;
         psd.RDTTimestamp[rdtTemp] = event_timestamp[i];
      }

      //ELUM
      /************************************************************************/
      if( isElum && (id[i]>=1090 && id[i]<1130)&&(idDet>=200&&idDet<=240)) {
         Int_t elumTemp = idDet - 200;

         if (elumTemp<16) {
            psd.ELUM[elumTemp] = ((float)(post_rise_energy[i])-(float)(pre_rise_energy[i]))/M;
         } else {
            psd.ELUM[elumTemp] = ((float)(pre_rise_energy[i])-(float)(post_rise_energy[i]))/M;
         }
         
         psd.ELUMTimestamp[elumTemp] = event_timestamp[i];
      }//end ELUM

      //EZERO
      /************************************************************************/
      if( isEZero && (id[i]>1000&&id[i]<2000)&&(idDet>=300&&idDet<310)) {
         Int_t ezeroTemp = idDet - 300;
         if (ezeroTemp<4) {
            psd.EZERO[ezeroTemp] = ((float)(post_rise_energy[i])-(float)(pre_rise_energy[i]))/M;
            psd.EZEROTimestamp[ezeroTemp] = event_timestamp[i];
         }
      }//end EZERO
      
   }//end of NumHits
   
   if( countE > 0 && ( countXF > 0 || countXN > 0) ) isPSDValid = true;
   
   if( !isPSDValid || !isTACValid ) return kTRUE;

      
   //Trace
   /************************************************************************/
   if( isTraceON ) {
      int countTrace = 0;
      for(Int_t i = 0; i < NumHits ; i++) {
         Int_t idTemp   = id[i] - idConst;
         idDet  = idDetMap[idTemp];
         idKind = idKindMap[idTemp];
         
         //trace when energy or tac
         if( idKind != 0 && idKind != 9 ) continue;
         
         gTrace = (TGraph*) arr->ConstructedAt(countTrace);
         gTrace->Clear();
         countTrace ++;
         
         //Set gTrace
         
         if( traceMethod == 0 ){
            for ( int j = 0 ; j < 300; j++){
               gTrace->SetPoint(j, j, trace[i][j]);
            }
            continue;
         }
         
         if( traceMethod == 1){
            double base = 0;
            for( int j = 0; j < traceLength; j++){ 
               if( trace[i][j] < 16000){
                  base = trace[i][j];
                  gTrace->SetPoint(j, j, trace[i][j]);
               }else{
                  gTrace->SetPoint(j, j, base);
               }
            }
            
            //Set gFit
            gFit->SetLineColor(idKind == 0 ? 2 : idKind);
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
         }
         
         switch (idKind) {
            case 0 : 
               te[idDet] = gFit->GetParameter(0);
               te_t[idDet] = gFit->GetParameter(1); 
               break;
            case 9 : ttac_t        = gFit->GetParameter(1); break; 
         }
      
      } // End NumHits Loop
   }// end of trace

   //Clock
   /************************************************************************/
   saveFile->cd(); //set focus on this file
   newTree->Fill();  

   gClock.Stop("timer");
   Double_t time = gClock.GetRealTime("timer");
   gClock.Start("timer");

   if ( !shown ) {
      if (fmod(time, 10) < 1 ){
         printf( "%10lld[%2d%%]|%3.0f min %5.2f sec | expect:%5.2f min\n", 
               entry, 
               TMath::Nint((entry+1)*100./EffEntries),
               TMath::Floor(time/60.), 
               time - TMath::Floor(time/60.)*60.,
               EffEntries*time/(entry+1.)/60.);
               shown = 1;
      }
   }else{
      if (fmod(time, 10) > 9 ){
         shown = 0;
      }
   }
   
   return kTRUE;
}

void GeneralSortTrace::SlaveTerminate()
{

}

void GeneralSortTrace::Terminate()
{
   newTree->Write();
   int validCount = newTree->GetEntries();
   saveFile->Close();

   printf("=======================================================\n");
   printf("----- Total processed entries : %3.1f k\n",EffEntries/1000.0);
   gClock.Stop("timer");
   Double_t time = gClock.GetRealTime("timer");
   printf("----- Total run time : %6.0f sec \n", time);
   printf("----- Rate for sort: %6.3f k/sec\n",EffEntries/time/1000.0);
   printf("----- saved as %s. valid event: %d ", "trace_2.root", validCount); 
}
