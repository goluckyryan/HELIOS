#include "HELIOS_LIB.h"

void Aux(){
   TargetScattering ms;
   
   ms.LoadStoppingPower(1, "16N_in_CD2.txt");
   ms.LoadStoppingPower(2, "3He_in_CD2.txt");
   ms.LoadStoppingPower(3, "15C_in_CD2.txt");
   
   ms.SetTarget(1.0, 0.1);
   
   Isotopes N16(16,7);
   double mass = N16.Mass;
   
   double KE = 12;
   double k = TMath::Sqrt(TMath::Power(mass + KE * 16 ,2) - mass* mass);
   
   TLorentzVector P(0,0,k, mass + KE);
   
   TLorentzVector Pnew = ms.Scattering(16, P);
   
   //P.Print();
   //Pnew.Print();
   
}
