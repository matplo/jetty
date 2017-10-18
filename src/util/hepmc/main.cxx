#include "readfile.h"
#include "util/args.h"
#include "util/blog.h"

int main ( int argc, char *argv[] )
{
	SysUtil::Args args(argc, argv);
	Linfo << args.asString();
	return 0;
};

