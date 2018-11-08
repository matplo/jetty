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
	void Args::cook()
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
		add("Main:numberOfEvents", nEvent);
		Ltrace << "[PyUtil::Args::_cook] Main:numberOfEvents = " << get("Main:numberOfEvents");

		if (isSet("-h") || isSet("--help"))
		{
			add("--dry");
		}

		if (isSet("--test"))
		{
			add("Main:numberOfEvents=1");
			if (isSet("--out") == false)
			{
				add("--out=test_output_pyargs.root");
				add("--default-test-output-pyargs");
			}
		}

		if (isSet("--time-seed"))
		{
			add("Random:setSeed=on");
			add("Random:seed=0");
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
			add("Beams:eCM=0"); // this will fail on initialization
			add("--dry"); // this if checked should halt the execution
			add("--invalid"); // or this...
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
			add("PhaseSpace:pTHatMin",0.0);
			add("PhaseSpace:pTHatMax", -1.0);
			add("WeakSingleBoson:ffbar2gmZ=on");
			add("PhaseSpace:mHatMin", 70);
			add("PhaseSpace:mHatMax", 110);
		}

		if (isSet("--hardQCD"))
		{
			add("HardQCD:all=on");
		}

		if (isSet("--hardQCDlf"))
		{
			add("HardQCD:all=off");
			add("HardQCD:gg2gg=on");
			add("HardQCD:qg2qg=on");
			add("HardQCD:qqbar2gg=on");
			add("HardQCD:gg2qqbar=on");
			add("HardQCD:qq2qq=on");
			add("HardQCD:qqbar2qqbarNew=on");

			add("HardQCD:hardccbar=off");
			add("HardQCD:hardbbbar=off");
		}

		if (isSet("--hardQCDcharm"))
		{
			add("HardQCD:all=off");
			add("HardQCD:hardccbar=on");
		}

		if (isSet("--hardQCDbeauty"))
		{
			add("HardQCD:all=off");
			add("HardQCD:hardbbbar=on");
		}

		if (isSet("--promptPhoton"))
		{
			add("PromptPhoton:all=on");
			// add("SoftQCD:all=off");
		}

		if (isSet("--hardQCDgluons"))
		{
			add("HardQCD:all=off");
			add("HardQCD:gg2gg=on");
			add("HardQCD:qg2qg=on");
			add("HardQCD:qqbar2gg=on");
		}

		if (isSet("--hardQCDquarks"))
		{
			add("HardQCD:all=off");
			add("HardQCD:gg2qqbar=on");
			add("HardQCD:qq2qq=on");
			add("HardQCD:qqbar2qqbarNew=on");
			add("HardQCD:hardccbar=on");
			add("HardQCD:hardbbbar=on");
		}

		if (isSet("--hardQCDuds"))
		{
			add("HardQCD:all=off");
			add("HardQCD:gg2qqbar=on");
			add("HardQCD:qq2qq=on");
			add("HardQCD:qqbar2qqbarNew=on");
		}

		if (isSet("Next:numberShowEvent") == false)
			add("Next:numberShowEvent", 0);
		if (isSet("Next:numberShowInfo") == false)
			add("Next:numberShowInfo", 0);
		if (isSet("Next:numberShowProcess") == false)
			add("Next:numberShowProcess", 0);
		if (isSet("Next:numberCount") == false)
			add("Next:numberCount", 0);

		if (isSet("Beams:eA") || isSet("Beams:eB"))
		{
			double eA = getD("Beams:eA");
			double eB = getD("Beams:eB");
			if (eA == eB)
			{
				add("Beams:frameType=1");
			}
			else
			{
				add("Beams:frameType=2");
			}

			Ltrace << "PyArgs::_cook: reset Beams:eCM because eA=" << eA << " eB=" << eB;
			double sqrts = PyUtil::sqrts(eA, eB);
			add(boost::str(boost::format("Beams:eCM=%f") % sqrts));
			Ltrace << "PyArgs::_cook:      Beams:eCM=" << getD("Beams:eCM");
		}
	}
};
