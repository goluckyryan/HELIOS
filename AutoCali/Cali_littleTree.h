//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Tue Jul 10 21:31:47 2018 by ROOT version 6.10/02
// from TTree gen_tree/PSD Tree
// found on file: gen_run11.root
//////////////////////////////////////////////////////////

#ifndef Cali_littleTree_h
#define Cali_littleTree_h

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


class Cali_littleTree : public TSelector {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain

   // Declaration of leaf types
   Float_t         e[100];
   Float_t         xf[100];
   Float_t         xn[100];
   Float_t         rdt[100];
   
   ULong64_t       e_t[100];
   ULong64_t       rdt_t[100];
   
   // List of branches
   TBranch        *b_Energy;   //!
   TBranch        *b_XF;   //!
   TBranch        *b_XN;   //!
   TBranch        *b_RDT;   //!
   
   TBranch        *b_EnergyTimestamp;   //!
   TBranch        *b_RDTTimestamp;   //!

   Cali_littleTree(TTree * /*tree*/ =0) : fChain(0) { }
   virtual ~Cali_littleTree() { }
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

   ClassDef(Cali_littleTree,0);
   
   //=============================== 
   TFile * saveFile;
   TTree * newTree;
   TString saveFileName;
   int totnumEntry; // of original root
  
   //tree  
   Int_t eventID;
   Int_t run;
   double eTemp;
   double xTemp; // unadjusted position, range (-1,1)
   double zTemp; 
   int detIDTemp;
   int hitID; // is e, xf, xn are all fired.
   int zMultiHit; // multipicity of z
   
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
   
};

#endif

#ifdef Cali_littleTree_cxx
void Cali_littleTree::Init(TTree *tree)
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
   fChain->SetBranchAddress("rdt", rdt, &b_RDT);
   
   fChain->SetBranchAddress("e_t", e_t, &b_EnergyTimestamp);
   fChain->SetBranchAddress("rdt_t", rdt_t, &b_RDTTimestamp);  
   
   //======================================
   totnumEntry = tree->GetEntries();
   printf( "========== reduce to litte tree, total Entry : %d \n", totnumEntry);
   
   saveFileName = "temp.root";
   saveFile = new TFile( saveFileName,"recreate");
   newTree =  new TTree("tree","tree");
   
   eventID = -1;
   run = 0;
   
   newTree->Branch("eventID",&eventID,"eventID/I"); 
   newTree->Branch("run",&run,"run/I"); 
   
   newTree->Branch("e" ,  &eTemp, "eTemp/D");
   newTree->Branch("x" ,   &xTemp, "xTemp/D");
   newTree->Branch("z" ,   &zTemp, "zTemp/D");
   newTree->Branch("detID", &detIDTemp, "detIDTemp/I");
   newTree->Branch("hitID", &hitID, "hitID/I");
   newTree->Branch("zMultiHit", &zMultiHit, "zMultiHit/I");
   
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

   printf("================================== numDet : %d \n", numDet);
   
}

Bool_t Cali_littleTree::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}


#endif // #ifdef Cali_littleTree_cxx
