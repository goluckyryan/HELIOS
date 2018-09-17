#include "HELIOS_LIB.h"
#include "TROOT.h"
#include "TBenchmark.h"
#include "TLorentzVector.h"
#include "TMath.h"
#include "TFile.h"
#include "TF1.h"
#include "TTree.h"
#include "TRandom.h"
#include <vector>
#include <fstream>
#include <TObjArray.h>

// A(a, b) B
//      b -> 1 + 2

//----------- usage 
// $root n2alpha.C+ | tee output.txt
// this will same the massage to output.txt

void n2alpha(){

   //================================================= User Setting
   //---- reaction
   int AA = 28, zA = 12;
   int Aa = 9,  za = 4;
   int Ab = 8,  zb = 4;
   
   //---- beam
   double KEAmean = 8.5; // MeV/u 
   double KEAsigma = 0; //KEAmean*0.001; // MeV/u , assume Guassian
   double thetaMean = 0.; // mrad 
   double thetaSigma = 0.; // mrad , assume Guassian due to small angle
   
   int numEvent = 100000;
   
   //---- HELIOS detector geometry
   string heliosDetGeoFile = "";// "detectorGeo.txt";
   double BField = 2.5; // T
   double BFieldTheta = 0.; // direction of B-field
   bool isCoincidentWithRecoil = false; 
   double eSigma = 0.0001 ; // detector energy sigma MeV
   double zSigma = 0.001 ; // detector position sigma mm
   
   //---- excitation of Beam 
   int nExA = 1;
   double ExAList[nExA];
   ExAList[0] = 0.000; // MeV
   //ExAList[1] = 1.567;
    
   //---- excitation of recoil
   string excitationFile = "excitation_energies.txt";
   
   //---- save root file name
   TString saveFileName = "n2alpha.root";

   //---- Auxiliary setting
   bool isDecayB = false;
   bool isReDo = false; // redo calculation until detected. 
   
   //=============================================================
   //=============================================================
   //=============================================================
   //===== Set Reaction
   TransferReaction reaction;
   int AB = AA+Aa-Ab, zB = zA+za-zb;
   reaction.SetA(AA,zA);
   reaction.Seta(Aa,za);
   reaction.Setb(Ab,zb);
   reaction.SetB(AB,zB);
   reaction.SetIncidentEnergyAngle(KEAmean, 0, 0);
   reaction.CalReactionConstant();
   
   printf("===================================================\n");
   printf("=========== %s ===========\n", reaction.GetReactionName().Data());
   printf("       KE: %7.4f +- %5.4f MeV/u, dp/p = %5.2f %% \n", KEAmean, KEAsigma, KEAsigma/KEAmean * 50.);
   printf("    theta: %7.4f +- %5.4f MeV/u \n", thetaMean, thetaSigma);
   printf(" Q-value : %7.4f MeV \n", reaction.GetQValue() );
   printf("   Max Ex: %7.4f MeV \n", reaction.GetMaxExB() );
   printf("===================================================\n");
   
   //======== Set HELIOS
   printf("############################################## HELIOS configuration\n");   
   HELIOS helios;
   helios.SetMagneticFieldDirection(BFieldTheta);
   bool sethelios = helios.SetDetectorGeometry(heliosDetGeoFile);
   if( !sethelios){
		helios.SetMagneticField(BField);
		printf("======== B-field : %5.2f T, Theta : %6.2f deg\n", BField, BFieldTheta);
	}
   helios.SetCoincidentWithRecoil(isCoincidentWithRecoil);
   int mDet = helios.GetNumberOfDetectorsInSamePos();
   printf("========== energy resol.: %f MeV\n", eSigma);
   printf("=========== pos-Z resol.: %f mm \n", zSigma);
   
   
   //======= Decay of particle-B and b -> 1 + 2
   Decay decayB;
   decayB.SetMotherDaugther(AB, zB, AB-1,zB); //neutron decay
   Decay decayb;
   decayb.SetMotherDaugther(Ab, zb, Ab-4, 2); //alpha decay
   
   //======= loading excitation energy
   printf("############################################## excitation energies\n");
   vector<double> ExKnown;
   vector<double> y0; // intercept of e-z plot
   vector<double> kCM; // momentum of b in CM frame
   printf("----- loading excitation energy levels.");
   ifstream file;
   file.open(excitationFile.c_str());
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
         if( isDecayB ) {
            TLorentzVector temp(0,0,0,0);
            int decayID = decayB.CalDecay(temp, ExKnown[i], 0);
            if( decayID == 1) {
               printf("%d, Ex: %6.2f MeV --> Decay. \n", i, ExKnown[i]);
            }else{
               printf("%d, Ex: %6.2f MeV\n", i, ExKnown[i]);
            }
         }else{
            printf("%d, Ex: %6.2f MeV \n", i, ExKnown[i]);
         }
      }
   }else{
       printf("... fail\n");
       return;
   }
   
   //====================== build tree
   TFile * saveFile = new TFile(saveFileName, "recreate");
   TTree * tree = new TTree("tree", "tree");
   
   double thetaCM;
   int ExID;
   double Ex, KEA, theta, phi; // beam incident angle
   double ExA;
   int ExAID;
   
   tree->Branch("thetaCM", &thetaCM, "thetaCM/D");
   tree->Branch("ExID", &ExID, "ExID/I");
   tree->Branch("Ex", &Ex, "Ex/D");
   tree->Branch("KEA", &KEA, "KEA/D");
   tree->Branch("theta", &theta, "theta/D");
   tree->Branch("phi", &phi, "phi/D");
   tree->Branch("ExAID", &ExAID, "ExAID/I");
   tree->Branch("ExA", &ExA, "ExA/D");

   double thetaB, phiB, TB;
   double thetab, phib, Tb;
   tree->Branch("thetab", &thetab, "thetab/D");
   tree->Branch("phib", &phib, "phib/D");
   tree->Branch("Tb", &Tb, "Tb/D");
   tree->Branch("thetaB", &thetaB, "thetaB/D");
   tree->Branch("phiB", &phiB, "phiB/D");
   tree->Branch("TB", &TB, "TB/D");
      
   int hit1, hit2; // the output of Helios.CalHit
   double theta1, phi1, T1;
   double theta2, phi2, T2;
   tree->Branch("hit1", &hit1, "hit1/I");
   tree->Branch("theta1", &theta1, "theta1/D");
   tree->Branch("phi1", &phi1, "phi1/D");
   tree->Branch("T1", &T1, "T1/D");
   
   tree->Branch("hit2", &hit2, "hit2/I");
   tree->Branch("theta2", &theta2, "theta2/D");
   tree->Branch("phi2", &phi2, "phi2/D");
   tree->Branch("T2", &T2, "T2/D");
   
   double e1, z1, t1;
   double x1Hit, y1Hit; //x, y positon of proton
   double rho1; //rad of rotation, and radius
   tree->Branch("e1", &e1, "e1/D");
   tree->Branch("z1", &z1, "z1/D");
   tree->Branch("t1", &t1, "t1/D");   
   tree->Branch("rho1", &rho1, "rho1/D");
   tree->Branch("x1Hit", &x1Hit, "xHit1/D");
   tree->Branch("y1Hit", &y1Hit, "yHit1/D");
   
   double e2, z2, t2;
   double x2Hit, y2Hit; //x, y positon of proton
   double rho2; //rad of rotation, and radius
   tree->Branch("e2", &e2, "e2/D");
   tree->Branch("z2", &z2, "z2/D");
   tree->Branch("t2", &t2, "t2/D");   
   tree->Branch("rho2", &rho2, "rho2/D");
   tree->Branch("x2Hit", &x2Hit, "xHit2/D");
   tree->Branch("y2Hit", &y2Hit, "yHit2/D");
   
   double recoilT, rhoBHit;
   double rxHit, ryHit;
   tree->Branch("recoilT", &recoilT, "recoilT/D");
   tree->Branch("rhoBHit", &rhoBHit, "rhoBHit/D");
   tree->Branch("rxHit", &rxHit, "rxHit/D");
   tree->Branch("ryHit", &ryHit, "ryHit/D");
   
   //========timer
   TBenchmark clock;
   bool shown ;   
   clock.Reset();
   clock.Start("timer");
   shown = false;
   printf("############################################## generating %d events \n", numEvent);
   
   //====================================================== calculate 
   int count = 0;
   for( int i = 0; i < numEvent; i++){
      bool redoFlag = true;
      if( !isReDo ) redoFlag = false;
      do{
      
         //==== Set Ex of A
         ExAID = gRandom->Integer(nExA);
         ExA = ExAList[ExAID];
         reaction.SetExA(ExA);
         
         //==== Set Ex of B
         ExID = gRandom->Integer(ExKnown.size());
         Ex = ExKnown[ExID]; 
         reaction.SetExB(Ex);
         
         //==== Set incident beam
         if( KEAsigma == 0 ){
            KEA = KEAmean;
         }else{
            KEA = gRandom->Gaus(KEAmean, KEAsigma);
         }
         if( thetaSigma == 0 ){
            theta = thetaMean;
         }else{
            theta = gRandom->Gaus(thetaMean, thetaSigma);
         }
         //for taregt scattering
         reaction.SetIncidentEnergyAngle(KEA, theta, 0.);
         reaction.CalReactionConstant();
         TLorentzVector PA = reaction.GetPA();            
         
         //==== Calculate reaction
         thetaCM = TMath::ACos(2 * gRandom->Rndm() - 1) ; 
         double phiCM = TMath::TwoPi() * gRandom->Rndm(); 
         
         TLorentzVector * output = reaction.Event(thetaCM, phiCM);
         
         TLorentzVector Pb = output[2];
         TLorentzVector PB = output[3];
         
         //======= Decay of particle-B
         if( isDecayB){
            int decayID = decayB.CalDecay(PB, Ex, 0); // decay to ground state
            if( decayID == 1 ){
               PB = decayB.GetDaugther_D();
            }
         }
         
         //======= Decay of particle-b -> 1 + 2
         decayb.CalDecay(Pb, 0, 0); // decay to ground state, assume Pb is not excited.
         TLorentzVector P1 = decayb.GetDaugther_D();
         TLorentzVector P2 = decayb.GetDaugther_d();
         
         
         //################################### tree branches
         //===== reaction
         thetab = Pb.Theta() * TMath::RadToDeg();
         phib = Pb.Phi() * TMath::RadToDeg();
         Tb = Pb.E() - Pb.M();
         
         thetaB = PB.Theta() * TMath::RadToDeg();
         phiB = PB.Phi() * TMath::RadToDeg();
         TB = PB.E() - PB.M();
         
         //==== Helios
         hit1 = helios.CalHit(P1, 2, PB, zB);
         e1 = helios.GetEnergy() + gRandom->Gaus(0, eSigma);
         z1 = helios.GetZ() ; 
         t1 = helios.GetTime();
         rho1 = helios.GetRho();
         x1Hit = helios.GetXPos(z1);
         y1Hit = helios.GetYPos(z1);
         z1 += gRandom->Gaus(0, zSigma);
         
         rhoBHit = helios.GetRecoilRhoHit();
         recoilT = helios.GetRecoilTime();
         rxHit = helios.GetRecoilXHit();
         ryHit = helios.GetRecoilYHit();
         
         hit2 = helios.CalHit(P2, 2, PB, zB);
         e2 = helios.GetEnergy() + gRandom->Gaus(0, eSigma);
         z2 = helios.GetZ() ; 
         t2 = helios.GetTime();
         rho2 = helios.GetRho();
         x2Hit = helios.GetXPos(z2);
         y2Hit = helios.GetYPos(z2);
         z2 += gRandom->Gaus(0, zSigma);
         
         //change thetaCM into deg
         thetaCM = thetaCM * TMath::RadToDeg();
         
         if( hit1 == 1 && hit2 == 1) {
            count ++;
         }
         
         if( isReDo ){
            if( hit1 == 1 && hit2 == 1) {
               redoFlag = false;
            }else{
               redoFlag = true;
               //printf("%d, %2d, thetaCM : %f, theta : %f, z0: %f \n", i, hit, thetaCM * TMath::RadToDeg(), thetab, helios.GetZ0());
            }
         }else{
            redoFlag = false;
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
   
   printf("=============== done. saved as %s. count(hit==1) : %d\n", saveFileName.Data(), count);
   gROOT->ProcessLine(".q");
}
