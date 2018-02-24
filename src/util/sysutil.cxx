#include <jetty/util/sysutil.h>
#include <jetty/util/blog.h>

#include <string>
#include <vector>

//#include <boost/algorithm/string.hpp>
//#include <boost/filesystem.hpp>
//namespace fs = boost::filesystem;

#include <algorithm>
#include <iostream>

namespace SysUtil
{
	bool file_exists (const std::string& name)
	{
	  struct stat buffer;
	  return (stat (name.c_str(), &buffer) == 0);
	}

	// std::vector<std::string> stokens(std::string s, char c)
	// {
	// 	std::vector<std::string> v;
	// 	return v;
	// }


	bool find_file(const boost::filesystem::path& dir_path, const boost::filesystem::path& file_name, boost::filesystem::path& path_found)
	{
		using namespace std;
		using namespace boost::filesystem;

		const recursive_directory_iterator end;
		const auto it = find_if(recursive_directory_iterator(dir_path), end,
		                        [&file_name](const directory_entry& e)
		                        { return e.path().filename() == file_name;});
		if (it == end) {
		return false;
		} else {
		path_found = it->path();
		return true;
		}
	}

	//http://www.technical-recipes.com/2014/using-boostfilesystem/#Iterating
	std::vector<std::string> find_files_ext(const char *dir, const char* ext)
	{
		boost::filesystem::recursive_directory_iterator rdi(dir);
		boost::filesystem::recursive_directory_iterator end_rdi;

		std::vector<std::string> found_files;
		std::string ext_str0(ext);
		for (; rdi != end_rdi; rdi++)
		{
			//rdi++;

			if (ext_str0.compare((*rdi).path().extension().string()) == 0)
			{
				found_files.push_back((*rdi).path().string());
			}
		}
		return found_files;
	}

	std::vector<std::string> find_files_exact(const char *dir, const char* exact)
	{
		boost::filesystem::recursive_directory_iterator rdi(dir);
		boost::filesystem::recursive_directory_iterator end_rdi;

		std::vector<std::string> found_files;
		std::string sexact(exact);
		for (; rdi != end_rdi; rdi++)
		{
			//rdi++;

			if (sexact.compare((*rdi).path().filename().string()) == 0)
			{
				found_files.push_back((*rdi).path().string());
			}
		}
		return found_files;
	}

}
