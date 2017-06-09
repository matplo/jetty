#ifndef __PYUTIL_ARGS__HH
#define __PYUTIL_ARGS__HH

#include "args.h"

namespace PyUtil
{
	class Args : public SysUtil::Args
	{
	public:
		Args(int argc, char **argv)
		: SysUtil::Args(argc, argv)
		{
			_cook();
		}

		Args(const std::vector<std::string> &v)
		: SysUtil::Args(v)
		{
			_cook();
		}

		Args(const std::string &s)
		: SysUtil::Args(s)
		{
			_cook();
		}

		Args(const Args &v)
		: SysUtil::Args(v)
		{
			_cook();
		}

		Args()
		: SysUtil::Args()
		{
			_cook();
		}

		virtual ~Args() {;}

	protected:
		void _cook();

	};
};
#endif
