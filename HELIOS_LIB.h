#include "TBenchmark.h"
#include "TLorentzVector.h"
#include "TVector3.h"
#include "TMath.h"
#include "TFile.h"
#include "TTree.h"
#include "TRandom.h"
#include <vector>
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
   
   TLorentzVector GetPA(){return PA;}
   TLorentzVector GetPa(){return Pa;}
   TLorentzVector GetPb(){return Pb;}
   TLorentzVector GetPB(){return PB;}
   
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
   
   TLorentzVector PA, Pa, Pb, PB;
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
   
   TLorentzVector temp (0,0,0,0);
   PA = temp;
   Pa = temp;
   Pb = temp;
   PB = temp;
   
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
   
   this->PA = PA;
   this->Pa = Pa;
   this->Pb = Pb;
   this->PB = PB;
   
   return output;   
}


class HELIOS{
public:

   // what is the input? particle initial motion, detector configuration
   HELIOS();
   ~HELIOS();
   
   int CalHit(TLorentzVector Pb, int Zb, TLorentzVector PB, int ZB); // return 0 for no hit, 1 for hit
   void SetDetectorGeometry(string filename);

   double GetEnergy(){return e;}
   double GetZ(){return z;}
   double GetX(){return x;}
   int GetDetID(){return detID;}
   double GetTime(){return t;}
   double GetRho(){return rho;}
   double GetdPhi(){return dphi;}
   int GetLoop(){return loop;}
   double GetRecoilEnergy(){return eB;}
   double GetRecoilTime(){return tB;}
private:
   //what is the output? energy, x, z, rho, dphi
   double e, z, x, rho, dphi, t;
   int detID, loop;   // multiloop
   double eB, rhoB, tB;
   bool isReady;
   
   //detetcor Geometry
   double Bfield; // T
   double bore; // bore , mm
   double a; // distance from axis
   double w; // width   
   double posRecoil; // recoil, downstream
   double l; // length
   double support;
   double firstPos; // m 
   vector<double> pos; // near position in m
   int nDet;
};

HELIOS::HELIOS(){
   e = TMath::QuietNaN();
   z = TMath::QuietNaN();
   x = TMath::QuietNaN();
   t = TMath::QuietNaN();
   rho = TMath::QuietNaN();
   dphi = TMath::QuietNaN();
   detID = -1;
   loop = -1;
   eB = TMath::QuietNaN();
   rhoB = TMath::QuietNaN();
   tB = TMath::QuietNaN();
   isReady = false;

   Bfield = 0;
   bore = 1000;
   a = 10;
   w = 10;
   posRecoil = 0;
   l = 0;
   support = 0;
   firstPos = 0;
   pos.clear();
   nDet = 0;
}

HELIOS::~HELIOS(){
}

void HELIOS::SetDetectorGeometry(string filename){
   
   pos.clear();
   
   printf("----- loading detector geometery : %s.", filename.c_str());
   ifstream file;
   file.open(filename.c_str());
   string line;
   int i = 0;
   if( file.is_open() ){
      string x;
      while( file >> x){
         //printf("%d, %s \n", i,  x.c_str());
         if( x.substr(0,2) == "//" )  continue;
         
         if( i == 0 ) Bfield = atof(x.c_str());
         if( i == 1 ) bore = atof(x.c_str());
         if( i == 2 ) a    = atof(x.c_str());
         if( i == 3 ) w    = atof(x.c_str());
         if( i == 4 ) posRecoil = atof(x.c_str());
         if( i == 5 ) l    = atof(x.c_str());
         if( i == 6 ) support = atof(x.c_str());
         if( i == 7 ) firstPos = atof(x.c_str());
         if( i >= 8 ) {
            pos.push_back(atof(x.c_str()));
         }
         i = i + 1;
      }
      
      nDet = pos.size();
      file.close();
      printf("... done.\n");
      
      for(int id = 0; id < nDet; id++){
         pos[id] = firstPos + pos[id];
      }
      
      printf("========== B-field: %6.2f T \n", Bfield);
      printf("========== Recoil detector: %6.2f mm \n", posRecoil);
      printf("========== gap of multi-loop: %6.2f mm \n", firstPos > 0 ? firstPos - support : firstPos + support );
      for(int i = 0; i < nDet ; i++){
         printf("%d, %6.2f mm \n", i, pos[i]);
      }
      
   }else{
       printf("... fail\n");
   }
   isReady = true;
}

int HELIOS::CalHit(TLorentzVector Pb, int Zb, TLorentzVector PB, int ZB){
   if( isReady == false ) {
      printf("Please Set Detector Geometry. SetDetectorGeometry(TString filename)");
      return;
   }
   
   //initialization
   int hit = 0;
   const double c = 299.792458; // m/s
   //double dphiAccept = 2* TMath::ATan(2*a/w);
   e = TMath::QuietNaN();
   z = TMath::QuietNaN();
   x = TMath::QuietNaN();
   t = TMath::QuietNaN();
   rho = TMath::QuietNaN();
   dphi = TMath::QuietNaN();
   detID = -1;
   loop = -1;
   eB = TMath::QuietNaN();
   rhoB = TMath::QuietNaN();
   tB = TMath::QuietNaN();
   
   //--- X-Y plane
   rho = Pb.Pt() / Bfield / Zb / c * 1000; //mm
   double  theta = Pb.Theta();
   
   if( bore > 2 * rho && rho > a && ((firstPos > 0 && theta < TMath::PiOver2())  || (firstPos < 0 && theta > TMath::PiOver2())) ){
      //rotate Pb back to X-Z plane
      //double phib = Pb.Phi();
      //Pb.RotateZ(-phib);
      
      // calculate rotation angle
      dphi = TMath::TwoPi() + TMath::ASin(-a/rho);  // exact position
      double hit_y = rho - TMath::Sqrt(rho*rho - a*a); // always positive
      if( hit_y > w/2.  ) {
         return -4;
      }

      //infinite small detector   
       
      double    vt0 = Pb.Beta() * TMath::Sin(theta) * c ; // mm / nano-second  
      double    vp0 = Pb.Beta() * TMath::Cos(theta) * c ; // mm / nano-second  
      double t0 = dphi * rho / vt0; // nano-second   
      double z0 = vp0 * t0; // mm        
      
      //printf("====== z0: %f \n",  z0);
      
      // calculate number of loop
      if( firstPos > 0 ){ // for downstream setting
         double minNoBlock = pos[0] - support ;
         double minDis = pos[0];
         if( minNoBlock < z0 &&  z0 < minDis/2. ) {
            loop = 2;
            dphi += TMath::TwoPi() * (loop - 1);
         }else if( pos[0] < z0 && z0 < pos[5] + l ){
            loop = 1;
         }else{
            loop = 0;
            dphi = 0;
            return -5;
         }
      }else{ // for upstream setting
         double minNoBlock = pos[5] + support ;
         double minDis = pos[5];
         if( minDis/2. < z0 &&  z0 < minNoBlock ) {
            loop = 2;
            dphi += TMath::TwoPi() * (loop - 1);
         }else if( pos[0]-l < z0 && z0 < pos[5] ){
            loop = 1;
         }else{
            loop = 0;
            dphi = 0;
            return -6;
         }
      }
      
      //printf("====== theta: %f deg, z0: %f \n", theta * TMath::RadToDeg(), z0); 
      //printf("         Loop : %d , dphi: %4.2fpi \n", loop, dphi/ TMath::Pi());
      
      // final calculation
      e = Pb.E() - Pb.M();
      t = dphi * rho / vt0;
      z = vp0 * t;
      
      for( int i = 0 ; i < nDet ; i++){
         if( firstPos > 0 ){
            if( pos[i] < z  && z < pos[i] + l ){
               detID = i;
               x = ( z - (pos[i] + l/2 ))/ l*2 ;// range from -1 , 1 
            }
         }else{
            if( pos[i] - l < z  && z < pos[i] ){
               detID = i;
               x = ( z - (pos[i] + l/2 ))/ l*2 ;// range from -1 , 1 
            }
         }
      }
      
      if( detID >=0  ){
         hit = 1;      
      }else{
         hit = -7;
      }
      //printf("%d, %f, %f, %f, %d\n", hit, e, z, t, detID);
      double  thetaB = Pb.Theta();
      rhoB = PB.Pt() / Bfield / ZB / c * 1000; //mm
      tB   = posRecoil / (PB.Beta() * TMath::Cos(thetaB) * c ); // nano-second
      eB   = PB.E() - PB.M();
      
   }else{
      hit = 0;
      //printf(" theta: %f deg \n", theta*TMath::RadToDeg());
   }

   return hit;
}

