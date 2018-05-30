#include "TBenchmark.h"
#include "TLorentzVector.h"
#include "TMath.h"
#include "TFile.h"
#include "TTree.h"
#include "TRandom.h"
#include <vector>
#include "TransferReaction.h"
#include <fstream>

void HELIOS(){

   int option;
   printf(" include finite detector size correction (1 = yes, 0 = No)? ");
   scanf("%d", &option);
   float energyResolution;
   printf(" Energy resolution [MeV]? ");
   scanf("%f", &energyResolution);
   printf(" Energy resol. = %f MeV\n", energyResolution);
   float posResolution;
   printf(" Position resolution [mm]? ");
   scanf("%f", &posResolution);
   printf(" Position resol. = %f mm\n", posResolution);
   int numEvent;
   printf(" Number of Events ? ");
   scanf("%d", &numEvent);

   const double c = 299.792458;

   int Zb, ZB;
   double mA, ma, mb, mB;
   int AA;
   double T;
   
   vector<double> ExKnown;
   printf("----- loading reaction parameters.");
   ifstream file;
   file.open("reaction.txt");
   string reactionName;
   if( file.is_open() ){
      string x;
      int i = 0;
      while( file >> x){
         //printf("%d, %s \n", i,  x.c_str());
         if( x.substr(0,2) == "//" ) continue;
         if( i == 0 ) reactionName = x;
         if( i == 1 ) mA = atof(x.c_str());
         if( i == 2 ) ma = atof(x.c_str());
         if( i == 3 ) mb = atof(x.c_str());
         if( i == 4 ) mB = atof(x.c_str());
         if( i == 5 ) Zb = atoi(x.c_str());
         if( i == 6 ) ZB = atoi(x.c_str());
         if( i == 7 ) AA = atoi(x.c_str());
         if( i == 8 ) T  = atof(x.c_str()) * AA; 
         if ( i >= 9 ){
            ExKnown.push_back(atof(x.c_str()));
         }
         i = i + 1;
      }
      file.close();
      printf("... done.\n");
      printf("========== %s, beam energy : %6.2f MeV/u \n", reactionName.c_str(), T / AA);
      int n = ExKnown.size();
      for(int i = 0; i < n ; i++){
         printf("%d, %6.2f MeV \n", i, ExKnown[i]);
      }
   }else{
       printf("... fail\n");
       return;
   }
   
   //=========== detector geometry
   double Bfield = 2.85; // Tesla
   double bore = 925.0; // bore , mm
   double a = 11.5; // distance from axis
   double w = 9.0; // width   
   double posRecoil = 1044.5 ; // recoil, downstream
   double l = 50.5; // length
   double support = 43.5;
   double firstPos = -218.5; // m 
   double pos[6] = {-294.8, -236.2, -176.9, -118, -59., 0.}; // near position in m
   
   printf("----- loading detector geometery.");
   file.open("detectorGeo.txt");
   if( file.is_open() ){
      string x;
      int i = 0;
      while( file >> x){
         //printf("%d, %s \n", i,  x.c_str());
         if( x.substr(0,2) == "//" ) continue;
         if( i == 0 ) Bfield = atof(x.c_str());
         if( i == 1 ) bore = atof(x.c_str());
         if( i == 2 ) a    = atof(x.c_str());
         if( i == 3 ) w    = atof(x.c_str());
         if( i == 4 ) posRecoil = atof(x.c_str());
         if( i == 5 ) l    = atof(x.c_str());
         if( i == 6 ) support = atof(x.c_str());
         if( i == 7 ) firstPos = atof(x.c_str());
         if( i >= 8 ) {
            pos[i-8] = atof(x.c_str());
         }
         i = i + 1;
      }
      file.close();
      printf("... done.\n");
      
      for(int id = 0; id < 6; id++){
         pos[id] = firstPos + pos[id];
      }
      
      printf("========== Recoil detector: %6.2f mm \n", posRecoil);
      printf("========== gap of multi-loop: %6.2f mm \n", -(firstPos + support));
      for(int i = 0; i < 6 ; i++){
         printf("%d, %6.2f mm \n", i, pos[i]);
      }
   }else{
       printf("... fail\n");
       return;
   }
   
   double dphiAccept = 2* TMath::ATan(2*a/w);
   if( option == 0) {
      printf(" setting a = 0 \n");
      a = 0; 
      dphiAccept = TMath::TwoPi(); 
   }
      
   //====================== build tree
   TString saveFileName = "test_2.root";
   TFile * saveFile = new TFile(saveFileName, "recreate");
   TTree * tree = new TTree("tree", "tree");

   double z, x;
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
   tree->Branch("x", &x, "x/D");
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
         thetaCM = TMath::Pi() * gRandom->Rndm();         
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
         if( bore > 2*rho &&  rho > a/2. && ((firstPos < 0 && theta > TMath::PiOver2() ) || (firstPos > 0 && theta < TMath::PiOver2() ) ) ){ 

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

            if( firstPos < 0 ) {
               double minNoBlock = pos[5] + support ;
               double minDis = pos[5];
               if( minDis/2. < z0 && z0 < minNoBlock ) {
                  tag = 1;
                  dphi += TMath::TwoPi() ;
               }else if( pos[0] - l < z0 && z0 < pos[5] ){
                  tag = 2;
               }else{
                  tag = 3;
                  dphi = 0;
               }
            }else{
               double minNoBlock = pos[0] - l - support ;
               double minDis = pos[0] - l;
               if( minNoBlock < z0 &&  z0 < minDis/2. ) {
                  tag = 1;
                  dphi += TMath::TwoPi() ;
               }else if( pos[0] - l < z0 && z0 < pos[5] ){
                  tag = 2;
               }else{
                  tag = 3;
                  dphi = 0;
               }
            }
            //printf("tag %d------- z:%f, dphi: %f , phiAccpeted %d\n", tag, z0, dphi, phiAccepted);
               
            if( phiAccepted ){
               t = dphi * rho / vt0;
               z = vp0 * t; // milimeter   
               z += gRandom->Gaus(0, posResolution); // add pos resolution
            
               // check z-acceptance
               for( int ID = 0; ID < 6; ID ++){
                  if( pos[ID]-l < z && z < pos[ID] ) {
                     redoFlag = false;
                     detID = ID;
                     //Calculate x
                     x = (z - (pos[ID]- l/2))/l*2; // range from -1, 1
                  } 
               }
               
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

   saveFile->Write();
   saveFile->Close();
   
   printf("========== done.\n");
   printf("========= saved as %s.\n", saveFileName.Data());
   
}
