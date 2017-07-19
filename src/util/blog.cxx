#include "blog.h"

#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
//#include <boost/log/utility/setup/console.hpp>

namespace logging = boost::log;

namespace SysUtil
{
	void blog_set_severity(int level)
	{
		logging::core::get()->set_filter
		(
			logging::trivial::severity >= level
		);
		// boost::log::add_console_log(std::cout, boost::log::keywords::format = ">> %Message%");
	}
}
