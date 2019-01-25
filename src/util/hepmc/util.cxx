#include <jetty/util/hepmc/util.h>

#include <HepMC/GenEvent.h>
// #include <HepMC/GenParticle.h>
// #include <HepMC/GenVertex.h>

#include <jetty/util/blog.h>
#include <TLorentzVector.h>

namespace HepMCUtil
{
	bool is_beam(HepMC::GenParticle *p)
	{
		HepMC::GenEvent *ev = p->parent_event();
		auto bparts = ev->beam_particles();
		if (p == bparts.first or p == bparts.second)
			return true;
		return false;
	}

	bool is_off_beam_electron(HepMC::GenParticle *p)
	{
		// HepMC::GenEvent *ev = p->parent_event();
		if ( p->production_vertex() )
		{
		    for ( HepMC::GenVertex::particle_iterator mother = p->production_vertex()->particles_begin(HepMC::parents);
		          mother != p->production_vertex()->particles_end(HepMC::parents);
		          ++mother )
		    {
		        // std::cout << "-mother beam? : " << is_beam(*mother) << " \t";
		        // (*mother)->print();
	    		if ((*mother)->pdg_id() != 11 && is_beam(*mother))
	    			continue;
		    	if ((*mother)->pdg_id() == 11 && is_beam(*mother))
		    		return true;
		    	if ((*mother)->pdg_id() == 11)
		    		return is_off_beam_electron(*mother);
	    		return false;
		    }
		}
		return true;
	}

	std::vector<HepMC::GenParticle*> find_outgoing_electron(HepMC::GenEvent *ev, bool debug)
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
							bool is_beam_recoil = is_off_beam_electron((*des));
							if (debug)
							{
								Linfo << "descendant and an electron status=" << (*des)->status();
								(*des)->print( std::cout );
								Linfo << "all mothers electrons? : " << is_beam_recoil;
							}
							if (is_beam_recoil)
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
		auto bparts = ev->beam_particles ();
		retv.push_back(bparts.first);
		retv.push_back(bparts.second);
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

	HepMC::GenParticle* eIC_hadron_beam(HepMC::GenEvent *ev)
	{
		auto beams = beam_particles(ev);
		for (auto b : beams)
		{
			if (b->pdg_id() != 11)
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


	EICkine::EICkine(HepMC::GenEvent *ev)
		: fQ2(0), fW(0), fx(0), fy(0)
		, fInev(), fOutev(), fInOutev(), fInhv()
	{
		HepMC::GenParticle* be = eIC_electron_beam(ev);
		if (be == 0x0)
		{
			Lerror << "no electron beam?";
			return;
		}
		auto oes = find_outgoing_electron(ev);
		if (oes.size() < 1)
		{
			Lerror << "unable to find the outgoing electron in the event record!";
			return;
		}
		HepMC::GenParticle *oe = oes[0];
		fInev.SetPxPyPzE(be->momentum().px(), be->momentum().py(), be->momentum().pz(), be->momentum().e());
		fOutev.SetPxPyPzE(oe->momentum().px(), oe->momentum().py(), oe->momentum().pz(), oe->momentum().e());
		fInOutev = fInev - fOutev;
		fQ2 = fInOutev.Mag2() * -1.;

		HepMC::GenParticle* bh = eIC_hadron_beam(ev);
		if (bh == 0x0)
		{
			Lerror << "no non electron beam?";
			return;
		}

		fInhv.SetPxPyPzE(bh->momentum().px(), bh->momentum().py(), bh->momentum().pz(), bh->momentum().e());

	    fW    = (fInhv + fInOutev).Mag2();
    	fx    = fQ2 / (2. * fInhv.Dot(fInOutev));
	    fy    = fInhv.Dot(fInOutev) / fInhv.Dot(fInev);
	}

	std::string EICkine::dump()
	{
		std::ostringstream s;
		s << "Q2 = " << fQ2 <<
			" W = " << fW <<
			" x = " << fx <<
			" y = " << fy;
		return s.str();
	}

	std::ostringstream EICkine::as_oss(const char *prefix)
	{
		std::ostringstream s;
		s <<
			prefix <<"Q2" << " " << fQ2 << " " <<
			prefix <<"W"  << " " << fW << " "  <<
			prefix <<"x"  << " " << fx << " "  <<
			prefix <<"y"  << " " << fy;
		return s;
	}
}
