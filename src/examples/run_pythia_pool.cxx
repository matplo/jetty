#include <jetty/examples/run_pythia_pool.h>

#include <jetty/util/pythia/pythia_pool.h>
#include <jetty/util/pythia/event_pool.h>
#include <jetty/util/pythia/pyargs.h>
#include <jetty/util/pythia/pyutil.h>
#include <jetty/util/pythia/param_sigmas.h>

#include <jetty/util/looputil.h>
#include <jetty/util/blog.h>

#include <jetty/util/rstream/hstream.h>

#include <TFile.h>
#include <TTree.h>
#include <TH1F.h>
#include <TF1.h>
#include <TMath.h>
#include <TRandom3.h>

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

	double eA = args.getD("Beams:eA", 5000.);
	double eB = args.getD("Beams:eB", 5000.);
	args.set("Beams:eA", eA);
	args.set("Beams:eB", eB);

	PyUtil::PythiaPool &pypool = PyUtil::PythiaPool::Instance();
	pypool.SetCommonSettings(args.asString().c_str());
	pypool.SetupECMs(eA, eB, args.getD("--ndiv", 50));

	//TRandom3 rndm;
	TF1 fgaus("fgaus", "gaus", 0, 1.);
	fgaus.SetParameter(0, 1.);
	fgaus.SetParameter(1, 1.);
	fgaus.SetParameter(2, args.getD("--sigma-eloss", 0.1));

	bool fixed_energy_flag = args.isSet("--fixed-energy");

	auto nEv = args.getI("Main:numberOfEvents", 10);
	if (nEv == 0) nEv = args.getI("--nev", 1);
	args.set("Main:numberOfEvents", nEv);

	Linfo << args.asString("[pythia_run_pool:status]");

	PyUtil::EventPool ev_pool;

	LoopUtil::TPbar pbar(nEv);
	for (int iE = 0; iE < nEv; iE++)
	{
		pbar.Update();
		double rndmA = fgaus.GetRandom() * eA ;
		double rndmB = fgaus.GetRandom() * eB ;
		if (fixed_energy_flag)
		{
			rndmA = eA;
			rndmB = eB;
		}
		if (rndmA < 10 || rndmB < 10)
		{
			Lwarn << "too small nucleon energy A=" << rndmA << " : B=" << rndmB;
			continue;
		}

		auto ppythia = pypool.GetPythia(rndmA, rndmB);
		if (!ppythia) break;
		Pythia8::Pythia &pythia = *ppythia;
		auto &event             = pythia.event;
		if (pythia.next() == false) continue;

		Linfo << "-";
		PyUtil::ParamSigmas &sigmas = PyUtil::ParamSigmas::Instance();
		Linfo << "total cross-section: " << sigmas.Get("TOTAL", rndmA, rndmB);
		Linfo << "inel enum: " << PyUtil::ParamSigmas::kINEL;
		Linfo << " inel cross-section: " << sigmas.Get(PyUtil::ParamSigmas::kINEL, rndmA, rndmB);

		Linfo << "total_et : " << PyUtil::total_et_from_final_particles(pythia);
		TLorentzVector t = PyUtil::total_vector_final_particles(pythia);
		Linfo << "total TLV : " << t.Px() << " " << t.Py() << " " << t.Pz() << " eT=" << t.Et();
		TVector3 bv = t.BoostVector();
		Linfo << " ... boost vector: mag=" << bv.Mag() << " px=" << bv.Px() << " py=" << bv.Py() << " pz=" << bv.Pz();

		ev_pool.AddEvent(event);
		Linfo << "number of events in the ev_pool: " << ev_pool.GetPool().size();
		std::vector<Pythia8::Particle> fparts = ev_pool.GetFinalParticles();
		Linfo << "number of final state particles in the ev_pool: " << fparts.size();

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
			for (int ip = 0; ip < event.size(); ip++)
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
	pypool.WriteECMsToFile("pypool_output.root");
	Linfo << "Generation done.";

	return 0;
}
