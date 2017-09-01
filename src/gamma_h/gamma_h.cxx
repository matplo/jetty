#include "gamma_h.h"

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
#include <TNtuple.h>
#include <TVector3.h>

#include <string>
#include <iostream>

using namespace std;

int run_gamma_h (const std::string &s)
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
	TNtuple *gntuple = new TNtuple("gntuple", "gntuple", "gpT:geta:gphi");
	TNtuple *ghntuple = new TNtuple("ghntuple", "ghntuple", "gpT:geta:gphi:hpT:heta:hphi:dphi");

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

	Stat_t nPhotons = 0;

	// this is where the event loop section starts
	auto nEv = pyargs.getI("Main:numberOfEvents");
	LoopUtil::TPbar pbar(nEv);
	for (unsigned int iE = 0; iE < nEv; iE++)
	{
		pbar.Update();
		if (pywrap.next() == false) continue;

		// some kinematics info
		*tk << PyUtil::OutKinematics(pythia);

		auto photons = PyUtil::prompt_photon_indexes(event);
		for (auto &ipho : photons)
		{
			TVector3 photon;
			photon.SetPtEtaPhi (event[ipho].pT(), event[ipho].eta(), event[ipho].phi());
			gntuple->Fill(event[ipho].pT(), event[ipho].eta(), event[ipho].phi());
			for (unsigned int ip = 0; ip < event.size(); ip++)
			{
				if (event[ip].isFinal())
				{
					if (event[ip].isCharged())
					{
						TVector3 particle;
						particle.SetPtEtaPhi (event[ip].pT(), event[ip].eta(), event[ip].phi());
						double dphi = TMath::Abs(photon.DeltaPhi(particle));
						ghntuple->Fill(event[ipho].pT(), event[ipho].eta(), event[ipho].phi(), event[ip].pT(), event[ip].eta(), event[ip].phi(), dphi);
					} // if charged
				} // if final
			} // loop over particles
		} // for all prompt photons
	} // end event loop
	double _scale = pythia.info.sigmaGen() / pythia.info.weightSum();
	Ltrace << "Scaling with " << _scale;
	// hstream.Scale(_scale);
	// gntuple->SetWeight(_scale);
	pythia.stat();
	Linfo << "Generation done.";

	return 0;
}
