#include "crosssections.h"

#include "util/strutil.h"
#include "util/rstream/tstream.h"

#include <fstream>
#include <iostream>

namespace PyUtil
{
	CrossSections::CrossSections(Pythia8::Pythia &pythia, const char *fname)
		: codes()
		, xsec()
		, xsec_err()
		, eA()
		, eB()
		, eCM()
	{
		eCM = pythia.parm("Beams:eCM");
		eA  = pythia.parm("Beams:eA");
		eB  = pythia.parm("Beams:eB");
		if (pythia.mode("Beams:frameType") == 1)
		{
			eA = eCM / 2;
			eB = eCM / 2;
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
		}
		if (fname)
		{
			std::ofstream foutput(fname, std::ofstream::out);
			if (!foutput.good())
			{
				Lerror << "output file " << fname << "not ok.";
				Linfo << "xsections... ";
				Linfo << "Beams:eCM="	<< eCM 	;
				Linfo << "Beams:eA=" 	<< eA 	;
				Linfo << "Beams:eB=" 	<< eB 	;
				Linfo << "weightSum=" << pythia.info.weightSum() ;
				for ( unsigned int i = 0; i < codes.size(); i++)
				{
					Linfo << "XSec:" << i << "=" << xsec[i] ;
					Linfo << "XSecErr:" << i << "=" << xsec_err[i] ;
				}
			}
			else
			{
				Linfo << "xsections go to a file " << fname;
				foutput << "Beams:eCM="	<< eCM 	<< std::endl;
				foutput << "Beams:eA=" 	<< eA 	<< std::endl;
				foutput << "Beams:eB=" 	<< eB 	<< std::endl;
				foutput << "weightSum=" << pythia.info.weightSum() << std::endl;
				for ( unsigned int i = 0; i < codes.size(); i++)
				{
					foutput << "XSec:" << i << "=" << xsec[i] << std::endl;
					foutput << "XSecErr:" << i << "=" << xsec_err[i] << std::endl;
				}
				foutput.close();
			}
		}
	}
};

TTree & operator << (TTree & t, const PyUtil::CrossSections &xs)
{
	RStream::TStream e("", &t);
	e << "eA" << xs.eA;
	e << "eB" << xs.eB;
	e << "eCM" << xs.eCM;

	for (unsigned int i = 0; i < xs.codes.size(); i++)
	{
		// std::cout << xs.codes[i] << " xsec=" << xs.xsec[i] << " +- " << xs.xsec_err[i] << std::endl;
		e << StrUtil::sT(xs.codes[i]) + "_x_sec" << xs.xsec[i];
		e << StrUtil::sT(xs.codes[i]) + "_x_err" << xs.xsec_err[i];
	}
	e << std::endl;
	return t;
}
