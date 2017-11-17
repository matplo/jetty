#ifndef __PYUTIL__HH
#define __PYUTIL__HH

#include <Pythia8/Pythia.h>
#include <vector>
#include <exception>
#include <iostream>
#include <fstream>

#include "util/args.h"

#include <TParticle.h>

namespace PyUtil
{
	Pythia8::Pythia *make_pythia(const SysUtil::Args &args);

    double sqrts(double _eA, double _eB, double mA = 0.93827, double mB = 0.93827);

	void PrintParticle(const Pythia8::Particle &p);
	void PrintParticle(const Pythia8::Event  &event, 	int idx);
	void PrintParticle(const Pythia8::Pythia &pythia, 	int idx);

	void PrintEventInfo(const Pythia8::Pythia &pythia);

	std::vector<int> GetDaughters	(const Pythia8::Event &event, int idx, int minID = 0, int maxID = 10000, bool quiet = true);
	std::vector<int> FollowDaughters(const Pythia8::Event &event, int idx, int minID = 0, int maxID = 10000, bool quiet = true);

	TParticle TParticleFromPythia(const Pythia8::Particle &p);
	std::vector<TParticle> TParticlesFromPythia(const Pythia8::Pythia &py);

	TLorentzVector TLVFromPythia(const Pythia8::Particle &p);
	TLorentzVector total_vector_final_particles(const Pythia8::Pythia &py, bool no_beam_particles = true);

	double total_et_from_final_particles(const Pythia8::Pythia &py, bool no_beam_particles = true);

	std::vector<std::pair <double, double>> make_pThat_bins(std::vector<double> ptbins);
	std::vector<double> hard_bins_from_string(const std::string &s); //const SysUtil::Args &args)

	class bad_mothers_exception: public std::exception
	{
		virtual const char* what() const throw()
		{
			return "Bad number of mothers as compared to requested.";
		}
	};

	double delta_pz_mother(const int i, const Pythia8::Event &event, const int maxmothers = 1);
	std::string pypart_to_str(const Pythia8::Particle &p);

	int has_beam_mother(const Pythia8::Particle &p);

	bool is_prompt_photon(const int i, const Pythia8::Event &event);
	bool is_decay_photon(const int i, const Pythia8::Event &event);

	int z0_index(const Pythia8::Event &event);
	std::vector<int> prompt_photon_indexes(const Pythia8::Event &event);
	std::vector<int> decay_photon_indexes(const Pythia8::Event &event);

};
#endif // __PYUTIL__HH
