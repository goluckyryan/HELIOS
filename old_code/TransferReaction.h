#ifndef TRANSFERREACTION
#define TRANSFERREACTION

#include "TLorentzVector.h"
#include "TMath.h"

TLorentzVector * Reaction(double mA, double ma, double mb, double mB, double T, double thetaCM, double phiCM){
   // reaction A(a,b)B
   // particle A is incident particle
   // thetaCM defined as the angle between A and B or a and b
   double k = TMath::Sqrt(TMath::Power(mA + T, 2) - mA * mA);    
   double beta = k / (mA + ma + T);
   double gamma = 1 / TMath::Sqrt(1- beta * beta);
   
   double Etot = TMath::Sqrt(TMath::Power(mA + ma + T,2) - k * k);
   
   double p = TMath::Sqrt( (Etot*Etot - TMath::Power(mb + mB,2)) * (Etot*Etot - TMath::Power(mb - mB,2)) ) / 2 / Etot;
   
   TLorentzVector * FourVector = new TLorentzVector[2];
   
   //particle b
   FourVector[0].SetPxPyPzE(- p * TMath::Sin(thetaCM) * TMath::Cos(phiCM),
                            - p * TMath::Sin(thetaCM) * TMath::Sin(phiCM),
                            gamma * beta * TMath::Sqrt(mb * mb + p * p) - gamma *p * TMath::Cos(thetaCM),
                            gamma * TMath::Sqrt(mb * mb + p * p) - gamma * beta * p * TMath::Cos(thetaCM)
                           );
   //particle B       
   FourVector[1].SetPxPyPzE( p * TMath::Sin(thetaCM) * TMath::Cos(phiCM),
                             p * TMath::Sin(thetaCM) * TMath::Sin(phiCM),
                            gamma * beta * TMath::Sqrt(mB * mB + p * p) + gamma *p * TMath::Cos(thetaCM),
                            gamma * TMath::Sqrt(mB * mB + p * p) + gamma * beta * p * TMath::Cos(thetaCM)
                            );
                            
                            
   return FourVector;
}

double * ReactionInveriance(double mA, double ma, double mb, double mB, double T){

   double k = TMath::Sqrt(TMath::Power(mA + T, 2) - mA * mA);    
   double beta = k / (mA + ma + T);
   double gamma = 1 / TMath::Sqrt(1- beta * beta);   
   double Etot = TMath::Sqrt(TMath::Power(mA + ma + T,2) - k * k);
   double p = TMath::Sqrt( (Etot*Etot - TMath::Power(mb + mB,2)) * (Etot*Etot - TMath::Power(mb - mB,2)) ) / 2 / Etot;
   
   double * output = new double[3];
   output[0] = beta;
   output[1] = gamma;
   output[2] = p; 
                            
   return output;
}

#endif
