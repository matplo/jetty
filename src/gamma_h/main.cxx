#include "util/args.h"
#include "gamma_h.h"

int main ( int argc, char *argv[] )
{
	int rv = 0;

    SysUtil::Args args(argc, argv);

    if (args.isSet("--gh"))
    {
    	rv = run_gamma_h(args.asString());
    }

    return rv;
}
