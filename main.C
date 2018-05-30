#include "HELIOS_LIB.h"

int main(){

   TransferReaction reaction;
   
   reaction.SetA(25,12);
   reaction.Seta(2,1);
   reaction.Setb(1,1);
   reaction.SetB(26,12);
   
   reaction.SetIncidentEnergyPerU(6);
   reaction.SetIncidentAngle(0, 0);
   
   HELIOS helios;
   helios.SetDetectorGeometry("detectorGeo_upstream.txt");
   
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
   
   //====================== build tree
   TString saveFileName = "test_2.root";
   TFile * saveFile = new TFile(saveFileName, "recreate");
   TTree * tree = new TTree("tree", "tree");
   
   double thetaCM;
   double thetab, Tb;
   double thetaB, TB;
   
   double e, z, x, t;
   int loop, detID;
   
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
   
   //timer
   TBenchmark clock;
   bool shown ;   
   clock.Reset();
   clock.Start("timer");
   shown = false;
   int numEvent = 1000;
   printf("================= generating %d events \n", numEvent);
   
   int count = 0;
   for( int i = 0; i < numEvent; i++){
      bool redoFlag = true;
      do{
         thetaCM = 0.5*TMath::Pi() * gRandom->Rndm(); 
         TLorentzVector * output = reaction.Event(0, thetaCM, 0);
      
         TLorentzVector Pb = output[2];
         TLorentzVector PB = output[3];
         
         thetab = Pb.Theta() * TMath::RadToDeg();
         thetaB = PB.Theta() * TMath::RadToDeg();
      
         Tb = Pb.E() - Pb.M();
         TB = PB.E() - PB.M();
         
         int hit = helios.CalHit(Pb, 1, PB, 12);
         if( hit == 1) {
            count ++;
            e = helios.GetEnergy();
            z = helios.GetZ();
            x = helios.GetX();
            t = helios.GetTime();
            loop = helios.GetLoop();
            detID = helios.GetDetID();
            redoFlag = false;
         }else{
            redoFlag = true;
            //printf("%d, %2d, thetaCM : %f, theta : %f \n", i, hit, thetaCM * TMath::RadToDeg(), thetab);
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
   
   return 0;
}
