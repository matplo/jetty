#include "sysutil.h"

#include "blog.h"

#include <string>
#include <vector>

//#include <boost/algorithm/string.hpp>
//#include <boost/filesystem.hpp>
//namespace fs = boost::filesystem;

namespace SysUtil
{
	bool file_exists (const std::string& name)
	{
	  struct stat buffer;
	  return (stat (name.c_str(), &buffer) == 0);
	}

	std::vector<std::string> stokens(std::string s, char c)
	{
		std::vector<std::string> v;
		return v;
	}

}
