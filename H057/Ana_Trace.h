//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Fri Jul 20 13:19:29 2018 by ROOT version 5.34/36
// from TTree tree/tree
// found on file: data/run234.root
//////////////////////////////////////////////////////////

#ifndef Ana_Trace_h
#define Ana_Trace_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TSelector.h>
#include <TCanvas.h>
#include <TGraph.h>
#include <TClonesArray.h>
#include <TBenchmark.h>
#include <TMath.h>
#include <TF1.h>

// Header file for the classes stored in the TTree if any.

// Fixed size dimensions of array or collections stored in the TTree if any.

class Ana_Trace : public TSelector {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain

   // Declaration of leaf types
   Int_t           RunNum;
   Int_t           NumHits;
   Short_t         id[71];   //[NumHits]
   UShort_t        tpe[71];   //[NumHits]
   UShort_t        tid[71];   //[NumHits]
   UShort_t        chan_id[71];   //[NumHits]
   UShort_t        board_id[71];   //[NumHits]
   UShort_t        geo_addr[71];   //[NumHits]
   UShort_t        packet_length[71];   //[NumHits]
   UShort_t        header_type[71];   //[NumHits]
   UShort_t        event_type[71];   //[NumHits]
   UShort_t        header_length[71];   //[NumHits]
   ULong64_t       event_timestamp[71];   //[NumHits]
   ULong64_t       last_disc_timestamp[71];   //[NumHits]
   ULong64_t       peak_timestamp[71];   //[NumHits]
   UShort_t        timestamp_match_flag[71];   //[NumHits]
   UShort_t        external_disc_flag[71];   //[NumHits]
   UShort_t        cfd_valid_flag[71];   //[NumHits]
   UShort_t        pileup_only_flag[71];   //[NumHits]
   UShort_t        offset_flag[71];   //[NumHits]
   UShort_t        sync_error_flag[71];   //[NumHits]
   UShort_t        general_error_flag[71];   //[NumHits]
   UShort_t        peak_valid_flag[71];   //[NumHits]
   UShort_t        pileup_flag[71];   //[NumHits]
   Int_t           sampled_baseline[71];   //[NumHits]
   Int_t           cfd_sample_0[71];   //[NumHits]
   Int_t           cfd_sample_1[71];   //[NumHits]
   Int_t           cfd_sample_2[71];   //[NumHits]
   Int_t           pre_rise_energy[71];   //[NumHits]
   Int_t           post_rise_energy[71];   //[NumHits]
   UShort_t        m1_begin_sample[71];   //[NumHits]
   UShort_t        m1_end_sample[71];   //[NumHits]
   UShort_t        m2_begin_sample[71];   //[NumHits]
   UShort_t        m2_end_sample[71];   //[NumHits]
   UShort_t        peak_sample[71];   //[NumHits]
   UShort_t        base_sample[71];   //[NumHits]
   Int_t           baseline[71];   //[NumHits]
   UShort_t        trace_length[71];   //[NumHits]
   Short_t         trace[71][1024];   //[NumHits]

   // List of branches
   TBranch        *b_RunNum;   //!
   TBranch        *b_NumHits;   //!
   TBranch        *b_id;   //!
   TBranch        *b_tpe;   //!
   TBranch        *b_tid;   //!
   TBranch        *b_chan_id;   //!
   TBranch        *b_board_id;   //!
   TBranch        *b_geo_addr;   //!
   TBranch        *b_packet_length;   //!
   TBranch        *b_header_type;   //!
   TBranch        *b_event_type;   //!
   TBranch        *b_header_length;   //!
   TBranch        *b_event_timestamp;   //!
   TBranch        *b_last_disc_timestamp;   //!
   TBranch        *b_peak_timestamp;   //!
   TBranch        *b_timestamp_match_flag;   //!
   TBranch        *b_external_disc_flag;   //!
   TBranch        *b_cfd_valid_flag;   //!
   TBranch        *b_pileup_only_flag;   //!
   TBranch        *b_offset_flag;   //!
   TBranch        *b_sync_error_flag;   //!
   TBranch        *b_general_error_flag;   //!
   TBranch        *b_peak_valid_flag;   //!
   TBranch        *b_pileup_flag;   //!
   TBranch        *b_sampled_baseline;   //!
   TBranch        *b_cfd_sample_0;   //!
   TBranch        *b_cfd_sample_1;   //!
   TBranch        *b_cfd_sample_2;   //!
   TBranch        *b_pre_rise_energy;   //!
   TBranch        *b_post_rise_energy;   //!
   TBranch        *b_m1_begin_sample;   //!
   TBranch        *b_m1_end_sample;   //!
   TBranch        *b_m2_begin_sample;   //!
   TBranch        *b_m2_end_sample;   //!
   TBranch        *b_peak_sample;   //!
   TBranch        *b_base_sample;   //!
   TBranch        *b_baseline;   //!
   TBranch        *b_trace_length;   //!
   TBranch        *b_trace;   //!

   Ana_Trace(TTree * /*tree*/ =0) : fChain(0) { }
   virtual ~Ana_Trace() { }
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

   ClassDef(Ana_Trace,0);
   
   //===========================================
   TFile * saveFile; //!
   TTree * newTree; //!
   int totnumEntry; // of original root
   
   //clock   
   TBenchmark clock;
   Bool_t shown;
   Int_t count;
   
   //tree
   TClonesArray * arr ;//!
   TGraph * gTrace; //!
   
   
   //TClonesArray * arrF ;//!
   TF1 * gFit; //!
   
   double base;
   double energy;
   double time;
   double riseTime;
   
};

#endif

#ifdef Ana_Trace_cxx
void Ana_Trace::Init(TTree *tree)
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

   fChain->SetBranchAddress("RunNum", &RunNum, &b_RunNum);
   fChain->SetBranchAddress("NumHits", &NumHits, &b_NumHits);
   fChain->SetBranchAddress("id", id, &b_id);
   fChain->SetBranchAddress("tpe", tpe, &b_tpe);
   fChain->SetBranchAddress("tid", tid, &b_tid);
   fChain->SetBranchAddress("chan_id", chan_id, &b_chan_id);
   fChain->SetBranchAddress("board_id", board_id, &b_board_id);
   fChain->SetBranchAddress("geo_addr", geo_addr, &b_geo_addr);
   fChain->SetBranchAddress("packet_length", packet_length, &b_packet_length);
   fChain->SetBranchAddress("header_type", header_type, &b_header_type);
   fChain->SetBranchAddress("event_type", event_type, &b_event_type);
   fChain->SetBranchAddress("header_length", header_length, &b_header_length);
   fChain->SetBranchAddress("event_timestamp", event_timestamp, &b_event_timestamp);
   fChain->SetBranchAddress("last_disc_timestamp", last_disc_timestamp, &b_last_disc_timestamp);
   fChain->SetBranchAddress("peak_timestamp", peak_timestamp, &b_peak_timestamp);
   fChain->SetBranchAddress("timestamp_match_flag", timestamp_match_flag, &b_timestamp_match_flag);
   fChain->SetBranchAddress("external_disc_flag", external_disc_flag, &b_external_disc_flag);
   fChain->SetBranchAddress("cfd_valid_flag", cfd_valid_flag, &b_cfd_valid_flag);
   fChain->SetBranchAddress("pileup_only_flag", pileup_only_flag, &b_pileup_only_flag);
   fChain->SetBranchAddress("offset_flag", offset_flag, &b_offset_flag);
   fChain->SetBranchAddress("sync_error_flag", sync_error_flag, &b_sync_error_flag);
   fChain->SetBranchAddress("general_error_flag", general_error_flag, &b_general_error_flag);
   fChain->SetBranchAddress("peak_valid_flag", peak_valid_flag, &b_peak_valid_flag);
   fChain->SetBranchAddress("pileup_flag", pileup_flag, &b_pileup_flag);
   fChain->SetBranchAddress("sampled_baseline", sampled_baseline, &b_sampled_baseline);
   fChain->SetBranchAddress("cfd_sample_0", cfd_sample_0, &b_cfd_sample_0);
   fChain->SetBranchAddress("cfd_sample_1", cfd_sample_1, &b_cfd_sample_1);
   fChain->SetBranchAddress("cfd_sample_2", cfd_sample_2, &b_cfd_sample_2);
   fChain->SetBranchAddress("pre_rise_energy", pre_rise_energy, &b_pre_rise_energy);
   fChain->SetBranchAddress("post_rise_energy", post_rise_energy, &b_post_rise_energy);
   fChain->SetBranchAddress("m1_begin_sample", m1_begin_sample, &b_m1_begin_sample);
   fChain->SetBranchAddress("m1_end_sample", m1_end_sample, &b_m1_end_sample);
   fChain->SetBranchAddress("m2_begin_sample", m2_begin_sample, &b_m2_begin_sample);
   fChain->SetBranchAddress("m2_end_sample", m2_end_sample, &b_m2_end_sample);
   fChain->SetBranchAddress("peak_sample", peak_sample, &b_peak_sample);
   fChain->SetBranchAddress("base_sample", base_sample, &b_base_sample);
   fChain->SetBranchAddress("baseline", baseline, &b_baseline);
   fChain->SetBranchAddress("trace_length", trace_length, &b_trace_length);
   fChain->SetBranchAddress("trace", trace, &b_trace);
   
   //============================================
   totnumEntry = tree->GetEntries();
   printf( "========== Make a new tree with calibration, total Entry : %d \n", totnumEntry);

   saveFile = new TFile( "trace.root","recreate");
   newTree =  new TTree("tree","tree");

   arr = new TClonesArray("TGraph");
   newTree->Branch("trace", arr, 256000);
   arr->BypassStreamer();
   
	gFit = new TF1("gFit", "[0]/(1+TMath::Exp(-(x-[1])/[2]))+[3]", 0, 100);
   
   newTree->Branch("base", &base, "base/D");
   newTree->Branch("energy", &energy, "energy/D");
   newTree->Branch("time", &time, "time/D");
   newTree->Branch("riseTime", &riseTime, "riseTime/D");

   clock.Reset();
   clock.Start("timer");
   shown = 0;
   count = 0;

}

Bool_t Ana_Trace::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

#endif // #ifdef Ana_Trace_cxx
