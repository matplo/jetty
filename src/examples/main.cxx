#include <jetty/util/args.h>
#include <jetty/examples/run_pythia.h>
#include <jetty/examples/run_pythia_wrapper.h>
#include <jetty/examples/run_pythia_pool.h>
#include <jetty/examples/et.h>
#include <jetty/examples/gentasks.h>

int main ( int argc, char *argv[] )
{
	int rv = 0;

    SysUtil::Args args(argc, argv);

    if (args.isSet("--pythia"))
    {
    	rv = run_pythia(args.asString());
    }

    if (args.isSet("--pythia-wrapper"))
    {
        rv = run_pythia_wrapper(args.asString());
    }

    if (args.isSet("--pythia-pool"))
    {
        rv = run_pythia_pool(args.asString());
    }

    if (args.isSet("--et"))
    {
        rv = et(args.asString());
    }

    if (args.isSet("--gentasks"))
    {
        rv = gentasks(args.asString());
    }

    return rv;
}
