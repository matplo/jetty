#include <util/args.h>
#include <util/blog.h>

#include "simple.h"

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
