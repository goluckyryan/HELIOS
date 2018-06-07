#include "HELIOS_LIB.h"
#include "TBenchmark.h"
#include "TLorentzVector.h"
#include "TMath.h"
#include "TFile.h"
#include "TTree.h"
#include "TRandom.h"
#include <vector>
#include <fstream>

void production(){

   int numEvent;
   printf(" Number of Events ? ");
   scanf("%d", &numEvent);
   
   bool isTargetScattering = true;
   
   //===== Set Reaction
   TransferReaction reaction;
   
   int AA = 15, zA = 7;
   int Aa = 2,  za = 1;
   int Ab = 1,  zb = 1;
   int AB = AA+Aa-Ab, zB = zA+za-zb;
   
   reaction.SetA(AA,zA);
   reaction.Seta(Aa,za);
   reaction.Setb(Ab,zb);
   reaction.SetB(AB,zB);
   
   reaction.SetIncidentEnergyAngle(14., 0, 0);
   reaction.CalReactioConstant();
   
   printf("=========== Q-value : %f MeV, Max Ex: %f MeV \n", reaction.GetQValue(), reaction.GetMaxExB());
   
   
   //-----loading excitation energy
   vector<double> ExKnown;
   printf("----- loading excitation energy levels.");
   ifstream file;
   file.open("Ex16N.txt");
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
   
   //====================== build tree
   TString saveFileName = "test_p.root";
   TFile * saveFile = new TFile(saveFileName, "recreate");
   TTree * tree = new TTree("tree", "tree");
   
   double thetaCM;
   double thetaB, TB;
   
   int ExID;
   double Ex, KEA, theta, phi;
   
   double KEAnew;
   
   tree->Branch("thetaB", &thetaB, "thetaB/D");
   tree->Branch("TB", &TB, "Tb/D");
   tree->Branch("thetaCM", &thetaCM, "thetaCM/D");
  
   tree->Branch("ExID", &ExID, "ExID/I");
   tree->Branch("Ex", &Ex, "Ex/D");
   tree->Branch("theta", &theta, "theta/D");
   tree->Branch("phi", &phi, "phi/D");
   tree->Branch("KEA", &KEA, "KEA/D");
   tree->Branch("KEAnew", &KEAnew, "KEAnew/D");
   
   //==== Target scattering, only energy loss
   TargetScattering msA;
   TargetScattering msB;

   double density = 0.913; // 0.913 g/cm3
   double thickness = 500 ; // ug/cm2
   double targetThickness = 500 * 1e-4 ; //thickness * 1e-6 / 0.913; // in cm
            
   if( isTargetScattering ){
      msA.LoadStoppingPower("16N_in_CD2.txt");
      msB.LoadStoppingPower("16N_in_CD2.txt");
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
      
      //==== Set Ex of B
      ExID = gRandom->Integer(ExKnown.size());
      Ex = ExKnown[ExID]; 
      reaction.SetExB(Ex);
      
      //==== Set incident beam
      //KEA = 12 + 0.5*(gRandom->Rndm()-0.5);
      //KEA = gRandom->Gaus(15., 0.1);
      KEA = 15.;
      theta = 0;
      //theta = gRandom->Gaus(0, 0.025);
      phi = 0.;
      //phi = TMath::TwoPi() * gRandom->Rndm();
      reaction.SetIncidentEnergyAngle(KEA, theta, phi);
      reaction.CalReactioConstant();
      TLorentzVector PA = reaction.GetPA();            
      
      double depth = 0;
      if( isTargetScattering ){
         //==== Target scattering, only energy loss
         depth = targetThickness * gRandom->Rndm();
         msA.SetTarget(density, depth); 
         TLorentzVector PAnew = msA.Scattering(PA);
         KEAnew = msA.GetKE()/AA;
         reaction.SetIncidentEnergyAngle(KEAnew, theta, phi);
      }else{
         KEAnew = KEA;
      }
      
      //==== Calculate reaction
      thetaCM = TMath::Pi() * gRandom->Rndm(); 
      TLorentzVector * output = reaction.Event(thetaCM, 0);
   
      TLorentzVector Pb = output[2];
      TLorentzVector PB = output[3];
      
      if( isTargetScattering ){
         //==== Calculate energy loss of scattered and recoil in target
         msB.SetTarget(density, targetThickness - depth);
         PB = msB.Scattering(PB);
      }
      
      //------------- 
      thetaB = PB.Theta() * TMath::RadToDeg(); 
      TB = PB.E() - PB.M();
      
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
