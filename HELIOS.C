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
   
   double T = 30 * 25;
   double Bfield = 2; // Tesla
   
   const double c = 299.792458;
   const int numEvent = 30000;
   
   TCanvas * cHelios = new TCanvas("cHelios", "cHelios", 0, 0, 1000, 500);
   cHelios->Divide(2,1);
   for( int i = 1; i <= 2 ; i++){
      cHelios->cd(i)->SetGrid();
   }
   cHelios->cd(1);
   
   TH2F * h = new TH2F("h", "h", 200, -1.5, 1, 200, 0, 30);
   h->SetXTitle("z [m]");
   h->SetYTitle("KE [MeV]");

   TH2F * g = new TH2F("g", "g", 200, -1.5, 1, 200, 0, 30);
   g->SetXTitle("z [m]");
   g->SetYTitle("KE [MeV]");

   for( int i = 0; i < numEvent; i++){
      double thetaCM =   1 * gRandom->Rndm();
      double   phiCM = 360. * gRandom->Rndm();
      double Ex = gRandom->Integer(3) * 10;
      TLorentzVector * P = Reaction(mA, ma, mb, mB + Ex, T, thetaCM , phiCM);
      
      double   rho0 = P[0].Pt() / Bfield / Zb / c ; // meter
      double    vt0 = P[0].Beta() * TMath::Sin(P[0].Theta()) * c / 1e+3; // meter/ nano-second  
      double    vp0 = P[0].Beta() * TMath::Cos(P[0].Theta()) * c / 1e+3; // meter/ nano-second  
      double Tcyle0 = 2 * TMath::Pi() * rho0 / vt0; // nano-second   
      double  Zcyc0 = vp0 * Tcyle0; // meter
      double    KE0 = P[0].E() - mb + gRandom->Gaus(0, 0.4);
      
      h->Fill( Zcyc0 + gRandom->Gaus(0, 0.02) , KE0);
      
      double  a = 0.0115* ( 1 - (TMath::Sqrt(2) - 1)* gRandom->Rndm()) ;
      Zcyc0 = rho0 * vp0 / vt0 * (2 * TMath::Pi() - TMath::ACos(1- TMath::Power(a/rho0,2)/2 ) );
      
      g->Fill(Zcyc0 + gRandom->Gaus(0, 0.02), KE0);
   }
   
   h->Draw("colz");
   cHelios->cd(2);
   g->Draw("colz");
   
}
