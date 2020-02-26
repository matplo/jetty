#include <jetty/util/voronoi/varea_calc.h>
#include <jetty/util/voronoi/event_area.h>
#include <jetty/util/args.h>
#include <jetty/util/blog.h>
#include <jetty/util/strutil.h>
#include <jetty/util/looputil.h>
#include <jetty/util/hepmc/readfile.h>

#include <TFile.h>
#include <TProfile.h>
#include <TRandom.h>

#include <string>

#include <boost/algorithm/string.hpp>

#include <jetty/mljets/jet_efp.h>

using namespace std;

int main ( int argc, char *argv[] )
{
	SysUtil::Args args(argc, argv);
	Linfo << args.asString();

	if (args.isSet("--jetefp"))
	{
		return jet_efp(argc, argv);
	}
	return 0;
};
