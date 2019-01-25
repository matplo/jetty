#ifndef __HEPMCUTIL_UTIL__hh
#define __HEPMCUTIL_UTIL__hh

namespace HepMC
{
	class GenEvent;
	class GenParticle;
}

#include <vector>

namespace HepMCUtil
{
	std::vector<HepMC::GenParticle*> find_outgoing_electron(HepMC::GenEvent *ev);
}

#endif
