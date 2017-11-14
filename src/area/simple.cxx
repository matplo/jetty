#include <util/args.h>
#include <util/blog.h>
#include <util/strutil.h>
#include <util/looputil.h>
#include <util/hepmc/readfile.h>
#include <util/rstream/tstream.h>

#include <TFile.h>
#include <TProfile.h>
#include <TRandom.h>
#include <TTree.h>

#include <string>

#include <boost/algorithm/string.hpp>

#include <util/voronoi/varea_calc.h>
#include <util/voronoi/event_area.h>

using namespace std;

int simple ( int argc, char *argv[] )
{
	SysUtil::Args args(argc, argv);
	Linfo << args.asString();

	double etamax = TMath::Abs(args.getD("--etamax", 1.0));
	string sfoutputname = args.get("--output", "voronoi_area_simple_tree.root");
	if (sfoutputname == "voronoi_area_simple_tree.root")
	{
		string ext = StrUtil::sT("_etamax_") + StrUtil::sT(etamax) + StrUtil::sT(".root");
		boost::algorithm::replace_last(sfoutputname, ".root", ext);
	}
	double deta = etamax * 2.;
	double acceptance = deta * 2.*TMath::Pi();
	Long64_t nghosts = TMath::Abs(args.getI("--nghost-eta", 100)) *deta;
	if (args.isSet("--fixed-nghost"))
	{
		nghosts = TMath::Abs(args.getI("--fixed-nghost", 100));
		string ext = StrUtil::sT("_fixed_nghost_") + StrUtil::sT(nghosts) + StrUtil::sT(".root");
		boost::algorithm::replace_last(sfoutputname, ".root", ext);
	}
	else
	{
		string ext = StrUtil::sT("_nghost_eta_") + StrUtil::sT(nghosts/(etamax*2.)) + StrUtil::sT(".root");
		boost::algorithm::replace_last(sfoutputname, ".root", ext);
	}
	Linfo << "             number of ghosts : " << nghosts;
	Linfo << "number of ghosts per unit eta : " << nghosts / deta;

	double min_ghost_area = args.getD("--min-ghost-area", 0.0);
	Linfo << "minimum ghost area : " << min_ghost_area;

	bool random_flag = args.isSet("--random");
	Linfo << "random_flag : " << random_flag;
	if (random_flag == true)
	{
		string ext = StrUtil::sT("_random.root");
		boost::algorithm::replace_last(sfoutputname, ".root", ext);
	}
	VoronoiUtil::EventAreaSetup ea_setup(etamax, TMath::Abs(args.getI("--nghost-eta", 0)), min_ghost_area, args.isSet("--fixed_nghosts"));
	ea_setup.Dump();

	Stat_t nevents = args.getI("--nev", 0);
	LoopUtil::TPbar pbar(nevents);

	double max_mult = 3000.;

	Linfo << "output goes to : " << sfoutputname;
	TFile *fout = new TFile(sfoutputname.c_str(), "recreate");
	TTree *t = new TTree("t", "t");
	RStream::TStream astream("va", t);

	ReadHepMCFile f(args.get("--file").c_str());
	while (f.NextEvent())
	{
		pbar.Update();
		// Ldebug << "number of particles:" << f.HepMCParticles(false).size();
		// Ldebug << "number of final particles:" << f.HepMCParticles(true).size();

		double total_pt = 0.0;

		std::vector<HepMC::GenParticle*> particles = f.HepMCParticles(true);
		std::vector<VoronoiUtil::point_2d_t> particle_reco_area_estimation;

		for (std::vector<HepMC::GenParticle*>::const_iterator iterator = particles.begin(); iterator != particles.end(); iterator++)
		{
			const HepMC::GenParticle *p = *iterator;
			HepMC::FourVector v4 = p->momentum();
			// Ltrace << "eta of a particle: " << v4.eta();
			if (TMath::Abs(v4.eta()) < etamax)
			{
				total_pt += v4.perp();
				if (random_flag == false)
				{
					particle_reco_area_estimation.push_back( VoronoiUtil::point_2d_t(v4.eta(), v4.phi()));
				}
				else
				{
					double eta = 2. * gRandom->Rndm() * etamax - etamax;
					double phi = 2. * gRandom->Rndm() * TMath::Pi() - TMath::Pi();
					particle_reco_area_estimation.push_back( VoronoiUtil::point_2d_t(eta, phi) );
				}
			}
		}

		if (particle_reco_area_estimation.size() == 0) continue;

		double multiplicity = double(particle_reco_area_estimation.size());
		double total_reco_area = 0.0;
		double total_reco_area_ghost = 0.0;
		VoronoiUtil::EventArea ea(ea_setup, particle_reco_area_estimation, true);
		ea.TotalArea(total_reco_area, total_reco_area_ghost);

		astream << "deta" << deta;
		astream << "mult" << multiplicity;
		astream << "total_pt" << total_pt;
		astream << "a_parts" << total_reco_area;
		astream << "a_ghost" << total_reco_area_ghost;
		astream << "acc" << acceptance;
		astream << endl;

		if (nevents > 0 && pbar.NCalls() >= nevents) break;
	}

	fout->Write();
	fout->Close();
	delete fout;
	return 0;
};
