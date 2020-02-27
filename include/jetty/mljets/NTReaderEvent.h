//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Wed Feb 26 15:32:30 2020 by ROOT version 6.18/00
// from TTree tree_event_char/tree_event_char
// found on file: alice_pp.root
//////////////////////////////////////////////////////////

#ifndef JETTY_MLJETS_NTReaderEvent_H
#define JETTY_MLJETS_NTReaderEvent_H

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.
#include "TString.h"

class NTReaderEvent {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

// Fixed size dimensions of array or collections stored in the TTree if any.

   Int_t           current_run_number;
   UInt_t          current_ev_id;

   // Declaration of leaf types
   Float_t         centrality;
   Float_t         z_vtx_reco;
   Int_t           n_vtx_contributors;
   Int_t           n_tracks;
   Int_t           is_ev_rej;
   Int_t           run_number;
   UInt_t          ev_id;
   Int_t           n_tracklets;
   Int_t           V0Amult;
   ULong64_t       trigger_bitmap;
   Bool_t          trigger_online_INT7;
   Bool_t          trigger_online_HighMultSPD;
   Bool_t          trigger_online_HighMultV0;
   Bool_t          trigger_hasbit_INT7;
   Bool_t          trigger_hasbit_HighMultSPD;
   Bool_t          trigger_hasbit_HighMultV0;
   TString         *trigger_classes;
   Bool_t          trigger_hasclass_INT7;
   Bool_t          trigger_hasclass_HighMultSPD;
   Bool_t          trigger_hasclass_HighMultV0;
   Float_t         z_vtx_gen;
   Int_t           n_tracklets_corr;
   Int_t           n_tracklets_corr_shm;
   Int_t           v0m;
   Int_t           v0m_eq;
   Int_t           v0m_corr;
   Int_t           v0m_eq_corr;
   Int_t           mult_gen;
   Int_t           mult_gen_v0a;
   Int_t           mult_gen_v0c;
   Float_t         perc_v0m;

   // List of branches
   TBranch        *b_centrality;   //!
   TBranch        *b_z_vtx_reco;   //!
   TBranch        *b_n_vtx_contributors;   //!
   TBranch        *b_n_tracks;   //!
   TBranch        *b_is_ev_rej;   //!
   TBranch        *b_run_number;   //!
   TBranch        *b_ev_id;   //!
   TBranch        *b_n_tracklets;   //!
   TBranch        *b_V0Amult;   //!
   TBranch        *b_trigger_bitmap;   //!
   TBranch        *b_trigger_online_INT7;   //!
   TBranch        *b_trigger_online_HighMultSPD;   //!
   TBranch        *b_trigger_online_HighMultV0;   //!
   TBranch        *b_trigger_hasbit_INT7;   //!
   TBranch        *b_trigger_hasbit_HighMultSPD;   //!
   TBranch        *b_trigger_hasbit_HighMultV0;   //!
   TBranch        *b_trigger_classes;   //!
   TBranch        *b_trigger_hasclass_INT7;   //!
   TBranch        *b_trigger_hasclass_HighMultSPD;   //!
   TBranch        *b_trigger_hasclass_HighMultV0;   //!
   TBranch        *b_z_vtx_gen;   //!
   TBranch        *b_n_tracklets_corr;   //!
   TBranch        *b_n_tracklets_corr_shm;   //!
   TBranch        *b_v0m;   //!
   TBranch        *b_v0m_eq;   //!
   TBranch        *b_v0m_corr;   //!
   TBranch        *b_v0m_eq_corr;   //!
   TBranch        *b_mult_gen;   //!
   TBranch        *b_mult_gen_v0a;   //!
   TBranch        *b_mult_gen_v0c;   //!
   TBranch        *b_perc_v0m;   //!

   NTReaderEvent(TTree *tree=0);
   virtual ~NTReaderEvent();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef JETTY_MLJETS_NTReaderEvent_cxx
NTReaderEvent::NTReaderEvent(TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("alice_pp.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("alice_pp.root");
      }
      TDirectory * dir = (TDirectory*)f->Get("alice_pp.root:/PWGHF_TreeCreator");
      dir->GetObject("tree_event_char",tree);

   }
   Init(tree);
}

NTReaderEvent::~NTReaderEvent()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t NTReaderEvent::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t NTReaderEvent::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (fChain->GetTreeNumber() != fCurrent) {
      fCurrent = fChain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void NTReaderEvent::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set object pointer
   trigger_classes = 0;
   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("centrality", &centrality, &b_centrality);
   fChain->SetBranchAddress("z_vtx_reco", &z_vtx_reco, &b_z_vtx_reco);
   fChain->SetBranchAddress("n_vtx_contributors", &n_vtx_contributors, &b_n_vtx_contributors);
   fChain->SetBranchAddress("n_tracks", &n_tracks, &b_n_tracks);
   fChain->SetBranchAddress("is_ev_rej", &is_ev_rej, &b_is_ev_rej);
   fChain->SetBranchAddress("run_number", &run_number, &b_run_number);
   fChain->SetBranchAddress("ev_id", &ev_id, &b_ev_id);
   fChain->SetBranchAddress("n_tracklets", &n_tracklets, &b_n_tracklets);
   fChain->SetBranchAddress("V0Amult", &V0Amult, &b_V0Amult);
   fChain->SetBranchAddress("trigger_bitmap", &trigger_bitmap, &b_trigger_bitmap);
   fChain->SetBranchAddress("trigger_online_INT7", &trigger_online_INT7, &b_trigger_online_INT7);
   fChain->SetBranchAddress("trigger_online_HighMultSPD", &trigger_online_HighMultSPD, &b_trigger_online_HighMultSPD);
   fChain->SetBranchAddress("trigger_online_HighMultV0", &trigger_online_HighMultV0, &b_trigger_online_HighMultV0);
   fChain->SetBranchAddress("trigger_hasbit_INT7", &trigger_hasbit_INT7, &b_trigger_hasbit_INT7);
   fChain->SetBranchAddress("trigger_hasbit_HighMultSPD", &trigger_hasbit_HighMultSPD, &b_trigger_hasbit_HighMultSPD);
   fChain->SetBranchAddress("trigger_hasbit_HighMultV0", &trigger_hasbit_HighMultV0, &b_trigger_hasbit_HighMultV0);
   fChain->SetBranchAddress("trigger_classes", &trigger_classes, &b_trigger_classes);
   fChain->SetBranchAddress("trigger_hasclass_INT7", &trigger_hasclass_INT7, &b_trigger_hasclass_INT7);
   fChain->SetBranchAddress("trigger_hasclass_HighMultSPD", &trigger_hasclass_HighMultSPD, &b_trigger_hasclass_HighMultSPD);
   fChain->SetBranchAddress("trigger_hasclass_HighMultV0", &trigger_hasclass_HighMultV0, &b_trigger_hasclass_HighMultV0);
   fChain->SetBranchAddress("z_vtx_gen", &z_vtx_gen, &b_z_vtx_gen);
   fChain->SetBranchAddress("n_tracklets_corr", &n_tracklets_corr, &b_n_tracklets_corr);
   fChain->SetBranchAddress("n_tracklets_corr_shm", &n_tracklets_corr_shm, &b_n_tracklets_corr_shm);
   fChain->SetBranchAddress("v0m", &v0m, &b_v0m);
   fChain->SetBranchAddress("v0m_eq", &v0m_eq, &b_v0m_eq);
   fChain->SetBranchAddress("v0m_corr", &v0m_corr, &b_v0m_corr);
   fChain->SetBranchAddress("v0m_eq_corr", &v0m_eq_corr, &b_v0m_eq_corr);
   fChain->SetBranchAddress("mult_gen", &mult_gen, &b_mult_gen);
   fChain->SetBranchAddress("mult_gen_v0a", &mult_gen_v0a, &b_mult_gen_v0a);
   fChain->SetBranchAddress("mult_gen_v0c", &mult_gen_v0c, &b_mult_gen_v0c);
   fChain->SetBranchAddress("perc_v0m", &perc_v0m, &b_perc_v0m);
   Notify();
}

Bool_t NTReaderEvent::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void NTReaderEvent::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t NTReaderEvent::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef NTReaderEvent_cxx
