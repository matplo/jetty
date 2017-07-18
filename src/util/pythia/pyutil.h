#ifndef __PYUTIL__HH
#define __PYUTIL__HH

#include <Pythia8/Pythia.h>
#include <vector>
#include <exception>
#include <iostream>
#include <fstream>

#include "util/args.h"

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

	class CrossSections
	{
	public:
		CrossSections(Pythia8::Pythia &pythia, const char *fname = 0)
			: codes()
			, xsec()
			, xsec_err()
			, eA()
			, eB()
			, eCM()
		{
			std::ofstream foutput(fname, std::ofstream::out);
			if (!foutput.good())
			{
				std::cerr << "[e] output file " << fname << "not ok." << std::endl;
				fname = 0;
			}
			else
			{
				std::cout << "[i] xsections go to a file " << fname << std::endl;
			}

			eCM = pythia.parm("Beams:eCM");
			eA  = pythia.parm("Beams:eA");
			eB  = pythia.parm("Beams:eB");
			if (pythia.mode("Beams:frameType") == 1)
			{
				eA = eCM / 2;
				eB = eCM / 2;
			}
			if (fname)
			{
				foutput << "Beams:eCM="	<< eCM 	<< std::endl;
				foutput << "Beams:eA=" 	<< eA 	<< std::endl;
				foutput << "Beams:eB=" 	<< eB 	<< std::endl;
			}

			auto _codes = pythia.info.codesHard();
			_codes.push_back(0);
			for ( auto &xcode : _codes)
			{
				double xs     = pythia.info.sigmaGen(xcode);
				double xs_err = pythia.info.sigmaErr(xcode);
				codes.push_back(xcode);
				xsec.push_back(xs);
				xsec_err.push_back(xs_err);
				// std::cout << xcode << " " << xs << " " << xs_err << std::endl;
				if (fname)
				{
					foutput << "XSec:" << xcode << "=" << xs << std::endl;
					foutput << "XSecErr:" << xcode << "=" << xs_err << std::endl;
				}
			}
			foutput << "weightSum=" << pythia.info.weightSum() << std::endl;
			foutput.close();
		}

		double xsection_for_code(const int &c)
		{
			double err = 0;
			return xsection_for_code(c, err);
		}

		double xsection_for_code(const int &c, double &err)
		{
			for ( auto i : codes)
			{
				if (c == codes[i])
				{
					err = xsec_err[i];
					return xsec[i];
				}
			}
			err = -1;
			return -1;
		}

		~CrossSections() {;}

		std::vector<int> 	codes;
		std::vector<double> xsec;
		std::vector<double> xsec_err;

		double eA;
		double eB;
		double eCM;
	};

};
#endif // __PYUTIL__HH
