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

   double mA = 23268.0269 ; // 25Mg
   double ma =  1875.6129 ; // d
   double mb =   938.272  ; // p
   double mB = 24196.4992 ; // 26Mg
   
   int Zb = 1;
   int ZB = 12;
   
   double T =  6 * 25;
   double Bfield = 2; // Tesla

   TFile * saveFile = new TFile("test.root", "recreate");
   TTree * tree = new TTree("tree", "tree");

   double z;
   double e;
   double t;
   double thetaCM;
   double phiCM;
   double Ex;

   tree->Branch("e", &e, "e/D");
   tree->Branch("z", &z, "z/D");
   tree->Branch("t", &e, "t/D");
   tree->Branch("thetaCM", &thetaCM, "thetaCM/D");
   tree->Branch("phiCM", &phiCM, "phiCM/D");
   tree->Branch("Ex", &Ex, "Ex/D");

   const double c = 299.792458;
   const int numEvent = 30000;
   
   //TCanvas * cHelios = new TCanvas("cHelios", "cHelios", 0, 0, 1000, 500);
   //cHelios->Divide(2,1);
   //for( int i = 1; i <= 2 ; i++){
   //   cHelios->cd(i)->SetGrid();
   //}
   //cHelios->cd(1);
   
   //TH2F * h = new TH2F("h", "h", 200, -1.5, 1, 200, 0, 30);
   //h->SetXTitle("z [m]");
   //h->SetYTitle("KE [MeV]");

   //TH2F * g = new TH2F("g", "g", 200, -1.5, 1, 200, 0, 30);
   //g->SetXTitle("z [m]");
   //g->SetYTitle("KE [MeV]");

   for( int i = 0; i < numEvent; i++){
      thetaCM = TMath::Pi() * gRandom->Rndm();
      phiCM = 2*TMath::Pi() * gRandom->Rndm();
      Ex = gRandom->Integer(3) * 10.;
      
      //printf("%f, %f, %f \n", thetaCM, phiCM, Ex);

      TLorentzVector * P = Reaction(mA, ma, mb, mB + Ex, T, thetaCM , phiCM);
      
      double   rho0 = P[0].Pt() / Bfield / Zb / c ; // meter
      double    vt0 = P[0].Beta() * TMath::Sin(P[0].Theta()) * c / 1e+3; // meter/ nano-second  
      double    vp0 = P[0].Beta() * TMath::Cos(P[0].Theta()) * c / 1e+3; // meter/ nano-second  
      t = 2 * TMath::Pi() * rho0 / vt0; // nano-second   
      z = vp0 * t + gRandom->Gaus(0, 0.02); // meter
      e = P[0].E() - mb + gRandom->Gaus(0, 0.4);
      
      //h->Fill( z , e);
      
      
      //finite detector
      double a = 0.0115; // distance from axis
      double w = 0.0090; // width
      double l = 0.0505; // length
      double pos[6] = {0. ,59.0, 118.0, 176.9, 236.2, 294.8, 50.5}; // near position
      
      //calculate the hit point
      double alpha = a / 2 / rho0;
      
      // for side-L;
      double temp1 = 1. + TMath::Cos(2*phiCM) - 4 * alpha * TMath::Sin(phiCM);
      double temp2 = 2 * TMath::Sqrt( TMath::Power(TMath::Cos(phiCM),4) - 4 * alpha * TMath::Power(TMath::Cos(phiCM),2) * (alpha + TMath::Sin(phiCM)));
      double theta_hit1 = TMath::ACos( 0.5 * TMath::Sqrt(  temp1 + temp2 )  );
      double theta_hit2 = TMath::ACos( 0.5 * TMath::Sqrt(  temp1 - temp2 )  );
      
      
      
      //double  a = 0.0115* ( 1 - (TMath::Sqrt(2) - 1)* gRandom->Rndm()) ;
      //z = rho0 * vp0 / vt0 * (2 * TMath::Pi() - TMath::ACos(1- TMath::Power(a/rho0,2)/2 ) );
      //z = z + gRandom->Gaus(0, 0.02);

      //g->Fill( z , e);

      //saveFile->cd();
      //      tree->Write();
      tree->Fill();
   }
   
   //cHelios->cd(1);
   //h->Draw("colz");
   //cHelios->cd(2);
   //g->Draw("colz");

   saveFile->Write();
   saveFile->Close();
   
}
