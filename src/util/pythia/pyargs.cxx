#include <jetty/util/pythia/pyargs.h>
#include <jetty/util/pythia/pyutil.h>
#include <jetty/util/sysutil.h>
#include <jetty/util/blog.h>

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>

#include <iostream>
#include <sstream>

using namespace std;

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
		{
			readConfig(cfgfile.c_str());
			Ltrace << "[PyUtil::Args::_cook] using " << cfgfile << " to [pre-]configure pythia.";
		}

		int nEvent = getI("Main:numberOfEvents");
		int userNEvent = getI("--nev", nEvent);
		if (userNEvent > 0)
		{
			nEvent 		= userNEvent;
		}
		set("Main:numberOfEvents", nEvent);
		Ldebug << "[PyUtil::Args::_cook] Main:numberOfEvents = " << get("Main:numberOfEvents");

		if (isSet("-h") || isSet("--help"))
		{
			set("--dry");
		}

		if (isSet("--test"))
		{
			set("Main:numberOfEvents=1");
			set("--out=test.root");
		}

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
		add(boost::str(boost::format("PhaseSpace:pTHatMax=%1%") % pTHatMax));

		if (getD("PhaseSpace:pTHatMin") > getD("PhaseSpace:pTHatMax") && getD("PhaseSpace:pTHatMax") >= 0)
		{
			Lwarn << "something is not right with your pThat selection min=" << getD("PhaseSpace:pTHatMin") << " max=" <<  getD("PhaseSpace:pTHatMax") << endl;
			// Lwarn << "setting pTHatMax=-1";
			set("Beams:eCM=0"); // this will fail on initialization
			set("--dry"); // this if checked should halt the execution
			set("--invalid"); // or this...
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

		if (isSet("--inel"))
		{
			add("HardQCD:all=off");
			add("PromptPhoton:all=off");
			add("SoftQCD:all=off");
			add("SoftQCD:nonDiffractive=on"); //        ! Nondiffractive (inelastic)
			add("SoftQCD:inelastic=on"); //             ! All inelastic
		}


		if (isSet("--inel-d"))
		{
			add("HardQCD:all=off");
			add("PromptPhoton:all=off");
			add("SoftQCD:all=off");
			add("SoftQCD:singleDiffractive=on"); //     ! Single diffractive
			add("SoftQCD:doubleDiffractive=on"); //     ! Double diffractive
			add("SoftQCD:centralDiffractive=on"); //    ! Central diffractive
			add("SoftQCD:nonDiffractive=on"); //        ! Nondiffractive (inelastic)
			add("SoftQCD:inelastic=on"); //             ! All inelastic
		}

		if (isSet("--diff"))
		{
			add("HardQCD:all=off");
			add("PromptPhoton:all=off");
			add("SoftQCD:all=off");
			add("SoftQCD:singleDiffractive=on"); //     ! Single diffractive
			add("SoftQCD:doubleDiffractive=on"); //     ! Double diffractive
			add("SoftQCD:centralDiffractive=on"); //    ! Central diffractive
		}

		if (isSet("--inel-nsd"))
		{
			add("HardQCD:all=off");
			add("PromptPhoton:all=off");
			add("SoftQCD:all=off");
			add("SoftQCD:nonDiffractive=on"); //        ! Nondiffractive (inelastic)
			add("SoftQCD:inelastic=on"); //             ! All inelastic
			add("SoftQCD:doubleDiffractive=on"); //     ! Double diffractive
			add("SoftQCD:centralDiffractive=on"); //    ! Central diffractive
		}

		if (isSet("--el"))
		{
			add("HardQCD:all=off");
			add("PromptPhoton:all=off");
			add("SoftQCD:all=off");
			add("SoftQCD:elastic=on"); //             ! All inelastic
		}

		if (isSet("--nd"))
		{
			add("HardQCD:all=off");
			add("PromptPhoton:all=off");
			add("SoftQCD:all=off");
			add("SoftQCD:nonDiffractive=on"); //        ! Nondiffractive (inelastic)
		}

		if (isSet("--z0"))
		{
			add("HardQCD:all=off");
			add("PromptPhoton:all=off");
			add("SoftQCD:all=off");
			set("PhaseSpace:pTHatMin",0.0);
			set("PhaseSpace:pTHatMax", -1.0);
			add("WeakSingleBoson:ffbar2gmZ=on");
			set("PhaseSpace:mHatMin", 70);
			set("PhaseSpace:mHatMax", 110);
		}

		if (isSet("--hardQCD"))
		{
			add("HardQCD:all=on");
		}

		if (isSet("--promptPhoton"))
		{
			add("PromptPhoton:all=on");
			// add("SoftQCD:all=off");
		}

		if (isSet("Next:numberShowEvent") == false)
			set("Next:numberShowEvent", 0);

		if (isSet("Next:numberShowInfo") == false)
			set("Next:numberShowInfo", 0);
		if (isSet("Next:numberShowProcess") == false)
			set("Next:numberShowProcess", 0);
		if (isSet("Next:numberCount") == false)
			set("Next:numberCount", 0);

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
