#include <util/args.h>
#include "run_pythia.h"
#include "run_jets.h"
#include "run_test.h"

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

	if (args.isSet("--testrun"))
	{
		rv = run_test(args.asString());
	}

	return rv;
}
