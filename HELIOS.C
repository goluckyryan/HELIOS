#include "TBenchmark.h"
#include "TLorentzVector.h"
#include "TMath.h"
#include "TFile.h"
#include "TTree.h"
#include "TRandom.h"
#include <vector>

TLorentzVector * Reaction(double mA, double ma, double mb, double mB, double T, double thetaCM, double phiCM){

   double k = TMath::Sqrt(TMath::Power(mA + T, 2) - mA * mA);    
   double beta = k / (mA + ma + T);
   double gamma = 1 / TMath::Sqrt(1- beta * beta);
   
   double Etot = TMath::Sqrt(TMath::Power(mA + ma + T,2) - k * k);
   
   double p = TMath::Sqrt( (Etot*Etot - TMath::Power(mb + mB,2)) * (Etot*Etot - TMath::Power(mb - mB,2)) ) / 2 / Etot;
   
   TLorentzVector * FourVector = new TLorentzVector[2];
   
   // light particle
   FourVector[0].SetPxPyPzE(- p * TMath::Sin(thetaCM) * TMath::Cos(phiCM),
                            - p * TMath::Sin(thetaCM) * TMath::Cos(phiCM),
                            gamma * beta * TMath::Sqrt(mb * mb + p * p) - gamma *p * TMath::Cos(thetaCM),
                            gamma * TMath::Sqrt(mb * mb + p * p) - gamma * beta * p * TMath::Cos(thetaCM)
                           );
   // heavy particle       
   FourVector[1].SetPxPyPzE( p * TMath::Sin(thetaCM) * TMath::Cos(phiCM),
                             p * TMath::Sin(thetaCM) * TMath::Cos(phiCM),
                            gamma * beta * TMath::Sqrt(mB * mB + p * p) + gamma *p * TMath::Cos(thetaCM),
                            gamma * TMath::Sqrt(mB * mB + p * p) + gamma * beta * p * TMath::Cos(thetaCM)
                            );
                            
   // beta, gamma, p
   //FourVector[2].SetPxPyPzE(beta, gamma, 0, p);
                            
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


void HELIOS(){

   int option;
   printf(" include finite detector size correction (1 = yes, 0 = No)? ");
   scanf("%d", &option);
   float energyResolution;
   printf(" Energy resolution ? ");
   scanf("%f", &energyResolution);
   printf(" Energy resol. = %f \n", energyResolution);
   int numEvent;
   printf(" Number of Events ? ");
   scanf("%d", &numEvent);

   const double c = 299.792458;


   int Zb = 1;
   int ZB = 12;
   
   double Bfield = 2.85; // Tesla

   double mA = 23268.0269 ; // 25Mg
   double ma =  1875.6129 ; // d
   double mb =   938.272  ; // p
   double mB = 24196.4992 ; // 26Mg

   double T =  6 * 25;
   
   vector<double> ExKnown;
   ExKnown.push_back(0);
   ExKnown.push_back(1.80874);
   ExKnown.push_back(2.93833);
//   ExKnown.push_back(3.58856);
   ExKnown.push_back(3.94157);
   ExKnown.push_back(4.33252);
//   ExKnown.push_back(4.83513);
   ExKnown.push_back(4.97230);
   ExKnown.push_back(5.29174);
   ExKnown.push_back(5.47605);
   ExKnown.push_back(5.69108);
//   ExKnown.push_back(5.71591);
   ExKnown.push_back(6.12547);
//   ExKnown.push_back(6.2562);
   ExKnown.push_back(7.0619);
   ExKnown.push_back(7.428);
   ExKnown.push_back(8.1);
   ExKnown.push_back(8.5);
      
   //====================== build tree
   TString saveFileName = "test_2.root";
   TFile * saveFile = new TFile(saveFileName, "recreate");
   TTree * tree = new TTree("tree", "tree");

   double z;
   double e;
   double t;
   double thetaCM;
   double phiCM;
   double Ex;
   double theta;
   double dphi;
   double rho;
   int tag;
   int detID;
   double thetaR, eR, rhoR;
   double tR;
   int ExID;
   //double zThetaCMSlope, zThetaCMConst; // becasue the finite detector correction, the effective slope and constant must be deduced from simulation.

   tree->Branch("e", &e, "e/D");
   tree->Branch("z", &z, "z/D");
   tree->Branch("t", &t, "t/D");
   tree->Branch("rho", &rho, "rho/D");
   tree->Branch("dphi", &dphi, "dphi/D");
   tree->Branch("theta", &theta, "theta/D");
   tree->Branch("thetaCM", &thetaCM, "thetaCM/D");
   tree->Branch("phiCM", &phiCM, "phiCM/D");
   tree->Branch("Ex", &Ex, "Ex/D");
   tree->Branch("tag", &tag, "tag/I");
   tree->Branch("detID", &detID, "detID/I");
   tree->Branch("eR", &eR, "eR/D");
   tree->Branch("thetaR", &thetaR, "thetaR/D");
   tree->Branch("rhoR", &rhoR, "rhoR/D");
   tree->Branch("tR", &tR, "tR/D");
   tree->Branch("ExID", &ExID, "ExID/I");
   //tree->Branch("zThetaCMSlope", &zThetaCMSlope, "zThetaCMSlope/D");
   //tree->Branch("zThetaCMConst", &zThetaCMConst, "zThetaCMSConst/D");

   //=========== detector geometry
   double bore = 925.0; // bore , mm
   double a = 11.5; // distance from axis
   if( option == 0) {
      printf(" setting a = 0 \n");
      a = 0; 
   }
   double w = 9.0; // width
   
   double dphiAccept = 2* TMath::ATan(2*a/w);
   if( option == 0 ) dphiAccept = TMath::TwoPi(); 
   
   double posRecoil = 1044.5 ; // recoil, downstream
   
   double l = 50.5; // length
   double support = 43.5;
   double firstPos = 218.5; // m 
   double pos[6] = {0. , 59., 118., 176.9, 236.2, 294.8}; // near position in m
   
   for(int id = 0; id < 6; id++){
      pos[id] = -firstPos - pos[id];
   }

   //timer
   TBenchmark clock;
   bool shown ;   
   clock.Reset();
   clock.Start("timer");
   shown = false;
   printf("================= generating %d events \n", numEvent);
   
   int i = 0;
   for( i = 0; i < numEvent; i++){  
      bool phiAccepted = false;
      bool redoFlag = true;
      do{
         detID = 0;
         tag = 0;
         redoFlag = true;
         thetaCM = 0.5*TMath::Pi() * gRandom->Rndm();
         //phiCM = 2*TMath::Pi() * gRandom->Rndm();
         phiCM = 0.;
         ExID = gRandom->Integer(ExKnown.size());
         Ex = ExKnown[ExID]; 
         //Ex = 10 * gRandom->Rndm(); 
      
         TLorentzVector * P = Reaction(mA, ma, mb, mB + Ex, T, thetaCM , phiCM);
         
         thetaCM = thetaCM * TMath::RadToDeg();
         
         Ex += gRandom->Gaus(0, energyResolution); // add energy resolution
         
         theta = P[0].Theta(); // Lab theta
         rho   = P[0].Pt() / Bfield / Zb / c * 1000.; // mm
         
         thetaR = P[1].Theta();
         rhoR   = P[1].Pt() / Bfield / ZB / c * 1000 ; // mm 
         tR     = posRecoil / (P[1].Beta() * TMath::Cos(thetaR) * c ); // nano-second
         eR     = P[1].E() - mB;
         
         //set gate on the obital size, and must be scattered backward 
         if( bore > 2*rho &&  rho > a/2. && theta > TMath::PiOver2() ){ 

            double    vt0 = P[0].Beta() * TMath::Sin(theta) * c ; // mm / nano-second  
            double    vp0 = P[0].Beta() * TMath::Cos(theta) * c ; // mm / nano-second  
            e = P[0].E() - mb + gRandom->Gaus(0, energyResolution); // KE
            
            
            if( rho > a ) {
               dphi = TMath::TwoPi() + TMath::ASin(-a/rho);  // exact position
            }else{
               dphi = TMath::TwoPi() - 2*TMath::ASin(TMath::Sqrt(a/2/rho));
            }
            
            //double * para = ReactionInveriance(mA, ma, mb, mB + Ex, T);
            //zThetaCMConst = dphi/ Bfield/ Zb / c * para[0] * para[1] * TMath::Sqrt( TMath::Power(mb,2) + TMath::Power(para[2],2)) * 1000.;
            //zThetaCMSlope = dphi/ Bfield/ Zb / c * para[1] * para[2] * 1000.;
            
            if( TMath::Abs(dphi/2 - TMath::PiOver2()) < dphiAccept  || TMath::Abs(dphi/2 - TMath::Pi()) < dphiAccept ) phiAccepted = true;
            
            //infinite small detector
            double t0 = dphi * rho / vt0; // nano-second   
            double z0 = vp0 * t0; // mm   

            if( pos[0]+support < z0 && z0 < 0 ) {
               tag = 1;
               dphi += TMath::TwoPi();
            }else if( pos[5] - l < z0 && z0 < pos[0] ){
               tag = 2;
            }else{
               tag = 3;
               dphi = 0;
            }
            //printf("tag %d------- z:%f, dphi: %f , phiAccpeted %d\n", tag, z0, dphi, phiAccepted);
               
            if( phiAccepted ){
               t = dphi * rho / vt0;
               z = vp0 * t; // meter   
            
               // check z-acceptance
               for( int ID = 0; ID < 6; ID ++){
                  if( pos[ID]-l < z && z < pos[ID] ) {
                     redoFlag = false;
                     detID = 5-ID;
                  } 
               }
               
               //z += gRandom->Gaus(0, 0.002); // add position resolution   
               
               //printf("-------detID: %d,  z:%f, dphi: %f , redo : %d\n", detID, z, dphi, redoFlag);
               
            }else{
               redoFlag = true;
            }
            
            //if( 0.005 < rho && rho < a ) {
            //   printf("rho :%f, z0: %f, z: %f, tag: %d, phiAccepted: %d, redoFlag %d , dphi: %f deg\n", rho, z0, z, tag, phiAccepted, redoFlag, dphi *TMath::RadToDeg() );
            //}

         }
         
      }while( redoFlag );
      
      tree->Fill();
      
      //#################################################################### Timer  
      clock.Stop("timer");
      Double_t time = clock.GetRealTime("timer");
      clock.Start("timer");

      if ( !shown ) {
         if (fmod(time, 10) < 1 ){
            printf( "%10d[%2d%%]| %8.2f sec | expect: %5.1f min \n", i, TMath::Nint((i+1)*100./numEvent), time , numEvent*time/(i+1)/60);
            shown = 1;
         }
      }else{
         if (fmod(time, 10) > 9 ){
            shown = 0;
         }
      }

      
   }
   
   //cHelios->cd(1);
   //h->Draw("colz");
   //cHelios->cd(2);
   //g->Draw("colz");

   saveFile->Write();
   saveFile->Close();
   
   printf("========== done.\n");
   printf("========= saved as %s.\n", saveFileName.Data());
   
}
