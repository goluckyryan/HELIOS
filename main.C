#include "HELIOS_LIB.h"

int main(){
   TransferReaction test;
   
   test.SetA(16,7);
   test.Seta(2,1);
   test.Setb(3,2);
   test.SetB(15,6);
   
   //double thetaCM = 40 * TMath::DegToRad();
   
   test.SetIncidentEnergyPerU(12);
   test.SetIncidentAngle(0, 0);
   
   /*
   TLorentzVector * output = test.Event(0, thetaCM, 0);
   TLorentzVector PA = output[0];
   TLorentzVector Pa = output[1];
   TLorentzVector Pb = output[2];
   TLorentzVector PB = output[3];
   PA.Print();
   Pa.Print();
   Pb.Print();
   PB.Print();
   printf("==================\n");
   (PA+Pa).Print();
   (PB+Pb).Print();
   */
   
   //====================== build tree
   TString saveFileName = "test_2.root";
   TFile * saveFile = new TFile(saveFileName, "recreate");
   TTree * tree = new TTree("tree", "tree");
   
   double thetaCM;
   double thetab, Tb;
   double thetaB, TB;
   
   tree->Branch("thetab", &thetab, "thetab/D");
   tree->Branch("Tb", &Tb, "Tb/D");
   tree->Branch("thetaB", &thetaB, "thetaB/D");
   tree->Branch("TB", &TB, "Tb/D");
   tree->Branch("thetaCM", &thetaCM, "thetaCM/D");
   
   
   for( int i = 0; i < 10000; i++){
   
      thetaCM = TMath::Pi() * gRandom->Rndm(); 
      TLorentzVector * output = test.Event(0, thetaCM, 0);
   
      TLorentzVector Pb = output[2];
      TLorentzVector PB = output[3];
      
      thetab = Pb.Theta() * TMath::RadToDeg();
      thetaB = PB.Theta() * TMath::RadToDeg();
   
      Tb = Pb.E() - Pb.M();
      TB = PB.E() - PB.M();
      
      tree->Fill();
   }
   
   saveFile->Write();
   saveFile->Close();
   
   printf("========== done.\n");
   printf("========= saved as %s.\n", saveFileName.Data());
}
