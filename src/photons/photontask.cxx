#include <jetty/photons/photontask.h>

#include <jetty/util/tasks/gentaskoutput.h>
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
#include <map>

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

namespace Photons
{
	template <class T>
	class VarPair
	{
	public:
		VarPair(const char *name) : fName(name), fV() {;}
		virtual ~VarPair() {;}
		void push_back(T v)
		{
			fV.push_back(v);
		}
		const std::string & name() const {return fName;}
		const std::vector<T> & values() const {return fV;}
	private:
		VarPair() : fName("noname"), fV() {;}
		std::string fName;
		std::vector<T> fV;
	};

	class VarCollector
	{
	public:
		template <class T>
		void push_back(const char *name, T v)
		{
			double dv(v);
			fMap[name].push_back(dv);
		}
		std::map<const char *, std::vector<double>> fMap;
	};

	//class VarCollector
	//{
	//public:
	//	VarCollector() : fPairs() {;}
	//	virtual ~VarCollector() {;}
	//	// ostream ...
	//	template <class T>
	//	void push_back(const char *name, T v)
	//	{
	//		double vd(v);
	//		for (auto p : fPairs)
	//		{
	//			if (p.name() == name)
	//		}
	//		fPairs.
	//	}
	//private:
	//	std::vector<VarPair<double>> fPairs;
	//}

	PhotonTask::Settings::Settings() :
		maxEta(10.),
		jetR(0.4)
	{;}

	std::string PhotonTask::Settings::str()
	{
		std::ostringstream _ss;
		_ss << "jetR" << jetR << "_"
			<< "maxEta" << maxEta << "_";
		std::string _cs = _ss.str();
		return _cs;
	}

	void PhotonTask::Settings::setup_from_string(const char *s, const char *comment)
	{
		PyUtil::Args args(s);
		jetR = args.getD("--jetR", jetR);
		maxEta = args.getD("--eta", maxEta);
		if (maxEta < 2. * jetR + 0.1) maxEta = jetR * 2. + 0.1;
		if (comment!=0) Linfo << comment << " running with jetR = " << jetR;
		if (comment!=0) Linfo << comment << " running with particle |eta| < " << maxEta;
	}


	PhotonTask::~PhotonTask()
	{
		delete fMCEvWrapper;
		Ltrace << "destructor " << GetName();
	}

	unsigned int PhotonTask::InitThis(const char *opt)
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
		fOutputTree 	= new TTree("gm", "gm");
		fTStream    	= new RStream::TStream("p", fOutputTree);

		fMCEvWrapper = new GenUtil::HepMCEventWrapper();
		if (fArgs.isSet("--write-hepmc"))
		{
			std::string mcevout(GetName() + ".hepmc");
			fMCEvWrapper->SetOutputFile(mcevout.c_str());
		}

		TFile *f = GenUtil::GenTaskOutput::Instance().GetOutput("test.root");
		f->cd();
		TNtuple *tnj = new TNtuple("tnj", "tnj", "pt:e:eta:phi:lpid:nsplits");
		GenUtil::GenTaskOutput::Instance().RegisterOutputObject(tnj);
		TNtuple *tnd = new TNtuple("tnd", "tnd", "pt:e:eta:phi:lpid:lund_dR:lund_pt:lund_e:lund_pt1:lund_pt2:lund_lpid:nsplits");
		GenUtil::GenTaskOutput::Instance().RegisterOutputObject(tnd);
		return kGood;
	}

	unsigned int PhotonTask::ExecThis(const char * /*opt*/)
	{
		// auto vpair = VarPair<double>("something");
		// vpair.push_back(1.);
		// VarCollector coll;
		// coll.push_back("ala", 1);
		// coll.push_back("bela", 2);
		// coll.push_back("bela", 23);
		// Linfo << coll.fMap["ala"].size();
		// Linfo << coll.fMap["bela"].size();

		TNtuple *tnj = dynamic_cast<TNtuple*>(GenUtil::GenTaskOutput::Instance().GetOutputObject("tnj"));
		TNtuple *tnd = dynamic_cast<TNtuple*>(GenUtil::GenTaskOutput::Instance().GetOutputObject("tnd"));

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
		auto oes = HepMCUtil::find_outgoing_photon(fMCEvWrapper->GetEvent());
		if (oes.size() < 1)
		{
			Lwarn << "unable to find the outgoing photon in the event record!";
		}
		else
		{
			std::vector<int> _idx;
			for (auto p : oes) _idx.push_back(p->barcode());
			ps << "out_photon_bc" << _idx;
			// Linfo << "prompt photon HEPMC:" << oes[0]->barcode();
		}
		if (pythia)
		{
			std::vector<int> _idx_py = PyUtil::prompt_photon_indexes(pythia->event);
			// Linfo << "prompt photon pythia:" << _idx_py[0];
			ps << "out_photon_bc_pythia" << _idx_py;
			if (oes.size() < 1 && fArgs.debugMode())
			{
				pythia->process.list();
				Linfo << "pythia final state prompt photons: " << _idx_py.size();
				for (auto ip : _idx_py)
					Linfo << " - " << ip;
				Linfo << " . ";
				pythia->event.list();
			}
		}

		fj::JetDefinition jet_def(fj::antikt_algorithm, fSettings.jetR);
		fj::ClusterSequence ca(parts_selected, jet_def);
		auto jets = jetSelector(ca.inclusive_jets());
		ps << "j_" << jets;
		for (const auto & jakt: jets)
		{
			if (!fj::SelectorNHardest(1)(jakt.constituents())[0].has_user_info<GenUtil::HepMCPSJUserInfo>())
			{
				Lerror << "user info missing for the leading constituent";
				break;
			}
			auto uinfo = fj::SelectorNHardest(1)(jakt.constituents())[0].user_info<GenUtil::HepMCPSJUserInfo>();
			int lpid = uinfo.getParticle()->pdg_id();
			int lidx = uinfo.getParticle()->barcode();
			int nsplits = 0;
			fj::JetDefinition decl_jet_def(fj::cambridge_algorithm, 1.0);
			fj::ClusterSequence decl_ca(jakt.constituents(), decl_jet_def);
			for (const auto & j : decl_ca.inclusive_jets(0))
			{
				fj::PseudoJet jj, j1, j2;
				jj = j;
				while (jj.has_parents(j1,j2))
				{
					nsplits++;
					if (j1.pt2() < j2.pt2()) swap(j1,j2);
					double delta_R = j1.delta_R(j2);
					if (!fj::SelectorNHardest(1)(jj.constituents())[0].has_user_info<GenUtil::HepMCPSJUserInfo>())
					{
						Lerror << "user info missing for the leading constituent of a radiator";
						break;
					}
					auto _uinfo = fj::SelectorNHardest(1)(jj.constituents())[0].user_info<GenUtil::HepMCPSJUserInfo>();
					int _lpid = _uinfo.getParticle()->pdg_id();
					int _lidx = _uinfo.getParticle()->barcode();
					tnd->Fill(jakt.perp(), jakt.e(), jakt.eta(), jakt.phi(), lpid,
					         delta_R, jj.perp(), jj.e(), j1.perp(), j2.perp(), _lpid, nsplits);

					jj = j1;
				}
			}
			tnj->Fill(jakt.perp(), jakt.e(), jakt.eta(), jakt.phi(), lpid, nsplits);
		}

		ps << endl;
		return kGood;
	}

	unsigned int PhotonTask::FinalizeThis(const char *opt)
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
