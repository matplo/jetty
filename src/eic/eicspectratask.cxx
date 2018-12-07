#include <jetty/eic/eicspectratask.h>

#include <jetty/util/tasks/pythiatask.h>
#include <jetty/util/hepmc/readfile.h>

#include <jetty/util/tglaubermc/tglaubermc.h>
#include <jetty/util/pythia/event_pool.h>
#include <jetty/util/blog.h>

#include <jetty/util/rstream/tstream.h>

#include <algorithm>
#include <type_traits>

#include <TH1F.h>
#include <TFile.h>
#include <TTree.h>
#include <TParticle.h>
#include <TParticlePDG.h>

#include <fastjet/PseudoJet.hh>
#include <fastjet/ClusterSequence.hh>
namespace fj = fastjet;

namespace EIC
{
	SpectraTask::Settings::Settings() :
		R1(0.05),
		R2(0.5),
		maxEta(10.)
	{;}

	std::string SpectraTask::Settings::str()
	{
		std::ostringstream _ss;
		_ss << "R1" << R1 << "_"
			<< "R2" << R2 << "_"
			<< "maxEta" << maxEta << "_";
		std::string _cs = _ss.str();
		return _cs;
	}

	void SpectraTask::Settings::setup_from_string(const char *s, const char *comment)
	{
		PyUtil::Args args(s);
		if (comment!=0) Linfo << comment << " reading settings from string: " << s;
		R1 = args.getD("--EICR1", R1);
		if (comment!=0) Linfo << comment << " running with R1 = " << R1;
		R2 = args.getD("--EICR2", R2);
		if (comment!=0) Linfo << comment << " running with R2 = " << R2;
		maxEta = args.getD("--eta", maxEta);
		if (comment!=0) Linfo << comment << " running with particle |eta| < " << maxEta;
	}


	SpectraTask::~SpectraTask()
	{
		Ltrace << "destructor " << GetName();
	}

	unsigned int SpectraTask::InitThis(const char *opt)
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
		fOutputTree = new TTree("eic", "eic");
		fTStream    = new RStream::TStream("p", fOutputTree);

		return kGood;
	}

	unsigned int SpectraTask::ExecThis(const char * /*opt*/)
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

		for (auto &t : fInputTasks)
		{
			GenUtil::PythiaTask *tp = (GenUtil::PythiaTask*)t;
			Ldebug << GetName() << " processing input from task " << t->GetName();
			if (tp)
			{
				Ldebug << GetName() << " got GenUtil::PythiaTask input from task " << t->GetName();
				if (tp->GetPythia())
				{
					Ldebug << GetName() << " got non 0x0 pythia from " << t->GetName() << " at " << tp->GetPythia();
					// note this will be the last pythia in the pool!
					icode = tp->GetPythia()->info.code();
					xsec_code = tp->GetPythia()->info.sigmaGen(icode);
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
				parts.insert(parts.end(), _pv.begin(), _pv.end());
				wxsec = hepmc->GetCrossSecion()->cross_section();
			}
		}

		// do subjet stuff
		Ldebug << "number of particles : " << parts.size();
		for (auto p : parts)
		{
			if (TMath::Abs(p.eta()) > fSettings.maxEta)
				continue;

			ps << "xsec" << wxsec;
			ps << "icode" << icode;
			ps << "xsec_code" << xsec_code;

			ps << "_" << p;
			// delete sj_info; deleted when deleting jets
		} // end of the jet loop

		ps << endl;
		return kGood;
	}

	unsigned int SpectraTask::FinalizeThis(const char *opt)
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
