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


Int_t idKindMap[160] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
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
Int_t     CrapPrint = 0;
int countValidPSDEvent = 0;

bool isTraceON = true;
int traceLength = 200;

bool isTACRF = false;
bool isRecoil = false;
bool isElum = false;
bool isEZero = false;

//trace
TClonesArray * arr ;//!
TGraph * gTrace; //!
TF1 * gFit; //!
double tBase[100];
double tEnergy[100];
double tTime[100];
double tRiseTime[100];
double tChiSq[100];

//PSD struct
typedef struct {
   Float_t Energy[100];
   Float_t XF[100];
   Float_t XN[100];
   Float_t Ring[100];
   Float_t RDT[100];
   Float_t TAC[100];
   Float_t ELUM[32];
   Float_t EZERO[4];//0,1 - DE0,E0
   // 2,3 - DEX,EX

   //TODO replace timestamp to sec
   ULong64_t EnergyTimestamp[100];
   ULong64_t XFTimestamp[100];
   ULong64_t XNTimestamp[100];
   ULong64_t RingTimestamp[100];
   ULong64_t RDTTimestamp[100];
   ULong64_t TACTimestamp[100];
   ULong64_t ELUMTimestamp[32];
   ULong64_t EZEROTimestamp[4];
} PSD;

PSD psd; 

void GeneralSortTrace::Begin(TTree * tree)
{
   
   TString option = GetOption();
   NumEntries = tree->GetEntries();
   EffEntries = TMath::Min(MaxProcessedEntries, NumEntries);
   
   printf( "========== Make a new tree with trace, total Entry : %d, use : %d\n", NumEntries, EffEntries);
   printf( "is TAC/RF on : %s \n", isTACRF ? "true" : "false");
   printf( "is Recoil on : %s \n", isRecoil ? "true" : "false");
   printf( "is Elum   on : %s \n", isElum ? "true" : "false");
   printf( "is EZero  on : %s \n", isEZero ? "true" : "false");

   saveFile = new TFile("trace_2.root","RECREATE");
   newTree = new TTree("tree","PSD Tree w/ trace");

   newTree->Branch("e",psd.Energy,"Energy[100]/F");
   newTree->Branch("e_t",psd.EnergyTimestamp,"EnergyTimestamp[100]/l");

   newTree->Branch("xf",psd.XF,"XF[100]/F");
   newTree->Branch("xf_t",psd.XFTimestamp,"XFTimestamp[100]/l");

   newTree->Branch("xn",psd.XN,"XN[100]/F");
   newTree->Branch("xn_t",psd.XNTimestamp,"XNTimestamp[100]/l"); 

   if( isRecoil){
      newTree->Branch("rdt",psd.RDT,"RDT[100]/F");
      newTree->Branch("rdt_t",psd.RDTTimestamp,"RDTTimestamp[100]/l"); 
   }
   
   if( isTACRF ){
      newTree->Branch("tac",psd.TAC,"TAC[100]/F");
      newTree->Branch("tac_t",psd.TACTimestamp,"TACTimestamp[100]/l"); 
   }
   
   if( isElum ) {
      newTree->Branch("elum",psd.ELUM,"ELUM[32]/F");
      newTree->Branch("elum_t",psd.ELUMTimestamp,"ELUMTimestamp[32]/l"); 
   }
   
   if( isEZero ){
      newTree->Branch("ezero",psd.EZERO,"EZERO[4]/F");
      newTree->Branch("ezero_t",psd.EZEROTimestamp,"EZEROTimestamp[4]/l"); 
   }
   if( isTraceON ){
      arr = new TClonesArray("TGraph");
      newTree->Branch("trace", arr, 256000);
      arr->BypassStreamer();
      
	   gFit = new TF1("gFit", "[0]/(1+TMath::Exp(-(x-[1])/[2]))+[3]", 0, 140);
      
      newTree->Branch("tBase",     tBase,     "tBase[100]/D");
      newTree->Branch("tEnergy",   tEnergy,   "tEnergy[100]/D");
      newTree->Branch("tTime",     tTime,     "tTime[100]/D");
      newTree->Branch("tRiseTime", tRiseTime, "tRiseTime[100]/D");
      newTree->Branch("tChiSq",    tChiSq,    "tChiSq[100]/D");
      //TODO seperate detID, reduce array size as NumHits
      //newTree->Branch("tChiSq",    tChiSq,    "tChiSq[NumHits]/D");
   }
   
   
   StpWatch.Start();
   
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
   ProcessedEntries++;
   if(ProcessedEntries >= MaxProcessedEntries) return kTRUE;


/**///======================================= Zero struct
   for (Int_t i=0;i<100;i++) {//num dets
      psd.Energy[i]  = TMath::QuietNaN();
      psd.XF[i]      = TMath::QuietNaN();
      psd.XN[i]      = TMath::QuietNaN();
      psd.Ring[i]    = TMath::QuietNaN();
      psd.RDT[i]     = TMath::QuietNaN();
      psd.TAC[i]     = TMath::QuietNaN();
      if (i<32) psd.ELUM[i] = TMath::QuietNaN();
      if (i<4) psd.EZERO[i] = TMath::QuietNaN();

      psd.EnergyTimestamp[i] = 0;
      psd.XFTimestamp[i]     = 0;
      psd.XNTimestamp[i]     = 0;
      psd.RingTimestamp[i]   = 0;
      psd.RDTTimestamp[i]    = 0;
      psd.TACTimestamp[i]    = 0;
      if (i<32) psd.ELUMTimestamp[i] = 0;
      if (i<4) psd.EZEROTimestamp[i] = 0;	    
   }
   
   if( isTraceON ){
      for(int i = 0; i < 100; i++){
         tBase[i]     = TMath::QuietNaN();
         tEnergy[i]   = TMath::QuietNaN();
         tTime[i]     = TMath::QuietNaN();
         tRiseTime[i] = TMath::QuietNaN();
         tChiSq[i]    = TMath::QuietNaN();
      }
      arr->Clear();
   }

/**///======================================= Pull needed entries
   b_NumHits->GetEntry(entry);
   b_id->GetEntry(entry);
   b_pre_rise_energy->GetEntry(entry);
   b_post_rise_energy->GetEntry(entry);
   //   b_base_sample->GetEntry(entry);
   //    b_baseline->GetEntry(entry);
   b_event_timestamp->GetEntry(entry);
   if( isTraceON ) b_trace->GetEntry(entry);

   //ID PSD Channels
   Int_t idKind  = -1;
   Int_t idDet   = -1; // Detector number

   /* -- Loop over NumHits -- */
   int iValidHits = 0;
   for(Int_t i=0;i<NumHits;i++) {
      Int_t psd8Chan = id[i]%10;     
      Int_t idTemp   = id[i] - idConst;
      idDet  = idDetMap[idTemp];
      idKind = idKindMap[idTemp];
      
      if( (id[i] > 1000 && id[i] < 2000) && psd8Chan<8 &&  idDet>-1 ) { //IF PSD	valid
         
         //Information
         //if (idDet<0 && CrapPrint==0) {printf("ohhhhhhhhhhh craaaaaaap\n"); CrapPrint=1;}
         //if (ProcessedEntries < NUMPRINT) {
         //   printf("id %i, idKind %i, idDet %i, idConst %i\n",id[i],idKind,idDet,idConst);
         //}
         
         switch(idKind){
            case 0: /* Energy signal */
               psd.Energy[idDet] = ((float)(post_rise_energy[i])-(float)(pre_rise_energy[i]))/M;
               psd.EnergyTimestamp[idDet] = event_timestamp[i];
               break;
            case 1: // XF
               psd.XF[idDet] = ((float)(post_rise_energy[i])-(float)(pre_rise_energy[i]))/M;
               psd.XFTimestamp[idDet] = event_timestamp[i];
               break;
            case 2: // XN
               psd.XN[idDet] = ((float)(post_rise_energy[i])-(float)(pre_rise_energy[i]))/M;
               psd.XNTimestamp[idDet] = event_timestamp[i];
               break;
         }
         
         //============= trace
         if( isTraceON ) {
            gTrace = (TGraph*) arr->ConstructedAt(iValidHits);
            gTrace->Clear();
            
            double base = 0;
            for( int j = 0; j < traceLength; j++){ 
               if( trace[iValidHits][j] < 16000){
                  base = trace[iValidHits][j];
                  gTrace->SetPoint(j, j, trace[iValidHits][j]);
               }else{
                  gTrace->SetPoint(j, j, base);
               }
            }

	         base = gTrace->Eval(1);
	         double temp = gTrace->Eval(80) - base;
	
	         gFit->SetParameter(0, temp);
	         gFit->SetParameter(3, base);
            gFit->SetParameter(2, 1); //riseTime
	            
	         //sometimes, the trace will drop 
	         if( gTrace->Eval(120) < base ) {
	            gFit->SetRange(0, 100);
            }else{
               gFit->SetRange(0, 170);
            }
            //sometimes, the trace drop after 5 ch
            if( gTrace->Eval(20) < base){
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
           
         }// end of trace
         
         iValidHits ++;
      }//end of PSD valid
      
      //TAC & RF TIMING
      /***********************************************************************/
      if( isTACRF && (id[i]>1000&&id[i]<2000)&&(idDet>=400&&idDet<=450)) { //RF TIMING SWITCH
         //if (ProcessedEntries < NUMPRINT) printf("RF id %i, idDet %i\n",id[i],idDet);

         Int_t tacTemp = idDet-400;
         psd.TAC[tacTemp] = ((float)(post_rise_energy[i])-(float)(pre_rise_energy[i]))/M;
         psd.TACTimestamp[tacTemp] = event_timestamp[i];
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
   } // End NumHits Loop
   
   if( iValidHits >= 1) countValidPSDEvent ++; // countValidPSDEvent for valid event for Si array
   if( iValidHits == 0 && !isTACRF && !isRecoil && !isElum && !isEZero ) return kTRUE;
      
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
  saveFile->Close();

  printf("=======================================================\n");
  printf("Total processed entries : %3.1f k\n",EffEntries/1000.0);
  gClock.Stop("timer");
  Double_t time = gClock.GetRealTime("timer");
  printf("Rate for sort: %3.1f k/sec\n",EffEntries/time/1000.0);
  printf("-------- saved as %s. valid PSD event: %d ", "trace_2.root", countValidPSDEvent); 
}
