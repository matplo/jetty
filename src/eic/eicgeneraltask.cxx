#include <jetty/eic/eicgeneraltask.h>

#include <jetty/util/tasks/pythiatask.h>
#include <jetty/util/hepmc/readfile.h>
#include <jetty/util/hepmc/eventwrapper.h>
#include <jetty/util/hepmc/util.h>

#include <jetty/util/tglaubermc/tglaubermc.h>
#include <jetty/util/pythia/event_pool.h>
#include <jetty/util/pythia/pyutil.h>
#include <jetty/util/blog.h>

#include <jetty/util/rstream/tstream.h>

#include <algorithm>
#include <type_traits>

#include <TH1F.h>
#include <TFile.h>
#include <TTree.h>
#include <TParticle.h>
#include <TParticlePDG.h>
#include <TF1.h>

#include <fastjet/PseudoJet.hh>
#include <fastjet/ClusterSequence.hh>
namespace fj = fastjet;

#include <HepMC/GenRanges.h>
#include <HepMC/GenParticle.h>
#include <boost/foreach.hpp>

namespace EIC
{
	GeneralTask::Settings::Settings() :
		maxEta(10.),
		jetR(0.4)
	{;}

	std::string GeneralTask::Settings::str()
	{
		std::ostringstream _ss;
		_ss << "jetR" << jetR << "_"
			<< "maxEta" << maxEta << "_";
		std::string _cs = _ss.str();
		return _cs;
	}

	void GeneralTask::Settings::setup_from_string(const char *s, const char *comment)
	{
		PyUtil::Args args(s);
		jetR = args.getD("--jetR", jetR);
		maxEta = args.getD("--eta", maxEta);
		if (maxEta < 2. * jetR + 0.1) maxEta = jetR * 2. + 0.1;
		if (comment!=0) Linfo << comment << " running with jetR = " << jetR;
		if (comment!=0) Linfo << comment << " running with particle |eta| < " << maxEta;
	}


	GeneralTask::~GeneralTask()
	{
		delete fMCEvWrapper;
		Ltrace << "destructor " << GetName();
	}

	unsigned int GeneralTask::InitThis(const char *opt)
	{
		fArgs.merge(opt);
		fSettings.setup_from_string(fArgs.asString().c_str(), GetName().c_str());

		std::ostringstream _ss;
		_ss << GetName() << "_" << fSettings.str() << ".root";
		string outname = _ss.str();
		fOutput = new TFile(outname.c_str(), "recreate");
		if (!fOutput)
		{
			Lfatal << "unable to create output file: " << outname;
			return kError;
		}

		fOutput->cd();
		fOutputTree 	= new TTree("eic", "eic");
		fTStream    	= new RStream::TStream("p", fOutputTree);

		fMCEvWrapper = new GenUtil::HepMCEventWrapper();
		std::string mcevout(GetName() + ".hepmc");
		fMCEvWrapper->SetOutputFile(mcevout.c_str());

		return kGood;
	}

	unsigned int GeneralTask::ExecThis(const char * /*opt*/)
	{
		int npart = 2;
		Ldebug << "fpGlauberMC: " << fpGlauberMC;
		if (fpGlauberMC)
			npart = fpGlauberMC->GetNpart();

		RStream::TStream &ps = *fTStream;

		double wxsec = 1.;
		int icode = 0;
		double xsec_code = 0;

		std::vector<fj::PseudoJet> parts;

		Pythia8::Pythia *pythia = 0;
		for (auto &t : fInputTasks)
		{
			GenUtil::PythiaTask *tp = dynamic_cast<GenUtil::PythiaTask*>(t);
			Ldebug << GetName() << " processing input from task " << t->GetName();
			if (tp)
			{
				Ldebug << GetName() << " got GenUtil::PythiaTask input from task " << t->GetName();
				pythia = tp->GetPythia();
				if (tp->GetPythia())
				{
					Ldebug << GetName() << " got non 0x0 pythia from " << t->GetName() << " at " << tp->GetPythia();
					// note this will be the last pythia in the pool!
					icode = tp->GetPythia()->info.code();
					xsec_code = tp->GetPythia()->info.sigmaGen(icode);

					Pythia8::Event &ev = tp->GetPythia()->event;
					std::vector<Pythia8::Particle> pyparts;
					for (int i = 0; i < ev.size(); i++)
					{
						Pythia8::Particle &pyp = ev[i];
						if (TMath::Abs(pyp.eta()) > fSettings.maxEta)
							continue;
						if (pyp.isFinal())
						{
							pyparts.push_back(pyp);
						}
					}
					ps << "py_" << pyparts;

					fMCEvWrapper->SetPythia8(tp->GetPythia());
					fMCEvWrapper->FillEvent();
					Ldebug << "hepmc event at " << fMCEvWrapper->GetEvent();
					Ldebug << "number of particle bar codes in hepmc event = " << fMCEvWrapper->GetEvent()->particles_size ();
					fMCEvWrapper->WriteEvent();
				}
			}
			auto evpool = t->GetData()->get<PyUtil::EventPool>();
			if (evpool)
			{
				// handle pythia stuff here
				Ldebug << GetName() << " adding particles from " << t->GetName();
				auto _pv = evpool->GetFinalPseudoJets();
				parts.insert(parts.end(), _pv.begin(), _pv.end());
			}
			auto hepmc = t->GetData()->get<GenUtil::ReadHepMCFile>();
			if (hepmc)
			{
				Ldebug << GetName() << " adding particles from " << t->GetName();
				auto _pv = hepmc->PseudoJetParticles(true);
				// cuts need to happen here
				parts.insert(parts.end(), _pv.begin(), _pv.end());
				wxsec = hepmc->GetCrossSecion()->cross_section();
				fMCEvWrapper->SetEvent(hepmc->GetEvent());
				ps << "hepmc_" << parts;
			}
		}

		// do particle stuff
		Ldebug << "number of particles : " << parts.size();
		ps << "xsec" << wxsec;
		ps << "icode" << icode;
		ps << "xsec_code" << xsec_code;
		ps << "psj_" << parts;

		ps << "hepmc_reread_" << fMCEvWrapper->PseudoJetParticles(true);

		Linfo << "hepmc beam particles: " << fMCEvWrapper->HepMCParticles(false, 4).size() << endl;
		Linfo << "hepmc outgoing particles final: " << fMCEvWrapper->HepMCParticles(true, 1).size() << endl;
		// Linfo << "number of vertices: " << fMCEvWrapper->Vertices().size();

		auto els = HepMCUtil::find_outgoing_electron(fMCEvWrapper->GetEvent());
		Linfo << "outgoing electrons: " << els.size();

		auto eic_Q2 = HepMCUtil::eIC_Q2(fMCEvWrapper->GetEvent());
		Linfo << "Q2 = " << eic_Q2;

		// debugging of the electron recoil from the beam
		// if (HepMCUtil::find_outgoing_electron(fMCEvWrapper->GetEvent(), true).size() > 1)
		// {
		// 	if (pythia)
		// 	{
		// 		pythia->info.list();
		// 		pythia->event.list(true, true, 3);
		// 	}
		// }


		// Linfo << "hepmc outgoing particle: " << fMCEvWrapper->HepMCParticles(false, 63)[0]->pdg_id() << endl;
		// for (auto p : parts)
		// {
		// 	if (TMath::Abs(p.eta()) > fSettings.maxEta)
		// 		continue;
		// } // end of the jet loop

		ps << endl;
		return kGood;
	}

	unsigned int GeneralTask::FinalizeThis(const char *opt)
	{
		fArgs.merge(opt);
		// if (fArgs.isSet("--write"))
		// {
		if (fOutput)
		{
			fOutput->cd();
			fOutputTree->Write();
			fOutput->Write();
			Linfo << GetName() << " Finalize " << " file written: " << fOutput->GetName();
		}
		//}
		return kDone;
	}
}
