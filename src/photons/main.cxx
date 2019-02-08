#include <jetty/util/args.h>
#include <jetty/photons/photon_tasks.h>

int main ( int argc, char *argv[] )
{
	int rv = 0;

    SysUtil::Args args(argc, argv);

    if (args.isSet("--photons"))
    {
    	rv = photon_tasks(args.asString());
    }

    return rv;
}
