//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Thu Feb 13 16:35:33 2020 by ROOT version 6.18/04
// from TTree tree_Particle/tree_Particle
// found on file: alice_pp.root
//////////////////////////////////////////////////////////

#ifndef JETTY_MLJETS_NTReader_H
#define JETTY_MLJETS_NTReader_H

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.

class NTReader {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

// Fixed size dimensions of array or collections stored in the TTree if any.

   // Declaration of leaf types
   Int_t           run_number;
   UInt_t          ev_id;
   Float_t         ParticlePt;
   Float_t         ParticleEta;
   Float_t         ParticlePhi;

   // List of branches
   TBranch        *b_run_number;   //!
   TBranch        *b_ev_id;   //!
   TBranch        *b_ParticlePt;   //!
   TBranch        *b_ParticleEta;   //!
   TBranch        *b_ParticlePhi;   //!

   NTReader(TTree *tree=0);
   virtual ~NTReader();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef JETTY_MLJETS_NTReader_cxx
NTReader::NTReader(TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("alice_pp.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("alice_pp.root");
      }
      TDirectory * dir = (TDirectory*)f->Get("alice_pp.root:/PWGHF_TreeCreator");
      dir->GetObject("tree_Particle",tree);

   }
   Init(tree);
}

NTReader::~NTReader()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t NTReader::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t NTReader::LoadTree(Long64_t entry)
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

void NTReader::Init(TTree *tree)
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
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("run_number", &run_number, &b_run_number);
   fChain->SetBranchAddress("ev_id", &ev_id, &b_ev_id);
   fChain->SetBranchAddress("ParticlePt", &ParticlePt, &b_ParticlePt);
   fChain->SetBranchAddress("ParticleEta", &ParticleEta, &b_ParticleEta);
   fChain->SetBranchAddress("ParticlePhi", &ParticlePhi, &b_ParticlePhi);
   Notify();
}

Bool_t NTReader::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void NTReader::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t NTReader::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef NTReader_cxx
