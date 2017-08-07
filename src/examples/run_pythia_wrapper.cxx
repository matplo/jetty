#include "run_pythia_wrapper.h"

#include "util/pythia/pythia_wrapper.h"
#include "util/pythia/pyargs.h"
#include "util/pythia/pyutil.h"
#include "util/pythia/crosssections.h"
#include "util/pythia/outkinematics.h"

#include "util/looputil.h"
#include "util/blog.h"

#include "util/rstream/hstream.h"

#include <TFile.h>
#include <TTree.h>
#include <TH1F.h>
#include <TMath.h>

#include <string>
#include <iostream>

using namespace std;

int run_pythia_wrapper (const std::string &s)
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
		}

		if (jets_flag)
		{
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
				}
			}
		}
	} // end event loop
	hstream.Scale(pythia.info.sigmaGen() / pythia.info.weightSum());
	pythia.stat();
	Linfo << "Generation done.";

	return 0;
}
