#include <util/args.h>
#include "run_pythia.h"
#include "et.h"

int main ( int argc, char *argv[] )
{
	int rv = 0;

    SysUtil::Args args(argc, argv);

    if (args.isSet("--pythia"))
    {
    	rv = run_pythia(args.asString());
    }

    if (args.isSet("--et"))
    {
        rv = et(args.asString());
    }

    return rv;
}
