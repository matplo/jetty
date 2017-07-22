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
		CrossSections(Pythia8::Pythia &pythia, const char *fname = 0);

		double xsection_for_code(const int &c)
		{
			double err = 0;
			return xsection_for_code(c, err);
		}

		double xsection_for_code(const int &c, double &err)
		{
			for ( unsigned int i = 0; i < codes.size(); i++)
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
