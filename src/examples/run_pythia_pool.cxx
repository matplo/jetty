#include "run_pythia_pool.h"

#include "util/pythia/pythia_pool.h"
#include "util/pythia/pyargs.h"
#include "util/pythia/pyutil.h"

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

int run_pythia_pool (const std::string &s)
{
	PyUtil::Args args(s);
	if (args.isSet("--invalid"))
	{
		Linfo << "invalid parameters. stop here.";
		return 0;
	}
	Linfo << args.asString("[pythia_run_pool:status]");

	bool photons_flag = args.isSet("--photons");
	bool z0_flag = args.isSet("--z0");
	bool jets_flag = args.isSet("--jets"); //!(photons_flag && z0_flag);
	Linfo << "jets flag   : " << jets_flag;
	Linfo << "photon flag : " << photons_flag;
	Linfo << "z0 flag     : " << z0_flag;

	PyUtil::PythiaPool &pypool = PyUtil::PythiaPool::Instance();
	pypool.SetCommonSettings(args.asString().c_str());

	auto nEv = args.getI("Main:numberOfEvents", 10);
	LoopUtil::TPbar pbar(nEv);
	for (unsigned int iE = 0; iE < nEv; iE++)
	{
		pbar.Update();
		auto ppythia = pypool.GetPythia(1000.+iE, 1000.-iE);
		if (!ppythia) break;
		Pythia8::Pythia &pythia = *ppythia;
		auto &event             = pythia.event;
		if (pythia.next() == false) continue;

		if (photons_flag)
		{
			auto photons = PyUtil::prompt_photon_indexes(event);
			for (auto &ipho : photons)
			{
				if (TMath::Abs(event[ipho].eta()) < 1.)
					Linfo << "photon_pt_cms" << event[ipho].pT();
				if (TMath::Abs(event[ipho].eta()) < 0.35)
					Linfo << "photon_pt_phenix" << event[ipho].pT();
			}
		}

		if (z0_flag)
		{
			auto iZ0 = PyUtil::z0_index(event);
			Linfo << "z0_pt_atlas" << event[iZ0].pT();
		}

		if (jets_flag)
		{
			// loop over particles in the event
			for (unsigned int ip = 0; ip < event.size(); ip++)
			{
				if (event[ip].isFinal())
				{
					// Linfo << "part_" << event[ip];
					if (TMath::Abs(event[ip].eta()) < 1.)
						Linfo << "part_pt_cms" << event[ip].pT();
					if (TMath::Abs(event[ip].eta()) < 0.35)
						Linfo << "part_pt_phenix" << event[ip].pT();
				}
			}
		}
	} // end event loop
	Linfo << "Generation done.";

	return 0;
}
