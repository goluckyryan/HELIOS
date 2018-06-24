#include "TBenchmark.h"
#include "TLorentzVector.h"
#include "TVector3.h"
#include "TMath.h"
#include "TFile.h"
#include "TTree.h"
#include "TRandom.h"
#include "TGraph.h"
#include <vector>
#include <fstream>
#include "Isotope.h"

//=======================================================
//#######################################################
//Class for Transfer Reaction
//======================================================= 
class TransferReaction {
public:
   TransferReaction();
   ~TransferReaction();
  
   void SetA(int A, int Z, double Ex = 0){
      Isotope temp (A, Z);
      mA = temp.Mass;
      AA = A;
      zA = Z;
      ExA = Ex;
      nameA = temp.Name;
      isReady = false;
      isBSet = true;
   }
   void Seta(int A, int Z){
      Isotope temp (A, Z);
      ma = temp.Mass;
      Aa = A;
      za = Z;
      namea = temp.Name;
      isReady = false;
      isBSet = false;
   }
   void Setb(int A, int Z){
      Isotope temp (A, Z);
      mb = temp.Mass;
      Ab = A;
      zb = Z;
      nameb = temp.Name;
      isReady = false;
      isBSet = false;
   }
   void SetB(int A, int Z){
      Isotope temp (A, Z);
      mB = temp.Mass;
      AB = A;
      zB = Z;
      nameB = temp.Name;
      isReady = false;
      isBSet = true;
   }
   void SetIncidentEnergyAngle(double KEA, double theta, double phi){
      this->TA = KEA;
      this->T = TA * AA;
      this->thetaIN = theta;
      this->phiIN = phi;
      isReady = false;
   }
   void SetExA(double Ex){
      this->ExA = Ex;
      isReady = false;
   }
   void SetExB(double Ex){
      this->ExB = Ex;
      isReady = false;
   }
   
   TString GetReactionName(){
      TString rName;
      rName.Form("%s(%s,%s)%s", nameA.c_str(), namea.c_str(), nameb.c_str(), nameB.c_str()); 
      return rName;
   }
   
   int GetAtomicNumber_A(){return AA;}
   int GetAtomicNumber_a(){return Aa;}
   int GetAtomicNumber_b(){return Ab;}
   int GetAtomicNumber_B(){return AB;}
   
   int GetCharge_A(){return zA;}
   int GetCharge_a(){return za;}
   int GetCharge_b(){return zb;}
   int GetCharge_B(){return zB;}
   
   double GetCMTotalKE() {return Etot - mA - ma;}
   double GetQValue() {return mA + ma - mb - mB;}
   double GetMaxExB() {return Etot - mb - mB;}
   
   TLorentzVector GetPA(){return PA;}
   TLorentzVector GetPa(){return Pa;}
   TLorentzVector GetPb(){return Pb;}
   TLorentzVector GetPB(){return PB;}
   
   void CalReactioConstant();
   TLorentzVector * Event(double thetaCM, double phiCM);
   
private:
   string nameA, namea, nameb, nameB;
   double thetaIN, phiIN;
   double mA, ma, mb, mB;
   int AA, Aa, Ab, AB;
   int zA, za, zb, zB;
   double TA, T; // TA = KE of A pre u, T = total energy
   double ExA, ExB;
   
   bool isReady;
   bool isBSet;
   
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
   
   ExA = 0;
   ExB = 0;
   
   CalReactioConstant();
   
   TLorentzVector temp (0,0,0,0);
   PA = temp;
   Pa = temp;
   Pb = temp;
   PB = temp;
   
}

TransferReaction::~TransferReaction(){

}

void TransferReaction::CalReactioConstant(){
   if( !isBSet){
      AB = AA + Aa - Ab;
      zB = zA + za - zb;
      Isotope temp (AB, zB);
      mB = temp.Mass;
      isBSet = true;
   }
   
   k = TMath::Sqrt(TMath::Power(mA + ExA + T, 2) - (mA + ExA) * (mA + ExA)); 
   beta = k / (mA + ExA + ma + T);
   gamma = 1 / TMath::Sqrt(1- beta * beta);   
   Etot = TMath::Sqrt(TMath::Power(mA + ExA + ma + T,2) - k * k);
   p = TMath::Sqrt( (Etot*Etot - TMath::Power(mb + mB + ExB,2)) * (Etot*Etot - TMath::Power(mb - mB - ExB,2)) ) / 2 / Etot;
   
   PA.SetXYZM(0, 0, k, mA + ExA);
   PA.RotateY(thetaIN);
   PA.RotateZ(phiIN);
   
   Pa.SetXYZM(0,0,0,ma);
   
   isReady = true;
}

TLorentzVector * TransferReaction::Event(double thetaCM, double phiCM)
{
   if( isReady == false ){
      CalReactioConstant();
   }

   //TLorentzVector Pa(0, 0, 0, ma);
   
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
   vb.Rotate(phiCM + TMath::PiOver2(), va); // somehow, the calculation turn the vector 90 degree.
   
   Pbc.SetVectM(vb, mb);
   
   //--- from PB
   TLorentzVector PBc;
   TVector3 vB = vA;
   vB.SetMag(p);

   TVector3 uB = vB.Orthogonal();
   vB.Rotate(-thetaCM, uB);
   vB.Rotate(-phiCM - TMath::PiOver2(), vA);
   
   PBc.SetVectM(vB, mB + ExB);
   
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
//=======================================================
//#######################################################
//Class for HELIOS
//======================================================= 
class HELIOS{
public:

   HELIOS();
   ~HELIOS();
   
   int CalHit(TLorentzVector Pb, int Zb, TLorentzVector PB, int ZB); // return 0 for no hit, 1 for hit
   void SetDetectorGeometry(string filename);
   
   int GetNumberOfDetectorsInSamePos(){return mDet;}

   double GetEnergy(){return e;}
   double GetZ(){return z;}
   double GetX(){return x;} // position in each detector, range from -1, 1
   int GetDetID(){return detID;}
   double GetTime(){return t;}
   double GetRho(){return rho;}
   double GetRhoHit(){return rhoHit;} // radius of particle-b hit recoil detector 
   double GetdPhi(){return dphi;}
   int GetLoop(){return loop;}
   double GetVt(){return vt0;}
   double GetVp(){return vp0;}
   double GetXPos(double ZPos){
      return rho * (TMath::Cos(phi) - TMath::Cos(TMath::Tan(theta) * ZPos / rho + phi) );
   }
   double GetYPos(double ZPos){
      return rho * (TMath::Sin(TMath::Tan(theta) * ZPos / rho + phi) - TMath::Sin(phi)) ;
   }
   double GetR(double ZPos){
      return rho * TMath::Sqrt(2 - 2* TMath::Cos( TMath::Tan(theta) * ZPos / rho));
   }
   
   double GetRecoilEnergy(){return eB;}
   double GetRecoilTime(){return tB;}
   double GetRecoilRho(){return rhoB;}
   double GetRecoilRhoHit(){return rhoBHit;}
   double GetRecoilVt(){return vt0B;}
   double GetRecoilVp(){return vp0B;}
   double GetRecoilXPos(double ZPos){
      return rhoB * (TMath::Cos(phiB) - TMath::Cos(TMath::Tan(thetaB) * ZPos / rhoB + phiB) );
   }
   double GetRecoilYPos(double ZPos){
      return rhoB * (TMath::Sin(TMath::Tan(thetaB) * ZPos / rhoB) - TMath::Sin(phiB));
   }   
   double GetRecoilR(double ZPos){
      return rhoB * TMath::Sqrt(2 - 2* TMath::Cos( TMath::Tan(thetaB) * ZPos / rhoB));
   }
   
   double GetZ0(){return z0;}
   double GetTime0(){return t0;}
private:
   double theta, phi; // polar angle of particle 
   double e, z, x, rho, dphi, t;
   double vt0, vp0;
   double rhoHit; // radius of particle-b hit on recoil detector
   int detID, loop;   // multiloop
   
   double thetaB, phiB;
   double eB, rhoB, tB;
   double vt0B, vp0B;
   double rhoBHit; // particle-B hit radius
   bool isReady;
   
   double z0, t0; // infinite detector 
   
   //detetcor Geometry
   double Bfield; // T
   double bore; // bore , mm
   double a; // distance from axis
   double w; // width   
   double posRecoil; // recoil, downstream
   double rhoRecoil; // radius recoil
   double l; // length
   double support;
   double firstPos; // m 
   vector<double> pos; // near position in m
   int nDet, mDet; // nDet = number of different pos, mDet, number of same pos
};

HELIOS::HELIOS(){
   theta = TMath::QuietNaN();
   phi = TMath::QuietNaN();
   e = TMath::QuietNaN();
   z = TMath::QuietNaN();
   x = TMath::QuietNaN();
   t = TMath::QuietNaN();
   rho = TMath::QuietNaN();
   rhoHit = TMath::QuietNaN();
   dphi = TMath::QuietNaN();
   detID = -1;
   loop = -1;
   
   thetaB = TMath::QuietNaN();
   phiB = TMath::QuietNaN();
   eB = TMath::QuietNaN();
   rhoB = TMath::QuietNaN();
   rhoBHit = TMath::QuietNaN();
   tB = TMath::QuietNaN();
   isReady = false;

   z0 = TMath::QuietNaN();
   t0 = TMath::QuietNaN();

   Bfield = 0;
   bore = 1000;
   a = 10;
   w = 10;
   posRecoil = 0;
   rhoRecoil = 0;
   l = 0;
   support = 0;
   firstPos = 0;
   pos.clear();
   nDet = 0;
   mDet = 0;
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
         if( i == 5 ) rhoRecoil   = atof(x.c_str());
         if( i == 6 ) l    = atof(x.c_str());
         if( i == 7 ) support = atof(x.c_str());
         if( i == 8 ) firstPos = atof(x.c_str());
         if( i == 9 ) mDet = atoi(x.c_str());
         if( i >= 10 ) {
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
      printf("========== Recoil detector: %6.2f mm, radius: %6.2f mm \n", posRecoil, rhoRecoil);
      printf("========== gap of multi-loop: %6.2f mm \n", firstPos > 0 ? firstPos - support : firstPos + support );
      for(int i = 0; i < nDet ; i++){
         if( firstPos > 0 ){
            printf("%d, %6.2f mm - %6.2f mm \n", i, pos[i], pos[i] + l);
         }else{
            printf("%d, %6.2f mm - %6.2f mm \n", i, pos[i] - l , pos[i]);
         }
      }
      printf("=======================\n");
      
   }else{
       printf("... fail\n");
       isReady = false;
   }
   isReady = true;
}

int HELIOS::CalHit(TLorentzVector Pb, int Zb, TLorentzVector PB, int ZB){
   if( isReady == false ) {
      printf("Please Set Detector Geometry. SetDetectorGeometry(TString filename)");
      return 0;
   }
   
   //initialization
   int hit = 0;
   const double c = 299.792458; // m/s
   theta = TMath::QuietNaN();
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
   
   //====================== X-Y plane
   rho = Pb.Pt() / Bfield / Zb / c * 1000; //mm
   theta = Pb.Theta();
   phi = Pb.Phi();
   //printf("bore:%f, rho:%f, firstPos:%f, theta:%f < %f \n", bore, rho, firstPos, theta, TMath::PiOver2() );
   
   if( bore > 2 * rho && rho > a && ((firstPos > 0 && theta < TMath::PiOver2())  || (firstPos < 0 && theta > TMath::PiOver2())) ){
      //====================== infinite small detector   
      vt0 = Pb.Beta() * TMath::Sin(theta) * c ; // mm / nano-second  
      vp0 = Pb.Beta() * TMath::Cos(theta) * c ; // mm / nano-second  
      t0 = TMath::TwoPi() * rho / vt0; // nano-second   
      z0 = vp0 * t0; // mm        
      //printf("====== z0: %f \n",  z0);

      //======================  recoil detector
      thetaB = PB.Theta();
      phiB = PB.Phi();
      rhoB = PB.Pt() / Bfield / ZB / c * 1000; //mm
      tB   = posRecoil / (PB.Beta() * TMath::Cos(thetaB) * c ); // nano-second
      eB   = PB.E() - PB.M();
      
      // check particle-b 
      rhoHit = GetR(posRecoil) ;// radius of light particle b at recoil detector
      if( z0 > 0 && posRecoil > 0 && z0 > posRecoil && rhoHit < rhoRecoil) {
          return -2 ;
      }
      if( z0 < 0 && posRecoil < 0 && z0 < posRecoil && rhoHit < rhoRecoil) {
          return -3 ;
      }
      // calculate particle-B hit radius on recoil dectector
      vt0B = PB.Beta() * TMath::Sin(thetaB) * c ; // mm / nano-second  
      vp0B = PB.Beta() * TMath::Cos(thetaB) * c ; // mm / nano-second  
      rhoBHit = GetRecoilR(posRecoil);
      if( rhoBHit > rhoRecoil ) return -4;
      
      //====================== calculate rotation angle
      // the calculation assume phi is near phi = 0, the detector is located at x = -a with |y| < w/2
      // for small rho trajectory that is unable to hit the detector but adjusance one, we ignored that.
      //dphi = TMath::TwoPi() - phi - TMath::ASin((a-rho*TMath::Sin(phi))/rho);  // exact position
      //double hit_y = rho*TMath::Cos(phi) - TMath::Sqrt(rho*rho - (a - rho*TMath::Sin(phi))*(a - rho*TMath::Sin(phi)));
      //if( hit_y > w/2. || hit_y < -w/2. ) {
      //   return -1;
      //}
      
      // kind of ray tracking. using GetXPos() and GetYPos()
      // Calulate z hit
      double zHit = TMath::QuietNaN();
      bool isHit = false;
      int hitMDetID = -1;
      for( int j = 0 ; j < mDet; j++){
         double phiDet = TMath::TwoPi() / mDet * j; // plane angle 
         
         //printf("========== mDet : %d , phiDet: %f deg, phi: %f deg, theta: %f deg \n", j, phiDet * TMath::RadToDeg(), phi*TMath::RadToDeg(), theta*TMath::RadToDeg() );
         
         int n = -1;
         bool isLess = true;
         bool isHitFromOutside = false;
         do{
            isLess = true;
            isHitFromOutside = false;
            n += 1;
            if( n > 10 ) {
               return -4;
               break; // maximum 6 loops
            }
            zHit = rho / TMath::Tan(theta) * ( phiDet - phi + TMath::Power(-1, n) * TMath::ASin(a/rho + TMath::Sin(phi-phiDet)) + TMath::Pi() * n );
            if( firstPos > 0 ){
               if( zHit > pos[0] ){
                  isLess = false;
               }
            }else{
               if( pos[nDet-1] > zHit ){ 
                  isLess = false;
               }
            }
            
            // this is the particel direction (normalized) dot normal vector of the detector plane
            double dir = TMath::Cos( TMath::Tan(theta) * zHit/ rho + phi - phiDet);
            if( dir < 0) isHitFromOutside = true;
            
            //printf(" ----  n : %d , zHit: %f mm, dir: %f, (%d, %d)  \n", n, zHit, dir, isLess, isHitFromOutside );
            
         }while(isLess || !isHitFromOutside); 
         // when zHit is less then min detector distance, redo
         // if zHit is larger then min detector distance, and if hit from inside, redo
         
         if(firstPos > 0 &&  zHit > pos[nDet-1] + l) return -5; 
         if(firstPos < 0 &&  zHit < pos[0] ) return -6; 
         
         
         // if zHit is lager, and hit from outside, use that zHit
         loop = n / 2 + 1 ; // number of loop
         double xHit = GetXPos(zHit);
         double yHit = GetYPos(zHit);
         
         // calculate the distance from middle of detector
         double sHit = TMath::Sqrt(xHit*xHit + yHit*yHit - a*a);
         
         //printf("  loop : %d, sHit : %f mm , w/2 : %f mm\n", loop, sHit, w/2.);
         
         if( sHit < w/2. ){
            isHit = true;
            hitMDetID = j;
            break;     // if isHit, break, don't calculate for the rest of the detector
         } 
      }
      
      if( !isHit ) return -1;
      
      //printf("#################### is Hit: %d \n", hitMDetID);
      
      /*
      //====================== calculate number of loop
      t0 = dphi * rho / vt0; // nano-second   
      z0 = vp0 * t0; // mm, the positon for an imaginary finite detector
      double zloop = TMath::TwoPi() * rho * vp0 / vt0 * t0; // the position for complete cycle
      if( firstPos > 0 ){ // for downstream setting
         double minNoBlock = pos[0] - support ;
         double minDis = pos[0];
         int loopA = TMath::Ceil((minNoBlock-z0)/zloop);
         double zLoopA = loopA * zloop + z0;
         if( zLoopA > minDis ) {
            loop = TMath::Ceil((minDis-z0)/zloop) + 1;
         }else{
            loop = 0;
            z = z0;
            e = Pb.E() - Pb.M();
            return -5;
         }
      }else{ // for upstream setting
         double minNoBlock = pos[nDet-1] + support ;
         double minDis = pos[nDet-1];
         int loopA = TMath::Ceil((minNoBlock-z0)/zloop);
         double zLoopA = loopA * zloop + z0;
         if( zLoopA < minDis ) {
            loop = TMath::Ceil((minDis-z0)/zloop) + 1;
         }else{
            loop = 0;
            z= z0;
            e = Pb.E() - Pb.M();
            return -5;
         }
      }
      */
      //printf("====== theta: %f deg, z0: %f \n", theta * TMath::RadToDeg(), z0); 
      //printf("         Loop : %d , dphi: %4.2fpi \n", loop, dphi/ TMath::Pi());
      
      //====================== final calculation for light particle
      e = Pb.E() - Pb.M();
      z = zHit;
      t = zHit / vp0;
      dphi = t * vt0 / rho;
      dphi += (loop - 1)*TMath::TwoPi();
      
      //t = dphi * rho / vt0;
      //z = vp0 * t;
      
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
      
      
   }else{
      hit = 0;
      //printf(" theta: %f deg \n", theta*TMath::RadToDeg());
   }

   return hit;
}

//=======================================================
//#######################################################
// Class for multi-scattering of the beam inside target
// using SRIM to generate stopping power
//=======================================================
class TargetScattering{
//input : TLorentzVector, data_files
//output : scattered TLorentzVector
public:
   TargetScattering();
   ~TargetScattering();
   
   double GetKE0(){return KE0;}
   double GetKE() {return KE;}
   double GetKELoss() {return KE0-KE;}
   double GetDepth() {return depth;}
   double GetPathLength() {return length;}
   
   void LoadStoppingPower(string file);
   
   void SetTarget(double density, double depth){
      this->density = density;
      this->depth = depth;
      isTargetSet = true;
      //printf("===== Target, density: %f g/cm3, depth: %f um \n", density, depth * 1e+4 );
   }
   
   TLorentzVector Scattering(TLorentzVector P){
      double mass = P.M();
      KE0 = (P.E() - mass);
      KE = KE0;
      double theta = P.Theta();   
      this->length = TMath::Abs(depth/TMath::Cos(theta));
      //printf("------- theta: %f deg, length: %f um, KE: %f MeV \n", theta * TMath::RadToDeg(), this->length * 1e+4, KE);
      
      //integrate the energy loss within the depth of A
      double dx = length/100.; 
      double x = 0;
      double densityUse = density;
      if( unitID == 0 ) densityUse = 1.;
      do{
         //assume the particle will not be stopped
         //printf(" x: %f, KE:  %f, S: %f \n", x, KE, gA->Eval(KE));
         KE = KE - densityUse * gA->Eval(KE) * 10. * dx ; // factor 10, convert MeV/mm -> MeV/cm
         x = x + dx;
      }while(x < length);
      
      //printf(" depth: %f cm = %f um, KE : %f -> %f MeV , dE = %f MeV \n", depth, depth * 1e+4, KE0, KE, KE0 - KE);
      
      double newk = TMath::Sqrt(TMath::Power(mass+KE,2) - mass * mass);
      
      TVector3 vb = P.Vect();
      vb.SetMag(newk);
      
      TLorentzVector Pnew;
      Pnew.SetVectM(vb,mass);
      
      return Pnew;
   }
   
private:
   bool isTargetSet;
   double density; // density [mg/cm2]
   int unitID; // 0 = MeV /mm or keV/um , 1 = MeV / (ug/cm2) 
   
   double depth; // depth in target [cm]
   double length; // total path length in target [cm]
   double KE0, KE;
   
   TGraph * gA; // stopping power of A, b, B, in unit of MeV/(mg/cm2)
      
};

TargetScattering::TargetScattering(){
   isTargetSet = false;
   density = 1; // mg/cm2
   unitID = 0; 
   KE0 = 0;
   KE = 0;
   depth = 0;
   length = 0;
   gA = NULL;
}

TargetScattering::~TargetScattering(){
   delete gA;
}

void TargetScattering::LoadStoppingPower(string filename){
   
   printf("##### loading Stopping power: %s.", filename.c_str());
   
   ifstream file;
   file.open(filename.c_str());
   
   vector<double> energy;
   vector<double> stopping;
   
   if( file.is_open() ){
      printf("... OK\n");
      char lineChar[16635];
      string line;
      while( !file.eof() ){
         file.getline(lineChar, 16635, '\n');
         line = lineChar;

         //printf("%s \n", line.c_str());
         size_t found = line.find("Target Density");
         if( found != string::npos ){
            printf("    %s\n", line.c_str());
            //density = atof(line.substr(18,12).c_str()); 
         }
         
         
         found = line.find("Stopping Units =");
         if( found != string::npos){         
            printf("      %s\n", line.c_str());
            if( line.find("MeV / mm") != string::npos ) { 
               unitID = 0;
            }else if( line.find("keV / micron") != string::npos ){
               unitID = 0;
            }else if( line.find("MeV / (mg/cm2)") != string::npos ) {
               unitID = 1;
            }else{
               printf("please using MeV/mm, keV/um, or MeV/(mg/cm2) \n");  
            }
         }
         
         found = line.find("keV   ");
         if ( found != string::npos){
            //printf("%s \n", line.c_str());
            energy.push_back( atof(line.substr(0,7).c_str()) * 0.001);
            double a = atof(line.substr(14,10).c_str());
            double b = atof(line.substr(25,10).c_str());
            stopping.push_back(a+b);
            //printf("%f , %f \n", a, b);
         }
         
         found = line.find("MeV   ");
         if ( found != string::npos){
            //printf("%s \n", line.c_str());
            energy.push_back( atof(line.substr(0,7).c_str()));
            double a = atof(line.substr(14,10).c_str());
            double b = atof(line.substr(25,10).c_str());
            stopping.push_back(a+b);
            //printf("%f \n", energy.back());   
         }
         
      }
      
   }else{
       printf("... fail\n");
   }
   
   gA = new TGraph(energy.size(), &energy[0], &stopping[0]);
   
}
//=======================================================
//#######################################################
// Class for Particle Decay
// B --> d + D
//=======================================================
class Decay{
//input : TLorentzVector, emitting particle
//output : scattered TLorentzVector
public:
   Decay();
   ~Decay();
   
   double GetQValue() { return Q;}
   
   double GetAngleChange(){
      TVector3 vD = PD.Vect();
      TVector3 vB = PB.Vect();
      vD.SetMag(1);
      vB.SetMag(1);
      double dot = vD.Dot(vB);
      return TMath::ACos(dot)*TMath::RadToDeg() ;
   }
    
   double GetCMMomentum(){ return k;}
   TLorentzVector GetDaugther_d() {return Pd;}
   TLorentzVector GetDaugther_D() {return PD;}
   
   void SetMotherDaugther(int AB, int zB, int AD, int zD){
      Isotope Mother(AB, zB);
      Isotope Daugther_D(AD, zD);
      Isotope Daugther_d(AB-AD, zB-zD);

      mB = Mother.Mass;
      mD = Daugther_D.Mass;
      md = Daugther_d.Mass;
      
      isMotherSet = true;
   }
   
   int CalDecay(TLorentzVector P, double ExB, double ExD){
      if( !isMotherSet ) {
         return -1;
      }
      this->PB = P;
      
      double MB = mB + ExB;
      double MD = mD + ExD;
      Q = MB - MD - md;
      if( Q < 0 ) {
         this->PD = this->PB;
         dTheta = TMath::QuietNaN();
         k = TMath::QuietNaN();
         return -2;
      }
      
      //clear 
      TLorentzVector temp(0,0,0,0);
      PD = temp;
      Pd = temp;
      
      k = TMath::Sqrt((MB+MD+md)*(MB+MD-md)*(MB-MD+md)*(MB-MD-md))/2./MB;
      
      //in mother's frame, assume isotropic decay
      double theta = TMath::Pi() * gRandom->Rndm();
      double phi = TMath::TwoPi() * gRandom->Rndm();
      PD.SetE(TMath::Sqrt(mD * mD + k * k ));
      PD.SetPz(k);
      PD.SetTheta(theta);
      PD.SetPhi(phi);
      
      Pd.SetE(TMath::Sqrt(md * md + k * k ));
      Pd.SetPz(k);
      Pd.SetTheta(theta + TMath::Pi());
      Pd.SetPhi(phi + TMath::Pi());
      
      //Transform to Lab frame;
      TVector3 boost = PB.BoostVector();
      
      PD.Boost(boost);
      Pd.Boost(boost);
      
      return 1;
      
   }
   
private:
   TLorentzVector PB, Pd, PD;
   
   double mB, mD, md;
   
   bool isMotherSet;
   double Q;
   double k; // momentum in B-frame
   
   double dTheta;
   
};

Decay::Decay(){
   TLorentzVector temp(0,0,0,0);
   PB = temp;
   Pd = temp;
   PD = temp;
  
   mB = TMath::QuietNaN();
   mD = TMath::QuietNaN();
   md = TMath::QuietNaN(); 
   
   k = TMath::QuietNaN();
   
   Q = TMath::QuietNaN();
   dTheta = TMath::QuietNaN();
   isMotherSet = false;

}

Decay::~Decay(){
   
}
