#include "run_pythia.h"
#include "fjutils.h"

#include <util/pyargs.h>
#include <util/pyutil.h>
#include <util/looputil.h>

#include <Pythia8/Pythia.h>

#include <fastjet/PseudoJet.hh>
#include <fastjet/ClusterSequence.hh>
namespace fj = fastjet;

#include <TFile.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TMath.h>

#include <string>
#include <iostream>

using namespace std;

int run_pythia (const std::string &s)
{
	cout << "[i] this is run_pythia subjets" << endl;
	PyUtil::Args args(s);
	cout << args.asString("[pythia_run_wrapper:status]") << endl;
	if (args.isSet("--dry")) return 0;

	// create the output root file
	string outfname = args.get("--output");
	if (outfname.size() < 1)
	{
		outfname = "default_output.root";
	}
	TFile *fout = TFile::Open(outfname.c_str(), "RECREATE");
	fout->cd();

	TH1F *hpT      = new TH1F("hpT", "pT;p_{T} (GeV/#it{c});counts", 100, 0, 100);

	TH2F *hpTzg      = new TH2F("hpTzg", "pTzg;p_{T} (GeV/#it{c});zg;counts", 100, 0, 100, 100, 0, 1);
	TH2F *hpTzgdRcut = new TH2F("hpTzgdRcut", "pTzgdRcut;p_{T} (GeV/#it{c});zg;counts", 100, 0, 100, 100, 0, 1);
	TH2F *hpTz       = new TH2F("hpTz", "pTz;p_{T} (GeV/#it{c});z;counts", 100, 0, 100, 100, 0, 1);
	TH2F *hpTrm      = new TH2F("hpTrm", "pTrm;p_{T} (GeV/#it{c});rm;counts", 100, 0, 100, 100, 0, 0.5);
	TH2F *hpTsjz     = new TH2F("hpTsjz", "pTsjz;p_{T} (GeV/#it{c});sjz;counts", 100, 0, 100, 100, 0, 1);
	TH2F *hpTlsjz    = new TH2F("hpTlsjz", "pTlsjz;p_{T} (GeV/#it{c});lsjz;counts", 100, 0, 100, 100, 0, 1);
	TH2F *hpTslsjz   = new TH2F("hpTslsjz", "pTslsjz;p_{T} (GeV/#it{c});slsjz;counts", 100, 0, 100, 100, 0, 1);
	TH2F *hpTdsj     = new TH2F("hpTdsj", "pTdsj;p_{T} (GeV/#it{c});dsj;counts", 100, 0, 100, 100, 0, 1);

	// initialize pythia with a config and command line args
	Pythia8::Pythia *ppythia = PyUtil::make_pythia(args.asString());
	if (!ppythia)
	{
		cerr << "[e] no pythia - no fun." << endl;
		return 1;
	}
	Pythia8::Pythia &pythia  = *ppythia;
	auto &event              = pythia.event;

	double R = args.getD("--R", 0.4);
	cout << "[i] running with R = " << R << endl;
	double maxEta = args.getD("--eta", 3.);
	cout << "[i] running with particle |eta| < " << maxEta << endl;
	double jptcut = args.getD("--jptcut", 5.);
	cout << "[i] running with a cut on jet pT > " << jptcut << endl;

	// this is where the event loop section starts
	auto nEv = args.getI("Main:numberOfEvents");
	LoopUtil::TPbar pbar(nEv);
	for (unsigned int iE = 0; iE < nEv; iE++)
	{
		pbar.Update();
		if (pythia.next() == false) continue;

		std::vector<fj::PseudoJet> parts;
		// loop over particles in the event
		for (unsigned int ip = 0; ip < event.size(); ip++)
		{
			if (event[ip].isFinal())
				if (TMath::Abs(event[ip].eta()) < maxEta)
				{
					fj::PseudoJet p(event[ip].px(), event[ip].py(), event[ip].pz(), event[ip].e());
					p.set_user_index(ip);
					parts.push_back(p);
				}

			fj::JetDefinition jet_def(fj::antikt_algorithm, R);
			fj::ClusterSequence cs(parts, jet_def);
			auto jets = fj::sorted_by_pt(cs.inclusive_jets());
			for (auto j : jets)
			{
				if (j.perp() < jptcut)
					continue;
				if (TMath::Abs(j.eta()) > maxEta - R)
					continue;

				hpT->Fill(j.perp());

				auto sj_info = new JettyFJUtils::SJInfo(&j);
				j.set_user_info(sj_info);

				auto sjs = sj_info->subjets();
				for (auto sj : sjs)
					hpTsjz->Fill(j.perp(), sj.perp() / j.perp());

				if (sj_info->has_at_least_2_subjets())
				{
					hpTlsjz->Fill(j.perp(), sjs[0].perp() / j.perp());
					hpTslsjz->Fill(j.perp(), sjs[1].perp() / j.perp());
					hpTdsj->Fill(j.perp(), (sjs[0].perp() - sjs[1].perp()) / j.perp());
				}

				auto zs   = sj_info->z();
				for (auto z : zs)
					hpTz->Fill(j.perp(), z);

				auto rm  = sj_info->rm();
				hpTrm->Fill(j.perp(), rm);

				auto zg  = sj_info->sd_zg();
				hpTzg->Fill(j.perp(), zg);
				if (sj_info->sd_dR() > 0.1)
					hpTzgdRcut->Fill(j.perp(), zg);
			}
		}
	}
	pythia.stat();
	cout << "[i] Generation done." << endl;

	// remember to properly save/update and close the output file
	fout->Write();
	fout->Close();
	delete fout;

	string xsec_outfname = outfname + ".txt";
	PyUtil::CrossSections(pythia, xsec_outfname.c_str());

	// delete the pythia
	delete ppythia;
	return 0;
}
