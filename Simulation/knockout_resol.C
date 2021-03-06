#include "HELIOS_LIB.h"
#include "TBenchmark.h"
#include "TLorentzVector.h"
#include "TVector3.h"
#include "TMath.h"
#include "TFile.h"
#include "TF1.h"
#include "TTree.h"
#include "TRandom.h"
#include "TClonesArray.h"
#include <vector>
#include <fstream>

//----------- usage 
// $root sim.C+ | tee output.txt
// this will same the massage to output.txt

void knockout_resol(){

   //================================================= User Setting
   //---- reaction
   int AA = 23, ZA = 9;
   int Aa = 1,  Za = 1;
   int A2 = 1,  Z2 = 1;
   
   bool isNormalKinematics = false;
   bool isOverRideExNegative = true;
   double maxkb = 200.;
   
   //---- beam
   double KEAmean = 300; // MeV/u 
   double KEAsigma = 0; //KEAmean*0.001; // MeV/u , assume Guassian
   double thetaMean = 0.; // mrad 
   double thetaSigma = 0.; // mrad , assume Guassian due to small angle
   
   int numEvent = 1000000;
   
   /*
   //---- HELIOS detector geometry
   string heliosDetGeoFile = "";//"detectorGeo_upstream.txt";
   double BField = 4.0; // if not detector, must set B-field, else, this value is not used.
   double eSigma = 0.0001 ; // detector energy sigma MeV
   double zSigma = 0.1 ; // detector position sigma mm
   */
   
   //---- excitation of Beam 
   int nExA = 1;
   double ExAList[nExA];
   ExAList[0] = 0.000; // MeV
   //ExAList[1] = 1.567;
    
   //---- Separation energy
   string separationFile = "separation_energies.txt";
   
   //---- save root file name
   TString saveFileName = "knockout_resol.root";
   
   //---- Auxiliary setting
   bool isTargetScattering = false;
   bool isDecay = false;
   bool isReDo = false; // redo calculation until detected. 
   
   //---- target
   double density = 0.913; // 0.913 g/cm3
   double targetThickness = 2./2. * 2.2e-4; // 2.2 um = 201 ug/cm2
   string stoppingPowerForA = "208Pb_in_CD2.txt"; // generated by SRIM
   string stoppingPowerForb = "1H_in_CD2.txt";
   string stoppingPowerForB = "209Pb_in_CD2.txt";
   
   //=============================================================
   //=============================================================
   //=============================================================
   //===== Set Reaction
   Knockout reaction;
   int AB = AA-A2, ZB = ZA-Z2;
   int A1 = Aa   , Z1 = Za;
   reaction.SetA(AA,ZA);
   reaction.Seta(Aa,Za);
   reaction.Set2(A2,Z2);
   reaction.SetIncidentEnergyAngle(KEAmean, 0, 0);
   reaction.OverRideExNegative(isOverRideExNegative);
   
   printf("===================================================\n");
   printf("=========== %s ===========\n", reaction.GetReactionName().Data());
   printf("=========== KE: %9.4f +- %5.4f MeV/u, dp/p = %5.2f %% \n", KEAmean, KEAsigma, KEAsigma/KEAmean * 50.);
   printf("======== theta: %9.4f +- %5.4f MeV/u \n", thetaMean, thetaSigma);
   printf("===================================================\n");

   /*
   //======== Set HELIOS
   printf("############################################## HELIOS configuration\n");   
   HELIOS helios1; // for particle-1
   HELIOS helios2; // for particle-2
   bool sethelios1 = helios1.SetDetectorGeometry(heliosDetGeoFile);
   bool sethelios2 = helios2.SetDetectorGeometry(heliosDetGeoFile);
   if( sethelios1 && sethelios2 ) {
		int mDet = helios1.GetNumberOfDetectorsInSamePos();
		printf("========== energy resol.: %f MeV\n", eSigma);
		printf("=========== pos-Z resol.: %f mm \n", zSigma); 
	}else{
		helios1.SetMagneticField(BField);
		helios2.SetMagneticField(BField);
		printf("======== B-field : %5.2f T", BField);
	}
	
	//==== Target scattering, only energy loss
   if(isTargetScattering) printf("############################################## Target Scattering\n");
   TargetScattering msA;
   TargetScattering msB;
   TargetScattering msb;

   if(isTargetScattering) printf("======== Target : (thickness : %6.2f um) x (density : %6.2f g/cm3) = %6.2f ug/cm2\n", 
                        targetThickness * 1e+4, 
                        density, 
                        targetThickness * density * 1e+6);  
            
   if( isTargetScattering ){
      msA.LoadStoppingPower(stoppingPowerForA);
      msb.LoadStoppingPower(stoppingPowerForb);
      msB.LoadStoppingPower(stoppingPowerForB);
   }
   */
   //======= Decay of particle-B
   Decay decay;
   decay.SetMotherDaugther(AB, ZB, AB-1,ZB); //neutron decay
	
	
   //======= loading excitation energy
   printf("############################################## excitation energies\n");
   vector<double> SpList;
   printf("----- loading separation energies.");
   ifstream file;
   file.open(separationFile.c_str());
   string isotopeName;
   if( file.is_open() ){
      string line;
      int i = 0;
      while( file >> line){
         //printf("%d, %s \n", i,  line.c_str());
         if( line.substr(0,2) == "//" ) continue;
         if( i == 0 ) isotopeName = line; 
         if ( i >= 1 ){
            SpList.push_back(atof(line.c_str()));
         }
         i = i + 1;
      }
      file.close();
      printf("... done.\n");
      printf("========== %s\n", isotopeName.c_str());
      int n = SpList.size();
      for(int i = 0; i < n ; i++){
         if( isDecay ) {
            TLorentzVector temp(0,0,0,0);
            int decayID = decay.CalDecay(temp, SpList[i], 0);
            if( decayID == 1) {
               printf("%d, Sp: %6.2f MeV --> Decay. \n", i, SpList[i]);
            }else{
               printf("%d, Sp: %6.2f MeV\n", i, SpList[i]);
            }
         }else{
            printf("%d, Sp: %6.2f MeV \n", i, SpList[i]);
         }
      }
   }else{
       printf("... fail\n");
       return;
   }

   //====================== build tree
   TFile * saveFile = new TFile(saveFileName, "recreate");
   TTree * tree = new TTree("tree", "tree");
   
   double thetaNN, phiNN;
   double theta1, phi1, T1;
   double theta2, phi2, T2;
   double thetaB, TB;
   double Sp, kb, thetab, phib;
   int SpID;

   double ExA;
   int ExAID;
   double KEA, KEAscattered, theta, phi;
   
   double mB,mb;

   tree->Branch("theta1", &theta1, "theta1/D");
   tree->Branch("phi1", &phi1, "phi1/D");
   tree->Branch("T1", &T1, "T1/D");
   tree->Branch("theta2", &theta2, "theta2/D");
   tree->Branch("phi2", &phi2, "phi2/D");
   tree->Branch("T2", &T2, "T2/D");
   tree->Branch("thetaB", &thetaB, "thetaB/D");
   tree->Branch("TB", &TB, "TB/D");
   tree->Branch("thetaNN", &thetaNN, "thetaNN/D");
   tree->Branch("phiNN", &phiNN, "phiNN/D");
   tree->Branch("Sp", &Sp, "Sp/D");
   tree->Branch("kb", &kb, "kb/D");
   tree->Branch("thetab", &thetab, "thetab/D");
   tree->Branch("phib", &phib, "phib/D");
   
   tree->Branch("SpID", &SpID, "SpID/I");
   
   tree->Branch("ExAID", &ExAID, "ExAID/I");
   tree->Branch("KEA", &KEA, "KEA/D");
   if(isTargetScattering) tree->Branch("KEAscattered", &KEAscattered, "KEAscattered/D");
   tree->Branch("theta", &theta, "theta/D");
   tree->Branch("phi", &phi, "phi/D");

   tree->Branch("mB", &mB, "mB/D");
   tree->Branch("mb", &mb, "mb/D");
   
   TClonesArray * arr = new TClonesArray("TLorentzVector");
   tree->Branch("fV", arr, 256000);
   arr->BypassStreamer();
   
   TClonesArray * arrN = new TClonesArray("TLorentzVector");
   tree->Branch("fVN", arrN, 256000);
   arrN->BypassStreamer();
   
   TLorentzVector* fourVector = NULL;

   /*
	// tree branch from helios1, helios2
   double e1, z1, t1, rho1;
   double e2, z2, t2, rho2;

   tree->Branch("e1", &e1, "e1/D");
   tree->Branch("z1", &z1, "z1/D");
   tree->Branch("t1", &t1, "t1/D");
   tree->Branch("rho1", &rho1, "rho1/D");
   
   tree->Branch("e2", &e2, "e2/D");
   tree->Branch("z2", &z2, "z2/D");
   tree->Branch("t2", &t2, "t2/D");
   tree->Branch("rho2", &rho2, "rho2/D");
   */
   
   //resolution
   double eResol, angResol, SpResol, SpM;
   double e1new, ang1Change;
   double e2new, ang2Change;
   double Spnew;
   
   tree->Branch(  "eResol",   &eResol,   "eResol/D");
   tree->Branch("angResol", &angResol, "angResol/D");
   tree->Branch( "SpResol",  &SpResol,  "SpResol/D");
   tree->Branch(     "SpM",      &SpM,      "SpM/D");
   
   tree->Branch(     "e1new",   &e1new,   "e1new/D");
   tree->Branch("ang1Change",   &ang1Change,   "ang1Change/D");
   
   tree->Branch(     "e2new",   &e2new,   "e2new/D");
   tree->Branch("ang2Change",   &ang2Change,   "ang2Change/D");
   
   tree->Branch( "Spnew", &Spnew, "Spnew/D");

   TClonesArray * arrR = new TClonesArray("TLorentzVector");
   tree->Branch("fVR", arrR, 256000);
   arrR->BypassStreamer();
   
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
         
         //==== Set Sp of B
         SpID = gRandom->Integer(SpList.size());
         Sp = SpList[SpID]; 
         
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
         
         KEA = 50.*gRandom->Integer(10);
         
         reaction.SetIncidentEnergyAngle(KEA, theta, 0.);

         /*
         //For target scattering
         reaction.CalIncidentChannel(isNormalKinematics); // but only need is PA
         TLorentzVector PA = reaction.GetPA();            
         
         double depth = 0;
         if( isTargetScattering ){
            //==== Target scattering, only energy loss
            depth = targetThickness * gRandom->Rndm();
            msA.SetTarget(density, depth); 
            TLorentzVector PAnew = msA.Scattering(PA);
            KEAscattered = msA.GetKE()/AA;
            reaction.SetIncidentEnergyAngle(KEAscattered, theta, phi);
         }*/
         
         //==== Calculate reaction
         thetaNN = TMath::ACos(2 * gRandom->Rndm() - 1) ; 
         phiNN = TMath::TwoPi() * gRandom->Rndm(); 
         
         kb = maxkb * gRandom->Rndm();
         thetab = TMath::ACos(2 * gRandom->Rndm() - 1);
         phib = TMath::TwoPi() * gRandom->Rndm();

         reaction.SetBSpk(Sp, kb, thetab, phib);
         reaction.CalReactionConstant(isNormalKinematics);
         reaction.Event(thetaNN, phiNN);

         TLorentzVector PA = reaction.GetPA();
         TLorentzVector Pa = reaction.GetPa();
               
         TLorentzVector P1 = reaction.GetP1();
         TLorentzVector P2 = reaction.GetP2();
         TLorentzVector Pb = reaction.GetPb();
         TLorentzVector PB = reaction.GetPB();
         
         /*
         //==== Calculate energy loss of scattered and recoil in target
         if( isTargetScattering ){
            if( Pb.Theta() < TMath::PiOver2() ){
               msb.SetTarget(density, targetThickness - depth);
            }else{
               msb.SetTarget(density, depth);
            }
            Pb = msb.Scattering(Pb);
            TbLoss = msb.GetKELoss();
            msB.SetTarget(density, targetThickness - depth);
            PB = msB.Scattering(PB);
         }else{
            TbLoss = 0;
         }
         
         //======= Decay of particle-B
         if( isDecay){
            int decayID = decay.CalDecay(PB, Sp, 0); // decay to ground state
            if( decayID == 1 ){
               PB = decay.GetDaugther_D();
               decayTheta = decay.GetAngleChange();
            }else{
               decayTheta = TMath::QuietNaN();
            }
         }
         */
                  
         //################################### tree branches
         //===== reaction
         theta1 = P1.Theta() * TMath::RadToDeg();
         theta2 = P2.Theta() * TMath::RadToDeg();
         thetaB = PB.Theta() * TMath::RadToDeg();
      
         T1 = P1.E() - P1.M();
         T2 = P2.E() - P2.M();
         TB = PB.E() - PB.M();
         
         phi1 = P1.Phi() * TMath::RadToDeg();
         phi2 = P2.Phi() * TMath::RadToDeg();
         
         mB = PB.M();
         mb = Pb.M();
         
         TVector3 bA = PA.BoostVector();
            
         for(int i = 0; i < 6 ; i++){
            TLorentzVector temp;
            double xyzt[4];
            switch(i){
               case 0: temp = PA; break;
               case 1: temp = Pa; break;
               case 2: temp = P1; break;
               case 3: temp = P2; break;
               case 4: temp = PB; break;
               case 5: temp = Pb; break;
            }
            
            temp.GetXYZT(xyzt);
            
            fourVector = (TLorentzVector*) arr->ConstructedAt(i);
            fourVector->SetXYZT(xyzt[0], xyzt[1], xyzt[2], xyzt[3]);
            
            //into normal kinematic
            temp.Boost(-bA);  
            temp.GetXYZT(xyzt);
            
            fourVector = (TLorentzVector*) arrN->ConstructedAt(i);
            fourVector->SetXYZT(xyzt[0], xyzt[1], xyzt[2], xyzt[3]);
            
         }
         
         /*
         //==== Helios
         int hit1 = helios1.CalHit(P1, Z1, PB, ZB);
         int hit2 = helios2.CalHit(P2, Z2, PB, ZB);
         
         e1 = helios1.GetEnergy() + gRandom->Gaus(0, eSigma);
         z1 = helios1.GetZ() ; 
         t1 = helios1.GetTime();
         rho1 = helios1.GetRho();
         
         e2 = helios2.GetEnergy() + gRandom->Gaus(0, eSigma);
         z2 = helios2.GetZ() ; 
         rho2 = helios2.GetRho();
         */
         
         
         //===== resolution
         eResol   = 1000. * gRandom->Rndm(); // keV;
         angResol = 50 * gRandom->Rndm(); // mrad; 
         
         double energy1 = P1.E() + gRandom->Gaus(0, eResol/1000.);
         TVector3 k1 = P1.Vect();
         TVector3 n1 = k1.Orthogonal();
         
         k1.Rotate( gRandom->Gaus(0, angResol/1000.) , n1);
         k1.Rotate( gRandom->Rndm() * TMath::TwoPi() , P1.Vect());
         
         TLorentzVector P1new;
         P1new.SetVect(k1);
         P1new.SetE(energy1);
         
         e1new = P1new.E() - P1.M();
         ang1Change = k1.Angle(P1.Vect());
         
         double energy2 = P2.E() + gRandom->Gaus(0, eResol/1000.);
         TVector3 k2 = P2.Vect();
         TVector3 n2 = k2.Orthogonal();
         
         k2.Rotate( gRandom->Gaus(0, angResol/1000.) , n2);
         k2.Rotate( gRandom->Rndm() * TMath::TwoPi() , P2.Vect());
         
         TLorentzVector P2new;
         P2new.SetVect(k2);
         P2new.SetE(energy2);
         
         e2new = P2new.E() - P2.M();
         ang2Change = k2.Angle(P2.Vect());
         
         //SpResol
         TLorentzVector PBnew = PA + Pa - P1new - P2new;
         Spnew = PBnew.M() - reaction.GetMass_A() + reaction.GetMass_2();
         
         SpResol = Spnew - Sp; 
         
         TLorentzVector Pbnew = PA - PBnew;
         TVector3 boostA = PA.BoostVector();
         Pbnew.Boost(-boostA);
         
         double xyzt[4];
         
         P1new.GetXYZT(xyzt);         
         fourVector = (TLorentzVector*) arrR->ConstructedAt(0);
         fourVector->SetXYZT(xyzt[0], xyzt[1], xyzt[2], xyzt[3]);

         P2new.GetXYZT(xyzt);         
         fourVector = (TLorentzVector*) arrR->ConstructedAt(1);
         fourVector->SetXYZT(xyzt[0], xyzt[1], xyzt[2], xyzt[3]);
      
         PBnew.GetXYZT(xyzt);         
         fourVector = (TLorentzVector*) arrR->ConstructedAt(2);
         fourVector->SetXYZT(xyzt[0], xyzt[1], xyzt[2], xyzt[3]);
         
         Pbnew.GetXYZT(xyzt);         
         fourVector = (TLorentzVector*) arrR->ConstructedAt(3);
         fourVector->SetXYZT(xyzt[0], xyzt[1], xyzt[2], xyzt[3]);
         
         
         //change thetaNN into deg
         thetaNN = thetaNN * TMath::RadToDeg();
         
         int hit1 = 1; // for no helios;
         
         if( hit1 == 1) {
            count ++;
         }
         
         if( isReDo ){
            if( hit1 == 1) {
               redoFlag = false;
            }else{
               redoFlag = true;
               //printf("%d, %2d, thetaNN : %f, theta : %f, z0: %f \n", i, hit, thetaNN * TMath::RadToDeg(), thetab, helios.GetZ0());
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
   
   /**/

}
