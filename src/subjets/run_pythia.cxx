#include <jetty/subjets/run_pythia.h>
#include <jetty/subjets/fjutils.h>

#include <jetty/util/pythia/pyargs.h>
#include <jetty/util/pythia/pyutil.h>
#include <jetty/util/pythia/pythia_wrapper.h>
#include <jetty/util/pythia/crosssections.h>
#include <jetty/util/looputil.h>
#include <jetty/util/blog.h>

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
	PyUtil::Args args(s);
	Linfo << args.asString("[subjets/pythia_run:status]");
	if (args.isSet("--dry")) return 0;

	PyUtil::PythiaWrapper pywrap(args.asString());
	if (pywrap.initialized() == false)
	{
		Lwarn << "Pythia not initialized. Stop here.";
		return 0; // this is a normal termination
	}

	PyUtil::Args &pyargs    = *pywrap.args();
	Pythia8::Pythia &pythia = *pywrap.pythia();
	auto &event             = pythia.event;

	pywrap.outputFile()->cd();
	TH1F *hpT      = new TH1F("hpT", "pT;p_{T} (GeV/#it{c});counts", 100, 0, 100);

	TH2F *hpTzg      = new TH2F("hpTzg", "pTzg;p_{T} (GeV/#it{c});zg;counts", 100, 0, 100, 100, 0, 1);
	TH2F *hpTzgdRcut = new TH2F("hpTzgdRcut", "pTzgdRcut;p_{T} (GeV/#it{c});zg;counts", 100, 0, 100, 100, 0, 1);
	TH2F *hpTz       = new TH2F("hpTz", "pTz;p_{T} (GeV/#it{c});z;counts", 100, 0, 100, 100, 0, 1);
	TH2F *hpTrm      = new TH2F("hpTrm", "pTrm;p_{T} (GeV/#it{c});rm;counts", 100, 0, 100, 100, 0, 0.5);
	TH2F *hpTsjz     = new TH2F("hpTsjz", "pTsjz;p_{T} (GeV/#it{c});sjz;counts", 100, 0, 100, 100, 0, 1);
	TH2F *hpTlsjz    = new TH2F("hpTlsjz", "pTlsjz;p_{T} (GeV/#it{c});lsjz;counts", 100, 0, 100, 100, 0, 1);
	TH2F *hpTslsjz   = new TH2F("hpTslsjz", "pTslsjz;p_{T} (GeV/#it{c});slsjz;counts", 100, 0, 100, 100, 0, 1);
	TH2F *hpTdsj     = new TH2F("hpTdsj", "pTdsj;p_{T} (GeV/#it{c});dsj;counts", 100, 0, 100, 100, 0, 1);

	double R = args.getD("--R", 0.4);
	Linfo << "running with R = " << R;
	double maxEta = args.getD("--eta", 3.);
	Linfo << "running with particle |eta| < " << maxEta;
	double jptcut = args.getD("--jptcut", 5.);
	Linfo << "running with a cut on jet pT > " << jptcut;

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
	Linfo << "Generation done.";

	return 0;
}
