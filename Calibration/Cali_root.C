#define Cali_root_cxx
// The class definition in Cali_root.h has been generated automatically
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
// Root > T->Process("Cali_root.C")
// Root > T->Process("Cali_root.C","some options")
// Root > T->Process("Cali_root.C+")
//

#include "Cali_root.h"
#include <TH2.h>
#include <TStyle.h>


void Cali_root::Begin(TTree * /*tree*/)
{
   // The Begin() function is called at the start of the query.
   // When running with PROOF Begin() is only called on the client.
   // The tree argument is deprecated (on PROOF 0 is passed).

   TString option = GetOption();

}

void Cali_root::SlaveBegin(TTree * /*tree*/)
{
   // The SlaveBegin() function is called after the Begin() function.
   // When running with PROOF SlaveBegin() is called on each slave server.
   // The tree argument is deprecated (on PROOF 0 is passed).

   TString option = GetOption();

}

Bool_t Cali_root::Process(Long64_t entry)
{
   // The Process() function is called for each entry in the tree (or possibly
   // keyed object in the case of PROOF) to be processed. The entry argument
   // specifies which entry in the currently loaded tree is to be processed.
   // It can be passed to either Cali_root::GetEntry() or TBranch::GetEntry()
   // to read either all or the required parts of the data. When processing
   // keyed objects with PROOF, the object is already loaded and is available
   // via the fObject pointer.
   //
   // This function should contain the "body" of the analysis. It can contain
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
      xfC[i]   = TMath::QuietNaN();
      xnC[i]   = TMath::QuietNaN();
      x[i]     = TMath::QuietNaN();
      z[i]     = TMath::QuietNaN();
      eC_t[i]  = TMath::QuietNaN();
   }
   
   det = -4;
   hitID = -4;
   zMulti = 0;
   
   ddt = TMath::QuietNaN(); // H060
   
   if( option >= 2){
      Ex  = TMath::QuietNaN();
      thetaCM  = TMath::QuietNaN();
      energy    = TMath::QuietNaN();
      energy_t  = -100000;
      
      for(int i = 0; i < 8 ; i++){
         rdtC[i] = TMath::QuietNaN();
         rdtC_t[i] = TMath::QuietNaN();
      }
      rdt_m = 0;
   }
      
   if( option >= 3){
      tt  = TMath::QuietNaN();
      ttt = TMath::QuietNaN();
      t4  = TMath::QuietNaN();
      
      good = 0;
      
      tac_m = 0;
      for(int i = 0; i < 6 ; i++){
         tacC[i] = TMath::QuietNaN();
         tacC_t[i] = TMath::QuietNaN();
         dt[i] = -100000;
      }
   }
   //#################################################################### processing
   //eventID = entry;
   eventID += 1;
   //if(eventID > 100) return kTRUE;
   
   if( entry == 1 ) run += 1;
   
   b_Energy->GetEntry(entry,0);
   b_XF->GetEntry(entry,0);
   b_XN->GetEntry(entry,0);
   
   b_ELUM->GetEntry(entry, 0); // for H060_208Pb
   
   if( option >= 2){
      b_EnergyTimestamp->GetEntry(entry,0);
      b_RDT->GetEntry(entry,0);
      b_RDTTimestamp->GetEntry(entry,0);
   }

   if( option >= 3){
      b_TAC->GetEntry(entry,0);
      b_TACTimestamp->GetEntry(entry,0);
   }
   
   
   //gate on rdt, a kind of recoil detector, total has 8 of them
   //if( rdt[0] < 5000 && rdt[1] < 5000 && rdt[2] < 5000 && rdt[3] < 5000 && rdt[4] < 5000 && rdt[5] < 5000 && rdt[6] < 5000 && rdt[7] < 5000  ) return kTRUE; 
   
   if( option >= 2){
      for(int i = 0; i < 8; i++){
         if( rdt[i] > 0 ) {
            rdtC[i] = rdt[i];
            rdtC_t[i] = rdt_t[i];
            rdt_m = rdt_m +1;
         }
      }
   }
   if( option >= 3){
      for(int i = 0; i < 6; i++){
         tacC[i] = tac[i];
         tacC_t[i] = tac_t[i];
         tac_m = tac_m +1;
      }
   }
   
   bool rdt_energy = false;
   bool coincident_t = false;
   
   if( option < 3 ){
      rdt_energy = true;
      coincident_t = true;
   }else{
      for( int rID = 0; rID < 8; rID ++){
         if( rdt[rID] > 5000 ) rdt_energy = true;
         //for( int i = 0; i < numDet; i++){
         //   if( e[i] > 0 && -10 < e_t[i] - rdt_t[rID] &&  e_t[i] - rdt_t[rID] < 10) coincident_t = true;
         //}  
         coincident_t = true;
      }
   }
   
   int zMulti = 0;
   if(rdt_energy && coincident_t ){
      for(int i = 0; i < numDet; i++){
         //if( -10 < e_t[i] - rdt_t[rID] && e_t[i] - rdt_t[rID] < 10 && rdt[rID] > 5000){  // recoil energy and time gate
         if( e[i] > 0 ) {
            eC[i]   = e[i];
            eC_t[i] = e_t[i];
         }
         if( xf[i] > 0) xfC[i] = xf[i] * xfxneCorr[i][1] + xfxneCorr[i][0] ;
         if( xn[i] > 0) xnC[i] = xn[i] * xnCorr[i] * xfxneCorr[i][1] + xfxneCorr[i][0];
         
         //========= calculate x
         if(xf[i] > 0  && xn[i] > 0 ) {
            x[i] = (xfC[i]-xnC[i])/(xfC[i]+xnC[i]);
            hitID = 0;
         }else if(xf[i] == 0 && xn[i] > 0 ){
            x[i] = (1-2*xnC[i]/eC[i]);
            hitID = 1;
         }else if(xf[i] > 0 && xn[i] == 0 ){
            x[i] = (2*xfC[i]/eC[i]-1);
            hitID = 2;
         }else{
            x[i] = TMath::QuietNaN();
         }
         
         //========= calculate z, det
         if( TMath::IsNaN(x[i]) ) {
            z[i] = TMath::QuietNaN();
         }else{ 
            int detID = i%iDet;
            if( pos[detID] < 0 ){
               z[i] = pos[detID] - (-x[i] + 1.)*length/2 ; 
            }else{
               z[i] = pos[detID] + (x[i] + 1.)*length/2 ; 
            }
            zMulti ++;
            count ++;
            det = i;
            //printf(" det: %d, detID: %d, x: %f, pos:%f, z: %f \n", det, detID, x[i], pos[detID], z[i]);
         }
         
         if( option >=2 && !TMath::IsNaN(eC[i]) && !TMath::IsNaN(z[i]) ){
            //==== cal unified energy
            int posID = (i - i%iDet)/iDet;
            int detID = i%iDet;
            energy = ((m[detID] * z[i] - e[i])*c1[detID][posID] - c0[detID][posID])*j1[detID] + j0[detID];
            
            //==== cal Ex,thetaCM
            Ex = p0 + p1 * energy;
            Ex = Ex/1000.; // in to MeV
            
            
            double p0 = (0.521973 + 0.011473594 * Ex + 0.000816016 * Ex * Ex);
            double p1 = (-0.000721 - 0.000016868 * Ex - 0.000001344 * Ex * Ex); 
            double costhetaCM = p0 + p1 * z[i];  
            thetaCM = TMath::ACos(costhetaCM) * TMath::RadToDeg();
            
            // calculate coincident time
            int temp = 10000;
            ULong64_t ePicked = -10000000;
            for(int rID = 0; rID < 8; rID ++){
               if( !TMath::IsNaN(rdtC_t[rID]) ){
                  int a = e_t[i];
                  int b = rdt_t[rID];
                  if( TMath::Abs(a - b)  < TMath::Abs(temp)){
                     temp    = e_t[i] - rdt_t[rID];
                     ePicked = e_t[i];
                  } 
                   
               }
            }
            energy_t = temp;
            
         }
         
         
         if( option >= 3 && !TMath::IsNaN(Ex) ){
            //==== for tac Calibration
            if( tac[4] > 2000 &&  xf[i] !=0 && xn[i] !=0 ) {
               x[i] = (xfC[i] - xnC[i])/(xfC[i] + xnC[i]);
               tt = tac[4] - 650 * x[i]*x[i]; 
               if( tt < cut[i]){
                  tt += 1250;
               }
               
               //next correction
               if( polDeg[i] > 1 ) {
                  //ttt = tt - tc[i][1] * z[i] - tc[i][2]*TMath::Power(z[i],2) - tc[i][3]*TMath::Power(z[i],3) - tc[i][4]*TMath::Power(z[i],4);
                  double xxx = x[i];
                  double value = fit[i]->Eval(xxx);
                  
                  ttt = tt - value - mean[i];
               }else{
                  ttt = tt - mean[i];
               }
            
            }else{
               x[i] = TMath::QuietNaN();
            }  
                  
            int temp = 10000;
            ULong64_t ePicked = -10000000;
            for(int rID = 0; rID < 8; rID ++){
               if( !TMath::IsNaN(rdtC_t[rID]) ){
                  int a = e_t[i];
                  int b = rdt_t[rID];
                  if( TMath::Abs(a - b)  < TMath::Abs(temp)){
                     ePicked = e_t[i];
                  } 
                   
               }
            }
            for(int tID = 0; tID < 6; tID++){
               dt[tID] = ePicked - tac_t[tID];
            }
            
            if( z[i]> 0) printf("i:%d, xfC:%f, xnC:%f, x:%f \n", i, xfC[i], xnC[i], z[i]);
         
            // correction of ttt with energy
            t4 = ttt + 0.165 * energy + 0.000188935 * energy * energy + 7.87558e-8 * TMath::Power(energy,3);
            //check for good event
            
            if( tcut[i][1] > t4 && t4 > tcut[i][0] && TMath::Abs(energy_t)<20 ) good = 1;
            
         }
         
         //if( energy [i] < -3000 ){
         //   printf("%15.3f, %2d, %d, %d, m:%f,  c1:%f, c0:%f, j1:%f, j0:%f \n", energy[i], i, detID, posID, m[detID], 
         //                          c1[detID][posID], 
         //                          c0[detID][posID], j1[detID], j0[detID] );
         //}
          
      }// end of i
      
   }
   
   //for H060
   ddt = elum[0];
   
   //disable for H060, for 
   if( zMulti == 0 ) return kTRUE;
   
   //#################################################################### Timer  
   saveFile->cd(); //set focus on this file
   newTree->Fill();  

   clock.Stop("timer");
   Double_t time = clock.GetRealTime("timer");
   clock.Start("timer");

   if ( !shown ) {
      if (fmod(time, 10) < 1 ){
         printf( "%10d[%2d%%]|%3d min %5.2f sec | expect:%5.1fmin %10d\n", 
               eventID, 
               TMath::Nint((eventID+1)*100./totnumEntry),
               TMath::Floor(time/60.), time - TMath::Floor(time/60.)*60.,
               totnumEntry*time/(eventID+1)/60.,
               count);
               shown = 1;
      }
   }else{
      if (fmod(time, 10) > 9 ){
         shown = 0;
      }
   }

   return kTRUE;
}

void Cali_root::SlaveTerminate()
{
   // The SlaveTerminate() function is called after all entries or objects
   // have been processed. When running with PROOF SlaveTerminate() is called
   // on each slave server.
}

void Cali_root::Terminate()
{
   // The Terminate() function is the last function to be called during
   // a query. It always runs on the client, it can be used to present
   // the results graphically or save the results to file.
   
   saveFile->cd(); //set focus on this file
   newTree->Write(); 
   saveFile->Close();

   printf("-------------- done. %s, %d\n", saveFileName.Data(), count);
   

}
