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
	SubjetTask::Settings::Settings() :
		R(0.4),
		A(fastjet::antikt_algorithm),
		maxEta(3.),
		jptcut(0.),
		jptcutmax(1e4),
		sjR(0.1),
		sjA(fastjet::antikt_algorithm),
		sd_z_cut(0.1),
		sd_beta(0.0),
		sd_r_jet(R)
	{;}

	std::string SubjetTask::Settings::str()
	{
		std::ostringstream _ss;
		_ss << "R" << R << "_"
			<< "A" << A << "_"
			<< "r" << sjR << "_"
			<< "sjA" << sjA << "_"
			<< "sdzcut" << sd_z_cut << "_"
			<< "sdbeta" << sd_beta << "_"
			<< "sdr" << sd_r_jet << "_"
			<< "maxEta" << maxEta << "_"
			<< "minpt" << jptcut << "_"
			<< "maxpt" << jptcutmax;
		std::string _cs = _ss.str();
		return _cs;
	}

	void SubjetTask::Settings::setup_from_string(const char *s, const char *comment)
	{
		PyUtil::Args args(s);
		if (comment!=0) Linfo << comment << " reading settings from string: " << s;
		R = args.getD("--R", R);
		if (comment!=0) Linfo << comment << " running with R = " << R;
		A = fj::JetAlgorithm(args.getI("--A", A));
		if (comment!=0) Linfo << comment << " running with Algo = " << A;
		maxEta = args.getD("--eta", maxEta);
		if (comment!=0) Linfo << comment << " running with particle |eta| < " << maxEta;
		jptcut = args.getD("--jptcut", jptcut);
		if (comment!=0) Linfo << comment << " running with a cut on jet pT > " << jptcut;
		jptcutmax = args.getD("--jptcutmax", jptcutmax);
		if (comment!=0) Linfo << comment << " running with a cut on jet pT < " << jptcutmax;
		sjR = args.getD("--sjR", sjR);
		if (comment!=0) Linfo << comment << " running with subjet R = " << sjR;
		sjA = fj::JetAlgorithm(args.getI("--sjA", sjA));
		if (comment!=0) Linfo << comment << " running with subjet Algo = " << sjA;
		sd_z_cut = args.getD("--sd-z-cut", sd_z_cut);
		if (comment!=0) Linfo << comment << " running with SD z cut = " << sd_z_cut;
		sd_beta = args.getD("--sd-beta", sd_beta);
		if (comment!=0) Linfo << comment << " running with SD beta = " << sd_beta;
		sd_r_jet = args.getD("--sd-r-jet", sd_r_jet);
		if (comment!=0) Linfo << comment << " running with SD r jet = " << sd_r_jet;
	}


	SubjetTask::~SubjetTask()
	{
		Ltrace << "destructor " << GetName();
	}

	unsigned int SubjetTask::InitThis(const char *opt)
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
		fOutputTree = new TTree("jt", "jt");
		fTStream    = new RStream::TStream("j", fOutputTree);

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
		fj::JetDefinition jet_def(fSettings.A, fSettings.R);
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

			// Linfo << "n constits: " << j.constituents().size();

			JettyFJUtils::LundEntries lund(j);
			if (lund.CAjet())
			{
				jts << "lund_logzdr" << lund.LogzDeltaR();
				jts << "lund_log1odr" << lund.Log1oDeltaR();
			}

			auto sj_info = new JettyFJUtils::SJInfo(&j,
			                                        fSettings.sjR, fSettings.sjA,
			                                        fSettings.sd_z_cut, fSettings.sd_beta, fSettings.sd_r_jet);
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
			jts << "c_pt" << sj_info->c_pt();
			jts << "c_phi" << sj_info->c_phi();
			jts << "c_eta" << sj_info->c_eta();

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
