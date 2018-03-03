#include <jetty/subjets/subjettask.h>
#include <jetty/subjets/fjutils.h>

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

namespace GenUtil
{
	SubjetTask::~SubjetTask()
	{
		Ltrace << "destructor " << GetName();
	}

	unsigned int SubjetTask::InitThis(const char *opt)
	{
		fArgs.merge(opt);
		string outname = GetName(); outname += ".root";
		fOutput = new TFile(outname.c_str(), "recreate");
		if (!fOutput)
		{
			Lfatal << "unable to create output file: " << outname;
			return kError;
		}

		fOutput->cd();
		fOutputTree = new TTree("jt", "jt");
		fTStream    = new RStream::TStream("j", fOutputTree);

		fSettings.R = fArgs.getD("--R", 0.4);
		Linfo << GetName() << " running with R = " << fSettings.R;
		fSettings.maxEta = fArgs.getD("--eta", 3.);
		Linfo << GetName() << " running with particle |eta| < " << fSettings.maxEta;
		fSettings.jptcut = fArgs.getD("--jptcut", 0.);
		Linfo << GetName() << " running with a cut on jet pT > " << fSettings.jptcut;
		fSettings.jptcutmax = fArgs.getD("--jptcutmax", 1e4);
		Linfo << GetName() << " running with a cut on jet pT < " << fSettings.jptcutmax;
		fSettings.sjR = fArgs.getD("--sjR", 0.1);
		Linfo << GetName() << " running with subjet R = " << fSettings.sjR;


		return kGood;
	}

	unsigned int SubjetTask::ExecThis(const char * /*opt*/)
	{
		int npart = 2;
		Ldebug << "fpGlauberMC: " << fpGlauberMC;
		if (fpGlauberMC)
			npart = fpGlauberMC->GetNpart();

		RStream::TStream &jts = *fTStream;

		double wxsec = 1.;
		int icode = 0;
		double xsec_code = 0;

		std::vector<fj::PseudoJet> parts;

		for (auto &t : fInputTasks)
		{
			GenUtil::PythiaTask *tp = (GenUtil::PythiaTask*)t;
			if (tp)
			{
				if (tp->GetPythia())
				{
					// note this will be the last pythia in the pool!
					icode = tp->GetPythia()->info.code();
					xsec_code = tp->GetPythia()->info.sigmaGen(icode);
				}
			}
			auto evpool = t->GetData()->get<PyUtil::EventPool>();
			if (evpool)
			{
				// handle pythia stuff here
			}
			auto hepmc = t->GetData()->get<GenUtil::ReadHepMCFile>();
			if (hepmc)
			{
				auto _pv = hepmc->PseudoJetParticles(true);
				parts.insert(parts.end(), _pv.begin(), _pv.end());
			}
		}

		// do subjet stuff
		fj::JetDefinition jet_def(fj::antikt_algorithm, fSettings.R);
		fj::ClusterSequence cs(parts, jet_def);
		auto jets = fj::sorted_by_pt(cs.inclusive_jets());
		Ldebug << "number of particles : " << parts.size();
		Ldebug << "number of jets found : " << jets.size();
		for (auto j : jets)
		{
			if (j.perp() < fSettings.jptcut)
				continue;
			if (j.perp() > fSettings.jptcutmax)
				continue;
			if (TMath::Abs(j.eta()) > fSettings.maxEta - fSettings.R)
				continue;

			jts << "xsec" << wxsec;
			jts << "icode" << icode;
			jts << "xsec_code" << xsec_code;

			jts << "j" << j;

			auto sj_info = new JettyFJUtils::SJInfo(&j, fSettings.sjR);
			j.set_user_info(sj_info);

			auto sjs = sj_info->subjets();
			jts << "subj" << sjs;
			// for (auto sj : sjs)
			// 	hz->Fill(sj.perp() / j.perp());

			jts << "nsj" << sjs.size();

			if (sjs.size() < 1)
			{
				sjs.push_back(j);
			}
			if (sjs.size() < 2)
			{
				sjs.push_back(fj::PseudoJet(0,0,0,0));
			}

			jts << "sj_0" << sjs[0];
			jts << "sj_1" << sjs[1];
			jts << "sj_dR" << sjs[0].delta_R(sjs[1]);
			jts << "sj_dpt" << sjs[0].perp() - sjs[1].perp();
			jts << "sj_delta" << (sjs[0].perp() - sjs[1].perp()) / j.perp();
			jts << "sj_delta_sum" << (sjs[0].perp() - sjs[1].perp()) / (sjs[0].perp() + sjs[1].perp());
			jts << "sj_zg" << TMath::Min(sjs[0].perp(), sjs[1].perp()) / j.perp();
			//auto zs   = sj_info->z();
			jts << "z" << sj_info->z();

			//auto rm  = sj_info->rm();
			jts << "rm" << sj_info->rm();

			//auto zg  = sj_info->sd_zg();
			jts << "sd_zg" << sj_info->sd_zg();
			jts << "sd_dR" << sj_info->sd_dR();
			jts << "sd_mu" << sj_info->sd_mu();
			jts << endl;

			// delete sj_info; deleted when deleting jets
		} // end of the jet loop
		return kGood;
	}

	unsigned int SubjetTask::FinalizeThis(const char *opt)
	{
		fArgs.merge(opt);
		if (fArgs.isSet("--write"))
		{
			if (fOutput)
			{
				fOutput->cd();
				fOutputTree->Write();
				fOutput->Write();
				Linfo << GetName() << " Finalize " << " file written: " << fOutput->GetName();
			}
		}
		return kDone;
	}
}
