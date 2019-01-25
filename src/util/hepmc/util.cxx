#include <jetty/util/hepmc/util.h>

#include <HepMC/GenEvent.h>
// #include <HepMC/GenParticle.h>
// #include <HepMC/GenVertex.h>

#include <jetty/util/blog.h>
#include <TLorentzVector.h>

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

	std::vector<HepMC::GenParticle*> beam_particles(HepMC::GenEvent *ev)
	{
		std::vector<HepMC::GenParticle*> retv;
        for ( HepMC::GenEvent::particle_iterator p = ev->particles_begin();
             p != ev->particles_end();
             ++p )
        {
        	// find the beam electron
			if ((*p)->is_beam())
			{
				retv.push_back(*p);
			}
		}
		return retv;
	}

	HepMC::GenParticle* eIC_electron_beam(HepMC::GenEvent *ev)
	{
		auto beams = beam_particles(ev);
		for (auto b : beams)
		{
			if (b->pdg_id() == 11)
				return b;
		}
		return 0x0;
	}

	double eIC_Q2 (HepMC::GenEvent *ev)
	{
		HepMC::GenParticle* be = eIC_electron_beam(ev);
		if (be == 0x0)
		{
			Lerror << "no electron beam?";
			return 0;
		}
		auto oes = find_outgoing_electron(ev);
		if (oes.size() < 1)
		{
			Lerror << "unable to find the outgoing electron in the event record!";
			return 0;
		}
		HepMC::GenParticle *oe = 0x0;
		if (oes.size() > 1)
		{
			Lerror << "found more than 1 outgoing electron. using highest E for Q2... ";
			for (auto e : oes)
			{
				if (oe == 0x0)
					oe = e;
				else
				{
					if (e->momentum().e() > oe->momentum().e())
						oe = e;
				}
			}
		}
		else
		{
			oe = oes[0];
		}

		TLorentzVector vb;
		TLorentzVector vo;

		vb.SetPxPyPzE(be->momentum().px(), be->momentum().py(), be->momentum().pz(), be->momentum().e());
		vo.SetPxPyPzE(oe->momentum().px(), oe->momentum().py(), oe->momentum().pz(), oe->momentum().e());

		TLorentzVector result = vb - vo;

		return -1. * result.Mag2();
	}
}
