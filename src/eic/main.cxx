#include <jetty/util/args.h>
#include <jetty/eic/eic_tasks.h>

int main ( int argc, char *argv[] )
{
	int rv = 0;

    SysUtil::Args args(argc, argv);

    if (args.isSet("--eic"))
    {
    	rv = eic_tasks(args.asString());
    }

    return rv;
}
