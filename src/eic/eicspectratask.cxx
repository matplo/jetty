#include <jetty/eic/eicspectratask.h>

#include <jetty/util/tasks/pythiatask.h>
#include <jetty/util/hepmc/readfile.h>

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

namespace EIC
{
	SpectraTask::Settings::Settings() :
		R1(0.05),
		R2(0.5),
		maxEta(10.),
		deltaT(10.)
	{;}

	std::string SpectraTask::Settings::str()
	{
		std::ostringstream _ss;
		_ss << "R1" << R1 << "_"
			<< "R2" << R2 << "_"
			<< "maxEta" << maxEta << "_"
			<< "deltaT" << deltaT << "ps";
		std::string _cs = _ss.str();
		return _cs;
	}

	void SpectraTask::Settings::setup_from_string(const char *s, const char *comment)
	{
		PyUtil::Args args(s);
		if (comment!=0) Linfo << comment << " reading settings from string: " << s;
		R1 = args.getD("--tofR1", R1);
		if (comment!=0) Linfo << comment << " running with R1 = " << R1;
		R2 = args.getD("--tofR2", R2);
		if (comment!=0) Linfo << comment << " running with R2 = " << R2;
		maxEta = args.getD("--eta", maxEta);
		if (comment!=0) Linfo << comment << " running with particle |eta| < " << maxEta;
		deltaT = args.getD("--deltat", deltaT);
		if (comment!=0) Linfo << comment << " running with deltaT = " << deltaT << " ps";
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

		fDeltaTGauss = new TF1("fDeltaTGauss" ,"gaus", -10. * fSettings.deltaT, 10. * fSettings.deltaT);
		fDeltaTGauss->SetParameter(0, 1);
		fDeltaTGauss->SetParameter(1, 0);
		double sigma = fSettings.deltaT / (2 * TMath::Sqrt(2. * TMath::Log(2.)));
		Linfo << GetName() << " sigma dT = " << sigma << " ps";
		fDeltaTGauss->SetParameter(2, sigma);
		fDeltaTGauss->SetNpx(500);
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

				Pythia8::Event &ev = tp->GetPythia()->event;
				std::vector<Pythia8::Particle> pyparts;
				std::vector<double> z1v;
				std::vector<double> z2v;
				std::vector<double> L1v;
				std::vector<double> L2v;
				std::vector<double> t1v;
				std::vector<double> t2v;
				std::vector<double> tR1v;
				std::vector<double> tR2v;
				std::vector<double> velov;
				std::vector<double> pv;
				for (int i = 0; i < ev.size(); i++)
				{
					Pythia8::Particle &pyp = ev[i];
					if (TMath::Abs(pyp.eta()) > fSettings.maxEta)
						continue;
					if (pyp.isFinal())
					{
						pyparts.push_back(pyp);
						TLorentzVector tlv = PyUtil::TLVFromPythia(pyp);
						// double tR1 =
						// Linfo << -TMath::Log(TMath::Tan(pyp.theta() / 2.)) << " " << pyp.eta();
						double z1 = fSettings.R1 / TMath::Tan(pyp.theta());
						double z2 = fSettings.R2 / TMath::Tan(pyp.theta());
						double L1 = fSettings.R1 / TMath::Sin(pyp.theta());
						double L2 = fSettings.R2 / TMath::Sin(pyp.theta());
						double v = tlv.P() / (tlv.Gamma() * tlv.M()); // * TMath::C(); beta = v/c
						double t1 = L1/v;
						double t2 = L2/v;
						double _resol1 = fDeltaTGauss->GetRandom();
						double tR1 = t1 + _resol1 * 1e-12 * TMath::C();
						double _resol2 = fDeltaTGauss->GetRandom();
						double tR2 = t2 + _resol2 * 1e-12 * TMath::C();
						z1v.push_back(z1);
						z2v.push_back(z2);
						L1v.push_back(L1);
						L2v.push_back(L2);
						t1v.push_back(t1);
						t2v.push_back(t2);
						tR1v.push_back(tR1);
						tR2v.push_back(tR2);
						velov.push_back(v);
						pv.push_back(tlv.P());
					}
				}
				ps << "py_" << pyparts;
				ps << "z1" << z1v;
				ps << "z2" << z2v;
				ps << "L1" << L1v;
				ps << "L2" << L2v;
				ps << "t1" << t1v;
				ps << "t2" << t2v;
				ps << "tR1" << tR1v;
				ps << "tR2" << tR2v;
				ps << "v" << velov;
				ps << "py_p" << pv;
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

		// do particle stuff
		Ldebug << "number of particles : " << parts.size();
		ps << "xsec" << wxsec;
		ps << "icode" << icode;
		ps << "xsec_code" << xsec_code;
		ps << "psj_" << parts;

		// for (auto p : parts)
		// {
		// 	if (TMath::Abs(p.eta()) > fSettings.maxEta)
		// 		continue;
		// } // end of the jet loop

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
			fDeltaTGauss->Write();
			Linfo << GetName() << " Finalize " << " file written: " << fOutput->GetName();
		}
		//}
		return kDone;
	}
}
