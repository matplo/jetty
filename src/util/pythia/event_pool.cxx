#include <jetty/util/pythia/event_pool.h>
#include <jetty/util/pythia/pdg_mass.h>
#include <jetty/util/blog.h>
#include <TRandom.h>

namespace PyUtil
{
	EventPool::EventPool()
		: fPool()
		, fAllParticles()
		, fFinalParticles()
		, fFinalPseudoJets()
		{
			;
		}

	const std::vector<Pythia8::Particle> & EventPool::GetAllParticles()
	{
		fAllParticles.clear();
		for (unsigned int ie = 0; ie < fPool.size(); ie++)
		{
			for (int ip = 0; ip < fPool[ie].size(); ip++)
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
			for (int ip = 0; ip < fPool[ie].size(); ip++)
			{
				if (fPool[ie][ip].isFinal())
					fFinalParticles.push_back(fPool[ie][ip]);
			}
		}
		return fFinalParticles;
	}

	const std::vector<fastjet::PseudoJet> & EventPool::GetFinalPseudoJets()
	{
		fFinalPseudoJets.clear();
		for (unsigned int ie = 0; ie < fPool.size(); ie++)
		{
			for (int ip = 0; ip < fPool[ie].size(); ip++)
			{
				if (fPool[ie][ip].isFinal())
				{
					fastjet::PseudoJet psj ( fPool[ie][ip].px(), fPool[ie][ip].py(), fPool[ie][ip].pz(), fPool[ie][ip].e());
					psj.set_user_index(ie * 10000 + ip);
					if (ip > 10000)
						Lwarn << "setting user index particle beyond offset 10k - not reliable - needs a fix";
					fFinalPseudoJets.push_back(psj);
					// PDGMass::Instance().Add(fPool[ie][ip].m(), fPool[ie][ip].id());
					PDGMass::Instance().Add(psj.m(), fPool[ie][ip].id());
				}
			}
		}
		return fFinalPseudoJets;
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

	void EventPool::Clear()
	{
		fPool.clear();
		fAllParticles.clear();
		fFinalParticles.clear();
	}
};
