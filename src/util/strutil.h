#include <string>
#include <sstream>

namespace StrUtil
{
	template <class T> std::string sT(const T &t)
	{
		std::ostringstream ss;
		ss << t;
		return ss.str();
	};

	double str_to_double(const char *str, double defret = 0.0);
	long str_to_long(const char *str, long defret = 0);
	int str_to_int(const char *str, int defret = 0);
}
