#include <jetty/util/pythia/event_pool.h>
#include <jetty/util/blog.h>
#include <TRandom.h>

namespace PyUtil
{
	EventPool::EventPool()
		: fPool()
		, fAllParticles()
		, fFinalParticles()
		{
			;
		}

	const std::vector<Pythia8::Particle> & EventPool::GetAllParticles()
	{
		fAllParticles.clear();
		for (unsigned int ie = 0; ie < fPool.size(); ie++)
		{
			for (unsigned int ip = 0; ip < fPool[ie].size(); ip++)
			{
				fAllParticles.push_back(fPool[ie][ip]);
			}
		}
		return fAllParticles;
	}

	const std::vector<Pythia8::Particle> & EventPool::GetFinalParticles()
	{
		fFinalParticles.clear();
		for (unsigned int ie = 0; ie < fPool.size(); ie++)
		{
			for (unsigned int ip = 0; ip < fPool[ie].size(); ip++)
			{
				if (fPool[ie][ip].isFinal())
					fFinalParticles.push_back(fPool[ie][ip]);
			}
		}
		return fFinalParticles;
	}

	Pythia8::Particle EventPool::RandomParticle(bool remove_event)
	{
		unsigned int ie = gRandom->Rndm() * fPool.size();
		unsigned int ip = gRandom->Rndm() * fPool[ie].size();
		Pythia8::Particle p(fPool[ie][ip]);
		if (remove_event)
		{
			fPool.erase(fPool.begin() + ie);
		}
		return p;
	}

};
