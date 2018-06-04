#include "HELIOS_LIB.h"

void Aux(){
   TargetScattering ms;
   
   ms.LoadStoppingPower(1, "16N_in_CD2.txt");
   ms.LoadStoppingPower(2, "3He_in_CD2.txt");
   ms.LoadStoppingPower(3, "15C_in_CD2.txt");
   
   ms.SetTarget(0.913, 10e-4); // 0.913 g/cm3 , 10.0 um
   
   Isotopes N16(16,7);
   double mass = N16.Mass;
   
   double KE = 12.;
   double k = TMath::Sqrt(TMath::Power(mass + KE * 16 ,2) - mass* mass);
   
   TLorentzVector P(0,0,k, mass + KE * 16);
   
   //====================== build tree
   TString saveFileName = "test_3.root";
   TFile * saveFile = new TFile(saveFileName, "recreate");
   TTree * tree = new TTree("tree", "tree");
   
   double depth, KE0, KE;
   
   tree->Branch("depth", &depth, "depth/D");
   tree->Branch("KE0", &KE0, "KE0/D");
   tree->Branch("KE", &KE, "KE/D");
   
   
   printf("========================= \n");
   
   
   //========timer
   TBenchmark clock;
   bool shown ;   
   clock.Reset();
   clock.Start("timer");
   shown = false;
   int numEvent = 5000;
   printf("================= generating %d events \n", numEvent);
   
   for( int i = 0; i < numEvent; i++){
      TLorentzVector Pnew = ms.Scattering(16, P);
      
      depth = ms.GetDepth();
      KE0 = ms.GetKE0();
      KE = ms.GetKE();
      
      //printf("KE0 %f, KE %f \n", ms.GetKE0(), ms.GetKE());
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
   //P.Print();
   //Pnew.Print();
   
   saveFile->Write();
   saveFile->Close();
   
   printf("========== done.\n");
   
}
