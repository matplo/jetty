#include "strutil.h"

#include <string>
#include <exception>

namespace StrUtil
{

	double str_to_double(const char *str, double defret)
	{
		double d = defret;
		try
		{
			d = std::stod(str);
		}
		catch (const std::exception &e)
		{
			// cerr << "[e] " << e.what() << endl;
			// cerr << "    failed to stod >" << str << "<" << endl;
			d = defret;
		}
		return d;
	}

	long str_to_long(const char *str, long defret)
	{
		long d = defret;
		try
		{
			d = std::stol(str);
		}
		catch (const std::exception &e)
		{
			// cerr << e.what() << endl;
			// cerr << "[e] failed to stol (" << str << ")" << endl;
			d = defret;
		}
		return d;
	}

	int str_to_int(const char *str, int defret)
	{
		int d = defret;
		try
		{
			d = int(str_to_long(str));
		}
		catch (const std::exception &e)
		{
			// cerr << e.what() << endl;
			// cerr << "[e] failed to int(stol (" << str << ") )" << endl;
			d = defret;
		}
		return d;
	}

}
