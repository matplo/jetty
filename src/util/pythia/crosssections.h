#ifndef __PYUTIL_XSECTIONS__HH
#define __PYUTIL_XSECTIONS__HH

#include <Pythia8/Pythia.h>
#include <vector>
#include <exception>
#include <iostream>
#include <fstream>
#include <cmath>

#include "util/blog.h"

#include <TTree.h>

namespace PyUtil
{
	class CrossSections
	{
	public:
		CrossSections(Pythia8::Pythia &pythia, const char *fname = 0);

		double xsection_for_code(const int c);
		double xsection_for_code(const int c, double &err);
		bool is_nsigma(const int icode, const double xs, const double nsigma_test = 3.);
		double nsigma(const int icode, const double xs);

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
