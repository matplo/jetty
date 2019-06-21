#include <jetty/util/args.h>
#include <jetty/util/blog.h>
#include <jetty/util/lep/aleph2root.h>

#include <TFile.h>
#include <TProfile.h>
#include <TRandom.h>

#include <string>
using namespace std;

int main ( int argc, char *argv[] )
{
	SysUtil::Args args(argc, argv);
	Linfo << args.asString();

	if (args.isSet("--rootify"))
	{
		std::string _sin = args.get("--input");
		return Aleph::make_a_tree(_sin.c_str());
	}

	if (args.isSet("--dump"))
	{
		std::string _sin = args.get("--input");
		return Aleph::dump(_sin.c_str(), args.getI("--nev", -1), args.isSet("--noparts"));
	}
	return 0;
};
