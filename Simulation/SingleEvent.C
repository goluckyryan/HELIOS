#include "HELIOS_LIB.h"
#include "TBenchmark.h"
#include "TLorentzVector.h"
#include "TMath.h"
#include "TFile.h"
#include "TTree.h"
#include "TRandom.h"
#include <vector>
#include <fstream>

void SingleEvent(){

   //===== Set Reaction
   TransferReaction reaction;
   
   int AA = 25, zA = 12;
   int Aa = 2,  za = 1;
   int Ab = 1,  zb = 1;
   int AB = AA+Aa-Ab, zB = zA+za-zb;
   
   reaction.SetA(AA,zA);
   reaction.Seta(Aa,za);
   reaction.Setb(Ab,zb);
   reaction.SetB(AB,zB);
   
   reaction.SetIncidentEnergyAngle(6, 0, 0);
   reaction.CalReactioConstant();
   
   printf("=========== Q-value : %f MeV, Max Ex: %f MeV \n", reaction.GetQValue(), reaction.GetMaxExB());


   //======== Set HELIOS   
   HELIOS helios;
   helios.SetDetectorGeometry("detectorGeo_upstream.txt");
   
   
   //======== Calculation
   //double thetaCM = TMath::Pi() * gRandom->Rndm(); 
   double thetaCM = 30 * TMath::DegToRad(); 
   double Ex = 0.; 
   
   //KEA = 12 + 0.5*(gRandom->Rndm()-0.5);
   //KEA = gRandom->Gaus(6., 0.01);
   double KEA = 6.;
   double theta = 30. * TMath::DegToRad();
   //theta = gRandom->Gaus(0, 0.015);
   double phi = 0.;
   //phi = TMath::TwoPi() * gRandom->Rndm();
   reaction.SetIncidentEnergyAngle(KEA, theta, phi);
   reaction.SetExB(Ex);
   TLorentzVector * output = reaction.Event(thetaCM, 0);

   TLorentzVector PA = output[0];
   TLorentzVector Pa = output[1];
   
   TLorentzVector Pb = output[2];
   TLorentzVector PB = output[3];
   
   printf("=======================\n");
   PA.Print();
   Pa.Print();
   Pb.Print();
   PB.Print();
   
   printf("=======================\n");
   (PA+Pa).Print();
   (Pb+PB).Print();
   
   printf("=======================\n");
   double thetab = Pb.Theta() * TMath::RadToDeg();
   double thetaB = PB.Theta() * TMath::RadToDeg();
   printf("theta(b) : %f, theta(B) : %f , sum = %f\n", thetab, thetaB, thetab+ thetaB);

   printf("=======================\n");
   TVector3 vb = Pb.Vect();
   TVector3 vB = PB.Vect();
   vb.SetMag(1);
   vB.SetMag(1);
   double dot = vb.Dot(vB);
   printf("Dot-Produce : %f, Open-angle : %f deg\n", dot , TMath::ACos(dot)*TMath::RadToDeg() );

   printf("=======================\n");
   double Tb = Pb.E() - Pb.M();
   double TB = PB.E() - PB.M();
   printf("T(b) : %f, T(B) : %f \n", Tb, TB);


/*

   
   int hit = helios.CalHit(Pb, zb, PB, zB);
   count ++;
   e = helios.GetEnergy();
   z = helios.GetZ();
   x = helios.GetX();
   t = helios.GetTime();
   loop = helios.GetLoop();
   detID = helios.GetDetID();
   dphi = helios.GetdPhi();
   rho = helios.GetRho();

/**/
}
