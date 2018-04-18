#ifndef __SYSUTIL__HH
#define __SYSUTIL__HH

#include <string>
#include <sys/stat.h>

#define BOOST_FILESYSTEM_NO_DEPRECATED
#define BOOST_FILESYSTEM_VERSION 3

#include <boost/filesystem.hpp>

namespace SysUtil
{
	bool file_exists_stat (const std::string& name);
	bool file_exists (const std::string& name);
	// std::string expand_path_name(const std::string& spath);

	bool is_file (const std::string& pathname);
	bool is_link (const std::string& pathname);
	bool is_directory (const std::string& pathname);

	bool find_file(const boost::filesystem::path& dir_path, const boost::filesystem::path& file_name, boost::filesystem::path& path_found);

	std::vector<std::string> find_files_ext(const char *dir, const char* ext);
	std::vector<std::string> find_files_exact(const char *dir, const char* exact);

}

#endif
