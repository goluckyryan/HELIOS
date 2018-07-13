//#include "TH1.h"
//#include "TF1.h"
//#include "TList.h"
//#include <fstream>

//void test(){
{  
   TCanvas *c = new TCanvas("c","Graph2D example",0,0,600,400);
   Double_t x, y, z, P = 6.;
   Int_t np = 300;
   TGraph2D *dt = new TGraph2D();
   dt->SetTitle("Graph title; X axis title; Y axis title; Z axis title");
   TRandom *r = new TRandom();
   for (Int_t N=0; N<np; N++) {
      x = 2*P*(r->Rndm(N))-P;
      y = 2*P*(r->Rndm(N))-P;
      z = (sin(x-1.0)/(x-1.0))*(sin(y)/y)+0.2;
      dt->SetPoint(N,x,y,z);
      //dt->Draw("TRI1");
      //c->Update();
   }
   //gStyle->SetPalette(1);
   dt->Draw("TRI1");
   
   
}
