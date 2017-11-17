#include <jetty/util/blog.h>

#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/console.hpp>

#include <boost/log/expressions.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
// #include <boost/log/trivial.hpp> // already in blog.h
#include <boost/log/expressions.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/support/date_time.hpp>

#include <boost/algorithm/string/replace.hpp>
#include <cstdlib>

namespace logging = boost::log;
namespace src = boost::log::sources;
namespace expr = boost::log::expressions;
namespace keywords = boost::log::keywords;

// handy
// http://www.boost.org/doc/libs/1_61_0/libs/log/example/doc/tutorial_fmt_stream.cpp

namespace LogUtil
{
	bool common_attributes_added = false;
	boost::log::trivial::severity_level current_severity = boost::log::trivial::info;

	boost::log::trivial::severity_level current_blog_severity() { return current_severity;}

	void blog_set_severity(boost::log::trivial::severity_level level)
	{
		logging::core::get()->set_filter
		(
			logging::trivial::severity >= level
		);
		current_severity = level;
		//boost::log::add_console_log(std::cout, boost::log::keywords::format = "[%TimeStamp%] %Message%");

		if (common_attributes_added == false)
		{
			common_attributes_added = true;
			logging::add_console_log(std::cout,
				logging::keywords::format =
				(
					expr::stream
						<< expr::format_date_time< boost::posix_time::ptime >("TimeStamp", "%Y-%m-%d %H:%M:%S")
						<< " [" << logging::trivial::severity
						<< "] " << expr::smessage
				)
			);
			logging::add_common_attributes();
		}
	}

	std::string env_path_jetty(const char *fname)
	{
		std::string jetty_dir = std::getenv("JETTYDIR");
		std::string sfname(fname);
		boost::algorithm::replace_first(sfname, jetty_dir, "$JETTYDIR");
		return sfname;
	}
}
