#include "HELIOS_LIB.h"

int main(){
   TransferReaction test;
   
   test.SetA(16,7);
   test.Seta(2,1);
   test.Setb(3,2);
   test.SetB(15,6);
   
   double thetaCM = 40 * TMath::DegToRad();
   
   test.SetIncidentEnergyPerU(12);
   test.SetIncidentAngle(thetaCM, 0);
   
   TLorentzVector * output = test.Event(thetaCM, thetaCM);
   
   TLorentzVector PA = output[0];
   TLorentzVector Pa = output[1];
   TLorentzVector Pb = output[2];
   TLorentzVector PB = output[3];
   
   PA.Print();
   Pa.Print();
   Pb.Print();
   PB.Print();
   
   printf("==================\n");
   
   
   (PA+Pa).Print();
   (PB+Pb).Print();
   
   
}
