//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Fri Feb  2 14:49:00 2018 by ROOT version 5.34/36
// from TTree gen_tree/PSD Tree
// found on file: H052_Mg25.root
//////////////////////////////////////////////////////////

#ifndef Compare_h
#define Compare_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TSelector.h>
#include <fstream>

// Header file for the classes stored in the TTree if any.

// Fixed size dimensions of array or collections stored in the TTree if any.

class Compare : public TSelector {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   
   TFile * saveFile;
   TTree * newTree;
   
   Double_t slope[24];
   Double_t intep[24];
   Double_t nearPos[6];
   Double_t length;
   
   Int_t id; // detector ID;
   
   TString saveFileName;
   Int_t totnumEntry;
   
   TBenchmark clock;
   Bool_t shown;
   Int_t count;
   
   Int_t eventID;
   Float_t e1, e2, e3, e0;
   Float_t xf1, xf2, xf3, xf0;
   Float_t xn1, xn2, xn3, xn0;
   Float_t x1, x2, x3, x0;
   
   Int_t reg[12];

   // Declaration of leaf types
   Float_t         e[100];
   //ULong64_t       e_t[100];
   Float_t         xf[100];
   //ULong64_t       xf_t[100];
   Float_t         xn[100];
   //ULong64_t       xn_t[100];
   //Float_t         rdt[100];
   //ULong64_t       rdt_t[100];
   //Float_t         tac[100];
   //ULong64_t       tac_t[100];
   //Float_t         elum[32];
   //ULong64_t       elum_t[32];

   // List of branches
   TBranch        *b_Energy;   //!
   //TBranch        *b_EnergyTimestamp;   //!
   TBranch        *b_XF;   //!
   //TBranch        *b_XFTimestamp;   //!
   TBranch        *b_XN;   //!
   //TBranch        *b_XNTimestamp;   //!
   //TBranch        *b_RDT;   //!
   //TBranch        *b_RDTTimestamp;   //!
   //TBranch        *b_TAC;   //!
   //TBranch        *b_TACTimestamp;   //!
   //TBranch        *b_ELUM;   //!
   //TBranch        *b_ELUMTimestamp;   //!

   Compare(TTree * /*tree*/ =0) : fChain(0) { }
   virtual ~Compare() { }
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

   ClassDef(Compare,0);
};

#endif

#ifdef Compare_cxx
void Compare::Init(TTree *tree)
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
   //fChain->SetBranchAddress("e_t", e_t, &b_EnergyTimestamp);
   fChain->SetBranchAddress("xf", xf, &b_XF);
   //fChain->SetBranchAddress("xf_t", xf_t, &b_XFTimestamp);
   fChain->SetBranchAddress("xn", xn, &b_XN);
   //fChain->SetBranchAddress("xn_t", xn_t, &b_XNTimestamp);
   //fChain->SetBranchAddress("rdt", rdt, &b_RDT);
   //fChain->SetBranchAddress("rdt_t", rdt_t, &b_RDTTimestamp);
   //fChain->SetBranchAddress("tac", tac, &b_TAC);
   //fChain->SetBranchAddress("tac_t", tac_t, &b_TACTimestamp);
   //fChain->SetBranchAddress("elum", elum, &b_ELUM);
   //fChain->SetBranchAddress("elum_t", elum_t, &b_ELUMTimestamp);
   
   //===================== detector ID
   printf("Enter first detector ID : ");
   scanf("%d", &id); 
   printf( "========== Coverting detector id = %d, %d, %d, %d .\n", id, id+6, id+12, id+18);
   saveFileName = fChain->GetDirectory()->GetName();
   TString prefix;
   prefix.Form("X%d_", id);
   saveFileName = prefix + saveFileName;
   totnumEntry = tree->GetEntries();
   
   printf("Converting %s ------> %s , total Entry : %d \n", fChain->GetDirectory()->GetName(), saveFileName.Data(), totnumEntry);
   
   saveFile = new TFile( saveFileName,"recreate");
   newTree =  new TTree("tree","tree");
   
   clock.Reset();
   clock.Start("timer");
   shown = 0;
   count = 0;
   
   eventID = 0;
   e1 = TMath::QuietNaN();
   e2 = TMath::QuietNaN();
   e3 = TMath::QuietNaN();
   e0 = TMath::QuietNaN();
   
   xf1 = TMath::QuietNaN();
   xf2 = TMath::QuietNaN();
   xf3 = TMath::QuietNaN();
   xf0 = TMath::QuietNaN();
   
   xn1 = TMath::QuietNaN();
   xn2 = TMath::QuietNaN();
   xn3 = TMath::QuietNaN();
   xn0 = TMath::QuietNaN();
   
   x1 = TMath::QuietNaN();
   x2 = TMath::QuietNaN();
   x3 = TMath::QuietNaN();
   x0 = TMath::QuietNaN();
   
   newTree->Branch("eventID",&eventID,"eventID/I"); 
   
   newTree->Branch("e0",&e0,"e0/F"); 
   newTree->Branch("e1",&e1,"e1/F"); 
   newTree->Branch("e2",&e2,"e2/F"); 
   newTree->Branch("e3",&e3,"e3/F"); 
   
   newTree->Branch("xf0",&xf0,"xf0/F");
   newTree->Branch("xf1",&xf1,"xf1/F"); 
   newTree->Branch("xf2",&xf2,"xf2/F");
   newTree->Branch("xf3",&xf3,"xf3/F"); 
   
   newTree->Branch("xn0",&xn0,"xn0/F");
   newTree->Branch("xn1",&xn1,"xn1/F"); 
   newTree->Branch("xn2",&xn2,"xn2/F");
   newTree->Branch("xn3",&xn3,"xn3/F");
   
   newTree->Branch("x0",&x0,"x0/F");
   newTree->Branch("x1",&x1,"x1/F"); 
   newTree->Branch("x2",&x2,"x2/F");
   newTree->Branch("x3",&x3,"x3/F");
   
   newTree->Branch("reg",reg,"reg[12]/I");
   
   //========= load xf xn parameters
   printf("----- loading xf xn parameters.");
   ifstream file;
   file.open("xf_xn_correction_para.dat");
   double a, b;
   int i = 0;
   while( file >> a >> b ){
      if( i >= 24) break;
      slope[i] = b;
      intep[i] = a;
      i = i + 1;
   }
   printf("... done.\n");
   
   //========= load position parameters
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
   printf("... done.\n");
   for(int i = 0; i < 5 ; i++){
      printf("%f, ", nearPos[i]);
   }
   printf("%f, length : %f \n ", nearPos[5], length);
   
   printf( "========== Sensor Position %f - %f. \n", nearPos[id], nearPos[id] + length);

}

Bool_t Compare::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

#endif // #ifdef Compare_cxx
