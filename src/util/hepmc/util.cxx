#include <jetty/util/hepmc/util.h>

#include <HepMC/GenEvent.h>
// #include <HepMC/GenParticle.h>
// #include <HepMC/GenVertex.h>

namespace HepMCUtil
{
	std::vector<HepMC::GenParticle*> find_outgoing_electron(HepMC::GenEvent *ev)
	{
		std::vector<HepMC::GenParticle*> retv;
		// good links to iteration tests: http://lcgapp.cern.ch/project/simu/HepMC/205/html/classHepMC_1_1GenVertex_1_1particle__iterator.html#_details
		// Linfo << "find the outgoing electron...";
        for ( HepMC::GenEvent::particle_iterator p = ev->particles_begin();
             p != ev->particles_end();
             ++p )
        {
        	// find the beam electron
			if ((*p)->is_beam() && (*p)->pdg_id() == 11)
			{
				// Linfo << "beam and an an electron status=" << (*p)->status();
				// (*p)->print( std::cout );
				if ( (*p)->end_vertex() )
				{
					for ( HepMC::GenVertex::particle_iterator des =(*p)->end_vertex()->particles_begin(HepMC::descendants);
					     des != (*p)->end_vertex()->particles_end(HepMC::descendants);
					     ++des )
					{
						// look for a final state electron
						if ((*des)->pdg_id() == 11 && (*des)->status() == 1)
						{
							// Linfo << "descendant and an electron status=" << (*des)->status();
							// (*des)->print( std::cout );
							retv.push_back(*des);
						}
					}
				}
			}
		}
		return retv;
	}
}
