#include "TBenchmark.h"
#include "TLorentzVector.h"
#include "TVector3.h"
#include "TMath.h"
#include "TFile.h"
#include "TTree.h"
#include "TRandom.h"
#include <vector>
//#include "TransferReaction.h"
#include <fstream>
#include "isotopes.h"

//given a beam 4-vector, reaction type, loading detector geometric file, generate an event

// TransferReaction class ? -- only for reaction, both for beam generation and reaction
// scattering in target
// HELIOS class? -- only for detection

class TransferReaction {
public:

   TransferReaction();
   ~TransferReaction();
   

   void SetA(int A, int Z, double Ex = 0){
      Isotopes temp (A, Z);
      mA = temp.Mass;
      AA = A;
      zA = Z;
      ExA = Ex;
      isReady = false;
   }
   void Seta(int A, int Z){
      Isotopes temp (A, Z);
      ma = temp.Mass;
      Aa = A;
      za = Z;
      isReady = false;
   }
   void Setb(int A, int Z){
      Isotopes temp (A, Z);
      mb = temp.Mass;
      Ab = A;
      zb = Z;
      isReady = false;
   }
   void SetB(int A, int Z){
      Isotopes temp (A, Z);
      mB = temp.Mass;
      AB = A;
      zB = Z;
      isReady = false;
   }
   void SetIncidentEnergyPerU(double T){
      TA = T;
      this->T = TA * AA;
      isReady = false;
   }
   void SetIncidentAngle(double theta, double phi){
      thetaIN = theta;
      phiIN = phi;
      isReady = false;
   }
   
   int GetAtomicNumber_A(){return AA;}
   int GetAtomicNumber_a(){return Aa;}
   int GetAtomicNumber_b(){return Ab;}
   int GetAtomicNumber_B(){return AB;}
   
   int GetCharge_A(){return zA;}
   int GetCharge_a(){return za;}
   int GetCharge_b(){return zb;}
   int GetCharge_B(){return zB;}
   
   void Constant();
   TLorentzVector * Event(double Ex, double thetaCM, double phiCM);
   
private:
   double thetaIN, phiIN;
   double mA, ma, mb, mB;
   int AA, Aa, Ab, AB;
   int zA, za, zb, zB;
   double TA, T; // TA = KE of A pre u, T = total energy
   double ExA;
   
   bool isReady;
   
   double k; // Lab frame momentum mA 
   double beta, gamma;
   double Etot;
   double p; // CM frame momentum of b, B
};

TransferReaction::TransferReaction(){
   thetaIN = 0.;
   phiIN = 0.;
   SetA(12, 6, 0);
   Seta(2,1);
   Setb(1,1);
   SetB(13,6);
   TA = 6;
   T = TA * AA;
   
   Constant();
}

TransferReaction::~TransferReaction(){

}

void TransferReaction::Constant(){
   k = TMath::Sqrt(TMath::Power(mA + T, 2) - mA * mA); 
   beta = k / (mA + ma + T);
   gamma = 1 / TMath::Sqrt(1- beta * beta);   
   Etot = TMath::Sqrt(TMath::Power(mA + ma + T,2) - k * k);
   p = TMath::Sqrt( (Etot*Etot - TMath::Power(mb + mB,2)) * (Etot*Etot - TMath::Power(mb - mB,2)) ) / 2 / Etot;
   
   isReady = true;
}

TLorentzVector * TransferReaction::Event(double Ex, double thetaCM, double phiCM)
{
   if( isReady == false ){
      Constant();
   }

   TLorentzVector PA;
   PA.SetXYZM(0, 0, k, mA + ExA);
   PA.RotateY(thetaIN);
   PA.RotateZ(phiIN);
   
   TLorentzVector Pa(0, 0, 0, ma);
   
   //---- to CM frame
   TLorentzVector Pc = PA + Pa;
   TVector3 b = Pc.BoostVector();
   TVector3 v0 (0,0,0);
   TVector3 nb = v0 - b;
   
   TLorentzVector PAc = PA; 
   PAc.Boost(nb);
   TVector3 vA = PAc.Vect();
   
   TLorentzVector Pac = Pa;
   Pac.Boost(nb);
   TVector3 va = Pac.Vect();
   
   //--- from Pb
   TLorentzVector Pbc;
   TVector3 vb = va;
   vb.SetMag(p);

   TVector3 ub = vb.Orthogonal();
   vb.Rotate(thetaCM, ub);
   vb.Rotate(phiCM, va);
   
   Pbc.SetVectM(vb, mb);
   
   //--- from PB
   TLorentzVector PBc;
   TVector3 vB = vA;
   vB.SetMag(p);

   TVector3 uB = vB.Orthogonal();
   vB.Rotate(-thetaCM, uB);
   vB.Rotate(-phiCM, vA);
   
   PBc.SetVectM(vB, mB + Ex);
   
   //---- to Lab Frame
   TLorentzVector Pb = Pbc;
   Pb.Boost(b);
   TLorentzVector PB = PBc;
   PB.Boost(b);
   
   TLorentzVector * output = new TLorentzVector[4];
   output[0] = PA;
   output[1] = Pa;
   output[2] = Pb;
   output[3] = PB;

   return output;   
}

