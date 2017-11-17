#include <jetty/util/args.h>
#include <jetty/spectra/spectra.h>

int main ( int argc, char *argv[] )
{
	int rv = 0;

    SysUtil::Args args(argc, argv);

    if (args.isSet("--spectra"))
    {
    	rv = run_spectra(args.asString());
    }

    return rv;
}
