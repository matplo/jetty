#include "run_jets.h"
#include "fjutils.h"

#include "util/pythia/pyargs.h"
#include "util/pythia/pyutil.h"
#include "util/pythia/pythia_wrapper.h"
#include "util/pythia/crosssections.h"
#include "util/pythia/presample.h"

#include "util/looputil.h"
#include "util/blog.h"
#include "util/rstream/tstream.h"

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

int run_jets (const std::string &s)
{
	PyUtil::Args args(s);

	if (args.isSet("--invalid"))
	{
		Linfo << "invalid parameters. stop here.";
		return 0;
	}

	if (args.isSet("--out") == false)
		args.set("--out", "subjets.root");
	Linfo << args.asString("[subjets/pythia_run:status]");
	if (args.isSet("--dry")) return 0;

	PreSamplePythia presample(args.asString());
	if (args.isSet("--presample"))
	{
		presample.run();
	}

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
	TH1F *hpT = new TH1F("hpT", "pT;p_{T} (GeV/#it{c});counts", 100, 0, 100);
	TTree *jt = new TTree("jt", "jt");
	RStream::TStream jts("j", jt);

	double R = args.getD("--R", 0.4);
	Linfo << "running with R = " << R;
	double maxEta = args.getD("--eta", 3.);
	Linfo << "running with particle |eta| < " << maxEta;
	double jptcut = args.getD("--jptcut", 20.);
	Linfo << "running with a cut on jet pT > " << jptcut;
	double sjR = args.getD("--sjR", 0.1);
	Linfo << "running with subjet R = " << sjR;

	// this is where the event loop section starts
	auto nEv = args.getI("Main:numberOfEvents");
	LoopUtil::TPbar pbar(nEv);
	for (unsigned int iE = 0; iE < nEv; iE++)
	{
		pbar.Update();
		if (pywrap.next() == false) continue;

		double wxsec = 1.;
		auto icode = pythia.info.code();
		auto xsec_code = pythia.info.sigmaGen(icode);
		double nsigma = 0;
		double nsigma_code = 0;

		double hard_mean      = 0;
		double hard_std_dev   = 0;
		double hard_n_std_dev = 0;
		if (presample.ready())
		{
			nsigma      = presample.getXSections()->nsigma(0, pythia.info.sigmaGen(0));
			nsigma_code = presample.getXSections()->nsigma(icode, xsec_code);
			wxsec       = presample.getXsections()->xsection_for_code(0);

			hard_mean      = presample.getStatHard()->mean(pythia);
			hard_std_dev   = presample.getStatHard()->std_dev(pythia);
			hard_n_std_dev = presample.getStatHard()->n_std_dev(pythia);
			Ltrace << "presample ready...";
		}
		Ltrace << "xsec = " << wxsec << " nsigma(0) = " << nsigma << " code = " << icode << " nsigma_code = " << nsigma_code;
		Ltrace << "hard: mean = " << hard_mean << " std_dev = " << hard_std_dev << " n_std_dev = " << hard_n_std_dev;

		std::vector<fj::PseudoJet> parts;
		// loop over particles in the event
		for (unsigned int ip = 0; ip < event.size(); ip++)
		{

			if (event[ip].isFinal())
			{
				if (TMath::Abs(event[ip].eta()) < maxEta)
				{
					fj::PseudoJet p(event[ip].px(), event[ip].py(), event[ip].pz(), event[ip].e());
					p.set_user_index(ip);
					parts.push_back(p);
				}
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

				jts << "xsec" << wxsec;
				jts << "icode" << icode;
				jts << "xsec_code" << xsec_code;
				jts << "nsigma" << nsigma;
				jts << "nsigma_code" << nsigma_code;
				jts << "hard_mean" << hard_mean;
				jts << "hard_std_dev" << hard_std_dev;
				jts << "hard_n_std_dev" << hard_n_std_dev;

				hpT->Fill(j.perp());
				jts << "j" << j;

				auto sj_info = new JettyFJUtils::SJInfo(&j, sjR);
				j.set_user_info(sj_info);

				auto sjs = sj_info->subjets();
				jts << "subj" << sjs;

				jts << "nsj" << sjs.size();

				if (sj_info->has_at_least_2_subjets())
				{
					jts << "sj_0" << sjs[0];
					jts << "sj_1" << sjs[1];
					jts << "sj_dR" << sjs[0].delta_R(sjs[1]);
					jts << "sj_dpt" << sjs[0].perp() - sjs[1].perp();
					jts << "sj_delta" << (sjs[0].perp() - sjs[1].perp()) / j.perp();
					jts << "sj_delta_sum" << (sjs[0].perp() - sjs[1].perp()) / (sjs[0].perp() + sjs[1].perp());
					jts << "sj_zg" << TMath::Min(sjs[0].perp(), sjs[1].perp()) / j.perp();
				}
				//auto zs   = sj_info->z();
				jts << "z" << sj_info->z();

				//auto rm  = sj_info->rm();
				jts << "rm" << sj_info->rm();

				//auto zg  = sj_info->sd_zg();
				jts << "sd_zg" << sj_info->sd_zg();
				jts << "sd_dR" << sj_info->sd_dR();
				jts << "sd_mu" << sj_info->sd_mu();
				jts << endl;
			}
		}
	}

	double wxsec = 1.;
	if (presample.ready())
	{
		wxsec = presample.getXsections()->xsection_for_code(0);
	}
	else
	{
		auto xsec = PyUtil::CrossSections(pythia);
		wxsec = xsec.xsection_for_code(0);
	}
	jt->SetWeight(wxsec);
	Linfo << "setting output tree weight:" << wxsec;
	pythia.stat();
	Linfo << "Generation done.";

	return 0;
}
