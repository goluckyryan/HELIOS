#define Monitors_cxx
//test
#include "Monitors.h"
#include <TH2.h>
#include <TH1.h>
#include <TStyle.h>
#include <TCutG.h>

#define NUMPRINT 20 //>0
ULong64_t NUMSORT=100000000000;
ULong64_t NumEntries = 0;
ULong64_t ProcessedEntries = 0;
Float_t Frac = 0.1; //Progress bar
TStopwatch StpWatch;

TH2F* hxfxn[24];
TH2F* heVx[24]; 
TH2F* hecalVxcal[24];
TH2F* hecalVz;
TH2F* hecalVzR;
TH2F* hrdt[4];
TH2F* helum[2];
TH2F* he0dee;//ezero
TH2F* he0det;
TH2F* he0et;
TH1F* h0detet;
TH1F* h0dettact;
TH1F* h0ettact;
TH1F* h0de;
TH1F* h0e;
TH1F* h0tac;

TH1F* htacE;
//TH1F* hex;
TH1F* hexR;
TH1I* htacArray[24];
TH1F* htac[4];//0 array-rdt, 1 elum-rdt

Float_t x[24],z[24];
Float_t xcal[24],ecal[24],xfcal[24],xncal[24],ecrr[24],ezero[4];
Int_t tacA[24];
Float_t z_array_pos[6] = {35.868,29.987,24.111,18.248,12.412,6.676};//in cm
Float_t z_off=65.0;//to the physical end of array from the target (-20 to si start)
Float_t xcoef[24] = {0.977877, 0.988848, 0.977536, 0.940756, 1.07479, 0.948377,
		     0.938322, 0.987471, 0.98292, 0.991469, 0.996435, 0.0,
		     1.04777, 1.01165, 0.969064, 1.01138, 0.944809, 1.05395,
		     0.972853, 0.969341, 0.961707, 0.989349, 1.01909, 1.12121};
Float_t ecoef[24] = {0.87125, 0.9734, 1.05417, 1.11849, 0.87099, 0.967404,
		     1.00563, 0.976017, 0.927414, 0.959165, 0.916457, 0.0,
		     0.855793, 0.905978, 0.928997, 0.960064, 1.07153, 0.915901,
		     0.881094, 0.956482, 1.01326, 0.887234, 1.09654, 0.869338};
Float_t kcoef[24] = {0.003444976, 0.003457902, 0.003132563, 0.003341067, 0.003563254, 0.003654822, 
		     0.003366944, 0.003534284, 0.003526171, 0.003309869, 0.003735409, 0.0,
		     0.003791968, 0.003526171, 0.00364557, 0.003384254, 0.003353712, 0.003434705,
		     0.003505249, 0.003378299, 0.003289549, 0.003448586, 0.002994541, 0.003576529};
Float_t bcoef[24] = {0.453923445, 0.417753264, 0.501465942, 0.538881671, 0.449696876, 0.546873096,
		     0.436930294, 0.433740451, 0.448512397, 0.595999138, 0.389035019, 0.0,
		     0.369876234, 0.470022039, 0.533858228, 0.589323149, 0.387687336, 0.52972093,
		     0.494175262, 0.41855132, 0.473385494, 0.584180512, 0.49499818, 0.584009314};

Float_t tempTime=-1000;
Long64_t tempTimeLong=10001;

TCutG* cut0;
TCutG* cut1;
TCutG* cut2;
TCutG* cut3;




void Monitors::Begin(TTree *tree)
{
    

  TString option = GetOption();
  NumEntries = tree->GetEntries();

  //Generate all of the histograms needed for drawing later on

  for (Int_t i=0;i<24;i++) {//array loop
    hxfxn[i] = new TH2F(Form("hxfxn%d",i),
			Form("Raw PSD XF vs. XN (ch=%d);XF (channel);XN (channel)",i),
			500,0,4000,500,0,4000);
    heVx[i] = new TH2F(Form("heVx%d",i),
		       Form("Raw PSD E vs. X (ch=%d);X (channel);E (channel)",i),
		       500,-0.1,1.1,500,0,4000);
  }//array loop
  hecalVz = new TH2F("hecalVz","E vs. Z;Z (cm);E (MeV)",450,60,105,500,0,30);
  hecalVzR = new TH2F("hecalVzR","E vs. Z gated;Z (cm);E (MeV)",450,60,105,500,0,30);


  //Recoils
  for (Int_t i=0;i<4;i++) {
    hrdt[i] = new TH2F(Form("hrdt%d",i),
		       Form("Raw Recoil DE vs Eres (ch=%d); Eres (channel); DE (channel)",i),
		       500,20,8000,500,20,3000);
  }

  //ELUM
  helum[0] = new TH2F("helum0","Elum Ring Energies; E (channels); Ring Number",
		      500,100,4000,16,0,16);
  helum[1] = new TH2F("helum1","Elum Wedge Energies; E (channels); Ring Number",
		      500,100,4000,16,0,16);

  //TAC
  htac[0] = new TH1F("htac0","Array-RDT0 TAC; DT [clock ticks]; Counts",6,0,6);
  htac[1] = new TH1F("htac1","Array-RDT1 TAC; DT [clock ticks]; Counts",6,0,6);
  htac[2] = new TH1F("htac2","Array-RDT2 TAC; DT [clock ticks]; Counts",6,0,6);
  htac[3] = new TH1F("htac3","Array-RDT3 TAC; DT [clock ticks]; Counts",6,0,6);

  htacE = new TH1F("htacE","Elum-RDT TAC; DT [clock ticks]; Counts",4,0,4);

  // hex = new TH1F("hex","excitation spectrum",200,-2,10);
  hexR = new TH1F("hexR","excitation spectrum with Recoil",500,-2,10);

  for (Int_t i=0;i<24;i++) {
    htacArray[i] = new TH1I(Form("htacArray%d",i), Form("Array-RDT TAC for ch%d",i), 200, -100,100);
  }

  //EZERO

  he0dee = new TH2F("he0dee","EZERO DE-E; E [ch]; DE [ch]",500,0,8000,500,0,8000);//ezero
  he0det = new TH2F("he0det","EZERO DE-RF; RF [ch]; DE [ch]",500,2000,3500,500,0,8000);//
  he0et = new TH2F("he0et","EZERO E-RF; RF [ch]; DE [ch]",500,2000,3500,500,0,8000);//
  h0detet = new TH1F("h0detet","EZERO DE Time - E Time; DET-ET [ch]",500,-250,250);//
  h0dettact = new TH1F("h0dettact","EZERO DE Time - TAC Time; DET-ET [ch]",2000,-1000,1000);//
  h0ettact = new TH1F("h0ettact","EZERO E Time - TAC Time; DET-ET [ch]",2000,-1000,1000);//
  h0de = new TH1F("h0de","EZERO DE ; DE [ch]",500,50,4050);//
  h0e = new TH1F("h0e","EZERO - E; E [ch]",500,50,4050);//
  h0tac = new TH1F("h0tac","EZERO RF; RF [ch]",500,50,4050);//
  StpWatch.Start();
}

void Monitors::SlaveBegin(TTree * /*tree*/)
{
   TString option = GetOption();

}

Bool_t Monitors::Process(Long64_t entry)
{
  ProcessedEntries++;
    
    /*
     cut0 = new TCutG("cu0",15);
     cut0->SetVarX("rdt[1]");
     cut0->SetVarY("rdt[0]");
     cut0->SetTitle("Graph");
     cut0->SetFillColor(1);
     cut0->SetPoint(0,4387.41,408.805);
     cut0->SetPoint(1,5212.69,356.932);
     cut0->SetPoint(2,5719.8,339.416);
     cut0->SetPoint(3,5908.72,361.311);
     cut0->SetPoint(4,5769.51,394.321);
     cut0->SetPoint(5,5351.9,413.858);
     cut0->SetPoint(6,4993.94,417.226);
     cut0->SetPoint(7,4780.16,417.9);
     cut0->SetPoint(8,4606.16,426.658);
     cut0->SetPoint(9,4407.3,445.521);
     cut0->SetPoint(10,4173.63,456.973);
     cut0->SetPoint(11,4039.4,462.363);
     cut0->SetPoint(12,3840.53,453.942);
     cut0->SetPoint(13,3835.56,447.879);
     cut0->SetPoint(14,4387.41,408.805);
    
     cut1 = new TCutG("cut1",12);
     cut1->SetVarX("rdt[3]");
     cut1->SetVarY("rdt[2]");
     cut1->SetTitle("Graph");
     cut1->SetFillColor(1);
     cut1->SetPoint(0,5458.93,311.199);
     cut1->SetPoint(1,6391.58,280.783);
     cut1->SetPoint(2,6373.22,316.414);
     cut1->SetPoint(3,6226.34,358.127);
     cut1->SetPoint(4,5734.32,365.948);
     cut1->SetPoint(5,5477.29,375.508);
     cut1->SetPoint(6,5301.04,380.722);
     cut1->SetPoint(7,5198.23,365.948);
     cut1->SetPoint(8,4812.68,380.722);
     cut1->SetPoint(9,4724.56,357.258);
     cut1->SetPoint(10,4724.56,357.258);
     cut1->SetPoint(11,5458.93,311.199);
    
     cut2 = new TCutG("cut2",8);
     cut2->SetVarX("rdt[5]");
     cut2->SetVarY("rdt[4]");
     cut2->SetTitle("Graph");
     cut2->SetFillColor(1);
     cut2->SetPoint(0,5859.16,391.664);
     cut2->SetPoint(1,6292.44,362.409);
     cut2->SetPoint(2,6152.91,308.078);
     cut2->SetPoint(3,5506.66,342.441);
     cut2->SetPoint(4,5132.13,364.731);
     cut2->SetPoint(5,5301.04,387.021);
     cut2->SetPoint(6,5719.63,389.342);
     cut2->SetPoint(7,5859.16,391.664);
    
     cut3 = new TCutG("cut3",12);
     cut3->SetVarX("rdt[7]");
     cut3->SetVarY("rdt[6]");
     cut3->SetTitle("Graph");
     cut3->SetFillColor(1);
     cut3->SetPoint(0,6243.16,303.061);
     cut3->SetPoint(1,6473.96,307.439);
     cut3->SetPoint(2,6512.42,323.858);
     cut3->SetPoint(3,6246.01,341.371);
     cut3->SetPoint(4,6042.28,367.276);
     cut3->SetPoint(5,5708.91,381.141);
     cut3->SetPoint(6,5540.8,386.249);
     cut3->SetPoint(7,5384.08,374.209);
     cut3->SetPoint(8,5307.15,348.304);
     cut3->SetPoint(9,5640.52,332.615);
     cut3->SetPoint(10,5938.28,317.656);
     cut3->SetPoint(11,6243.16,303.061);*/
    /*
    cut0 = new TCutG("cut0",5);
    cut0 ->SetVarY("rdt[0]");
    cut0 ->SetVarX("rdt[1]");
    cut0 ->SetTitle("Graph");
    cut0 ->SetFillColor(1);
    cut0 ->SetPoint(0,4588.7,392);
    cut0 ->SetPoint(1,5701,331);
    cut0 ->SetPoint(2,5855,335.5);
    cut0 ->SetPoint(3,4686.6,415.2);
    cut0 ->SetPoint(4,4588.7,392);
    
    cut1 = new TCutG("cut1",5);
    cut1 ->SetVarY("rdt[2]");
    cut1 ->SetVarX("rdt[3]");
    cut1 ->SetTitle("Graph");
    cut1 ->SetFillColor(1);
    cut1 ->SetPoint(0,4865.8,347);
    cut1 ->SetPoint(1,5981.4,295);
    cut1 ->SetPoint(2,6092.4,319);
    cut1 ->SetPoint(3,4921.3,369.8);
    cut1 ->SetPoint(4,4865.8,347);
    
    cut2 = new TCutG("cut2",5);
    cut2 ->SetVarY("rdt[4]");
    cut2 ->SetVarX("rdt[5]");
    cut2 ->SetTitle("Graph");
    cut2 ->SetFillColor(1);
    cut2 ->SetPoint(0,6216.4,306);
    cut2 ->SetPoint(1,6290.7,333);
    cut2 ->SetPoint(2,5278.4,381);
    cut2 ->SetPoint(3,5278.4,350.5);
    cut2 ->SetPoint(4,6216.4,306);
    
    cut3 = new TCutG("cut3",10);
    cut3 ->SetVarY("rdt[6]");
    cut3 ->SetVarX("rdt[7]");
    cut3 ->SetPoint(0,6196.7,305.2);
    cut3 ->SetPoint(1,6274.2,330);
    cut3 ->SetPoint(2,5279.6,375);
    cut3 ->SetPoint(3,255.8,351.5);
    cut3 ->SetPoint(4,6196.7,305.2);
     */
    
    
    cut0 = new TCutG("cut0",7);
    cut0 ->SetVarY("rdt[0]");
    cut0 ->SetVarX("rdt[1]");
    cut0 ->SetTitle("Graph");
    cut0 ->SetFillColor(1);
    cut0 ->SetPoint(0,3766.9,516.9);
    cut0 ->SetPoint(1,5649.3,425.6);
    cut0 ->SetPoint(2,6065.7,346.0);
    cut0 ->SetPoint(3,5665.9,270.3);
    cut0 ->SetPoint(4,4932.9,311);
    cut0 ->SetPoint(5,3450.4,429.5);
    cut0 ->SetPoint(6,3766.9,516.9);
    
    cut1 = new TCutG("cut1",8);
    cut1 ->SetVarY("rdt[2]");
    cut1 ->SetVarX("rdt[3]");
    cut1 ->SetTitle("Graph");
    cut1 ->SetFillColor(1);
    cut1 ->SetPoint(0,4056,434.5);
    cut1 ->SetPoint(1,5874.5,411.7);
    cut1 ->SetPoint(2,5975.8,378.7);
    cut1 ->SetPoint(3,5975.8,329.2);
    cut1 ->SetPoint(4,5590,286.2);
    cut1 ->SetPoint(5,4523.5,329.2);
    cut1 ->SetPoint(6,3854,380.3);
    cut1 ->SetPoint(7,4056,434.5);
    
    cut2 = new TCutG("cut2",8);
    cut2 ->SetVarY("rdt[4]");
    cut2 ->SetVarX("rdt[5]");
    cut2 ->SetTitle("Graph");
    cut2 ->SetFillColor(1);
    cut2 ->SetPoint(0,6269.2,381);
    cut2 ->SetPoint(1,6012.7,277.5);
    cut2 ->SetPoint(2,5435.5,324.7);
    cut2 ->SetPoint(3,4550.4,379.2);
    cut2 ->SetPoint(4,4742.8,433.6);
    cut2 ->SetPoint(5,5102,408.2);
    cut2 ->SetPoint(6,5461.1,402.7);
    cut2 ->SetPoint(7,6269.2,381);
    
    
    cut3 = new TCutG("cut3",8);
    cut3 ->SetVarY("rdt[6]");
    cut3 ->SetVarX("rdt[7]");
    cut3 ->SetPoint(0,3985.7,467.8);
    cut3 ->SetPoint(1,4836.9,442.3);
    cut3 ->SetPoint(2,5420.6,389.8);
    cut3 ->SetPoint(3,5773.3,412.1);
    cut3 ->SetPoint(4,6223.2,381.8);
    cut3 ->SetPoint(5,6174.6,259.3);
    cut3 ->SetPoint(6,3937.01,445.5);
    cut3 ->SetPoint(7,3985.7,467.8);


    
  if (ProcessedEntries<NUMSORT) {
    
    if (ProcessedEntries>NumEntries*Frac-1) {
      printf(" %3.0f%% (%llu/%llu k) processed in %6.1f seconds\n",
	     Frac*100,ProcessedEntries/1000,NumEntries/1000,StpWatch.RealTime());
      StpWatch.Start(kFALSE);
      Frac+=0.1;
    }

    b_Energy->GetEntry(entry);
    b_XF->GetEntry(entry);
    b_XN->GetEntry(entry);
    b_RDT->GetEntry(entry);
    b_TAC->GetEntry(entry);
    b_ELUM->GetEntry(entry);
    b_EZERO->GetEntry(entry);
    b_EnergyTimestamp->GetEntry(entry);
    b_RDTTimestamp->GetEntry(entry);
    b_TACTimestamp->GetEntry(entry);
    b_ELUMTimestamp->GetEntry(entry);
    b_EZEROTimestamp->GetEntry(entry);

    //Do calculations and fill histograms
    //Array calcs first
    for (Int_t i=0;i<24;i++) {
      //Calibrations go here
      xfcal[i] = xf[i];
      xncal[i] = xn[i]*xcoef[i];
      ecal[i] = e[i]/ecoef[i];
      ecrr[i] = e[i]*kcoef[i]+bcoef[i];

      if (xf[i]>0 || xn[i]>0) {
	x[i] = 0.5*((xf[i]-xn[i]) / (xf[i]+xn[i]))+0.5;//for downstream?
	//x[i] = 0.5*((xf[i]-xn[i]) / (xf[i]+xn[i]))+0.5;

	if (xfcal[i]>xncal[i]) {
	  xcal[i] = xfcal[i]/ecal[i];
	} else if (xncal[i]>=xfcal[i]) {
	  xcal[i] = 1.0 - xncal[i]/ecal[i];
	}
	z[i] = -5.0*(xcal[i]-0.5) + z_off + z_array_pos[i%6];//for downstream?
	//z[i] = 5.0*(xcal[i]-0.5) - z_off - z_array_pos[i%6];
      }

      //Array fill next
      hxfxn[i]->Fill(xf[i],xn[i]);
      if (x[i]>-1.1&&x[i]<1.1&&e[i]>100&&(xn[i]>0||xf[i]>0)) {
	heVx[i]->Fill(x[i],e[i]);
	hecalVz->Fill(z[i],ecrr[i]);
      }

    }//array loop

    /* RECOILS */
      /*
      for (Int_t i=0;i<4;i++) //for (Int_t j=0;j<24;j++){
       //if(ecal[j]>4.0)
          hrdt[i]->Fill(rdt[i*2+1],rdt[i*2]);
     // }
      */
      
      for (Int_t i=0;i<4;i++){
          for(Int_t j=0;j<6;j++){
              if(i==3&&e[i*6+j]>500&&rdt[6]>200&&rdt[7]>500){
                  tacA[i*6+j]= (int)(rdt_t[6]-e_t[i*6+j]);
                  if(tacA[i*6+j]>-15&&tacA[i*6+j]<5){
                      hrdt[i]->Fill(rdt[7],rdt[6]);
                  }
              }
              if(i==0&&e[i*6+j]>500&&rdt[4]>200&&rdt[5]>500){
                  tacA[i*6+j]= (int)(rdt_t[4]-e_t[i*6+j]);
                  if(tacA[i*6+j]>-15&&tacA[i*6+j]<5){
                      hrdt[i]->Fill(rdt[5],rdt[4]);
                  }
              }
              if(i==1&&e[i*6+j]>500&&rdt[2]>200&&rdt[3]>500){
                  tacA[i*6+j]= (int)(rdt_t[2]-e_t[i*6+j]);
                  if(tacA[i*6+j]>-15&&tacA[i*6+j]<5){
                      hrdt[i]->Fill(rdt[3],rdt[2]);
                  }
              }
              if(i==2&&e[i*6+j]>500&&rdt[0]>200&&rdt[1]>500){
                  tacA[i*6+j]= (int)(rdt_t[0]-e_t[i*6+j]);
                  if(tacA[i*6+j]>-15&&tacA[i*6+j]<5){
                      hrdt[i]->Fill(rdt[1],rdt[0]);
                  }
              }

              }
      }


    /* ELUM */
    for (Int_t i=0;i<16;i++) {
      helum[0]->Fill(elum[i],i);
      helum[1]->Fill(elum[i+16],i);
      for(Int_t j=0;j<4;j++){
	if(rdt[j]-elum_t[i]>-10&&rdt[j]-elum[i]<10)htacE->Fill(0.5+j);
      }
    }

    //EZERO
   
    he0dee->Fill(ezero[1],ezero[0]);
    he0det->Fill(TMath::Abs(tac[0]),ezero[0]);
    he0et->Fill(TMath::Abs(tac[0]),ezero[1]);
    h0detet->Fill((int)(ezero_t[0]-ezero_t[1]));
    h0dettact->Fill((int)(ezero_t[0]-tac_t[0]));
    h0ettact->Fill((int)(ezero_t[1]-tac_t[0]));
    h0de->Fill(ezero[0]);
    h0e->Fill(ezero[1]);
    h0tac->Fill(TMath::Abs(tac[0]));

    //TACs
    for (Int_t i=0;i<4;i++){
      for(Int_t j=0;j<6;j++){
	//if(i==3&&e[i*6+j]>1000&&rdt[6]>300&&rdt[6]<395&&rdt[7]>5900&&rdt[7]<6500){//stable 11B
          if(i==3&&e[i*6+j]>100&&rdt[6]>300&&cut3->IsInside(rdt[7],rdt[6])&&ecrr[i*6+j]>15.86*z[i*6+j]/100-7.191){//12B
          //if(i==3&&e[i*6+j]>100&&rdt[6]>331.4&&rdt[6]<385.2&&rdt[7]>4751.8&&rdt[7]<5521){
	  tacA[i*6+j]= (int)(rdt_t[6]-e_t[i*6+j]);
	  htacArray[i*6+j]->Fill(tacA[i*6+j]);
	  if(tacA[i*6+j]>-10&&tacA[i*6+j]<5){hecalVzR->Fill(z[i*6+j],ecrr[i*6+j]);
	    hexR->Fill((z[i*6+j]*0.2644-9-ecrr[i*6+j])*1.6731-2.7309);
          htac[3]->Fill(j+0.5);   }
          }
	  //if(i==0&&e[i*6+j]>1000&&rdt[4]>300&&rdt[4]<340&&rdt[5]>6450&&rdt[5]<6650){//stable 11B
             if(i==0&&e[i*6+j]>100&&rdt[4]>300&&cut2->IsInside(rdt[5],rdt[4])&&ecrr[i*6+j]>15.86*z[i*6+j]/100-7.191){//12B
          //if(i==0&&e[i*6+j]>100&&rdt[4]>327.1&&rdt[4]<390.9&&rdt[5]>5263.3&&rdt[5]<6225.5){
	    	  tacA[i*6+j]= (int)(rdt_t[4]-e_t[i*6+j]);
	  htacArray[i*6+j]->Fill(tacA[i*6+j]);
	  if(tacA[i*6+j]>-10&&tacA[i*6+j]<5){//hecalVzR->Fill(z[i*6+j],ecrr[i*6+j]);
	    hexR->Fill((z[i*6+j]*0.2644-9-ecrr[i*6+j])*1.6731-2.7309);
	    htac[2]->Fill(j+0.5);}
	  }
	  //if(i==1&&e[i*6+j]>1000&&rdt[2]>280&&rdt[2]<320&&rdt[3]>6200&&rdt[3]<6400){//stable 11B
            if(i==1&&e[i*6+j]>100&&rdt[2]>280&&cut1->IsInside(rdt[3],rdt[2])&&ecrr[i*6+j]>15.86*z[i*6+j]/100-7.191){//12B
          //if(i==1&&e[i*6+j]>100&&rdt[2]>294.6&&rdt[2]<368&&rdt[3]>5397.5&&rdt[3]<6205.8){
	    	  tacA[i*6+j]= (int)(rdt_t[2]-e_t[i*6+j]);
	  htacArray[i*6+j]->Fill(tacA[i*6+j]);
	  if(tacA[i*6+j]>-10&&tacA[i*6+j]<5){hecalVzR->Fill(z[i*6+j],ecrr[i*6+j]);
	    hexR->Fill((z[i*6+j]*0.2644-9-ecrr[i*6+j])*1.6731-2.7309);
	  htac[1]->Fill(j+0.5);}
	  }
	  //if(i==2&&e[i*6+j]>1000&&rdt[0]>310&&rdt[0]<360&&rdt[1]>6100&&rdt[1]<6300){//stable 11B
             if(i==2&&e[i*6+j]>100&&rdt[0]>310&&cut0->IsInside(rdt[1],rdt[0])&&ecrr[i*6+j]>15.86*z[i*6+j]/100-7.191){//12B
                // if(i==2&&e[i*6+j]>100&&rdt[0]>346.8&&rdt[0]<416.5&&rdt[1]>4797.6&&rdt[1]<5622.6){
	    	  tacA[i*6+j]= (int)(rdt_t[0]-e_t[i*6+j]);
	  htacArray[i*6+j]->Fill(tacA[i*6+j]);
	  if(tacA[i*6+j]>-10&&tacA[i*6+j]<5){hecalVzR->Fill(z[i*6+j],ecrr[i*6+j]);
	    hexR->Fill((z[i*6+j]*0.2644-9-ecrr[i*6+j])*1.6731-2.7309);
	    htac[0]->Fill(j+0.5);}
	  }
      }
    }

   
  }
   return kTRUE;
}

void Monitors::SlaveTerminate()
{

}

void Monitors::Terminate()
{
  
  TCanvas *cxfxn = new TCanvas("cxfxn","XFXN",1200,800);
  cxfxn->Clear(); cxfxn->Divide(6,4);
  TCanvas *ceVx = new TCanvas("ceVx","EVX",1200,800);
  ceVx->Clear(); ceVx->Divide(6,4);
  for (Int_t i=0;i<24;i++) {
    cxfxn->cd(i+1); hxfxn[i]->Draw("col");
    ceVx->cd(i+1); heVx[i]->Draw("col");
  }
  TCanvas *cecalVz = new TCanvas("cevalVz","ECALVZ",1000,650);
  cecalVz->Clear();hecalVz->Draw("col");

  TCanvas *cecalVzR = new TCanvas("cevalVzR","ECALVZ Gated",1000,650);
  cecalVzR->Clear();hecalVzR->Draw("");


  TCanvas *crdt = new TCanvas("crdt","RDT",1000,1000);
  crdt->Clear();crdt->Divide(2,2);
  for (Int_t i=0;i<4;i++) {
    crdt->cd(i+1); hrdt[i]->Draw("col");
  }

  TCanvas *ctac = new TCanvas("ctac","TAC",1200,800);
  ctac->Clear(); ctac->Divide(6,4);
  for (Int_t i=0;i<24;i++) {
    ctac->cd(i+1); htacArray[i]->Draw("");
  }

  TCanvas *ctacA = new TCanvas("ctacA","TACA",1200,800);
  ctacA->Clear(); ctacA->Divide(2,2);
  for (Int_t i=0;i<4;i++) {
    ctacA->cd(i+1); htac[i]->Draw("");
  }

 TCanvas *cex = new TCanvas("cex","Excitation Spectrum",1200,800);
  cex->Clear(); cex->Divide(1,2);
  //  cex->cd(1); hex->Draw("");
  cex->cd(2); hexR->Draw("");
  /*
  TCanvas *celum = new TCanvas("celum","ELUM",1200,800);
  celum->Clear(); celum->Divide(1,3);
  celum->cd(1); helum[0]->Draw("col");
  celum->cd(2); helum[1]->Draw("col");
  celum->cd(3); htacE->Draw("");

  */
 TCanvas *ce0 = new TCanvas("ce0","EZERO - Energy - RF",1200,800);
 ce0->Clear(); ce0->Divide(2,2);
 ce0->cd(1); he0dee->Draw("col");
 ce0->cd(2); he0det->Draw("col");
 ce0->cd(3); he0et->Draw("col");
 ce0->Modified(); ce0->Update();

 TCanvas *ce00 = new TCanvas("ce00","EZERO - DeltaTs",1200,800);
 ce00->Clear(); ce00->Divide(2,2);
 ce00->cd(1); h0de->Draw("");
 ce00->cd(2); h0e->Draw("");
 ce00->cd(3); h0tac->Draw("");
 ce00->Modified(); ce00->Update();
 // ce00->WaitPrimitive();

  if (ProcessedEntries>=NUMSORT)
    printf("Sorted only %llu\n",NUMSORT);
  printf("Total time for sort: %3.1f\n",StpWatch.RealTime());
  printf("Which is a rate of: %3.1f k/s\n",(Float_t)ProcessedEntries/StpWatch.RealTime()/1000.);
  StpWatch.Start(kFALSE);
}
