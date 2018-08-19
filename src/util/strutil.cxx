#include <jetty/util/strutil.h>

#include <string>
#include <exception>
#include <iostream>

namespace StrUtil
{
	void replace_substring(std::string& this_s, const std::string& old_s, const std::string& new_s)
	{
		std::string::size_type pos = 0u;
		while((pos = this_s.find(old_s, pos)) != std::string::npos)
		{
			this_s.replace(pos, old_s.length(), new_s);
			pos += new_s.length();
		}
	}

	void replace_substring(std::string& _s, const char* old_s, const char* new_s)
	{
		std::string sold_s (old_s);
		std::string snew_s (new_s);
		replace_substring(_s, sold_s, snew_s);
	}

	std::string replace_substring_copy(std::string& _s, const char* old_s, const char* new_s)
	{
		std::string sold_s (old_s);
		std::string snew_s (new_s);
		return replace_substring_copy(_s, sold_s, snew_s);
	}

	std::string replace_substring_copy(std::string& this_s, const std::string& old_s, const std::string& new_s)
	{
		std::string copy_s(this_s);
		std::string::size_type pos = 0u;
		while((pos = copy_s.find(old_s, pos)) != std::string::npos)
		{
			copy_s.replace(pos, old_s.length(), new_s);
			pos += new_s.length();
		}
		return copy_s;
	}

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
			// std::cerr << e.what() << std::endl;
			// std::cerr << "[e] failed to stol (" << str << ")" << std::endl;
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
			// std::cerr << e.what() << std::endl;
			// std::cerr << "[e] failed to int(stol (" << str << ") )" << std::endl;
			d = defret;
		}
		return d;
	}

}
