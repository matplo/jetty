#include <jetty/subjets/run_hepmc.h>
#include <jetty/subjets/fjutils.h>

#include <jetty/util/args.h>
#include <jetty/util/looputil.h>
#include <jetty/util/blog.h>
#include <jetty/util/rstream/tstream.h>
#include <jetty/util/hepmc/readfile.h>

#include <fastjet/PseudoJet.hh>
#include <fastjet/ClusterSequence.hh>
namespace fj = fastjet;

#include <TTree.h>
#include <TFile.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TMath.h>

#include <string>
#include <iostream>

using namespace std;

int run_hepmc (const std::string &s)
{
	SysUtil::Args args(s);

	if (args.isSet("--invalid"))
	{
		Linfo << "invalid parameters. stop here.";
		return 0;
	}

	if (args.isSet("--out") == false)
		args.set("--out", "subjets_hepmc.root");
	Linfo << args.asString("[subjets/pythia_run:status]");
	if (args.isSet("--dry")) return 0;

	TFile *fout = new TFile(args.get("--out").c_str(), "recreate");
	fout->cd();
	TH1F *hpT = new TH1F("hpT", "pT;p_{T} (GeV/#it{c});counts", 100, 0, 1000);
	Double_t fbins[] = {5.000E-03, 1.000E-02, 3.000E-02, 5.000E-02, 8.000E-02, 1.000E-01, 2.000E-01, 3.000E-01, 4.000E-01, 6.000E-01, 8.000E-01, 9.000E-01, 9.500E-01, 1};
	TH1D *hz = new TH1D("hz", "hz;z;dN/dz", 13, fbins);
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
	auto nEv = args.getI("--nev", 0);

	LoopUtil::TPbar pbar(nEv);
	GenUtil::ReadHepMCFile f(args.get("--input").c_str());
	while (f.NextEvent())
	{
		if (nEv > 0)
			if (pbar.NCalls() >= nEv) break;

		pbar.Update();

		// get particles from the HEPMC file - true == final only
		std::vector<fj::PseudoJet> parts = f.PseudoJetParticles(true);

		fj::JetDefinition jet_def(fj::antikt_algorithm, R);
		fj::ClusterSequence cs(parts, jet_def);
		auto jets = fj::sorted_by_pt(cs.inclusive_jets());
		for (auto j : jets)
		{
			if (j.perp() < jptcut)
				continue;
			if (j.perp() > jptcutmax)
				continue;
			if (TMath::Abs(j.eta()) > maxEta - R)
				continue;

			hpT->Fill(j.perp());
			jts << "j" << j;

			auto sj_info = new JettyFJUtils::SJInfo(&j, sjR);
			j.set_user_info(sj_info);

			auto sjs = sj_info->subjets();
			jts << "subj" << sjs;
			for (auto sj : sjs)
				hz->Fill(sj.perp() / j.perp());

			jts << "nsj" << sjs.size();

			if (sjs.size() < 1)
			{
				sjs.push_back(j);
			}
			if (sjs.size() < 2)
			{
				sjs.push_back(fj::PseudoJet(0,0,0,0));
			}

			jts << "sj_0" << sjs[0];
			jts << "sj_1" << sjs[1];
			jts << "sj_dR" << sjs[0].delta_R(sjs[1]);
			jts << "sj_dpt" << sjs[0].perp() - sjs[1].perp();
			jts << "sj_delta" << (sjs[0].perp() - sjs[1].perp()) / j.perp();
			jts << "sj_delta_sum" << (sjs[0].perp() - sjs[1].perp()) / (sjs[0].perp() + sjs[1].perp());
			jts << "sj_zg" << TMath::Min(sjs[0].perp(), sjs[1].perp()) / j.perp();
			//auto zs   = sj_info->z();
			jts << "z" << sj_info->z();

			//auto rm  = sj_info->rm();
			jts << "rm" << sj_info->rm();

			//auto zg  = sj_info->sd_zg();
			jts << "sd_zg" << sj_info->sd_zg();
			jts << "sd_dR" << sj_info->sd_dR();
			jts << "sd_mu" << sj_info->sd_mu();
			jts << endl;

			// delete sj_info; deleted when deleting jets
		} //end of the jet loop
	} // end of the event loop

	if (hpT->GetEntries() > 0)
		hz->Scale(1./hpT->GetEntries(), "width");

	//jt->SetWeight(wxsec);
	//Linfo << "setting output tree weight:" << wxsec;

	fout->Write();
	fout->Close();
	delete fout;
	Linfo << "Done.";

	return 0;
}
