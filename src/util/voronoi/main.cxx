#include "varea_calc.h"
#include "util/args.h"
#include "util/blog.h"
#include "util/hepmc/readfile.h"

int main ( int argc, char *argv[] )
{
	SysUtil::Args args(argc, argv);
	Linfo << args.asString();

	ReadHepMCFile f(args.get("--file").c_str());
	while (f.NextEvent())
	{
		Linfo << "number of particles:" << f.HepMCParticles(false).size();
		Linfo << "number of final particles:" << f.HepMCParticles(true).size();

		std::vector<HepMC::GenParticle*> particles = f.HepMCParticles(true);
		std::vector<VoronoiUtil::point_2d_t> particle_reco_area_estimation;

		for (std::vector<HepMC::GenParticle*>::const_iterator iterator = particles.begin(); iterator != particles.end(); iterator++)
		{
			const HepMC::GenParticle *p = *iterator;
			HepMC::FourVector v4 = p->momentum();
			particle_reco_area_estimation.push_back( VoronoiUtil::point_2d_t(v4.eta(), v4.phi()));
		}

		std::vector<double> particle_reco_area;
		std::vector<std::set<size_t> > particle_reco_incident;

		VoronoiUtil::voronoi_area_incident(particle_reco_area, particle_reco_incident, particle_reco_area_estimation);

		for (size_t i = 0; i < particle_reco_area.size(); i++)
		{
			Linfo << "particle " << i << " area = " << particle_reco_area[i];
		}
	}

	return 0;
};

