#include <jetty/spectra/spectra.h>

#include <jetty/util/pythia/pythia_wrapper.h>
#include <jetty/util/pythia/pyargs.h>
#include <jetty/util/pythia/pyutil.h>
#include <jetty/util/pythia/crosssections.h>
#include <jetty/util/pythia/outkinematics.h>

#include <jetty/util/looputil.h>
#include <jetty/util/blog.h>

#include <jetty/util/rstream/hstream.h>

#include <TFile.h>
#include <TTree.h>
#include <TH1F.h>
#include <TMath.h>
#include <TNtuple.h>
#include <TVector3.h>

#include <string>
#include <iostream>

#include <fastjet/PseudoJet.hh>
#include <fastjet/ClusterSequence.hh>
namespace fj = fastjet;

using namespace std;

int run_spectra (const std::string &s)
{
	PyUtil::Args args(s);
	if (args.isSet("--invalid"))
	{
		Linfo << "invalid parameters. stop here.";
		return 0;
	}
	Linfo << args.asString("[pythia_run_wrapper:status]");

	PyUtil::PythiaWrapper pywrap(args.asString());
	if (!pywrap.initialized())
	{
		Lwarn << "PYTHIA not initialized. Stop Here.";
		return 1;
	}
	PyUtil::Args &pyargs      = *pywrap.args();
	Pythia8::Pythia &pythia = *pywrap.pythia();
	auto &event             = pythia.event;

	pywrap.outputFile()->cd();
	TTree *tk = new TTree("kine", "kine");
	TH1F *hpT = new TH1F("hpT", "pT;p_{T} (GeV/#it{c});counts", 50, 0, 100);
	TNtuple *nt_parts = new TNtuple("nt_parts", "nt_parts", "pT:y:eta:phi");
	TNtuple *nt_gamma = new TNtuple("nt_gamma", "nt_gamma", "pT:y:eta:phi");
	TNtuple *nt_z0 = new TNtuple("nt_z0", "nt_z0", "pT:y:eta:phi");
	TNtuple *nt_jets = new TNtuple("nt_jets", "nt_jets", "pT:y:eta:phi");

	RStream::HStream hstream;
	if (args.isSet("--hconfig"))
	{
		hstream.Init("h", args.get("--hconfig").c_str(), true, pywrap.outputFile());
		hstream.SkipUndefined(!args.isSet("--trace"));
	}

	bool photons_flag = args.isSet("--photons");
	bool z0_flag = args.isSet("--z0");
	bool jets_flag = !(photons_flag && z0_flag);

	Linfo << "jets flag   : " << jets_flag;
	Linfo << "photon flag : " << photons_flag;
	Linfo << "z0 flag     : " << z0_flag;

	double R = args.getD("--R", 0.4);
	double maxEta = args.getD("--maxeta", 3. + R);
	double jptcut = args.getD("--jptcut", 20.);

	if (jets_flag)
	{
		Linfo << "jets configuration";
		Linfo << "running with R = " << R;
		Linfo << "running with particle max |eta| < " << maxEta;
		Linfo << "running with jet max |eta| < " << maxEta - R;
		Linfo << "running with a cut on jet pT > " << jptcut;
	}

	if (args.get("PhaseSpace:bias2Selection", "off") == "on")
	{
		Lwarn << "PhaseSpace:bias2Selection";
		args.set("PhaseSpace:bias2SelectionPow", 4.);  // default pythia settings
        args.set("PhaseSpace:bias2SelectionRef", 20.); // 100 for pThat min = 0 (?)
	}

	// this is where the event loop section starts
	auto nEv = pyargs.getI("Main:numberOfEvents");
	LoopUtil::TPbar pbar(nEv);
	for (unsigned int iE = 0; iE < nEv; iE++)
	{
		pbar.Update();
		if (pywrap.next() == false) continue;

		// some kinematics info
		*tk << PyUtil::OutKinematics(pythia);

		if (photons_flag)
		{
			auto photons = PyUtil::prompt_photon_indexes(event);
			for (auto &ipho : photons)
			{
				if (TMath::Abs(event[ipho].eta()) < maxEta)
				{
					if (event[ipho].pT() > 0.5)
						nt_gamma->Fill(event[ipho].pT(), event[ipho].y(), event[ipho].eta(), event[ipho].phi());
				}
				if (TMath::Abs(event[ipho].eta()) < 1.)
					hstream << "photon_pt_cms" << event[ipho].pT();
				if (TMath::Abs(event[ipho].eta()) < 0.35)
					hstream << "photon_pt_phenix" << event[ipho].pT();
			}
		}

		if (z0_flag)
		{
			auto iZ0 = PyUtil::z0_index(event);
			hstream << "z0_pt_atlas" << event[iZ0].pT();
			if (TMath::Abs(event[iZ0].eta()) < maxEta)
			{
				if (event[iZ0].pT() > 0.5)
					nt_z0->Fill(event[iZ0].pT(), event[iZ0].y(), event[iZ0].eta(), event[iZ0].phi());
			}
		}

		if (jets_flag)
		{
			std::vector<fj::PseudoJet> parts;
			// loop over particles in the event
			for (unsigned int ip = 0; ip < event.size(); ip++)
			{
				if (event[ip].isFinal())
				{
					if (TMath::Abs(event[ip].eta()) < 1.)
						hpT->Fill(event[ip].pT(), 1./event[ip].pT());
					hstream << "part_" << event[ip];
					if (TMath::Abs(event[ip].eta()) < 1.)
						hstream << "part_pt_cms" << event[ip].pT();
					if (TMath::Abs(event[ip].eta()) < 0.35)
						hstream << "part_pt_phenix" << event[ip].pT();
					//hstream << "undefined_" << event[ip];
					double pt_eta[] = {event[ip].pT(), event[ip].eta()};
					hstream << "part_pt_eta" << pt_eta;
					double pt_rap[] = {event[ip].pT(), event[ip].y()};
					hstream << "part_pt_rap" << pt_rap;

					if (TMath::Abs(event[ip].eta()) < maxEta)
					{
						if (event[ip].pT() > 0.5)
							nt_parts->Fill(event[ip].pT(), event[ip].y(), event[ip].eta(), event[ip].phi());
						fj::PseudoJet p(event[ip].px(), event[ip].py(), event[ip].pz(), event[ip].e());
						p.set_user_index(ip);
						parts.push_back(p);
					}
				}
			} // particle loop

			fj::JetDefinition jet_def(fj::antikt_algorithm, R);
			fj::ClusterSequence cs(parts, jet_def);
			auto jets = fj::sorted_by_pt(cs.inclusive_jets());
			for (auto j : jets)
			{
				if (j.perp() < jptcut)
					continue;
				if (TMath::Abs(j.eta()) > maxEta - R)
					continue;
				nt_jets->Fill(j.pt(), j.rap(), j.eta(), j.phi());
				double pt_eta[] = {j.pt(), j.eta()};
				hstream << "jet_pt_eta" << pt_eta;
				hstream << "jet_pt_atlas" << j.pt();
				hstream << "jet_pt" << j.pt();
			} // jets loop

		}
	} // end event loop
	double _scale = pythia.info.sigmaGen() / pythia.info.weightSum();
	Ltrace << "Scaling with " << _scale;
	hstream.Scale(_scale);
	pythia.stat();
	Linfo << "Generation done.";

	return 0;
}
