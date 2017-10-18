#include "readfile.h"
#include "util/args.h"
#include "util/blog.h"

int main ( int argc, char *argv[] )
{
	SysUtil::Args args(argc, argv);
	Linfo << args.asString();

	ReadHepMCFile f(args.get("--file").c_str());
	while (f.NextEvent())
	{
		Linfo << "number of particles:" << f.HepMCParticles(false).size();
		Linfo << "number of final particles:" << f.HepMCParticles(true).size();
	}

	return 0;
};

