#include <jetty/util/args.h>
#include <jetty/deadcone/deadcone.h>

int main ( int argc, char *argv[] )
{
	int rv = 0;

    SysUtil::Args args(argc, argv);

    if (args.isSet("--std"))
    {
    	rv = deadcone(args.asString());
    }

    return rv;
}
