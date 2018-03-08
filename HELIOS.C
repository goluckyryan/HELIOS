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
   
   FourVector[0].SetPxPyPzE(- p * TMath::Sin(thetaCM) * TMath::Cos(phiCM),
                            - p * TMath::Sin(thetaCM) * TMath::Cos(phiCM),
                            gamma * beta * TMath::Sqrt(mb * mb + p * p) - gamma *p * TMath::Cos(thetaCM),
                            gamma * TMath::Sqrt(mb * mb + p * p) - gamma * beta * p * TMath::Cos(thetaCM)
                           );
                           
   FourVector[1].SetPxPyPzE( p * TMath::Sin(thetaCM) * TMath::Cos(phiCM),
                             p * TMath::Sin(thetaCM) * TMath::Cos(phiCM),
                            gamma * beta * TMath::Sqrt(mB * mB + p * p) + gamma *p * TMath::Cos(thetaCM),
                            gamma * TMath::Sqrt(mB * mB + p * p) + gamma * beta * p * TMath::Cos(thetaCM)
                            );
                            
   return FourVector;
}


void HELIOS(){

   int option;
   printf(" include finite detector size correction (1 = yes, 0 = No)? ");
   scanf("%d", &option);
   
   //timer
   TBenchmark clock;
   bool shown ;

   double mA = 23268.0269 ; // 25Mg
   double ma =  1875.6129 ; // d
   double mb =   938.272  ; // p
   double mB = 24196.4992 ; // 26Mg
   
   vector<double> ExKnown;
   ExKnown.push_back(0);
   ExKnown.push_back(1.80874);
   ExKnown.push_back(2.93833);
//   ExKnown.push_back(3.58856);
   ExKnown.push_back(3.94157);
   ExKnown.push_back(4.33252);
//   ExKnown.push_back(4.83513);
   ExKnown.push_back(4.97230);
//   ExKnown.push_back(5.29174);
   ExKnown.push_back(5.47605);
//   ExKnown.push_back(5.71591);
//   ExKnown.push_back(6.12547);
//   ExKnown.push_back(6.2562);
//   ExKnown.push_back(7.0619);
   ExKnown.push_back(7.2000);
      
   int Zb = 1;
   int ZB = 12;
   
   double T =  6 * 25;
   double Bfield = 2.85; // Tesla

   TFile * saveFile = new TFile("test.root", "recreate");
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

   tree->Branch("e", &e, "e/D");
   tree->Branch("z", &z, "z/D");
   tree->Branch("t", &e, "t/D");
   tree->Branch("rho", &rho, "rho/D");
   tree->Branch("dphi", &dphi, "dphi/D");
   tree->Branch("theta", &theta, "theta/D");
   tree->Branch("thetaCM", &thetaCM, "thetaCM/D");
   tree->Branch("phiCM", &phiCM, "phiCM/D");
   tree->Branch("Ex", &Ex, "Ex/D");
   tree->Branch("tag", &tag, "tag/I");
   tree->Branch("detID", &detID, "detID/I");

   const double c = 299.792458;
   const int numEvent = 2000000;
   
   //finite detector
   double bore = 0.925; // bore , meter
   double a = 0.0115; // distance from axis
   if( option == 0) {
      printf(" setting a = 0 \n");
      a = 0; 
   }
   double w = 0.0090; // width
   
   double dphiAccept = 2* TMath::ATan(2*a/w);
   if( option == 0 ) dphiAccept = TMath::TwoPi(); 
   
   double l = 0.0505; // length
   double support = 0.0435;
   double firstPos = 0.2185; // m 
   double pos[6] = {0. ,0.059, 0.118, 0.1769, 0.2362, 0.2948}; // near position in m
   
   for(int i = 0; i < 6; i++){
      pos[i] = -firstPos - pos[i];
   }
   
   clock.Reset();
   clock.Start("timer");
   shown = false;
   printf("================= generating events \n");
   
   for( int i = 0; i < numEvent; i++){  
      bool phiAccepted = false;
      bool redoFlag = true;
      do{
         detID = 0;
         tag = 0;
         redoFlag = true;
         thetaCM = 0.5*TMath::Pi() * gRandom->Rndm();
         //phiCM = 2*TMath::Pi() * gRandom->Rndm();
         phiCM = 0.;
         int seed = gRandom->Integer(ExKnown.size());
         Ex = ExKnown[seed] + + gRandom->Gaus(0, 0.04);
      
         TLorentzVector * P = Reaction(mA, ma, mb, mB + Ex, T, thetaCM , phiCM);
         
         theta = P[0].Theta(); // Lab theta
         rho   = P[0].Pt() / Bfield / Zb / c ; // meter

         if( bore > 2*rho &&  rho > a/2. && theta > TMath::PiOver2() ){ 

            double    vt0 = P[0].Beta() * TMath::Sin(theta) * c / 1e+3; // meter/ nano-second  
            double    vp0 = P[0].Beta() * TMath::Cos(theta) * c / 1e+3; // meter/ nano-second  
            e = P[0].E() - mb ; // KE
            
            
            if( rho > a ) {
               dphi = TMath::TwoPi() + TMath::ASin(-a/rho);  // exact position
            }else{
               dphi = TMath::TwoPi() - 2*TMath::ASin(TMath::Sqrt(a/2/rho));
            }
            
            if( TMath::Abs(dphi/2 - TMath::PiOver2()) < dphiAccept  || TMath::Abs(dphi/2 - TMath::Pi()) < dphiAccept ) phiAccepted = true;
            
            
            //infinite small detector
            double t0 = dphi * rho / vt0; // nano-second   
            double z0 = vp0 * t0; // meter   

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
                     detID = ID;
                  } 
               }
               //z += gRandom->Gaus(0, 0.002); // add position resolution   
               
               //printf("-------detID: %d,  z:%f, dphi: %f , redo : %d\n", detID, z, dphi, redoFlag);
               
            }else{
               redoFlag = true;
            }

         }
         
      }while( redoFlag );
      
      tree->Fill();
      
      //#################################################################### Timer  
      clock.Stop("timer");
      Double_t time = clock.GetRealTime("timer");
      clock.Start("timer");

      if ( !shown ) {
         if (fmod(time, 10) < 1 ){
            printf( "%10d[%2d%%]|%3d min %5.2f sec | expect:%5.1fmin\n", 
                  i, 
                  TMath::Nint((i+1)*100./numEvent),
                  TMath::Floor(time/60), time - TMath::Floor(time/60)*60,
                  numEvent*time/(i+1)/60.);
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
   
}
