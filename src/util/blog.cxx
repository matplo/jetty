#include "blog.h"

#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>

namespace logging = boost::log;

namespace SysUtil
{
	void blog_set_severity(int level)
	{
		logging::core::get()->set_filter
		(
			logging::trivial::severity >= level
		);
	}
}
