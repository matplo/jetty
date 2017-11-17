#include <jetty/util/args.h>
#include <jetty/util/blog.h>

#include <jetty/area/simple.h>

int main ( int argc, char *argv[] )
{
	SysUtil::Args args(argc, argv);
	Linfo << args.asString();

	if (args.isSet("--simple"))
	{
		return simple(argc, argv);
	}
	return 0;
};
