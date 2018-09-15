//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Tue Jul 10 21:31:47 2018 by ROOT version 6.10/02
// from TTree gen_tree/PSD Tree
// found on file: gen_run11.root
//////////////////////////////////////////////////////////

#ifndef Cali_e_h
#define Cali_e_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TSelector.h>
#include <TMath.h>
#include <TBenchmark.h>
#include <TF1.h>
#include <string>
#include <fstream>

// Headers needed by this particular selector


class Cali_e : public TSelector {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain

   // Declaration of leaf types
   Float_t         e[100];
   ULong64_t       e_t[100];
   Float_t         xf[100];
   ULong64_t       xf_t[100];
   Float_t         xn[100];
   ULong64_t       xn_t[100];
   Float_t         rdt[100];
   ULong64_t       rdt_t[100];
   Float_t         tac[100];
   ULong64_t       tac_t[100];
   Float_t         elum[32];
   ULong64_t       elum_t[32];

   // List of branches
   TBranch        *b_Energy;   //!
   TBranch        *b_EnergyTimestamp;   //!
   TBranch        *b_XF;   //!
   TBranch        *b_XFTimestamp;   //!
   TBranch        *b_XN;   //!
   TBranch        *b_XNTimestamp;   //!
   TBranch        *b_RDT;   //!
   TBranch        *b_RDTTimestamp;   //!
   TBranch        *b_TAC;   //!
   TBranch        *b_TACTimestamp;   //!
   TBranch        *b_ELUM;   //!
   TBranch        *b_ELUMTimestamp;   //!

   Cali_e(TTree * /*tree*/ =0) : fChain(0) { }
   virtual ~Cali_e() { }
   virtual Int_t   Version() const { return 2; }
   virtual void    Begin(TTree *tree);
   virtual void    SlaveBegin(TTree *tree);
   virtual void    Init(TTree *tree);
   virtual Bool_t  Notify();
   virtual Bool_t  Process(Long64_t entry);
   virtual Int_t   GetEntry(Long64_t entry, Int_t getall = 0) { return fChain ? fChain->GetTree()->GetEntry(entry, getall) : 0; }
   virtual void    SetOption(const char *option) { fOption = option; }
   virtual void    SetObject(TObject *obj) { fObject = obj; }
   virtual void    SetInputList(TList *input) { fInput = input; }
   virtual TList  *GetOutputList() const { return fOutput; }
   virtual void    SlaveTerminate();
   virtual void    Terminate();

   ClassDef(Cali_e,0);
   
   //=============================== 
   TFile * saveFile;
   TTree * newTree;
   TString saveFileName;
   int totnumEntry; // of original root
  
   //tree  
   Int_t eventID;
   Int_t run;
   Float_t eC[24];
   ULong64_t eC_t[24];
   Float_t x[24]; // unadjusted position, range (-1,1)
   Float_t z[24]; 
   int det;
   int hitID; // is e, xf, xn are all fired.
   int zMultiHit; // multipicity of z
   
   Float_t thetaCM;
   Float_t Ex;   
   Float_t thetaLab;
   
   Float_t rdtC[8];
   ULong64_t rdtC_t[8];
   ULong64_t coin_t; // when zMultiHit == 1 and rdtC_t(max(rdtC))
   
   //Float_t ddt, ddt_t; // downstream detector for deuteron, for H060_208Pb 
   //Float_t tacS; //tac for H060

   //clock   
   TBenchmark clock;
   Bool_t shown;
   Int_t count;
   
   //correction parameters
   int numDet;
   int iDet; // number of detector at different position
   int jDet; // number of detector at same position
   vector<double> pos;
   double Bfield;
   double a;
   double length;
   double firstPos;
   double xnCorr[24]; // xn correction for xn = xf
   double xfxneCorr[24][2]; //xf, xn correction for e = xf + xn
   
   double eCorr[24][2]; // e-correction
   
   bool isReaction;
   double G, Z, H; // for excitation calcualtion
   double mass, q;
   double beta, gamma;
   double alpha ;
   double Et, massB;


};

#endif

#ifdef Cali_e_cxx
void Cali_e::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the reader is initialized.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).
   
   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   //printf("========== number of tree loaded : %d \n", fChain->GetNTree());
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("e", e, &b_Energy);
   fChain->SetBranchAddress("xf", xf, &b_XF);
   fChain->SetBranchAddress("xn", xn, &b_XN);
   
   fChain->SetBranchAddress("e_t", e_t, &b_EnergyTimestamp);
   fChain->SetBranchAddress("rdt", rdt, &b_RDT);
   fChain->SetBranchAddress("rdt_t", rdt_t, &b_RDTTimestamp);   
   //fChain->SetBranchAddress("xf_t", xf_t, &b_XFTimestamp);
   //fChain->SetBranchAddress("xn_t", xn_t, &b_XNTimestamp);

   fChain->SetBranchAddress("tac", tac, &b_TAC);
   fChain->SetBranchAddress("tac_t", tac_t, &b_TACTimestamp);
   fChain->SetBranchAddress("elum", elum, &b_ELUM);
   fChain->SetBranchAddress("elum_t", elum_t, &b_ELUMTimestamp);
   
   //======================================
   totnumEntry = tree->GetEntries();
   printf( "========== Make a new tree with calibration, total Entry : %d \n", totnumEntry);
   
   saveFileName = fChain->GetDirectory()->GetName();
   //remove any folder path to get the name;
   int found;
   do{
      found = saveFileName.First("/");
      saveFileName.Remove(0,found+1);
   }while( found >= 0 );
   saveFileName = "A_" + saveFileName;
   
   saveFile = new TFile( saveFileName,"recreate");
   newTree =  new TTree("tree","tree");
   
   eventID = 0;
   run = 0;
   
   newTree->Branch("eventID",&eventID,"eventID/I"); 
   newTree->Branch("run",&run,"run/I"); 
   
   newTree->Branch("e" ,  eC, "eC[24]/F");
   newTree->Branch("x" ,   x, "x[24]/F");
   newTree->Branch("z" ,   z, "z[24]/F");
   newTree->Branch("detID", &det, "det/I");
   newTree->Branch("hitID", &hitID, "hitID/I");
   newTree->Branch("zMultiHit", &zMultiHit, "zMultiHit/I");
   
   newTree->Branch("Ex", &Ex, "Ex/F");
   newTree->Branch("thetaCM", &thetaCM, "thetaCM/F");
   newTree->Branch("thetaLab", &thetaLab, "thetaLab/F");
   
   newTree->Branch("e_t", eC_t, "e_t[24]/l");
   
   newTree->Branch("rdt", rdtC, "rdtC[8]/F");
   newTree->Branch("rdt_t", rdtC_t, "rdtC_t[8]/l");
   
   newTree->Branch("coin_t", &coin_t, "coin_t/l");
   
   /*
   newTree->Branch("rdt_m", &rdt_m, "rdt_m/I");
   newTree->Branch("tac", tacC, "tacC[6]/F");
   newTree->Branch("tac_t", tacC_t, "tacC_t[6]/F"); 
   */
   
   //H060
   //newTree->Branch("tac", &tacS, "tacS/F");
   //newTree->Branch("ddt", &ddt, "ddt/F");
   //newTree->Branch("ddt_t", &ddt_t, "ddt_t/F");
   
   clock.Reset();
   clock.Start("timer");
   shown = 0;
   count = 0;
   
   //===================================================== loading parameter
   
   //========================================= detector Geometry
   string detGeoFileName = "detectorGeo_upstream.txt";
   printf("----- loading detector geometery : %s.", detGeoFileName.c_str());
   ifstream file;
   file.open(detGeoFileName.c_str());
   int i = 0;
   if( file.is_open() ){
      string x;
      while( file >> x){
         //printf("%d, %s \n", i,  x.c_str());
         if( x.substr(0,2) == "//" )  continue;
         if( i == 0 ) Bfield   = atof(x.c_str());
         if( i == 2 ) a        = atof(x.c_str());         
         if( i == 6 ) length   = atof(x.c_str());
         if( i == 8 ) firstPos = atof(x.c_str());
         if( i == 9 ) jDet = atoi(x.c_str());
         if( i >= 10 ) {
            pos.push_back(atof(x.c_str()));
         }
         i = i + 1;
      }
      
      iDet = pos.size();
      file.close();
      printf("... done.\n");
      
      for(int id = 0; id < iDet; id++){
         pos[id] = firstPos + pos[id];
      }
      
      for(int i = 0; i < iDet ; i++){
         if( firstPos > 0 ){
            printf("%d, %6.2f mm - %6.2f mm \n", i, pos[i], pos[i] + length);
         }else{
            printf("%d, %6.2f mm - %6.2f mm \n", i, pos[i] - length , pos[i]);
         }
      }
      printf("=======================\n");
      
   }else{
       printf("... fail\n");
       return;
   }
   
   numDet = iDet * jDet;
   
   //========================================= xf = xn correction
   printf("----- loading xf-xn correction.");
   file.open("correction_xf_xn.dat");
   if( file.is_open() ){
      double a;
      int i = 0;
      while( file >> a ){
         if( i >= numDet) break;
         xnCorr[i] = a;
         //xnCorr[i] = 1;
         i = i + 1;
      }
      
      printf("... done.\n");
   }else{
      printf("... fail.\n");
      return;
   }
   file.close();
   
   //========================================= e = xf + xn correction
   
   printf("----- loading xf/xn-e correction.");
   file.open("correction_xfxn_e.dat");
   if( file.is_open() ){
      double a, b;
      int i = 0;
      while( file >> a >> b){
         if( i >= numDet) break;
         xfxneCorr[i][0] = a;
         xfxneCorr[i][1] = b;
         i = i + 1;
      }
      printf("... done.\n");
   }else{
      printf("... fail.\n");
      return;
   }
   file.close();

   //========================================= e correction
   
   printf("----- loading e correction.");
   file.open("correction_e.dat");
   if( file.is_open() ){
      double a, b;
      int i = 0;
      while( file >> a >> b){
         if( i >= numDet) break;
         eCorr[i][0] = a;
         eCorr[i][1] = b;
         //printf("\n%2d, e0: %f, e1: %f ", i, eCorr[i][0], eCorr[i][1]);
         i = i + 1;
      }
      printf("... done.\n");
      
   }else{
      printf("... fail.\n");
      for( int i = 0; i < 24 ; i++){
         eCorr[i][0] = 1.;
         eCorr[i][1] = 0.;
      }
      //return;
   }
   file.close();
   
   //========================================= reaction parameters
   printf("----- loading reaction parameter.");
   file.open("reaction.dat");
   isReaction = false;
   if( file.is_open() ){
      string x;
      int i = 0;
      while( file >> x ){
         if( x.substr(0,2) == "//" )  continue;
         if( i == 0 ) mass = atof(x.c_str());
         if( i == 1 ) q    = atof(x.c_str());
         if( i == 2 ) beta = atof(x.c_str()); 
         if( i == 3 ) Et   = atof(x.c_str()); 
         if( i == 4 ) massB = atof(x.c_str()); 
         i = i + 1;
      }
      printf("... done.\n");

      isReaction = true;
      alpha = 299.792458 * Bfield * q / TMath::TwoPi()/1000.;
      gamma = 1./TMath::Sqrt(1-beta*beta);
      G = alpha * gamma * beta * a ;
      printf("============\n");
      printf("mass-b  : %f MeV/c2 \n", mass);
      printf("charge-b: %f \n", q);
      printf("E-total : %f MeV \n", Et);
      printf("mass-B  : %f MeV/c2 \n", massB);      
      printf("beta    : %f \n", beta);
      printf("B-field : %f T \n", Bfield);
      printf("alpha   : %f MeV/mm \n", alpha);
      printf("a       : %f mm \n", a);
      printf("G       : %f MeV \n", G);


   }else{
      printf("... fail.\n");
      isReaction = false;
   }
   file.close();

   printf("================================== numDet : %d \n", numDet);
   

}

Bool_t Cali_e::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}


#endif // #ifdef Cali_e_cxx
