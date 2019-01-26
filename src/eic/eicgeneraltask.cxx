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

#include <fastjet/Selector.hh>
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
		if (fArgs.isSet("--write-hepmc"))
		{
			std::string mcevout(GetName() + ".hepmc");
			fMCEvWrapper->SetOutputFile(mcevout.c_str());
		}
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
				Ldebug << GetName() << " setting event from " << t->GetName();
				// auto _pv = hepmc->PseudoJetParticles(true);
				// cuts need to happen here
				// parts.insert(parts.end(), _pv.begin(), _pv.end());
				// wxsec = hepmc->GetCrossSecion()->cross_section();
				fMCEvWrapper->SetEvent(hepmc->GetEvent());
			}
		}

		wxsec = fMCEvWrapper->GetCrossSecion()->cross_section();

		// do particle stuff
		Ldebug << "number of particles : " << parts.size();
		ps << "xsec" << wxsec;

		HepMCUtil::EICkine eic_kine(fMCEvWrapper->GetEvent());
		Ldebug << eic_kine.dump();
		//Ldebug << eic_kine.as_oss("eic_kine_").str();
		ps << eic_kine.as_oss("kine_");

		if (pythia)
		{
			// pythia->info.list();
			ps << "py_x1pdf" << pythia->info.x1pdf();
			ps << "py_x2pdf" << pythia->info.x2pdf();
			ps << "py_QFac" << pythia->info.QFac();
			ps << "py_Q2Fac" << pythia->info.Q2Fac();
			ps << "py_QRen" << pythia->info.QRen();
			ps << "py_Q2Ren" << pythia->info.Q2Ren();
			icode = pythia->info.code();
			ps << "py_icode" << icode;
			xsec_code = pythia->info.sigmaGen(icode);
			ps << "py_xsec_code" << xsec_code;
		}

		parts = fMCEvWrapper->PseudoJetParticles(true);

		fj::Selector partSelector = fastjet::SelectorAbsEtaMax(fSettings.maxEta);
		fj::Selector jetSelector = fastjet::SelectorAbsEtaMax(fSettings.maxEta - fSettings.jetR - 0.01) * fastjet::SelectorPtMin(1.);

		std::vector<fj::PseudoJet> parts_selected = partSelector(parts);
		ps << "all_" << parts;
		ps << "sel_" << parts_selected;
		auto pdgs = HepMCUtil::PDGcodesForPseudoJets(fMCEvWrapper->GetEvent(), parts_selected);
		ps << "sel_pdg" << pdgs;
		auto bar_codes = HepMCUtil::barcodesForPseudoJets(parts_selected);
		ps << "sel_bc" << bar_codes;

		std::vector<int> beam_barcodes;
		auto beam_parts = HepMCUtil::beam_particles(fMCEvWrapper->GetEvent());
		for (auto b : beam_parts)
		{
			beam_barcodes.push_back(b->barcode());
		}
		ps << "beam_bc" << beam_barcodes;
		auto oes = HepMCUtil::find_outgoing_electron(fMCEvWrapper->GetEvent());
		if (oes.size() < 1)
		{
			Lerror << "unable to find the outgoing electron in the event record!";
		}
		else
		{
			ps << "out_e_bc" << oes[0]->barcode();
		}

		fj::JetDefinition jet_def(fj::antikt_algorithm, fSettings.jetR);
		fj::ClusterSequence ca(parts_selected, jet_def);
		auto jets = jetSelector(ca.inclusive_jets());
		for (const auto & jakt: jets)
		{
			if (TMath::Abs(jakt.eta()) > fSettings.maxEta - fSettings.jetR) continue;
			ps << "j_" << jakt;
		}

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
