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
   
   int AA = 16, zA = 7;
   int Aa = 2,  za = 1;
   int Ab = 3,  zb = 2;
   int AB = AA+Aa-Ab, zB = zA+za-zb;
   
   reaction.SetA(AA,zA);
   reaction.Seta(Aa,za);
   reaction.Setb(Ab,zb);
   reaction.SetB(AB,zB);
   
   reaction.SetIncidentEnergyAngle(12., 0, 0);
   reaction.CalReactioConstant();
   
   printf("=========== Q-value : %f MeV, Max Ex: %f MeV \n", reaction.GetQValue(), reaction.GetMaxExB());

   double Ex = 5.; 
   double KEA = 12.;
   double theta = 0. * TMath::DegToRad();
   double phi = 0.;
   reaction.SetIncidentEnergyAngle(KEA, theta, phi);
   reaction.SetExB(Ex);
   reaction.CalReactioConstant();
   
   TLorentzVector PA = reaction.GetPA();
   
   //==== Target scattering, only energy loss
   TargetScattering ms;
   ms.LoadStoppingPower("16N_in_CD2.txt");
   double density = 0.913; // 0.913 g/cm3
   double targetThickness = 2.2e-4; // 2.2 um = 201 ug/cm2
   double depth = 0.9e-4; //targetThickness * gRandom->Rndm();
   ms.SetTarget(density, depth); 

   TLorentzVector PAnew = ms.Scattering(PA);
   
   double KEAnew = ms.GetKE()/AA;
   printf("length : %f um, KE-loss : %f MeV , new KE/u : %f MeV/u\n", ms.GetPathLength() * 1e4, ms.GetKELoss(), KEAnew);
   
   //Calculate reaction
   double thetaCM = 40 * TMath::DegToRad(); 
   reaction.SetIncidentEnergyAngle(KEAnew, theta, phi);
   TLorentzVector * output = reaction.Event(thetaCM, 0);
   TLorentzVector Pb = output[2];
   TLorentzVector PB = output[3];
   printf("=======================\n");
   double thetab = Pb.Theta() * TMath::RadToDeg();
   double thetaB = PB.Theta() * TMath::RadToDeg();
   double Tb = Pb.E() - Pb.M();
   double TB = PB.E() - PB.M();
   printf("T(b) : %f MeV, theta(b) : %f deg\n", Tb, thetab);
   printf("T(B) : %f MeV, theta(B) : %f deg\n", TB, thetaB);
   printf("=======================\n");
   
   //Calculate energy loss of scattered and recoil in target
   TargetScattering msb;
   msb.LoadStoppingPower("3He_in_CD2.txt");
   msb.SetTarget(density, targetThickness - depth);
   TLorentzVector Pbnew = msb.Scattering(Pb);
   
   printf("length : %f um, KE-loss : %f MeV , new KE/u : %f MeV/u\n", 
            msb.GetPathLength() * 1e4, 
            msb.GetKELoss(), 
            msb.GetKE());
            
   //Pbnew.Print();
   
   TargetScattering msB;
   msB.LoadStoppingPower("15C_in_CD2.txt");
   msB.SetTarget(density, targetThickness - depth);
   TLorentzVector PBnew = msB.Scattering(PB);
   printf("length : %f um, KE-loss : %f MeV , new KE/u : %f MeV/u\n", 
            msB.GetPathLength() * 1e4, 
            msB.GetKELoss(), 
            msB.GetKE());
            
   //PBnew.Print();
   
   /*         
   //======== Set HELIOS   
   HELIOS helios;
   helios.SetDetectorGeometry("detectorGeo_downstream.txt");
   
   int hit = helios.CalHit(Pbnew, zb, PBnew, zB);
   
   printf("hit: %d \n", hit);
   double e = helios.GetEnergy();
   double z = helios.GetZ();
   double x = helios.GetX();
   double t = helios.GetTime();
   double loop = helios.GetLoop();
   double detID = helios.GetDetID();
   double dphi = helios.GetdPhi();
   double rho = helios.GetRho();
   
   printf("energy : %f MeV, z : %f mm, loop: %d, detID: %d, rho: %f mm\n", e, z, loop, detID, rho); 
   
   /*
   double zPos[100], xPos[100], yPos[100];
   for(int i = 0; i < 100 ; i++){
      double theta = Pb.Theta();
      zPos[i] = z/100.*i;
      //double xp = rho * (1-TMath::Cos( TMath::Tan(theta) * zPos[i]/rho));
      //double yp = rho * TMath::Sin( TMath::Tan(theta) * zPos[i]/rho);
      xPos[i] = helios.GetXPos(zPos[i]);
      yPos[i] = helios.GetXPos(zPos[i]);
      
      printf("%6f, %6f, %6f \n", zPos[i], xPos[i], xp);
   }
   */
   
   //===== Decay of particle B;
   printf("==================\n");
   Decay decay;
   
   decay.SetMotherDaugther(AB, zB, AB-1, zB);
   
   for( int i = 0; i < 20 ; i++){
      //printf("==================\n");
      int ok = decay.CalDecay(PBnew, Ex, 0); 
      //printf(" ok? %d, Q: %f \n", ok, decay.GetQValue());
   
      TLorentzVector PD = decay.GetDaugther_D();
      TLorentzVector Pd = decay.GetDaugther_d();
      
      //PD.Print();
      //Pd.Print();
    
      //PB.Print();
      //(Pd+PD).Print();
      
      //printf("=======================\n");
      TVector3 vD = PD.Vect();
      TVector3 vB = PBnew.Vect();
      vD.SetMag(1);
      vB.SetMag(1);
      double dot = vD.Dot(vB);
      printf("Dot-Produce : %f, Open-angle : %f deg\n", dot , TMath::ACos(dot)*TMath::RadToDeg() );
      
      printf(" k : %f , kB: %f \n", decay.GetCMMomentum(), PB.P());
      
   }
   
   /*
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
   */

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
