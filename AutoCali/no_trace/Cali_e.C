#define Cali_e_cxx
// The class definition in Cali_e.h has been generated automatically
// by the ROOT utility TTree::MakeSelector(). This class is derived
// from the ROOT class TSelector. For more information on the TSelector
// framework see $ROOTSYS/README/README.SELECTOR or the ROOT User Manual.


// The following methods are defined in this file:
//    Begin():        called every time a loop on the tree starts,
//                    a convenient place to create your histograms.
//    SlaveBegin():   called after Begin(), when on PROOF called only on the
//                    slave servers.
//    Process():      called for each event, in this function you decide what
//                    to read and fill your histograms.
//    SlaveTerminate: called at the end of the loop on the tree, when on PROOF
//                    called only on the slave servers.
//    Terminate():    called at the end of the loop on the tree,
//                    a convenient place to draw/fit your histograms.
//
// To use this file, try the following session on your Tree T:
//
// root> T->Process("Cali_e.C")
// root> T->Process("Cali_e.C","some options")
// root> T->Process("Cali_e.C+")
//


#include "Cali_e.h"
#include <TH2.h>
#include <TStyle.h>

void Cali_e::Begin(TTree * /*tree*/)
{
   // The Begin() function is called at the start of the query.
   // When running with PROOF Begin() is only called on the client.
   // The tree argument is deprecated (on PROOF 0 is passed).

   TString option = GetOption();
   
}

void Cali_e::SlaveBegin(TTree * /*tree*/)
{
   // The SlaveBegin() function is called after the Begin() function.
   // When running with PROOF SlaveBegin() is called on each slave server.
   // The tree argument is deprecated (on PROOF 0 is passed).

   TString option = GetOption();

}

Bool_t Cali_e::Process(Long64_t entry)
{
   // The Process() function is called for each entry in the tree (or possibly
   // keyed object in the case of PROOF) to be processed. The entry argument
   // specifies which entry in the currently loaded tree is to be processed.
   // When processing keyed objects with PROOF, the object is already loaded
   // and is available via the fObject pointer.
   //
   // This function should contain the \"body\" of the analysis. It can contain
   // simple or elaborate selection criteria, run algorithms on the data
   // of the event and typically fill histograms.
   //
   // The processing can be stopped by calling Abort().
   //
   // Use fStatus to set the return value of TTree::Process().
   //
   // The return value is currently not used.
   
   //#################################################################### initialization
   for(int i = 0; i < numDet; i++){
      eC[i]    = TMath::QuietNaN();
      x[i]     = TMath::QuietNaN();
      z[i]     = TMath::QuietNaN();
      eC_t[i]  = 0;
   }
   
   det = -4;
   hitID = -4;
   zMultiHit = 0;
   
   Ex  = TMath::QuietNaN();
   thetaCM  = TMath::QuietNaN();
   thetaLab  = TMath::QuietNaN();
   
   for(int i = 0; i < 8 ; i++){
      rdtC[i] = TMath::QuietNaN();
      rdtC_t[i] = 0;
   }
   coin_t = 0;
   
   //ddt = TMath::QuietNaN(); // H060
   //ddt_t = TMath::QuietNaN(); // H060
   //tacS = TMath::QuietNaN(); // H060

   //#################################################################### processing
   eventID += 1;
   if( entry == 1 ) run += 1;
   
   b_Energy->GetEntry(entry,0);
   b_XF->GetEntry(entry,0);
   b_XN->GetEntry(entry,0);
   b_RDT->GetEntry(entry,0);
   b_EnergyTimestamp->GetEntry(entry,0);
   b_RDTTimestamp->GetEntry(entry,0);
   
   //b_ELUM->GetEntry(entry, 0); // for H060_208Pb
   //b_ELUMTimestamp->GetEntry(entry, 0); // for H060_208Pb
   b_TAC->GetEntry(entry,0);
   
   //=========== gate
   bool rdt_energy = false;
   for( int rID = 0; rID < 8; rID ++){
      if( rdt[rID] > 5000 ) rdt_energy = true; 
   }
   if( !rdt_energy ) return kTRUE;
   
   
   for(int i = 0 ; i < 8 ; i++){
      rdtC[i]   = rdt[i];
      rdtC_t[i] = rdt_t[i]; 
   }

   ULong64_t eTime = 0; //this will be the time for Ex valid
   for(int idet = 0 ; idet < numDet; idet++){
      
      if( e[idet] > 0 ){
         eC[idet]   = e[idet]/eCorr[idet][0] + eCorr[idet][1];  
         eC_t[idet] = e_t[idet]; // ch
      }
      
      double xfC = 0, xnC = 0;
      if( xf[idet] > 0) xfC = xf[idet] * xfxneCorr[idet][1] + xfxneCorr[idet][0] ;
      if( xn[idet] > 0) xnC = xn[idet] * xnCorr[idet] * xfxneCorr[idet][1] + xfxneCorr[idet][0];
   
      //========= calculate x
      if(xf[idet] > 0  && xn[idet] > 0 ) {
         x[idet] = (xfC-xnC)/(xfC+xnC);
         hitID = 0;
      }else if(xf[idet] == 0 && xn[idet] > 0 ){
         x[idet] = (1-2*xnC/e[idet]);
         hitID = 1;
      }else if(xf[idet] > 0 && xn[idet] == 0 ){
         x[idet] = (2*xfC/e[idet]-1);
         hitID = 2;
      }else{
         x[idet] = TMath::QuietNaN();
      }
      
      //if( idet >= 17 && e[idet] > 0) printf("%d, %d , %f, %f \n", eventID, idet, eC[idet], e[idet]);
      
      //========= calculate z, det
      if( TMath::IsNaN(x[idet]) ) {
         z[idet] = TMath::QuietNaN();
      }else{ 
         int detID = idet%iDet;
         if( pos[detID] < 0 ){
            z[idet] = pos[detID] - (-x[idet] + 1.)*length/2 ; 
         }else{
            z[idet] = pos[detID] + (x[idet] + 1.)*length/2 ; 
         }
         zMultiHit ++;
         count ++;
         det = idet;
         //printf(" det: %d, detID: %d, x: %f, pos:%f, z: %f \n", det, detID, x[idet], pos[detID], z[idet]);
      
      
         //========== Calculate Ex and thetaCM
         if( TMath::IsNaN(eC[idet]) || isReaction == false ){
            Ex = TMath::QuietNaN();
            thetaCM = TMath::QuietNaN();
            thetaLab = TMath::QuietNaN();
            
         }else{
         
            eTime = eC_t[idet];
         
            double y = eC[idet] + mass;
            Z = alpha * gamma * beta * z[idet];
            H = TMath::Sqrt(TMath::Power(gamma * beta,2) * (y*y - mass * mass) ) ;
            
            if( TMath::Abs(Z) < H ) {            
              //using Newton's method to solve 0 ==  H * sin(phi) - G * tan(phi) - Z = f(phi) 
              double tolerrence = 0.001;
              double phi = 0; //initial phi = 0 -> ensure the solution has f'(phi) > 0
              double nPhi = 0; // new phi

              int iter = 0;
              do{
                phi = nPhi;
                nPhi = phi - (H * TMath::Sin(phi) - G * TMath::Tan(phi) - Z) / (H * TMath::Cos(phi) - G /TMath::Power( TMath::Cos(phi), 2));               
                iter ++;
                if( iter > 10 || TMath::Abs(nPhi) > TMath::PiOver2()) break;
              }while( TMath::Abs(phi - nPhi ) > tolerrence);
              phi = nPhi;

              // check f'(phi) > 0
              double Df = H * TMath::Cos(phi) - G / TMath::Power( TMath::Cos(phi),2);
              if( Df > 0 && TMath::Abs(phi) < TMath::PiOver2()  ){
                double K = H * TMath::Sin(phi);
                double x = TMath::ACos( mass / ( y * gamma - K));
                double k = mass * TMath::Tan(x); // momentum of particel b or B in CM frame
                double EB = TMath::Sqrt(mass*mass + Et*Et - 2*Et*TMath::Sqrt(k*k + mass * mass));
                Ex = EB - massB;
            
                double hahaha1 = gamma* TMath::Sqrt(mass * mass + k * k) - y;
                double hahaha2 = gamma* beta * k;
                thetaCM = TMath::ACos(hahaha1/hahaha2) * TMath::RadToDeg();
                
                double pt = k * TMath::Sin(thetaCM * TMath::DegToRad());
                double pp = gamma*beta*TMath::Sqrt(mass*mass + k*k) - gamma * k * TMath::Cos(thetaCM * TMath::DegToRad());
                
                thetaLab = TMath::ATan(pt/pp) * TMath::RadToDeg();
                
              }else{
                Ex = TMath::QuietNaN();
                thetaCM = TMath::QuietNaN();
                thetaLab = TMath::QuietNaN();
              }

            }else{
              Ex = TMath::QuietNaN();
              thetaCM = TMath::QuietNaN();  
              thetaLab = TMath::QuietNaN();
            } // end of if |Z| > H
         } //end of e is valid
      }//end of x is valid
   }//end of idet-loop
   
   //for H060
   //if( elum[0] != 0){
   //   ddt   = elum[0];
   //   ddt_t = elum_t[0]/1e8; //sec
   //   tacS  = tac[0];
   //}
   
   //for coincident time bewteen array and rdt
   if( zMultiHit == 1 ) {
      ULong64_t rdtTime = 0;
      Float_t rdtQ = 0;
      for(int i = 0; i < 8 ; i++){
         if( rdt[i] > rdtQ ) {
            rdtQ = rdt[i];
            rdtTime = rdt_t[i];
         }
      }
      
      coin_t = eTime - rdtTime;
      
   }
   
   if( zMultiHit == 0 ) return kTRUE;
   
   //#################################################################### Timer  
   saveFile->cd(); //set focus on this file
   newTree->Fill();  

   clock.Stop("timer");
   Double_t time = clock.GetRealTime("timer");
   clock.Start("timer");

   if ( !shown ) {
      if (fmod(time, 10) < 1 ){
         printf( "%10d[%2d%%]|%3.0f min %5.2f sec | expect:%5.2f min\n", 
               eventID, 
               TMath::Nint((eventID+1)*100./totnumEntry),
               TMath::Floor(time/60.), time - TMath::Floor(time/60.)*60.,
               totnumEntry*time/(eventID+1.)/60.);
               shown = 1;
      }
   }else{
      if (fmod(time, 10) > 9 ){
         shown = 0;
      }
   }

   return kTRUE;
}

void Cali_e::SlaveTerminate()
{
   // The SlaveTerminate() function is called after all entries or objects
   // have been processed. When running with PROOF SlaveTerminate() is called
   // on each slave server.

}

void Cali_e::Terminate()
{
   // The Terminate() function is the last function to be called during
   // a query. It always runs on the client, it can be used to present
   // the results graphically or save the results to file.
   
   saveFile->cd(); //set focus on this file
   newTree->Write(); 
   saveFile->Close();

   printf("-------------- done. %s, z-valid count: %d\n", saveFileName.Data(), count);

}
