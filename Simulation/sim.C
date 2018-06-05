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

   //===== Set Reaction
   TransferReaction reaction;
   
   int AA = 25, zA = 12;
   int Aa = 2,  za = 1;
   int Ab = 1,  zb = 1;
   int AB = AA+Aa-Ab, zB = zA+za-zb;
   
   reaction.SetA(AA,zA);
   reaction.Seta(Aa,za);
   reaction.Setb(Ab,zb);
   reaction.SetB(AB,zB);
   
   reaction.SetIncidentEnergyAngle(6, 0, 0);
   reaction.CalReactioConstant();
   
   printf("=========== Q-value : %f MeV, Max Ex: %f MeV \n", reaction.GetQValue(), reaction.GetMaxExB());
   
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
   helios.SetDetectorGeometry("detectorGeo_upstream.txt");
   
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
         thetaCM = TMath::Pi() * gRandom->Rndm(); 
         ExID = gRandom->Integer(ExKnown.size());
         Ex = ExKnown[ExID]; 
         
         //KEA = 12 + 0.5*(gRandom->Rndm()-0.5);
         //KEA = gRandom->Gaus(6., 0.05);
         KEA = 6.;
         //theta = 0;
         theta = gRandom->Gaus(0, 0.025);
         //phi = 0.;
         phi = TMath::TwoPi() * gRandom->Rndm();
         reaction.SetIncidentEnergyAngle(KEA, theta, phi);
         reaction.SetExB(Ex);
         TLorentzVector * output = reaction.Event(thetaCM, 0);
      
         TLorentzVector Pb = output[2];
         TLorentzVector PB = output[3];
         
         thetab = Pb.Theta() * TMath::RadToDeg();
         thetaB = PB.Theta() * TMath::RadToDeg();
      
         Tb = Pb.E() - Pb.M();
         TB = PB.E() - PB.M();
         
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
