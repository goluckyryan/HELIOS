#define EnergyTr_cxx

#include "EnergyTr.h"
#include <TH2.h>
#include <TStyle.h>
#include "TGraph.h"
#include "TCanvas.h"

#define NUMPRINT 10 //>0
ULong64_t NUMSORT=10000;//000000;

#define MAXNUMHITS 200 //Highest multiplicity
#define M 100 //M value for energy filter from digi setting

ULong64_t NumEntries = 0;
ULong64_t ProcessedEntries = 0;
Float_t Frac = 0.1; //Progress bar

//For traces
unsigned short int EtDataX[4096],RtDataX[4096],XFtDataX[4096],XNtDataX[4096];
unsigned short int EtDataY[4096],RtDataY[4096],XFtDataY[4096],XNtDataY[4096];
Float_t tSmooth[2000];
TCanvas *cc = new TCanvas("cc","Graph Draw",200,10,600,400);

TStopwatch StpWatch;

TFile *oFile;
TTree *psd_tree;

TH1F *hEvents;

TCanvas *cc0;

//PSD struct
typedef struct {
  //0-e, 1-xf, 2-xn, 3-r
  Float_t BaseLine[100]; //number of psd's
  Float_t Energy[100];
  Float_t XF[100];
  Float_t XN[100];
  Float_t Ring[100];
  ULong64_t EnergyTimestamp[100];
  ULong64_t XFTimestamp[100];
  ULong64_t XNTimestamp[100];
  ULong64_t RingTimestamp[100];
  Float_t EnergyRiseTime[100];
  Float_t X[100];
  //param[psd_num]//0-100
} PSD;

PSD psd; 

void EnergyTr::Begin(TTree * tree)
{
   
  TString option = GetOption();
  NumEntries = tree->GetEntries();

  hEvents = new TH1F("hEvents","Number of events; Events;",NumEntries*1.2,0,NumEntries*1.2);

  oFile = new TFile("psd.root","RECREATE");

  psd_tree = new TTree("psd_tree","PSD Tree");
  psd_tree->Branch("e",psd.Energy,"Energy[100]/F");
  psd_tree->Branch("e_t",psd.EnergyTimestamp,"EnergyTimestamp[100]/l");
  psd_tree->Branch("e_rt",psd.EnergyRiseTime,"EnergyRiseTime[100]/F");
  
  psd_tree->Branch("r",psd.Ring,"Ring[100]/F");
  psd_tree->Branch("r_t",psd.RingTimestamp,"RingTimestamp[100]/l");
  
  psd_tree->Branch("xf",psd.XF,"XF[100]/F");
  psd_tree->Branch("xf_t",psd.XFTimestamp,"XFTimestamp[100]/l");
 
  psd_tree->Branch("xn",psd.XN,"XN[100]/F");
  psd_tree->Branch("xn_t",psd.XNTimestamp,"XNTimestamp[100]/l"); 
 
  psd_tree->Branch("x",psd.X,"X[100]/F");
    
  StpWatch.Start();
}

void EnergyTr::SlaveBegin(TTree * /*tree*/)
{
  
  TString option = GetOption();
  
}

Bool_t EnergyTr::Process(Long64_t entry)
{ 
  ProcessedEntries++;
  if (ProcessedEntries<NUMSORT) {
    hEvents->Fill(ProcessedEntries);
    if (ProcessedEntries>NumEntries*Frac-1) {
      printf(" %3.0f%% (%llu/%llu Mil) processed in %6.1f seconds\n",Frac*100,ProcessedEntries/1000000,NumEntries/1000000,StpWatch.RealTime());
      StpWatch.Start(kFALSE);
      Frac+=0.1;
    }

    //Zero struct
    for (Int_t i=0;i<100;i++) {//num dets
      psd.BaseLine[i]=0; //number of psd's
      psd.Energy[i]=0;
      psd.XF[i]=0;
      psd.XN[i]=0;
      psd.Ring[i]=0;
      psd.EnergyTimestamp[i]=0;
      psd.XFTimestamp[i]=0;
      psd.XNTimestamp[i]=0;
      psd.RingTimestamp[i]=0;
      psd.EnergyRiseTime[i]=0;
      psd.X[i]=0;
    }


    for (Int_t k=0;k<1025;k++) {
      EtDataX[k]=0;EtDataY[k]=0;
      RtDataX[k]=0;RtDataY[k]=0;
      XFtDataX[k]=0;XFtDataY[k]=0;
      XNtDataX[k]=0;XNtDataY[k]=0;
    }
    
    //Pull needed entries
    b_NumHits->GetEntry(entry);
    b_id->GetEntry(entry);
    b_pre_rise_energy->GetEntry(entry);
    b_post_rise_energy->GetEntry(entry);
    b_base_sample->GetEntry(entry);
    b_baseline->GetEntry(entry);
    b_event_timestamp->GetEntry(entry);
    b_trace_length->GetEntry(entry);
    b_trace->GetEntry(entry);
    
    /* -- Loop over NumHits -- */
    for (Int_t i=0;i<NumHits;i++) {
      
      //ID PSD Channels
      if (id[i]>1000&&id[i]<2000) { //IF PSD
	Int_t idKind = id[i]%10; //e,xf,xn,or r
	Int_t idDet=-1; // Detector number
	Int_t idConst=102; //Temp value to get idDet
	idDet = (id[i]-idKind)/10-idConst;
	idKind=id[i];
	idDet=0;
	//Information
	if (idDet<0) printf("ohhhhhhhhhhh craaaaaaap\n");
	if (ProcessedEntries<NUMPRINT)
	  printf("id %i, idKind %i, idDet %i, idConst %i\n",id[i],idKind,idDet,idConst);
	
	switch(idKind)
	  {
	  case 1057: /* Energy signal */
	    psd.Energy[idDet] = ((float)(pre_rise_energy[i])-(float)(post_rise_energy[i]))/M;
	    psd.EnergyTimestamp[idDet] = event_timestamp[i];
	    for (Int_t ii=0;ii<trace_length[i]-1;ii++) {
	      EtDataX[ii]=ii; EtDataY[ii]=(trace[i][ii]);
	      //printf("here\n");
	    } 
	    break;
	  case 1051: // XF
	    psd.XF[idDet] = ((float)(pre_rise_energy[i])-(float)(post_rise_energy[i]))/M;
	    psd.XFTimestamp[idDet] = event_timestamp[i];
	    for (Int_t ii=0;ii<trace_length[i]-1;ii++) {
	      XFtDataX[ii]=ii; XFtDataY[ii]=(trace[i][ii]);
	    }
	    break;
	  case 1063: // XN
	    psd.XN[idDet] = ((float)(pre_rise_energy[i])-(float)(post_rise_energy[i]))/M;
	    psd.XNTimestamp[idDet] = event_timestamp[i];
	    for (Int_t ii=0;ii<trace_length[i]-1;ii++) {
	      XNtDataX[ii]=ii; XNtDataY[ii]=(trace[i][ii]);
	    }
	    break;
	  case 1056: //Ring
	    psd.RingTimestamp[idDet] = event_timestamp[i];
	    psd.Ring[idDet] = ((float)(pre_rise_energy[i])-(float)(post_rise_energy[i]))/M;
	    for (Int_t ii=0;ii<trace_length[i]-1;ii++) {
	      RtDataX[ii]=ii; RtDataY[ii]=(trace[i][ii]);
	    }
	    break;
	  }
      }
    } // End NumHits Loop

    // Calculate Other Parameters
    Int_t idDet=0;
    if (psd.Energy[idDet]>0) {
      psd.X[idDet] = ((psd.XF[idDet] - psd.XN[idDet]) /
		      (psd.XF[idDet] + psd.XN[idDet]));
    }

    //Process Trace
    // Try to draw a few traces
      Int_t n=1025;
      Double_t xE[1025],yE[1025];
      Double_t xR[1025],yR[1025];
      Double_t xXF[1025],yXF[1025];
      Double_t xXN[1025],yXN[1025];

      if (ProcessedEntries<NUMSORT) {
	Float_t ymin=1000,ymax=15000;
	
	for (Int_t i=0;i<1025;i++) {
	  xE[i] = (Double_t)EtDataX[i];
	  yE[i] = (Double_t)(EtDataY[i] & 0x3fff);
	  xR[i] = (Double_t)RtDataX[i];
	  yR[i] = (Double_t)(RtDataY[i] & 0x3fff);
	  xXF[i] = (Double_t)XFtDataX[i];
	  yXF[i] = (Double_t)(XFtDataY[i] & 0x3fff);
	  xXN[i] = (Double_t)XNtDataX[i];
	  yXN[i] = (Double_t)(XNtDataY[i] & 0x3fff);
	  //printf("X: %f, Y: %f\n",xE[i],yE[i]);
      }
	
	TGraph *gTraceE = new TGraph(n,xE,yE);
	gTraceE->SetMinimum(ymin); gTraceE->SetMaximum(ymax);
	gTraceE->SetMarkerColor(kBlack);gTraceE->SetLineWidth(3);
	gTraceE->Draw("AP");
	gTraceE->Draw("P");
	
	TGraph *gTraceR = new TGraph(n,xR,yR);
	gTraceR->SetMinimum(ymin); gTraceR->SetMaximum(ymax);
	gTraceR->SetMarkerColor(kRed);gTraceR->Draw("P");
	
	TGraph *gTraceXF = new TGraph(n,xXF,yXF);
	gTraceXF->SetMinimum(ymin); gTraceXF->SetMaximum(ymax);
	gTraceXF->SetMarkerColor(kBlue);gTraceXF->Draw("P");
	
	TGraph *gTraceXN = new TGraph(n,xXN,yXN);
	gTraceXN->SetMinimum(ymin); gTraceXN->SetMaximum(ymax);
	gTraceXN->SetMarkerColor(kGreen+2);gTraceXN->Draw("P");
	
	if (psd.Energy[0]<0) {
	  TString cnam("Canvas");
	  cnam+=ProcessedEntries; cnam+=".pdf";
	  if (ProcessedEntries>6000 && ProcessedEntries<(7000)) cc->SaveAs(cnam);
	  gTraceE->Delete();gTraceR->Delete();gTraceXF->Delete();gTraceXN->Delete();
	}
      }
      
      //Rise Time
      // Smooth
      for(Int_t j=0;j<1024;j++) {tSmooth[j]=0;}
      
      tSmooth[0]=yE[0];
      tSmooth[1]=(yE[0]+yE[1]+yE[2])/3.;
      tSmooth[2]=(yE[0]+yE[1]+yE[2]+yE[3]+yE[4])/5.;
      tSmooth[3]=(yE[0]+yE[1]+yE[2]+yE[3]+yE[4]+yE[5]+yE[6])/7.;
      for(Int_t i=4;(i<trace_length[0]-4)&&(i<1024);i++)
	{
	  tSmooth[i] = (yE[i-4] + yE[i-3] +yE[i-2] + yE[i-1] + yE[i] + yE[i+1] + yE[i+2] + yE[i+3] + yE[i+4])/9.;
	}
      
      //--- Get min and max of smooth within the rise area ---//
      Int_t iMin=0;
    Int_t iMax=0;
    Float_t RiseMin=16000;
    Float_t RiseMax=0;
    
    for (Int_t i=10;i<trace_length[0]-10;i++) {
      if (tSmooth[i]<RiseMin && tSmooth[i]>1000) {
	RiseMin=tSmooth[i];
	iMin=i;
      }
      if (tSmooth[i]>RiseMax && tSmooth[i]<16000) {
	RiseMax=tSmooth[i];
	iMax=i;
      }
    }
   
    
    int iTen=0;
    int iNinty=0;
    float fTen=0.0;
    float fNinty=0.0;
    float Slope=0;
    
    //--- Now determine 10% and 90% points get difference to get RiseTime ---//
    for(Int_t i=iMin;i<iMax;i++) { //Search a range from LED number
      if ((tSmooth[i]>((RiseMax-RiseMin)*0.1+RiseMin))&&(iTen==0)) {
	iTen=i;
	Slope = (tSmooth[iTen] - tSmooth[iTen-1]);
	fTen = (Float_t)iTen - (tSmooth[iTen] - ((RiseMax-RiseMin)*0.1+RiseMin)) / Slope;
      }
      if ((tSmooth[i]>((RiseMax-RiseMin)*0.9+RiseMin))&&(iNinty==0)) {
      iNinty=i; 
      Slope = (tSmooth[iNinty] - tSmooth[iNinty-1]);
      fNinty = (Float_t)iNinty - (tSmooth[iNinty] - ((RiseMax-RiseMin)*0.9+RiseMin)) / Slope;
      }
    }
      
    psd.EnergyRiseTime[0] = (fNinty-fTen)*10.0;

 if (ProcessedEntries<NUMPRINT){
    printf("RiseMin: %f, iMin: %d\n",RiseMin,iMin);
    printf("RiseMax: %f, iMax: %d\n",RiseMax,iMax);
    printf("RiseTime: %f\n",psd.EnergyRiseTime[0]);
    }
    
      psd_tree->Fill();
  }
  
 
  return kTRUE;
}

void EnergyTr::SlaveTerminate()
{

}

void EnergyTr::Terminate()
{
  if (ProcessedEntries>=NUMSORT)
    printf("Sorted only %llu\n",NUMSORT);
  psd_tree->Write();
  oFile->Close();
  
  cc0 = new TCanvas("cc0","cc0",800,600);
  cc0->Clear(); hEvents->Draw();  
  
  printf("Total time for sort: %3.1f\n",StpWatch.RealTime());
  StpWatch.Start(kFALSE);
}
