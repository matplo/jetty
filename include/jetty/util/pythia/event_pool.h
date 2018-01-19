#ifndef __JETTY_PYTHIA_EVENT_POOL__HH
#define __JETTY_PYTHIA_EVENT_POOL__HH

#include <Pythia8/Pythia.h>
#include <Pythia8/Event.h>
#include <vector>

namespace PyUtil
{
	class EventPool
	{
	public:
		EventPool();
		const std::vector<Pythia8::Particle> & GetAllParticles();
		const std::vector<Pythia8::Particle> & GetFinalParticles();
		Pythia8::Particle RandomParticle(bool remove_event = true);
		const std::vector<Pythia8::Event> & GetPool() const
		{
			return fPool;
		}
		void AddEvent(Pythia8::Event &ev) { fPool.push_back(ev); }
		void RemoveEvent(unsigned int idx) { if (idx < fPool.size()) fPool.erase(fPool.begin() + idx); }
		void Clear();
	private:
		std::vector<Pythia8::Event> 	fPool;
		std::vector<Pythia8::Particle> 	fAllParticles;
		std::vector<Pythia8::Particle> 	fFinalParticles;
	};
};

#endif
