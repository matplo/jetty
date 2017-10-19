#include <util/args.h>
#include "run_pythia.h"
#include "run_jets.h"
#include "run_test.h"
#ifdef USE_HEPMC
#include "run_hepmc.h"
#endif
int main ( int argc, char *argv[] )
{
	int rv = 0;

	SysUtil::Args args(argc, argv);

	if (args.isSet("--pythia"))
	{
		rv = run_pythia(args.asString());
	}

	if (args.isSet("--jets"))
	{
		rv = run_jets(args.asString());
	}

#ifdef USE_HEPMC
	if (args.isSet("--hepmc"))
	{
		rv = run_hepmc(args.asString());
	}
#endif

	if (args.isSet("--testrun"))
	{
		rv = run_test(args.asString());
	}

	return rv;
}
