#ifndef __PYUTIL_ARGS__HH
#define __PYUTIL_ARGS__HH

#include <jetty/util/args.h>

namespace PyUtil
{
	class Args : public SysUtil::Args
	{
	public:
		Args(int argc, char **argv)
		: SysUtil::Args(argc, argv)
		{
			cook();
		}

		Args(const std::vector<std::string> &v)
		: SysUtil::Args(v)
		{
			cook();
		}

		Args(const std::string &s, const char bchar = ' ')
		: SysUtil::Args(s, bchar)
		{
			cook();
		}

		Args(const char *s, const char bchar = ' ')
		: SysUtil::Args(s, bchar)
		{
			cook();
		}

		Args(const Args &v)
		: SysUtil::Args(v)
		{
			cook();
		}

		Args()
		: SysUtil::Args()
		{
			cook();
		}

		virtual ~Args() {;}

		virtual void cook();

	protected:

	};
};
#endif
