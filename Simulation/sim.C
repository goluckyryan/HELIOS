#include "HELIOS_LIB.h"
#include "TBenchmark.h"
#include "TLorentzVector.h"
#include "TMath.h"
#include "TFile.h"
#include "TTree.h"
#include "TRandom.h"
#include <vector>
#include <fstream>

void sim(){

   int numEvent;
   printf(" Number of Events ? ");
   scanf("%d", &numEvent);
   
   bool isTargetScattering = true;
   

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
   
   //---- excitation of Beam 
   double ExAList[2];
   ExAList[0] = 0.000;
   ExAList[1] = 0.120; 
   
   //-----loading excitation energy
   vector<double> ExKnown;
   printf("----- loading excitation energy levels.");
   ifstream file;
   file.open("excitation_energies.txt");
   string isotopeName;
   if( file.is_open() ){
      string line;
      int i = 0;
      while( file >> line){
         //printf("%d, %s \n", i,  line.c_str());
         if( line.substr(0,2) == "//" ) continue;
         if( i == 0 ) isotopeName = line; 
         if ( i >= 1 ){
            ExKnown.push_back(atof(line.c_str()));
         }
         i = i + 1;
      }
      file.close();
      printf("... done.\n");
      printf("========== %s\n", isotopeName.c_str());
      int n = ExKnown.size();
      for(int i = 0; i < n ; i++){
         printf("%d, %6.2f MeV \n", i, ExKnown[i]);
      }
   }else{
       printf("... fail\n");
   }
   
   //======== Set HELIOS   
   HELIOS helios;
   helios.SetDetectorGeometry("detectorGeo_downstream.txt");
   
   //====================== build tree
   TString saveFileName = "test_3.root";
   TFile * saveFile = new TFile(saveFileName, "recreate");
   TTree * tree = new TTree("tree", "tree");
   
   double thetaCM;
   double thetab, Tb;
   double thetaB, TB;
   
   double e, z, x, t;
   int loop, detID;
   double dphi, rho;
   int ExID;
   double Ex, KEA, theta, phi;
   double ExA;
   int ExAID;
   double beamk;
   double beamE;
   double rhoHit, rhoBHit;
   double zzb[100], xb[100], yb[100];
   
   tree->Branch("thetab", &thetab, "thetab/D");
   tree->Branch("Tb", &Tb, "Tb/D");
   tree->Branch("thetaB", &thetaB, "thetaB/D");
   tree->Branch("TB", &TB, "Tb/D");
   tree->Branch("thetaCM", &thetaCM, "thetaCM/D");
   tree->Branch("e", &e, "e/D");
   tree->Branch("x", &x, "x/D");
   tree->Branch("z", &z, "z/D");
   tree->Branch("t", &t, "t/D");
   tree->Branch("detID", &detID, "detID/I");
   tree->Branch("loop", &loop, "loop/I");
   tree->Branch("dphi", &dphi, "dphi/D");
   tree->Branch("rho", &rho, "rho/D");
   tree->Branch("ExID", &ExID, "ExID/I");
   tree->Branch("Ex", &Ex, "Ex/D");
   tree->Branch("theta", &theta, "theta/D");
   tree->Branch("phi", &phi, "phi/D");
   tree->Branch("KEA", &KEA, "KEA/D");
   tree->Branch("ExAID", &ExAID, "ExAID/I");
   tree->Branch("ExA", &ExA, "ExA/D");
   tree->Branch("beamk", &beamk, "beamk/D");
   tree->Branch("beamE", &beamE, "beamE/D");
   tree->Branch("rhoHit", &rhoHit, "rhoHit/D");
   tree->Branch("rhoBHit", &rhoBHit, "rhoBHit/D");
   tree->Branch("xb", xb, "xb[100]/D");
   tree->Branch("yb", yb, "yb[100]/D");
   tree->Branch("zb", zzb, "zb[100]/D");
   //==== Target scattering, only energy loss
   TargetScattering ms;
   TargetScattering msB;
   TargetScattering msb;
   double density = 0.913; // 0.913 g/cm3
   double targetThickness = 2.2e-4; // 2.2 um = 201 ug/cm2
            
   if( isTargetScattering ){
      ms.LoadStoppingPower("16N_in_CD2.txt");
      msb.LoadStoppingPower("3He_in_CD2.txt");
      msB.LoadStoppingPower("15C_in_CD2.txt");
   }
   
   //========timer
   TBenchmark clock;
   bool shown ;   
   clock.Reset();
   clock.Start("timer");
   shown = false;
   printf("================= generating %d events \n", numEvent);
   
   //====================================================== calculate 
   int count = 0;
   for( int i = 0; i < numEvent; i++){
      bool redoFlag = true;
      do{
      
         //==== Set Ex of A
         ExAID = gRandom->Integer(2);
         ExA = ExAList[ExAID];
         reaction.SetExA(ExA);
         
         //==== Set Ex of B
         ExID = gRandom->Integer(ExKnown.size());
         Ex = ExKnown[ExID]; 
         reaction.SetExB(Ex);
         
         //==== Set incident beam
         //KEA = 12 + 0.5*(gRandom->Rndm()-0.5);
         KEA = gRandom->Gaus(12., 0.05);
         //KEA = 12.;
         theta = 0;
         //theta = gRandom->Gaus(0, 0.025);
         phi = 0.;
         //phi = TMath::TwoPi() * gRandom->Rndm();
         reaction.SetIncidentEnergyAngle(KEA, theta, phi);
         reaction.CalReactioConstant();
         TLorentzVector PA = reaction.GetPA();            
         beamE = PA.E() - PA.M();
         beamk = PA.P();
         
         
         double depth = 0;
         if( isTargetScattering ){
            //==== Target scattering, only energy loss
            
            depth = targetThickness * gRandom->Rndm();
            ms.SetTarget(density, depth); 
            TLorentzVector PAnew = ms.Scattering(PA);
            double KEAnew = ms.GetKE()/AA;
            reaction.SetIncidentEnergyAngle(KEAnew, theta, phi);
         }
         
         //==== Calculate reaction
         thetaCM = TMath::Pi() * gRandom->Rndm(); 
         TLorentzVector * output = reaction.Event(thetaCM, 0);
      
         TLorentzVector Pb = output[2];
         TLorentzVector PB = output[3];
         
         if( isTargetScattering ){
            //==== Calculate energy loss of scattered and recoil in target
            msb.SetTarget(density, targetThickness - depth);
            Pb = msb.Scattering(Pb);
            msB.SetTarget(density, targetThickness - depth);
            PB = msB.Scattering(PB);
         }
         
         //------------- 
         thetab = Pb.Theta() * TMath::RadToDeg();
         thetaB = PB.Theta() * TMath::RadToDeg();
      
         Tb = Pb.E() - Pb.M();
         TB = PB.E() - PB.M();
         
         //==== Helios
         int hit = helios.CalHit(Pb, zb, PB, zB);
         if( hit == 1) {
            count ++;
            e = helios.GetEnergy();
            z = helios.GetZ();
            x = helios.GetX();
            t = helios.GetTime();
            loop = helios.GetLoop();
            detID = helios.GetDetID();
            dphi = helios.GetdPhi();
            rho = helios.GetRho();
            rhoHit = helios.GetRhoHit();
            rhoBHit = helios.GetRecoilRhoHit();
            
            for(int i = 0; i < 100 ; i++){
               double theta = Pb.Theta();
               zzb[i] = z/100.*( i + gRandom->Rndm() - 0.5 );
               xb[i] = rho * (1- TMath::Cos( TMath::Tan(theta) * zzb[i]/rho) );
               yb[i] = rho * TMath::Sin( TMath::Tan(theta) * zzb[i]/rho);
            }
            
            redoFlag = false;
         }else{
            redoFlag = true;
            //printf("%d, %2d, thetaCM : %f, theta : %f, z0: %f \n", i, hit, thetaCM * TMath::RadToDeg(), thetab, helios.GetZ0());
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
