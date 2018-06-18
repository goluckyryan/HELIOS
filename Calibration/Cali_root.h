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
#include <TMath.h>
#include <TBenchmark.h>
#include <TF1.h>
#include <string>
#include <fstream>

// Header file for the classes stored in the TTree if any.

// Fixed size dimensions of array or collections stored in the TTree if any.

class Cali_root : public TSelector {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   
   TFile * saveFile;
   TTree * newTree;
   TString saveFileName;
   Int_t totnumEntry;
   int option;
   
   TBenchmark clock;
   Bool_t shown;
   Int_t count;
   
   //correction parameters
   int numDet;
   int iDet; // number of detector at different position
   int jDet; // number of detector at same position
   vector<double> pos;
   double length;
   double firstPos;
   double xnCorr[24]; // xn correction for xn = xf
   double xfxneCorr[24][2]; //xf, xn correction for e = xf + xn
   
   double c1[6][4]; // same position e_correction
   double c0[6][4];
   double m[6]  ;   
   double j0[6]; // different position e_correction 
   double j1[6]; 
   double p0, p1; // e_to_Ex correction
   
   double cut[24]; // tac cut 
   double tc[24][5]; // tac correction parameter
   double mean[24]; // tt mean
   int polDeg[24]; // final correction , 0 for no correction, n for degree of polynormial.
   TF1 ** fit;
   double tcut[24][2]; // t4 cut for "good event"
     
   //tree  
   Int_t eventID;
   Float_t eC[24];
   Float_t xfC[24];
   Float_t xnC[24];
   Float_t x[24]; // unadjusted position, range (-1,1)
   Float_t z[24]; 
   int det;
   int hitID; // is e, xf, xn are all fired.
   int zMulti; // multipicity of z
      
   Float_t thetaCM;
   Float_t Ex;   
   Float_t energy;
   Int_t energy_t;
   Float_t rdtC[8];
   int rdt_m ;
   Float_t eC_t[24];
   Float_t rdtC_t[8];
   
   int tac_m;
   Float_t tacC[6];
   Float_t tacC_t[6];
   Int_t dt[6];
   Float_t tt; // corrected tac[4];
   Float_t ttt; // next corrected tac[4];
   Float_t t4; // ttt correction with energy 
   
   int good; // good event = 1, else = 0;
   
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

   //===================== custom code
   printf( "========== Coverting root to Calibrated root.\n");
   printf(" 0 = after alpha source,\n 1 = after xf/xn - e cali.,\n 2 = after e cali.,\n 3 = with gates: ");
   scanf("%d", &option);
   saveFileName = fChain->GetDirectory()->GetName();
   
   //remove any folder path to get the name;
   int found;
   do{
      found = saveFileName.First("/");
      saveFileName.Remove(0,found+1);
   }while( found >= 0 );
   
   TString prefix;
   if(option  < 3 ) prefix.Form("C_");
   if(option >= 3 ) prefix.Form("X_");
   
   saveFileName = prefix + saveFileName;
   totnumEntry = tree->GetEntries();
   printf("Converting ------> %s , total Entry : %d \n", saveFileName.Data(), totnumEntry);
   saveFile = new TFile( saveFileName,"recreate");
   newTree =  new TTree("tree","tree");
   
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
   
   if( option >= 1){
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
   }else{
      for( int i = 0; i < numDet; i++){
         xfxneCorr[i][0] = 0.; 
         xfxneCorr[i][1] = 1.;
      }
   }
   //for( int i = 0; i < 24; i++){
   //   printf("%d : %f, %f \n", i, xfxneCorr[i][0], xfxneCorr[i][1]);
   //}
   
   //========================================= 
   if( option >= 2 ){
      printf("----- loading energy calibration for same position. \n");
      for( int i = 0; i < iDet; i++){
         TString filename;
         filename.Form("correction_e%d.dat", i);
         printf("        %s", filename.Data());
         file.open(filename.Data());
         if( file.is_open() ){
            double a, b;
            int j = 0;
            while( file >> a >> b ){
               c0[i][j] = a;
               c1[i][j] = b;
               j = j + 1;
               if( j >= jDet) break;
            }
            file >> a;
            m[i] = a;
            
            printf("... done.\n");
            for(int j = 0; j < jDet; j++){ 
               printf("                %d,  c0 : %8.3f,  c1 : %8.3f, m : %5.2f \n", j, c0[i][j], c1[i][j], m[i]);
            }
         }else{
            printf("... fail.\n");
            return;
         }
         file.close();
      }
   
      //=========================================   
      printf("----- loading energy calibration for different position.");
      file.open("correction_e_diff.dat");
      if( file.is_open() ){
         double a, b;
         int i = 0;
         while( file >> a >> b ){
            j0[i] = a;
            j1[i] = b;
            i = i+ 1;
         }
         file.close();
         printf("... ok.\n");
         for(int i = 0; i < iDet ; i++){
            printf("                %d, j0: %9.3f, j1: %9.3f \n", i, j0[i], j1[i]);
         }
      }else{
         printf("... fail.\n");
         return;
      }
      file.close();
      
      //=========================================   
      printf("----- loading energy to Ex correction parameters.");
      file.open("correction_e_to_Ex.dat");
      if( file.is_open() ){
         double a, b;
         while( file >> a >> b){
            p0 = a;
            p1 = b;
         }
         file.close();
         printf("... ok.\n");
         printf("                p0: %9.3f, p1: %9.3f \n", p0, p1);
         
      }else{
         printf("... fail.\n");
         p0 = 0;
         p1 = 1;
      }
      file.close();
   }
   
   if( option >= 3 ){
   //========================================= 
      // tac cut
      cut[0] = 2500;
      cut[1] = 2000;
      cut[2] = 2000;
      cut[3] = 2870;
      cut[4] = 3000;
      cut[5] = 2281;
      cut[6] = 2500;
      cut[7] = 2000;
      cut[8] = 2400;
      cut[9] = 2000;
      cut[10] = 2000;
      cut[11] = 1000;
      cut[12] = 2500;
      cut[13] = 2500;
      cut[14] = 2500;
      cut[15] = 2300;
      cut[16] = 1800;
      cut[17] = 2000;
      cut[18] = 1500;
      cut[19] = 2200;
      cut[20] = 2000;
      cut[21] = 2000;
      cut[22] = 2000;
      cut[23] = 1800;
      
      printf("----- loading tac (mean) calibration.");
      file.open("tac_correction_mean.dat");
      if( file.is_open() ){
         double a;
         int i = 0;
         while( file >> a ){
            if( i > 24) break;
            mean[i] = a;
            i = i + 1;
         }
         printf("... done.\n");
      }else{
         printf("... fail.\n");
      }
      file.close();
      
      printf("----- loading tac (deg) calibration.");
      file.open("tac_correction_deg.dat");
      if( file.is_open() ){
         double a;
         int i = 0;
         while( file >> a ){
            if( i > 24) break;
            polDeg[i] = a;
            i = i + 1;
         }
         printf("... done.\n");
      }else{
         printf("... fail.\n");
      }
      file.close();
      
      printf("----- loading tac calibration.");
      file.open("tac_correction.dat");
      
      fit = new TF1*[24];
      
      if( file.is_open() ){
         for( int i = 0; i < 24; i++){
            char  line[1000];
            file.getline(line, 1000, '\n');
            string sLine(line);
            TString name;
            name.Form("fit%d", i);
            
            if( polDeg[i] > 1){
               if( (int) sLine.size() < 20 * (polDeg[i]+1) ) {
                  printf(" error ! missing parameter for fit-%d \n", i);
                  continue;
               }
               
               TString polyName;
               polyName.Form("pol%d", polDeg[i]);
               fit[i] = new TF1(name, polyName, -1, 1);
               
               for( int j = 0; j <= polDeg[i] ; j++){
                  double aa = atof(sLine.substr(20*j+1, 20).c_str());
                  //printf("(%d, %d), %f \n", i, j, aa);
                  if( j == 0) {
                     fit[i]->SetParameter(j, 0); 
                  }else{
                     fit[i]->SetParameter(j, aa);
                  }
               }
               
            }else{
               fit[i] = new TF1(name, "0");
            }
         
         }
         
         printf("... done.\n");
         
         //for(int i = 0; i < 24; i++){
         //   fit[i]->Print();
         //}
         
      }else{
         printf("... fail.\n");
      }
      file.close();
      
      
      tcut[0][0] = -200 ; tcut[0][1] = 400 ;
      tcut[1][0] = -200 ; tcut[1][1] = 400 ;
      tcut[2][0] = -200 ; tcut[2][1] = 400 ;
      tcut[3][0] = -600 ; tcut[3][1] = 400 ;
      tcut[4][0] =  100 ; tcut[4][1] = 500 ;
      tcut[5][0] = -400 ; tcut[5][1] = 400 ;
      
      tcut[6][0] = -200 ; tcut[6][1] = 400 ;
      tcut[7][0] = -200 ; tcut[7][1] = 400 ;
      tcut[8][0] = -200 ; tcut[8][1] = 400 ;
      tcut[9][0] =  -500 ; tcut[9][1] = 500 ;
      tcut[10][0] =  100 ; tcut[10][1] = 500 ;
      tcut[11][0] = -200 ; tcut[11][1] = -200 ;
      
      tcut[12][0] = -300 ; tcut[12][1] = 400 ;
      tcut[13][0] = -300 ; tcut[13][1] = 400 ;
      tcut[14][0] = -200 ; tcut[14][1] = 400 ;
      tcut[15][0] = -200 ; tcut[15][1] = 400 ;
      tcut[16][0] =  100 ; tcut[16][1] = 500 ;
      tcut[17][0] =  100 ; tcut[17][1] = 800 ;
      
      tcut[18][0] = -200 ; tcut[18][1] = 400 ;
      tcut[19][0] = -300 ; tcut[19][1] = 400 ;
      tcut[20][0] = -200 ; tcut[20][1] = 600 ;
      tcut[21][0] = -300 ; tcut[21][1] = 600 ;
      tcut[22][0] =   50 ; tcut[22][1] = 500 ;
      tcut[23][0] =  200 ; tcut[23][1] = 700 ;
      
   }
   //===================================================== tree branch
   
   newTree->Branch("eventID",&eventID,"eventID/I"); 
   
   newTree->Branch("e" ,  eC, "eC[24]/F");
   newTree->Branch("xf", xfC, "xfC[24]/F");
   newTree->Branch("xn", xnC, "xnC[24]/F");
   newTree->Branch("x" ,   x, "x[24]/F");
   newTree->Branch("z" ,   z, "z[24]/F");
   newTree->Branch("detID", &det, "det/I");
   newTree->Branch("hitID", &hitID, "hitID/I");
   newTree->Branch("zMulti", &zMulti, "zMulti/I");
   
   if( option >= 2 ){
      newTree->Branch("energy" ,  &energy, "energy/F");
      newTree->Branch("Ex", &Ex, "Ex/F");
      newTree->Branch("thetaCM", &thetaCM, "thetaCM/F");
      
      newTree->Branch("e_t", eC_t, "e_t[24]/F");
      newTree->Branch("energy_t" ,  &energy_t, "energy_t/I");
      newTree->Branch("rdt", rdtC, "rdtC[8]/F");
      newTree->Branch("rdt_t", rdtC_t, "rdtC_t[8]/F");
      newTree->Branch("rdt_m", &rdt_m, "rdt_m/I");
   }   
   
   if( option >= 3 ){   
      newTree->Branch("tac_m", &tac_m, "tac_m/I");
      newTree->Branch("tac", tacC, "tacC[6]/F");
      newTree->Branch("tac_t", tacC_t, "tacC_t[6]/F");
      newTree->Branch("dt", dt, "dt[6]/I");
      //newTree->Branch("tt", &tt, "tt/F");
      //newTree->Branch("ttt", &ttt, "ttt/F");
      newTree->Branch("t4", &t4, "t4/F");
      newTree->Branch("good", &good, "good/I");
   }
   
   printf("=========================================================\n");
   
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
