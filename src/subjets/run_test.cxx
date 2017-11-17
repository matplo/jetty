#include <jetty/subjets/run_test.h>

#include <jetty/util/pythia/pyargs.h>
#include <jetty/util/pythia/pyutil.h>
#include <jetty/util/pythia/pythia_wrapper.h>
#include <jetty/util/pythia/crosssections.h>

#include <jetty/util/looputil.h>
#include <jetty/util/blog.h>
#include <jetty/util/rstream/tstream.h>

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

int run_test (const std::string &s)
{
	PyUtil::Args args(s);

	if (args.isSet("--invalid"))
	{
		Linfo << "invalid parameters. stop here.";
		return 0;
	}

	if (args.isSet("--out") == false)
		args.set("--out", "subjets_test.root");
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
	TH1F *hpT = new TH1F("hpT", "pT;p_{T} (GeV/#it{c});counts", 100, 0, 1000);
	Double_t fbins[] = {5.000E-03, 1.000E-02, 3.000E-02, 5.000E-02, 8.000E-02, 1.000E-01, 2.000E-01, 3.000E-01, 4.000E-01, 6.000E-01, 8.000E-01, 9.000E-01, 9.500E-01, 1};
	TH1D *hz = new TH1D("hz", "hz;z;dN/dz", 13, fbins);
	TH1F *he = new TH1F("he", "E;E (GeV);counts", 100, 0, 1000);
	TH1D *hze = new TH1D("hze", "hze;z;dN/dz", 13, fbins);

	TTree *jt = new TTree("jt", "jt");
	RStream::TStream jts("j", jt);

	double R = args.getD("--R", 0.4);
	Linfo << "running with R = " << R;
	double maxEta = args.getD("--eta", 3.);
	Linfo << "running with particle |eta| < " << maxEta;
	double jptcut = args.getD("--jptcut", 20.);
	Linfo << "running with a cut on jet pT > " << jptcut;
	double jptcutmax = args.getD("--jptcutmax", 2000.);
	Linfo << "running with a cut on jet pT < " << jptcutmax;
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

		std::vector<fj::PseudoJet> parts;
		// loop over particles in the event
		for (unsigned int ip = 0; ip < event.size(); ip++)
		{

			if (event[ip].isFinal() && event[ip].isDiquark() == false)
			{
				if (TMath::Abs(event[ip].eta()) < maxEta)
				{
					fj::PseudoJet p(event[ip].px(), event[ip].py(), event[ip].pz(), event[ip].e());
					p.set_user_index(ip);
					parts.push_back(p);
				}
			}
		} // end of the particle loop

		fj::JetDefinition jet_def(fj::antikt_algorithm, R);
		fj::ClusterSequence cs(parts, jet_def);
		auto jets = fj::sorted_by_pt(cs.inclusive_jets());
		for (auto j : jets)
		{
			if (TMath::Abs(j.eta()) > maxEta - R)
				continue;

			if (j.perp() < jptcut)
				continue;
			if (j.perp() > jptcutmax)
				continue;

			jts << "xsec" << wxsec;
			jts << "icode" << icode;
			jts << "xsec_code" << xsec_code;

			hpT->Fill(j.perp());
			he->Fill(j.e());
			jts << "j" << j;

			// auto sj_info = new JettyFJUtils::SJInfo(&j, sjR);
			// j.set_user_info(sj_info);

			fj::JetDefinition jet_def_sj(fj::antikt_algorithm, sjR);
			fj::ClusterSequence cs_sj(j.constituents(), jet_def_sj);
			auto sjs = fj::sorted_by_pt(cs_sj.inclusive_jets());

			//auto sjs = sj_info->subjets();
			jts << "subj" << sjs;
			for (auto sj : sjs)
			{
				hz->Fill(sj.perp() / j.perp());
				hze->Fill(sj.e() / j.e());
			}

			jts << "nsj" << sjs.size();

			jts << endl;
		} // end of the jet loop
	} // end of the event loop

	if (hpT->GetEntries() > 0)
	{
		hz->Scale(1./hpT->GetEntries(), "width");
		hze->Scale(1./he->GetEntries(), "width");
	}

	auto xsec = PyUtil::CrossSections(pythia);
	double wxsec = xsec.xsection_for_code(0);

	jt->SetWeight(wxsec);
	Linfo << "setting output tree weight:" << wxsec;
	pythia.stat();
	Linfo << "Generation done.";

	return 0;
}
