#include "crosssections.h"

#include "util/strutil.h"
#include "util/rstream/tstream.h"

TTree & operator << (TTree & t, const PyUtil::CrossSections &xs)
{
	RStream::TStream e("", &t);
	e << "eA" << xs.eA;
	e << "eB" << xs.eB;
	e << "eCM" << xs.eCM;

	for (unsigned int i = 0; i < xs.codes.size(); i++)
	{
		std::cout << xs.codes[i] << " xsec=" << xs.xsec[i] << " +- " << xs.xsec_err[i] << std::endl;
		e << StrUtil::sT(xs.codes[i]) + "_x_sec" << xs.xsec[i];
		e << StrUtil::sT(xs.codes[i]) + "_x_err" << xs.xsec_err[i];
	}
	e << std::endl;
	return t;
}
