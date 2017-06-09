#include "pyargs.h"
#include "pyutil.h"
#include "sysutil.h"

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>

#include <iostream>
#include <sstream>

namespace PyUtil
{
	//pythia args - args preprocessed
	void Args::_cook()
	{
		std::string cfgfile = get("--config");
		if (cfgfile.length() == 0)
		{
			cfgfile = "./pythia.cmnd";
			if (SysUtil::file_exists(cfgfile) == false)
			{
				cfgfile = std::getenv("JETTYDIR");
				cfgfile += std::string("/config/pythia.cmnd");
			}
		}
		if (SysUtil::file_exists(cfgfile))
			readConfig(cfgfile.c_str());

		int nEvent = getI("Main:numberOfEvents");
		int userNEvent = getI("--nev", nEvent);
		if (userNEvent > 0)
		{
			nEvent 		= userNEvent;
		}
		set("Main:numberOfEvents", nEvent);

		double pTHatMin = getD("-pTHatMin", -99);
		if (pTHatMin == -99)
			pTHatMin = getD("--pTHatMin", -99); // backward compat.
		if (pTHatMin == -99)
			pTHatMin = getD("PhaseSpace:pTHatMin", -99); // backward compat.
		if (pTHatMin >= 0)
		{
			// add(TString::Format("PhaseSpace:pTHatMin=%f", pTHatMin).Data());
			add(boost::str(boost::format("PhaseSpace:pTHatMin=%1%") % pTHatMin));
		}

		double pTHatMax = getD("--pTHatMax", -99);
		if (pTHatMax == -99)
			pTHatMax = getD("PhaseSpace:pTHatMax", -99); // backward compat.
		if (pTHatMax >= -1)
		{
			if (pTHatMax > -1 && pTHatMax < pTHatMin)
			{
				std::cerr << "[e] bad pTHat selection - min:" << pTHatMin << " max:" << pTHatMax << std::endl;
				return;
			}
			else
			{
				// add(TString::Format("PhaseSpace:pTHatMax=%f", pTHatMax).Data());
				add(boost::str(boost::format("PhaseSpace:pTHatMax=%1%") % pTHatMax));
			}
		}

		if (isSet("--minbias"))
		{
			add("HardQCD:all=off");
			add("PromptPhoton:all=off");
		}

		if (isSet("--nsd"))
		{
			add("SoftQCD:all=off");
			add("SoftQCD:elastic=on"); //               ! Elastic
			add("SoftQCD:singleDiffractive=off"); //     ! Single diffractive
			add("SoftQCD:doubleDiffractive=on"); //     ! Double diffractive
			add("SoftQCD:centralDiffractive=on"); //    ! Central diffractive
			add("SoftQCD:nonDiffractive=on"); //        ! Nondiffractive (inelastic)
			add("SoftQCD:inelastic=on"); //             ! All inelastic
		}
		else
		{
			if (isSet("--minbias"))
				add("SoftQCD:all=on");
		}

		if (isSet("Beams:eA") || isSet("Beams:eB"))
		{
			add("Beams:frameType=2");
			double sqrts = PyUtil::sqrts(getD("Beams:eA"), getD("Beams:eB"));
			add(boost::str(boost::format("Beams:eCM=%f") % sqrts));
		}

		if (isSet("Next:numberCount") == false)
			add("Next:numberCount=0");
	}
};
