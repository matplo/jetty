#include "event_area.h"
#include <cmath>

#include <util/blog.h>

#include <TRandom.h>
#include <TMath.h>

namespace VoronoiUtil
{
	EventArea::EventArea(const EventAreaSetup &setup, const std::vector<VoronoiUtil::point_2d_t> &particles, bool use_ghosts)
		: fParticles()
		, fParticlesAndGhosts()
		, fGhosts()
		, fValidGhosts()
		, fParticleArea()
		, fParticleIncident()
		{
			fParticles.insert(fParticles.end(), particles.begin(), particles.end());
			fParticlesAndGhosts.insert(fParticlesAndGhosts.end(), particles.begin(), particles.end());
			if (use_ghosts == false)
			{
				VoronoiUtil::voronoi_area_incident(fParticleArea, fParticleIncident, fParticlesAndGhosts, setup.Eta());
			}
			else
			{
				AddGhosts(setup);

				std::vector<double> _parea;
				std::vector<std::set<size_t> > _p_incident;
				std::vector<VoronoiUtil::point_2d_t> _p_and_g;
				_p_and_g.insert(_p_and_g.end(), particles.begin(), particles.end());
				_p_and_g.insert(_p_and_g.end(), fGhosts.begin(), fGhosts.end());
				VoronoiUtil::voronoi_area_incident(_parea, _p_incident, _p_and_g, setup.Eta());

				for (unsigned int i = fParticles.size(); i < _parea.size(); i++)
				{
					if (setup.MinGhostArea() > _parea[i])
					{
						fValidGhosts.insert(fValidGhosts.end(), _p_and_g[i]);
					}
				}
				Ldebug << "valid nghosts : " << fValidGhosts.size();
				fParticlesAndGhosts.insert(fParticlesAndGhosts.end(), fValidGhosts.begin(), fValidGhosts.end());
				VoronoiUtil::voronoi_area_incident(fParticleArea, fParticleIncident, fParticlesAndGhosts, setup.Eta());
			}
		}

	void EventArea::AddGhosts(const EventAreaSetup &setup)
	{
		double deta = setup.Eta() * 2.;
		Long64_t nghosts = setup.NGhosts() * deta;
		if (setup.FixedNGhosts())
			nghosts = setup.NGhosts();
		for (Long64_t i = 0; i < nghosts; i++)
		{
			double eta = 2. * gRandom->Rndm() * setup.Eta() - setup.Eta();
			double phi = 2. * gRandom->Rndm() * TMath::Pi();
			fGhosts.push_back( VoronoiUtil::point_2d_t(eta, phi) );
		}
		Ldebug << "nghosts added : " << fGhosts.size();
	}

	EventArea::EventArea(const std::vector<VoronoiUtil::point_2d_t> &particles, double _etamax)
		: fParticles()
		, fParticlesAndGhosts()
		, fGhosts()
		, fValidGhosts()
		, fParticleArea()
		, fParticleIncident()
		{
			fParticles.insert(fParticles.end(), particles.begin(), particles.end());
			Ldebug << "number of particles : " << fParticles.size();
			fParticlesAndGhosts.insert(fParticlesAndGhosts.end(), particles.begin(), particles.end());
			Ldebug << "number of particles & ghosts : " << fParticlesAndGhosts.size();
			double etamax = _etamax;
			if (etamax <= 0)
				for (unsigned int i = 0; i < fParticles.size(); i++)
					if (etamax < std::fabs(fParticles[i].x()))
						etamax = std::fabs(fParticles[i].x());
			Ldebug << "etamax = " << etamax;
			VoronoiUtil::voronoi_area_incident(fParticleArea, fParticleIncident, fParticlesAndGhosts, etamax);
		}

	EventArea::EventArea(const EventAreaSetup &setup, const std::vector<VoronoiUtil::point_2d_t> &particles, const std::vector<VoronoiUtil::point_2d_t> &ghosts)
		: fParticles()
		, fParticlesAndGhosts()
		, fGhosts()
		, fValidGhosts()
		, fParticleArea()
		, fParticleIncident()
		{
			fParticles.insert(fParticles.end(), particles.begin(), particles.end());
			fParticlesAndGhosts.insert(fParticlesAndGhosts.end(), particles.begin(), particles.end());
			fGhosts.insert(fGhosts.end(), ghosts.begin(), ghosts.end());

			std::vector<double> _parea;
			std::vector<std::set<size_t> > _p_incident;
			std::vector<VoronoiUtil::point_2d_t> _p_and_g;
			_p_and_g.insert(_p_and_g.end(), particles.begin(), particles.end());
			_p_and_g.insert(_p_and_g.end(), ghosts.begin(), ghosts.end());
			VoronoiUtil::voronoi_area_incident(_parea, _p_incident, _p_and_g, setup.Eta());

			for (unsigned int i = fParticles.size(); i < _parea.size(); i++)
			{
				if (setup.MinGhostArea() > _parea[i])
				{
					fValidGhosts.insert(fValidGhosts.end(), _p_and_g[i]);
				}
			}
			fParticlesAndGhosts.insert(fParticlesAndGhosts.end(), fValidGhosts.begin(), fValidGhosts.end());
			VoronoiUtil::voronoi_area_incident(fParticleArea, fParticleIncident, fParticlesAndGhosts, setup.Eta());
		}

};
