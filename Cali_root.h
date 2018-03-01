//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Fri Feb 16 15:14:22 2018 by ROOT version 5.34/36
// from TTree gen_tree/PSD Tree
// found on file: H052_Mg25.root
//////////////////////////////////////////////////////////

#ifndef Cali_root_h
#define Cali_root_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TSelector.h>

// Header file for the classes stored in the TTree if any.

// Fixed size dimensions of array or collections stored in the TTree if any.

class Cali_root : public TSelector {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   
   TFile * saveFile;
   TTree * newTree;
   TString saveFileName;
   Int_t totnumEntry;
   
   TBenchmark clock;
   Bool_t shown;
   Int_t count;
   
   int numDet;
   Double_t nearPos[6];
   Double_t length;
   Double_t eCorr[24];
   Double_t xnCorr[24];
   
   Int_t eventID;
   Float_t eC[24];
   Float_t xfC[24];
   Float_t xnC[24];
   Float_t x[24];
   Float_t rdtC[8];
   

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

   Cali_root(TTree * /*tree*/ =0) : fChain(0) { }
   virtual ~Cali_root() { }
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

   ClassDef(Cali_root,0);
};

#endif

#ifdef Cali_root_cxx
void Cali_root::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("e", e, &b_Energy);
   fChain->SetBranchAddress("e_t", e_t, &b_EnergyTimestamp);
   fChain->SetBranchAddress("xf", xf, &b_XF);
   fChain->SetBranchAddress("xf_t", xf_t, &b_XFTimestamp);
   fChain->SetBranchAddress("xn", xn, &b_XN);
   fChain->SetBranchAddress("xn_t", xn_t, &b_XNTimestamp);
   fChain->SetBranchAddress("rdt", rdt, &b_RDT);
   fChain->SetBranchAddress("rdt_t", rdt_t, &b_RDTTimestamp);
   fChain->SetBranchAddress("tac", tac, &b_TAC);
   fChain->SetBranchAddress("tac_t", tac_t, &b_TACTimestamp);
   fChain->SetBranchAddress("elum", elum, &b_ELUM);
   fChain->SetBranchAddress("elum_t", elum_t, &b_ELUMTimestamp);
   
   //===================== custom code
   printf( "========== Coverting root to Calibrated root.\n");
   saveFileName = fChain->GetDirectory()->GetName();
   TString prefix;
   prefix.Form("C_");
   saveFileName = prefix + saveFileName;
   totnumEntry = tree->GetEntries();
   printf("Converting %s ------> %s , total Entry : %d \n", fChain->GetDirectory()->GetName(), saveFileName.Data(), totnumEntry);
   saveFile = new TFile( saveFileName,"recreate");
   newTree =  new TTree("tree","tree");
   
   clock.Reset();
   clock.Start("timer");
   shown = 0;
   count = 0;
   
   numDet = 24;
   //===================================================== loading parameter
   printf("----- loading sensor position.");
   ifstream file;
   file.open("nearPos.dat");
   double a;
   int i = 0;
   while( file >> a ){
      if( i >= 7) break;
      if( i == 6) length = a;
      nearPos[i] = a;
      i = i + 1;
   }
   file.close();
   printf("... done.\n      ");
   for(int i = 0; i < 5 ; i++){
      printf("%6.2f mm, ", nearPos[i]);
   }
   printf("%6.2f mm || length : %6.2f mm \n", nearPos[5], length);
   
   
   printf("----- loading xf-xn correction.");
   file.open("correction_xf_xn.dat");
   double a;
   int i = 0;
   while( file >> a ){
      if( i >= numDet) break;
      xnCorr[i] = a;
      //xnCorr[i] = 1;
      i = i + 1;
   }
   file.close();
   printf("... done.\n");
   
   
   //===================================================== tree branch
   
   for(int i = 0; i < numDet; i++){
      eC[i]  = TMath::QuietNaN();
      xfC[i] = TMath::QuietNaN();
      xnC[i] = TMath::QuietNaN();
      x[i]   = TMath::QuietNaN();
   }
   
   newTree->Branch("eventID",&eventID,"eventID/I"); 
   
   newTree->Branch("e" ,  eC, "eC[24]/F");
   newTree->Branch("xf", xfC, "xfC[24]/F");
   newTree->Branch("xn", xnC, "xnC[24]/F");
   newTree->Branch("x" ,   x, "x[24]/F");
   newTree->Branch("rdt", rdtC, "rdtC[8]/F");
   
}

Bool_t Cali_root::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

#endif // #ifdef Cali_root_cxx
