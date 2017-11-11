#include <util/args.h>
#include <util/blog.h>
#include <util/strutil.h>
#include <util/looputil.h>
#include <util/hepmc/readfile.h>

#include <TFile.h>
#include <TProfile.h>
#include <TRandom.h>

#include <string>

#include <boost/algorithm/string.hpp>

#include <util/voronoi/varea_calc.h>
#include <util/voronoi/event_area.h>

using namespace std;

int simple ( int argc, char *argv[] )
{
	SysUtil::Args args(argc, argv);
	Linfo << args.asString();

	double etamax = TMath::Abs(args.getD("--etamax", 0.9));
	string sfoutputname = args.get("--output", "voronoi_area_incident_t2.root");
	if (sfoutputname == "voronoi_area_incident_t2.root")
	{
		string ext = StrUtil::sT("_etamax_") + StrUtil::sT(etamax) + StrUtil::sT(".root");
		boost::algorithm::replace_last(sfoutputname, ".root", ext);
	}

	double deta = etamax * 2.;
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
	VoronoiUtil::EventAreaSetup ea_setup(etamax, TMath::Abs(args.getI("--nghost-eta", 100)), min_ghost_area, args.isSet("--fixed_nghosts"));
	ea_setup.Dump();

	Stat_t nevents = args.getI("--nev", 0);
	LoopUtil::TPbar pbar(nevents);

	double max_mult = 3000.;

	Linfo << "output goes to : " << sfoutputname;
	TFile *fout = new TFile(sfoutputname.c_str(), "recreate");
	TProfile *hpmult = new TProfile("hpmult", "hpmult", 100, 0, 100);
	TProfile *hpareaM = new TProfile("hpareaM", "hpareaM", 100, 0, max_mult);
	TProfile *hptotareaM = new TProfile("hptotareaM", "hptotareaM", 100, 0, max_mult);
	TProfile *hptotareasfractionM = new TProfile("hptotareasfractionM", "hptotareasfractionM", 100, 0, max_mult);

	TProfile *hpareaghostM = new TProfile("hpareaghostM", "hpareaghostM", 100, 0, max_mult);
	TProfile *hptotareaghostM = new TProfile("hptotareaghostM", "hptotareaghostM", 100, 0, max_mult);
	TProfile *hptotareaghostfractionM = new TProfile("hptotareaghostfractionM", "hptotareaghostfractionM", 100, 0, max_mult);

	TProfile *hptotareasfractionMdeta = new TProfile("hptotareasfractionMdeta", "hptotareasfractionMdeta", 100, 0, max_mult / deta);
	TProfile *hptotareaghostfractionMdeta = new TProfile("hptotareaghostfractionMdeta", "hptotareaghostfractionMdeta", 100, 0, max_mult / deta);

	ReadHepMCFile f(args.get("--file").c_str());
	while (f.NextEvent())
	{
		pbar.Update();
		// Ldebug << "number of particles:" << f.HepMCParticles(false).size();
		// Ldebug << "number of final particles:" << f.HepMCParticles(true).size();

		std::vector<HepMC::GenParticle*> particles = f.HepMCParticles(true);
		std::vector<VoronoiUtil::point_2d_t> particle_reco_area_estimation;

		for (std::vector<HepMC::GenParticle*>::const_iterator iterator = particles.begin(); iterator != particles.end(); iterator++)
		{
			const HepMC::GenParticle *p = *iterator;
			HepMC::FourVector v4 = p->momentum();
			// Ltrace << "eta of a particle: " << v4.eta();
			if (TMath::Abs(v4.eta()) < etamax)
			{
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

		double multiplicity = double(particle_reco_area_estimation.size());
		//Ldebug << "multiplicity of final particles in |eta| <" << etamax << " :" << multiplicity;
		hpmult->Fill(pbar.NCalls() % 100, multiplicity);

		double total_reco_area = 0.0;
		double total_reco_area_ghost = 0.0;
		//VoronoiUtil::EventArea ea(particle_reco_area_estimation, -1);
		// VoronoiUtil::EventArea ea(ea_setup, particle_reco_area_estimation, ghosts);
		VoronoiUtil::EventArea ea(ea_setup, particle_reco_area_estimation, true);
		ea.TotalArea(total_reco_area, total_reco_area_ghost);

		hptotareaM->Fill(multiplicity, total_reco_area);
		hptotareasfractionM->Fill(multiplicity, total_reco_area);
		hptotareasfractionMdeta->Fill(multiplicity/deta, total_reco_area);

		hptotareaghostM->Fill(multiplicity, total_reco_area_ghost);
		hptotareaghostfractionM->Fill(multiplicity, total_reco_area_ghost);
		hptotareaghostfractionMdeta->Fill(multiplicity/deta, total_reco_area_ghost);

		if (nevents > 0 && pbar.NCalls() >= nevents) break;
	}

	double acceptance = 2.*etamax*2.*TMath::Pi();
	// hptotareasfractionM->Sumw2();
	if (etamax>0)
	{
		hptotareasfractionM->Scale(1./acceptance);
		hptotareasfractionMdeta->Scale(1./acceptance);
		hptotareaghostfractionM->Scale(1./acceptance);
		hptotareaghostfractionMdeta->Scale(1./acceptance);
	}
	fout->Write();
	fout->Close();
	delete fout;
	return 0;
};
