#ifndef __HEPMCUTIL_UTIL__hh
#define __HEPMCUTIL_UTIL__hh

namespace HepMC
{
	class GenEvent;
	class GenParticle;
}

#include <vector>
#include <string>
#include <sstream>
#include <TLorentzVector.h>

namespace HepMCUtil
{
	std::vector<HepMC::GenParticle*> find_outgoing_electron(HepMC::GenEvent *ev, bool debug = false);
	std::vector<HepMC::GenParticle*> beam_particles(HepMC::GenEvent *ev);
	double eIC_Q2 (HepMC::GenEvent *ev);

	class EICkine
	{
	public:
		EICkine(HepMC::GenEvent *ev);
		~EICkine() {;}
		double Q2() {return fQ2;}
		double W() {return fW;}
		double x() {return fx;}
		double y() {return fy;}
		const TLorentzVector & eInTLV() {return fInev;}
		const TLorentzVector & eOutTLV() {return fOutev;}
		const TLorentzVector & eInOutTLV() {return fInOutev;}
		const TLorentzVector & hInTLV() {return fInhv;}
		std::string 			dump();
		std::ostringstream 		as_oss(const char *prefix);

	private:
		EICkine() {;}
		double fQ2;
		double fW;
		double fx;
		double fy;
		TLorentzVector fInev;
		TLorentzVector fOutev;
		TLorentzVector fInOutev;
		TLorentzVector fInhv;
	};
}

#endif
