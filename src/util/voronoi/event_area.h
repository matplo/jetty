#ifndef __JETTY_EVENT_AREA_HH
#define __JETTY_EVENT_AREA_HH

#include "varea_calc.h"

namespace VoronoiUtil
{
	class EventAreaSetup
	{
	public:
		EventAreaSetup(double max_eta = 1, int n_ghosts = 100, double min_ghost_area = 0.001, bool fixed_nghosts = false)
			: fMaxEta(max_eta)
			, fNGhosts(n_ghosts)
			, fMinGhostArea(min_ghost_area)
			, fFixedNGhosts(fixed_nghosts)
			{
				;
			}

		EventAreaSetup(const EventAreaSetup &s)
			: fMaxEta(s.fMaxEta)
			, fNGhosts(s.fNGhosts)
			, fMinGhostArea(s.fMinGhostArea)
			, fFixedNGhosts(s.fFixedNGhosts)
			{
				;
			}

		double 	MaxEta() 		const {return fMaxEta;}
		double 	Eta() 			const {return fMaxEta;}
		int		NGhosts() 		const {return fNGhosts;}
		double  MinGhostArea() 	const {return fMinGhostArea;}
		bool	FixedNGhosts() 	const {return fFixedNGhosts;}
		double 	Acceptance();
		void Dump();

		virtual ~EventAreaSetup()
			{
				;
			}

	private:
		double 	fMaxEta;
		int 	fNGhosts;
		double 	fMinGhostArea;
		bool 	fFixedNGhosts;
	};

	class EventArea
	{
	public:
		EventArea(const std::vector<VoronoiUtil::point_2d_t> &particles, double _etamax);
		EventArea(const EventAreaSetup &setup, const std::vector<VoronoiUtil::point_2d_t> &particles, bool use_ghosts = false);
		EventArea(const EventAreaSetup &setup, const std::vector<VoronoiUtil::point_2d_t> &particles, const std::vector<VoronoiUtil::point_2d_t> &ghosts);

		virtual ~EventArea() {;}

		std::vector<VoronoiUtil::point_2d_t> Particles() const {return fParticles;}
		std::vector<VoronoiUtil::point_2d_t> Ghosts() const {return fGhosts;}
		std::vector<VoronoiUtil::point_2d_t> ValidGhosts() const {return fValidGhosts;}

		std::vector<double> 				 	ParticleAreas() const {return fParticleArea;}
		std::vector<std::set<size_t> > 			ParticleIncidents() const {return fParticleIncident;}

		double ParticleArea(unsigned int i)
			{
				if (i < fParticlesAndGhosts.size())
					return fParticleArea[i];
				else return -1.0;
			}

		bool IsRealParticle(unsigned int i)
		{
			if (i < fParticles.size())
				return true;
			return false;
		}

		bool IsGhost(unsigned int i)
		{
			if (i >= fParticles.size())
				return true;
			return false;
		}

		std::set<size_t> ParticleIncident(unsigned int i) const
			{
				if (i < fParticlesAndGhosts.size())
				{
					return fParticleIncident[i];
				}
				std::set<size_t> _set;
				return _set;
			}

		void TotalArea(double &particle_area, double &ghost_area);

	private:
		EventArea() {;}
		void AddGhosts();
		EventAreaSetup 							fSetup;
		std::vector<VoronoiUtil::point_2d_t> 	fParticles;
		std::vector<VoronoiUtil::point_2d_t> 	fParticlesAndGhosts;
		std::vector<VoronoiUtil::point_2d_t> 	fGhosts;
		std::vector<VoronoiUtil::point_2d_t> 	fValidGhosts;
		std::vector<double> 				 	fParticleArea;
		std::vector<std::set<size_t> > 			fParticleIncident;

	};
};

#endif
