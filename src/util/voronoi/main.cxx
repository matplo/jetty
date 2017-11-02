#include "varea_calc.h"
#include "util/args.h"
#include "util/blog.h"
#include "util/looputil.h"
#include "util/hepmc/readfile.h"

#include <TFile.h>
#include <TProfile.h>
#include <TRandom.h>

#include <string>
using namespace std;

int main ( int argc, char *argv[] )
{
	SysUtil::Args args(argc, argv);
	Linfo << args.asString();

	double etamax = TMath::Abs(args.getD("--etamax", 0.9));
	string sfoutputname = args.get("--output", "voronoi_area_incident.root");

	Stat_t nevents = args.getI("--nev", 0);
	LoopUtil::TPbar pbar(nevents);

	double max_mult = 3000.;

	TFile *fout = new TFile(sfoutputname.c_str(), "recreate");
	TProfile *hpmult = new TProfile("hpmult", "hpmult", 100, 0, 100);
	TProfile *hpareaM = new TProfile("hpareaM", "hpareaM", 100, 0, max_mult);
	TProfile *hptotareaM = new TProfile("hptotareaM", "htotareaM", 100, 0, max_mult);
	TProfile *hptotareasfractionM = new TProfile("hptotareasfractionM", "htotareasfractionM", 100, 0, max_mult);

	TProfile *hptotareaghostM = new TProfile("hptotareaghostM", "htotareaghostM", 100, 0, max_mult);

	Long64_t nghosts = TMath::Abs(args.getI("--nseeds", 100));

	ReadHepMCFile f(args.get("--file").c_str());
	while (f.NextEvent())
	{
		pbar.Update();
		Ldebug << "number of particles:" << f.HepMCParticles(false).size();
		Ldebug << "number of final particles:" << f.HepMCParticles(true).size();

		std::vector<HepMC::GenParticle*> particles = f.HepMCParticles(true);
		std::vector<VoronoiUtil::point_2d_t> particle_reco_area_estimation;

		for (std::vector<HepMC::GenParticle*>::const_iterator iterator = particles.begin(); iterator != particles.end(); iterator++)
		{
			const HepMC::GenParticle *p = *iterator;
			HepMC::FourVector v4 = p->momentum();
			Ltrace << "eta of a particle: " << v4.eta();
			if (TMath::Abs(v4.eta()) < etamax)
				particle_reco_area_estimation.push_back( VoronoiUtil::point_2d_t(v4.eta(), v4.phi()));
		}

		double multiplicity = double(particle_reco_area_estimation.size());
		Ldebug << "multiplicity of final particles in |eta| <" << etamax << " :" << multiplicity;
		hpmult->Fill(pbar.NCalls() % 100, multiplicity);

		// now add some ghosts
		std::vector<VoronoiUtil::point_2d_t> ghosts;
		for (Long64_t i = 0; i < nghosts; i++)
		{
			double eta = 2. * gRandom->Rndm() * 0.9 - 0.9;
			double phi = 2. * gRandom->Rndm() * TMath::Pi();
			particle_reco_area_estimation.push_back( VoronoiUtil::point_2d_t(eta, phi) );
			ghosts.push_back( VoronoiUtil::point_2d_t(eta, phi) );
		}

		std::vector<double> particle_reco_area;
		std::vector<std::set<size_t> > particle_reco_incident;

		VoronoiUtil::voronoi_area_incident(particle_reco_area, particle_reco_incident, particle_reco_area_estimation);

		double total_reco_area = 0.0;
		double total_reco_area_ghost = 0.0;

		for (size_t i = 0; i < particle_reco_area.size(); i++)
		{
			// Linfo << "particle " << i << " area = " << particle_reco_area[i];
			hpareaM->Fill(multiplicity, particle_reco_area[i]);
			if ( i < multiplicity)
				total_reco_area = total_reco_area + particle_reco_area[i];
			else
				total_reco_area_ghost = total_reco_area_ghost + particle_reco_area[i];
		}

		hptotareaM->Fill(multiplicity, total_reco_area);
		hptotareaghostM->Fill(multiplicity, total_reco_area_ghost);
		hptotareasfractionM->Fill(multiplicity, total_reco_area);

		if (nevents > 0 && pbar.NCalls() >= nevents) break;
	}

	// hptotareasfractionM->Sumw2();
	if (etamax>0)
		hptotareasfractionM->Scale(1./(2.*etamax*2.*TMath::Pi()));
	fout->Write();
	fout->Close();
	delete fout;
	return 0;
};

