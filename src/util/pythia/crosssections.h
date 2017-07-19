#ifndef __PYUTIL_XSECTIONS__HH
#define __PYUTIL_XSECTIONS__HH

#include <Pythia8/Pythia.h>
#include <vector>
#include <exception>
#include <iostream>
#include <fstream>

#include "util/blog.h"

#include <TTree.h>

namespace PyUtil
{
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
				Lerror << "output file " << fname << "not ok.";
				fname = 0;
			}
			else
			{
				Linfo << "xsections go to a file " << fname;
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
				// Linfo << xcode << " " << xs << " " << xs_err << std::endl;
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

TTree & operator << (TTree & t, const PyUtil::CrossSections &xs);

#endif  // __PYUTIL_XSECTIONS__HH
