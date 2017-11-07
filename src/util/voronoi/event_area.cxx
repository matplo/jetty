#include "event_area.h"
#include <cmath>

#include <util/blog.h>

#include <TRandom.h>
#include <TMath.h>

namespace VoronoiUtil
{
	void EventAreaSetup::Dump()
	{
		Linfo << "- EventAreaSetup -";
		Linfo << "   max eta         : " << fMaxEta;
		Linfo << "   nghosts         : " << fNGhosts * fMaxEta * 2.;
		Linfo << "   dnghosts/deta   : " << fNGhosts;
		Linfo << "   fixed nghosts   : " << fFixedNGhosts;
		Linfo << "   min. ghost area : " << fMinGhostArea;

		unsigned int nghosts = 0;
		double ngh2 = TMath::Sqrt(NGhosts() * Eta() * 2.);
		double eta_step = 2. * Eta() / ngh2;
		double phi_step = 2. * TMath::Pi() / ngh2;
		for (double eta = -Eta(); eta < Eta(); eta = eta + eta_step)
		{
			for (double phi = -TMath::Pi(); phi < TMath::Pi(); phi = phi + phi_step)
			{
				nghosts++;
			}
		}
		Linfo << "   nghosts calc       : " << nghosts;
	}

	double EventAreaSetup::Acceptance()
	{
		if (fMaxEta == 0) return 1.e-6;
		return fMaxEta * 2. * 2. * TMath::Pi();
	}

	void EventArea::TotalArea(double &particle_area, double &ghost_area)
	{
		particle_area = 0.0;
		ghost_area = 0.0;
		for (unsigned int i = 0; i < fParticleArea.size(); i++)
		{
			if (i < fParticles.size())
				particle_area = particle_area + fParticleArea[i];
			else
				ghost_area = ghost_area + fParticleArea[i];
		}
		Ldebug << "particle area : " << particle_area << " ghost area : " << ghost_area << " => sum : " << particle_area + ghost_area;
		Ldebug << "fractions particle area : " << particle_area / fSetup.Acceptance()
			<< " ghost area : " << ghost_area / fSetup.Acceptance()
			<< " => sum : " << (particle_area + ghost_area ) / fSetup.Acceptance();
	}

	void EventArea::AddGhosts()
	{
		double deta = fSetup.Eta() * 2.;
		Long64_t nghosts = fSetup.NGhosts() * deta;
		if (fSetup.FixedNGhosts())
			nghosts = fSetup.NGhosts();
		//for (Long64_t i = 0; i < nghosts; i++)
		//{
		//	double eta = 2. * gRandom->Rndm() * fSetup.Eta() - fSetup.Eta();
		//	double phi = 2. * gRandom->Rndm() * TMath::Pi();
		//	fGhosts.push_back( VoronoiUtil::point_2d_t(eta, phi) );
		//}
		double ngh2 = TMath::Sqrt(fSetup.NGhosts() * deta);
		double eta_step = 2. * fSetup.Eta() / ngh2;
		double phi_step = 2. * TMath::Pi() / ngh2;
		for (double eta = -fSetup.Eta(); eta < fSetup.Eta(); eta = eta + eta_step)
		{
			for (double phi = -TMath::Pi(); phi < TMath::Pi(); phi = phi + phi_step)
			{
				double _eta = eta + gRandom->Rndm() * eta_step / 4.;
				double _phi = phi + gRandom->Rndm() * phi_step / 4.;
				fGhosts.push_back( VoronoiUtil::point_2d_t(_eta, _phi) );
			}
		}
		Ldebug << "nghosts added : " << fGhosts.size();
	}

	EventArea::EventArea(const EventAreaSetup &setup, const std::vector<VoronoiUtil::point_2d_t> &particles, bool use_ghosts)
		: fSetup(setup)
		, fParticles()
		, fParticlesAndGhosts()
		, fGhosts()
		, fValidGhosts()
		, fParticleArea()
		, fParticleIncident()
		{
			Ldebug << " --- " << __FUNCTION__;
			fParticles.insert(fParticles.end(), particles.begin(), particles.end());
			fParticlesAndGhosts.insert(fParticlesAndGhosts.end(), particles.begin(), particles.end());
			if (use_ghosts == false)
			{
				VoronoiUtil::voronoi_area_incident(fParticleArea, fParticleIncident, fParticlesAndGhosts, fSetup.Eta());
			}
			else
			{
				AddGhosts();

				std::vector<double> _parea;
				std::vector<std::set<size_t> > _p_incident;
				std::vector<VoronoiUtil::point_2d_t> _p_and_g;
				_p_and_g.insert(_p_and_g.end(), particles.begin(), particles.end());
				_p_and_g.insert(_p_and_g.end(), fGhosts.begin(), fGhosts.end());
				VoronoiUtil::voronoi_area_incident(_parea, _p_incident, _p_and_g, fSetup.Eta());

				for (unsigned int i = fParticles.size(); i < _parea.size(); i++)
				{
					if (_parea[i] > fSetup.MinGhostArea())
					{
						Ltrace << "keeping ghost " << _p_and_g[i].x() << ":" << _p_and_g[i].y()
								<< " with area " << _parea[i] << " > " << fSetup.MinGhostArea();
						fValidGhosts.insert(fValidGhosts.end(), _p_and_g[i]);
					}
					else
					{
						int has_particle_neighbour = 0;
						for (std::set<size_t>::iterator it=_p_incident[i].begin(); it!=_p_incident[i].end(); ++it)
						{
							if (*it < fParticles.size())
							{
								has_particle_neighbour += 1;
								// Ltrace << "keeping ghost " << _p_and_g[i].x() << ":" << _p_and_g[i].y()
								// 		<< " with area " << _parea[i]
								// 		<< " neighbour to particle : " << *it;
							}
						}
						if (has_particle_neighbour == 0)
						{
							fValidGhosts.insert(fValidGhosts.end(), _p_and_g[i]);
							Ltrace << "keeping ghost " << _p_and_g[i].x() << ":" << _p_and_g[i].y()
									<< " with area " << _parea[i]
									<< " neighbour to n-particles : " << has_particle_neighbour;
						}
					}
				}
				Ldebug << "valid nghosts : " << fValidGhosts.size();
				Ldebug << "n particles : " << fParticles.size();
				fParticlesAndGhosts.insert(fParticlesAndGhosts.end(), fValidGhosts.begin(), fValidGhosts.end());
				Ldebug << "n particles + v.ghosts : " << fParticlesAndGhosts.size();
				VoronoiUtil::voronoi_area_incident(fParticleArea, fParticleIncident, fParticlesAndGhosts, fSetup.Eta());
			}
		}

	EventArea::EventArea(const std::vector<VoronoiUtil::point_2d_t> &particles, double _etamax)
		: fSetup(_etamax)
		, fParticles()
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
		: fSetup(setup)
		, fParticles()
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
			VoronoiUtil::voronoi_area_incident(_parea, _p_incident, _p_and_g, fSetup.Eta());

			for (unsigned int i = fParticles.size(); i < _parea.size(); i++)
			{
				if (_parea[i] > fSetup.MinGhostArea())
				{
					fValidGhosts.insert(fValidGhosts.end(), _p_and_g[i]);
				}
				else
				{
					bool has_particle_neighbour = false;
					for (std::set<size_t>::iterator it=_p_incident[i].begin(); it!=_p_incident[i].end(); ++it)
					{
						if (*it < fParticles.size())
						{
							has_particle_neighbour = true;
							fValidGhosts.insert(fValidGhosts.end(), _p_and_g[i]);
							break;
						}
					}
				}
			}

			fParticlesAndGhosts.insert(fParticlesAndGhosts.end(), fValidGhosts.begin(), fValidGhosts.end());
			VoronoiUtil::voronoi_area_incident(fParticleArea, fParticleIncident, fParticlesAndGhosts, fSetup.Eta());
		}

};
