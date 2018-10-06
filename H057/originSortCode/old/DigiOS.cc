#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <math.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>

#include "TROOT.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TObject.h"
#include "TTree.h"
#include "TObjArray.h"
#include "TGraph.h"
#include "TCanvas.h"
#include "TString.h"
#include "TBox.h"
#include "TLine.h"

#include "DigiOS.h"

// add a comment
TCanvas *cc = new TCanvas("cc","Graph Draw",200,10,600,400);
int DetCh[10];
unsigned short int tDataY[4096];
unsigned short int tDataX[4096];
float tSmooth[4096];
long long EventNum=0;
unsigned long long int LEDTimeStamp0 = 0;
unsigned long long int CFDTimeStamp0 = 0;
unsigned long long int TempTS = 0;

unsigned short int GA=0;

FILE *DataFileIn;
char DataName[255];
int  nBadTestPat;
DigiOS_HEADER Header;
DigiOS_TRACE Trace;
DigiOS_EVENT Event;

TFile *f;
TTree *tree;
TObjArray *RootObjects;
char RootName[255];

//--- Event limit
long MaxEventNum;
//--- LED Params
Int_t k;
Float_t LEDThresh;
//--- CFD ---//
Int_t Delay;
Float_t Fraction;
Int_t CFDCross;
//--- ESum (Not Trapezoid) ---//
Int_t D1;
Int_t D2;
Int_t M1;
Int_t M2;
//--- Event Build ---//
int OverlapTime;

//---UDR ---//
FILE* pFile;
char* buffer;
unsigned long lSize;
size_t result;
char* cKeyword;
char* cResult = NULL;

//--- Relative TS ---/
double RFN=0;
double RFTime=82.474; //ns
//double LEDtsRelative=0.0;

void UserDataRead() { //--UDR--//
  std::stringstream ss;
  int a;
  
  //--- file read from http://www.cplusplus.com/reference/cstdio/fread/?kw=fread ---//
  pFile = fopen("./UserDataFile.txt","rb");
  if (pFile==NULL) {fputs ("User File Error",stderr); exit (1);}
  
  fseek(pFile, 0, SEEK_END);
  lSize = ftell(pFile);
  rewind(pFile);
  
  buffer = (char*)malloc(sizeof(char)*lSize); // allocate memory
  if (buffer == NULL) {fputs ("Memory error",stderr); exit (2);}
  
  result = fread(buffer, 1, lSize, pFile);
  if (result != lSize) {fputs ("Reading error",stderr); exit (3);}
  
  //--- strstr use from http://www.cplusplus.com/reference/cstring/strstr/ ---//
  cKeyword = strstr(buffer,"MAXEVENTS;"); 
  if (cKeyword != NULL) {
    cResult = cKeyword+strlen("MAXEVENTS;");
    ss<<cResult;
    ss>>MaxEventNum;
    printf("MAXEVENTS=%ld\n",MaxEventNum);
  }
  ss.str(""); ss.clear();
  
  cKeyword = strstr(buffer,"LED;"); 
  if (cKeyword != NULL) {
    cResult = cKeyword+strlen("LED;");
    ss<<cResult;
    ss>>k;
    printf("LED k=%d\n",k);
    ss<<cResult;
    ss>>LEDThresh;
    printf("LEDThresh=%f\n",LEDThresh);
  }
  ss.str(""); ss.clear();
    
  cKeyword = strstr(buffer,"CFD;"); 
  if (cKeyword != NULL) {
    cResult = cKeyword+strlen("CFD;");
    ss<<cResult;
    ss>>Delay;
    printf("CFD Delay=%d\n",Delay);
    ss<<cResult;
    ss>>Fraction;
    printf("CFD Fraction=%f\n",Fraction); 
    ss<<cResult;
    ss>>CFDCross;
    printf("CFDCross=%d\n",CFDCross);
  } 
 
  ss.str(""); ss.clear();
  cKeyword = strstr(buffer,"ENERGY;"); 
  if (cKeyword != NULL) {
    cResult = cKeyword+strlen("ENERGY;");
    ss<<cResult;
    ss>>D1;
    printf("ENERGY D1=%d\n",D1);
    ss<<cResult;
    ss>>D2;
    printf("ENERGY D2=%d\n",D2); 
    ss<<cResult;
    ss>>M1;
    printf("ENERGY M1=%d\n",M1);
    ss<<cResult;
    ss>>M2;
    printf("ENERGY M2=%d\n",M2);
  }
  ss.str(""); ss.clear();
  
  cKeyword = strstr(buffer,"EVENT;"); 
  if (cKeyword != NULL) {
    cResult = cKeyword+strlen("EVENT;");
    ss<<cResult;
    ss>>OverlapTime;
    printf("EVENT OverlapTime=%d\n",OverlapTime);
  }
  ss.str(""); ss.clear();
  
  cKeyword = strstr(buffer,"CHANNELMAP;"); 
  if (cKeyword != NULL) {
    cResult = cKeyword+strlen("CHANNELMAP;");
    for (int i=0;i<10;i++){
      ss<<cResult;
      ss>>a;
      DetCh[i] = a;
      printf("CHANNELMAP%d=%d\n",i,DetCh[i]);
    }
  }

  free(buffer); // free memory
  fclose(pFile);
}//&(*_*)&

void SetupRoot(){

  f = new TFile(RootName,"RECREATE");

  //TTree
  tree = new TTree("tree","DigIOS Event Tree");
  tree->Branch("Event",Event.EnergyMult,"EnergyMult[2]/I:Energy[2]/F:EnergyTimeStamp[2]/D:EnergyRiseTime[2]/F");
  tree->Branch("Event",Event.XNearMult,"XNearMult[2]/I:XNear[2]/F:XNearTimeStamp[2]/D:XNearRiseTime[2]/F");
  tree->Branch("Event",Event.XFarMult,"XFarMult[2]/I:XFar[2]/F:XFarTimeStamp[2]/D:XFarRiseTime[2]/F");
  tree->Branch("Event",Event.RingMult,"RingMult[2]/I:Ring[2]/F:RingTimeStamp[2]/D:RingRiseTime[2]/F");
  
  RootObjects = new TObjArray();
  RootObjects->Add(tree);

}

//--- Draw Trace ---//----------//---------------//------------------//---------------------//
void DrawTrace(long long EvtNum=0) {
  Int_t n=1025;
  Double_t x[n],y[n];
  Double_t xS[n],yS[n];
  Double_t ymin=16000,ymax=0;
  Double_t xmin=0,xmax=1000;

  for (Int_t i=0;i<1000;i++) {
    x[i] = (Double_t)i/100.;
    y[i] = (Double_t)(Trace.Data[i] & 0x3fff);
    if ((y[i]<ymin)&&(y[i]>1000)) ymin = y[i];
    if ((y[i]>ymax)&&(y[i]<16384)) ymax = y[i];
    xS[i] = (Double_t)i/100.;
    yS[i] = (Double_t)Trace.Smooth[i];
    //if ((yS[i]<ymin)&&(yS[i]>1000)) ymin = y[i];
    //if ((yS[i]>ymax)&&(yS[i]<16384)) ymax = y[i];
  }

  TGraph *gTrace = new TGraph(n,x,y);
  gTrace->SetMinimum(ymin-500); gTrace->SetMaximum(ymax+500);gTrace->Draw("AP");

  TGraph *gSmooth = new TGraph(n,xS,yS);
  gSmooth->SetMinimum(ymin-500); gSmooth->SetMaximum(ymax+500);//gSmooth->Draw("AP");

  xmin = (double)Trace.LEDts/100.;
  xmax = (double)Trace.LEDts/100.;

  TLine *lLED = new TLine(xmin,ymin,xmax,ymax);
  TLine *lCFD = new TLine(Trace.CFDts/100.,ymin,Trace.CFDts/100.,ymax);
  //printf("LED: %f CFD: %f\n",Trace.LEDts/100.,Trace.CFDts/100.);

  lLED->SetLineColor(kBlue); lCFD->SetLineColor(kRed);
  lLED->SetLineWidth(1); lCFD->SetLineWidth(1);
  xmin=2.5;
  xmax=4.5;

  xmin = (double)Trace.CFDp2 - (double)D1 - (double)M1;
  xmax = xmin + (double)M1;
  TBox *bM1 = new TBox(xmin/100.,ymin,xmax/100.,ymax);
  bM1->SetFillColor(kBlue-7);bM1->Draw("same");
  TBox *bM2 = new TBox((Trace.CFDp2+D2)/100.,ymin,(Trace.CFDp2+D2+M2)/100.,ymax);
  bM2->SetFillColor(kBlue-9);bM2->Draw("same");

  xmin=Trace.RiseMinTS10/100.; xmax=Trace.RiseMaxTS90/100.;
  ymin=((Trace.RiseMax-Trace.RiseMin)*0.1+Trace.RiseMin);
  ymax=((Trace.RiseMax-Trace.RiseMin)*0.9+Trace.RiseMin);
  TBox *bRT = new TBox(xmin,ymin,xmax,ymax);
  bRT->SetFillColor(kBlue-10);bRT->Draw("same");
 lLED->Draw(); lCFD->Draw();

  gTrace->SetTitle(Form("Number: %lld, ID: %u(%d), TS: %2.9f, RT: %4.2f",EvtNum,Header.id,DetCh[Header.id],Header.LEDtsAbs,Trace.RiseTime));
  gTrace->GetXaxis()->SetRangeUser(2.0,5.0);
  gTrace->SetMarkerStyle(21);gTrace->SetMarkerSize(0.2);

  gSmooth->SetLineColor(kGray);gSmooth->Draw("L");
  gTrace->Draw("P");

  TString cnam("Canvas");
  cnam+=EvtNum; cnam+=".pdf";
  cc->SaveAs(cnam);
  gTrace->Delete();

}
//------//----------//---------------//------------------//---------------------//

//////////////////////////////////////////////////////////////////////////////
void ResetEvent() {
  for (Int_t i=0;i<2;i++) {
  Event.EnergyMult[i]=0;
  Event.Energy[i]=0;
  Event.EnergyTimeStamp[i]=0;
  Event.EnergyRiseTime[i]=0;
  Event.XNearMult[i]=0;
  Event.XNear[i]=0;
  Event.XNearTimeStamp[i]=0;
  Event.XNearRiseTime[i]=0;
  Event.XFarMult[i]=0;
  Event.XFar[i]=0;
  Event.XFarTimeStamp[i]=0;
  Event.XFarRiseTime[i]=0;  
  Event.RingMult[i]=0;
  Event.Ring[i]=0;
  Event.RingTimeStamp[i]=0;
  Event.RingRiseTime[i]=0;
  }
}

//////////////////////////////////////////////////////////////////////
void ProcessHeader(){
  //Trace.LEDtsRel = 0.0; // need a reset for trace

  //--- All time stamps relative to first ---//
  if (EventNum==0) LEDTimeStamp0 = Header.LEDts;
  //printf("LEDts: %1.10f\n\n",((double)Header.LEDts-(double)LEDTimeStamp0)/100000000.); 
  Header.LEDtsAbs = ((double)Header.LEDts-(double)LEDTimeStamp0)/100000000.;// [sec]
}


//////////////////////////////////////////////////////////////////////
void ProcessTrace(){
  //--- LED
  Int_t d1,d2,d3,d4,d5;
  Float_t dd1,dd2,dd3;
  Int_t SumLED= k+2;
  //--- CFD
  Float_t TempCFD=16384.0;
  Float_t Base;
  //---Energy
  Int_t I,J;

  
  // need a reset function!
  for(Int_t i=0;i<Trace.Len;i++){
    tDataY[i]=0;
    tDataX[i]=0;
    tSmooth[i]=0.0;
    Trace.tLED[i]=0;
    Trace.tCFD[i]=0;
    
    tDataY[i] = (Trace.Data[i] & 0x3fff);
    tDataX[i]=i;
  }
  
  // need a reset function!
  Trace.LEDp2=0; Trace.LEDp1=0; Trace.LEDts=0;
  
  //--- Determine LED ---//
  for (Int_t i=SumLED;i<1024;i++) {
    d1 = tDataY[i-2]-tDataY[i-k-2];
    d2 = tDataY[i-1]-tDataY[i-k-1];
    d3 = tDataY[i]-tDataY[i-k];
    d4 = tDataY[i+1]-tDataY[i-k+1];
    d5 = tDataY[i+2]-tDataY[i-k+2];
    
    dd1 = ((Float_t)d1+2*(Float_t)d2+(Float_t)d3)/4.;
    dd2 = ((Float_t)d2+2*(Float_t)d3+(Float_t)d4)/4.;
    dd3 = ((Float_t)d3+2*(Float_t)d4+(Float_t)d5)/4.;
    Trace.tLED[i] = (dd1+2*dd2+dd3)/4.;
    
    if ((Trace.LEDp2==0) && (Trace.LEDp1==0) && (Trace.tLED[i]>LEDThresh)) {
      Trace.LEDp1 = i-1; // over thresh then set one below
      Trace.LEDp2 = i; // one above
     }
  }
  /* determine cross from linear interp */
  Float_t Slope = (Trace.tLED[Trace.LEDp2] - Trace.tLED[Trace.LEDp1]);
  Trace.LEDts = (Float_t)Trace.LEDp2 - (Trace.tLED[Trace.LEDp2] - LEDThresh) / Slope;
  
  //  Trace.LEDtsRel = Trace.LEDtsRel - ((double)Trace.LEDp2-(double)Trace.LEDts)*10.0;

  // need a reset function!
  Trace.CFDp2=0; Trace.CFDp1=0; Trace.CFDts=0;
  
  //--- Determine CFD values ---//
  Base = (Float_t)tDataY[Trace.LEDp2];
  
  for (Int_t i=Delay;i<1024;i++) {
    //--- Calculate CFD ---//
    Trace.tCFD[i-Delay] = -1.0*((Float_t)tDataY[i] - Base)*Fraction + ((Float_t)tDataY[i-Delay] - Base);
    //--- Find min in CFD spectrum ---//
    //--- had to add in the 500 because it kept choosing ---//
    //--- the end of the trace (974)                     ---//
    if (Trace.tCFD[i-Delay]<TempCFD&&(i-Delay)<500) {
      TempCFD = Trace.tCFD[i-Delay];
      CFDCross = i-Delay;
    }
  }
  
  //--- Determine (starting from min) where 0 crossing is ---//
  while(Trace.tCFD[CFDCross]<0.0)CFDCross++;
  Trace.CFDp2 = CFDCross; Trace.CFDp1 = CFDCross-1;  
  Slope = (Trace.tCFD[Trace.CFDp2] - Trace.tCFD[Trace.CFDp1]);
  Trace.CFDts = (Float_t)Trace.CFDp2 - (Trace.tCFD[Trace.CFDp2] - 0.0) / Slope;
   
  //--- Energy determination ---//
  // need a reset function!
  Trace.PreEnergy=0; Trace.PostEnergy=0; Trace.Energy=0;
  
  //-- Actual Pre and Post Calcs ---//
  if (Trace.CFDp2-D1-M1>=0 && Trace.CFDp2+D2+M2<1024) {
    J = Trace.CFDp2 - D1;
    for (I=Trace.CFDp2-D1-M1;I<J;I++) {
      Trace.PreEnergy = Trace.PreEnergy + (Float_t)tDataY[I]/(Float_t)M1;
    }
    J = Trace.CFDp2 + D2 + M2;
    for (I=Trace.CFDp2+D2;I<J;I++) {
      //No pole zero here yet
      Trace.PostEnergy = Trace.PostEnergy + ((Float_t)tDataY[I])/(Float_t)M2;
    }
  }
  
  Trace.Energy = Trace.PostEnergy - Trace.PreEnergy;
  // need a reset function!
  Trace.RiseMax = 0.0; Trace.RiseMin = 16384.0;

  //--- Rise time determination ---//
  tSmooth[0]=(float)tDataY[0];
  tSmooth[1]=((float)tDataY[0]+(float)tDataY[1]+(float)tDataY[2])/3.;
  tSmooth[2]=((float)tDataY[0]+(float)tDataY[1]+(float)tDataY[2]+(float)tDataY[3]+(float)tDataY[4])/5.;
  tSmooth[3]=((float)tDataY[0]+(float)tDataY[1]+(float)tDataY[2]+(float)tDataY[3]+(float)tDataY[4]+(float)tDataY[5]+(float)tDataY[6])/7.;
  for(Int_t i=4;i<Trace.Len-1;i++)
    {
      tSmooth[i] = ((float)tDataY[i-4] + (float)tDataY[i-3] +(float)tDataY[i-2] + (float)tDataY[i-1] + (float)tDataY[i] + (float)tDataY[i+1] + (float)tDataY[i+2] + (float)tDataY[i+3] + (float)tDataY[i+4])/9.;
      Trace.Smooth[i] = tSmooth[i];
    }

  //--- Get min and max of smooth within the rise area ---//
  Int_t iMin=0;
  Int_t iMax=0;

  for (Int_t i=Trace.CFDp2-D1-M1;i<Trace.CFDp2;i++) {
    if (tSmooth[i]<Trace.RiseMin) {
      Trace.RiseMin=tSmooth[i];
      iMin=i;
    }
  }
 
  for (Int_t i=Trace.CFDp2;i<Trace.CFDp2+D2+M2;i++) {
    if (tSmooth[i]>Trace.RiseMax) {
      Trace.RiseMax=tSmooth[i];
      iMax=i;
    }
  }

  //  printf("Max: %f, Min: %f\n",Trace.RiseMax,Trace.RiseMin);
  // printf("Max - Min: %f\n",Trace.RiseMax-Trace.RiseMn);

  int iTen=0;
  int iNinty=0;
  float fTen=0.0;
  float fNinty=0.0;

  //--- Now determine 10% and 90% points get difference to get RiseTime ---//
  for(Int_t i=iMin;i<iMax;i++) {
    if ((tSmooth[i]>((Trace.RiseMax-Trace.RiseMin)*0.1+Trace.RiseMin))&&(iTen==0)) {
      iTen=i;
      Slope = (tSmooth[iTen] - tSmooth[iTen-1]);
      fTen = (Float_t)iTen - (tSmooth[iTen] - ((Trace.RiseMax-Trace.RiseMin)*0.1+Trace.RiseMin)) / Slope;
      Trace.RiseMinTS10 = fTen;
    }
    
    if ((tSmooth[i]>((Trace.RiseMax-Trace.RiseMin)*0.9+Trace.RiseMin))&&(iNinty==0)) {
      iNinty=i; 
      Slope = (tSmooth[iNinty] - tSmooth[iNinty-1]);
      fNinty = (Float_t)iNinty - (tSmooth[iNinty] - ((Trace.RiseMax-Trace.RiseMin)*0.9+Trace.RiseMin)) / Slope;
      Trace.RiseMaxTS90 = fNinty;
    }
  }
  
  //  printf("Ten: %f - %f - %f, Ninty: %f - %f\n",fTen,tSmooth[iTen],tSmooth[iTen-1],fNinty,tSmooth[iNinty]);

  Trace.RiseTime = (fNinty-fTen)*10.0;
  
}//(*_*)//

//////////////////////////////////////////////////////////////////////
void ProcessEvent(){
  long long int TempDT;
  //--- Create Events from Header.LEDts but write Trace.LEDts to Event ---//
  if (EventNum==0) {TempDT=0;} else {TempDT =  Header.LEDts - TempTS;}
  TempTS = Header.LEDts;
  
  //--- Big jump in time set and write data
  if ((TempDT<(-1*OverlapTime)) || (TempDT>OverlapTime)) {//*_*
    tree->Fill();
    //--- Reset all Event Params ---//
    ResetEvent();
  }//*_*
   //printf("LEDtsRel: %1.10f\n\n",Trace.LEDtsRel);
  //--- Set Params of current event 
  int HighLow=-1;
  if (Header.id<5) {HighLow=0;} else {HighLow=1;}
  // printf("Header.id: %u, HighLow: %d\n",Header.id,HighLow);
  if (HighLow>-1) {
    switch (DetCh[Header.id]) 
      {
      case 0:
	Event.EnergyMult[HighLow]++;
	Event.Energy[HighLow] = Trace.Energy;
	Event.EnergyTimeStamp[HighLow] = Header.LEDtsAbs;
	Event.EnergyRiseTime[HighLow] = Trace.RiseTime;
	break;
      case 1:
	Event.XNearMult[HighLow]++;
	Event.XNear[HighLow] = Trace.Energy;
	Event.XNearTimeStamp[HighLow] = Header.LEDtsAbs;
	Event.XNearRiseTime[HighLow] = Trace.RiseTime;
	break;
      case 2:
	Event.XFarMult[HighLow]++;
	Event.XFar[HighLow] = Trace.Energy;
	Event.XFarTimeStamp[HighLow] = Header.LEDtsAbs;
	Event.XFarRiseTime[HighLow] = Trace.RiseTime;
	break;
      case 3:
	Event.RingMult[HighLow]++;
	Event.Ring[HighLow] = Trace.Energy;
	Event.RingTimeStamp[HighLow] = Header.LEDtsAbs;
	Event.RingRiseTime[HighLow] = Trace.RiseTime;
	break;
      case 4:
	break;
      default:
	printf("Error in DetCh switch: %d\n",Header.id);
      }
  }
}/*_*/


int GetEv(DigiOS_HEADER *Header, DigiOS_TRACE *Trace) { 
  /* function will read the gretina data file and */
  /* extract the events, but not interpret them. */
  /* That is done higher up. All this function does */
  /* is to pull out the header and the trace */
  /* and send them back to to calling function */
  /* through the DigiOS_HEADER Header structure */
  /* this function is specific to data file processing */
  
  /* declarations */
  
  static int        nn = 0;
  int               i;
  unsigned int      testPattern=0;
  int               Counter=0;
  int               TEMP[MAXLENINTS];
  int               t1, t2, t3, t4;
  int               EventLen;
  
  /* normally we have one EOE marks */
  /* when we start reading......... */
  /* note: no need to swap bytes... */
  
  while(testPattern != EOE){
    if (fread(&testPattern,sizeof(int),1,DataFileIn) != 1){
      if (feof(DataFileIn)){
      	printf("End of file %s\n",DataName);
        return 0;
      }
      printf("file read error %s\n",DataName);
      return 0;
    }
    if(Counter<MAXLENINTS) TEMP[Counter]=testPattern;
    else {printf("counter=%i\n",Counter);return 2;}
    Counter++;                                           //Counter = EventLength + 1
  }
  
  if(Counter==1){
    //printf("** File Header or Double EventHeader **\n");
    return 2;
  }
  
  for(i = 0; i < Counter-1; i++)                        //Swap bytes
    if(i<MAXLENINTS){
      t1 = (TEMP[i] & 0x000000ff) << 24;
      t2 = (TEMP[i] & 0x0000ff00) << 8;
      t3 = (TEMP[i] & 0x00ff0000) >> 8;
      t4 = (TEMP[i] & 0xff000000) >> 24;
      TEMP[i] = t1 + t2 + t3 + t4;
    }
  
  Header->id = (unsigned short int) (TEMP[0] & 0xf);
  Header->board_id = (unsigned short int)(TEMP[0] >> 4);
  EventLen = (unsigned short int) ((TEMP[0] >>= 16) & 0x7ff) + 1;
  GA = (unsigned short int) ((TEMP[0] >>= 11) & 0x1f);
  
  if(EventLen != Counter ){                          //inconsistent event length
    if(Counter <= HDRLENINTS){                         //unreasonable EOE markers
      printf("Bad event appeared, search ahead\n");
      return 2;
    }
    if(Counter>HDRLENINTS){     //reasonable EOE markers, we can try to get data out
      nBadTestPat++;
      if((nBadTestPat % 1000) == 0)
        printf("** warning: %i inconsistant EOE markers\n",nBadTestPat);
    }
  }


  Header->LEDts = 0;
  Header->LEDts = (unsigned long long int) (TEMP[1] & 0xffffffff); //Need this for correct ts's.
  Header->LEDts+=((unsigned long long int) (TEMP[2] & 0xffff)  << 32);
  
  Header->rawE = (int)((unsigned int)((TEMP[2] >> 16) & 0xffff));
  Header->rawE+= (int)(((unsigned int)(TEMP[3] & 0xf)) << 16);

  /* extract various bits */

  if (TEMP[3] & 0x8000)
    Header->pu = 1;
  else
    Header->pu = 0;

  Header->CFDts = (unsigned long long int) ((TEMP[3] >> 16) & 0xffff);
  Header->CFDts+=((unsigned long long int) TEMP[4] << 16);

  Header->CFDp1=TEMP[5];
  Header->CFDp2=TEMP[6];

  Header->PreEnergy=(TEMP[7] & 0xffffff);

  Header->PostEnergy = ((TEMP[7] >> 24) & 0xff);
  Header->PostEnergy+= ((TEMP[8] & 0xffff) << 8);

  Header->PEAKts = (unsigned long long int) ((TEMP[8] >> 16) & 0xffff);
  Header->PEAKts+=((unsigned long long int) TEMP[9] << 16);

  Header->M1BeginSample = (TEMP[10] & 0xffff);
  Header->M1EndSample = ((TEMP[10] >> 16) & 0xffff);

  Header->M2BeginSample = (TEMP[11] & 0xffff);
  Header->M2EndSample = ((TEMP[11] >> 16) & 0xffff);

  Header->PeakSample = (TEMP[12] & 0xffff);
  Header->BaseSample = ((TEMP[12] >> 16) & 0xffff);

  Trace->Len = 0;
  for(i = 13; i < Counter-1; i++)
    if(i<1037){
      Trace->Data[2*(i-13)] = (unsigned short int) (TEMP[i] & 0xffff);
      Trace->Data[2*(i-13)+1] = (unsigned short int) ((TEMP[i] >> 16) & 0xffff);
      Trace->Len += 2;
    }

  nn++;
  fflush(stdout);
  return 1;
}

//////////////////////////////////////////////////////////////////////////////
int main (int argc, char **argv)
{
  int status;
  int Counter=0;

  if (argc!=3) {
    printf("USAGE: RootFileName DataName\n");
    exit(-1);
  }

  UserDataRead();
  
  printf("RootName is %s\n",argv[1]);
  strcpy(RootName,argv[1]);
  SetupRoot();

  printf("DataName is %s\n",argv[2]);
  strcpy(DataName,argv[2]);
  DataFileIn = fopen(DataName,"rb");

  while(1){
    status=GetEv(&Header,&Trace);
    if(status==0)break;
    if(status==1){
      if(Header.board_id>0){
        ProcessHeader();
        ProcessTrace();
	ProcessEvent();
	
	if((EventNum % 100) == 0) Counter=0;
	if ((Counter < 10) && (EventNum<10000) && (Trace.RiseTime<200) && (Trace.Energy>2000) && (Trace.Energy<7000)) {
	  //DrawTrace(EventNum);
	  Counter++;
	}	 

	EventNum++;
      }
      if((EventNum % 100000) == 0)
	printf("%lld Events Processed\n",EventNum);
      if((EventNum==MaxEventNum))break;
    }
  }
  
  fclose(DataFileIn);
  printf("Total %lld Events Processed\n",EventNum);
  printf("writing histogram....\n");
  f->Write();
  f->Close();
  exit(0);
}
