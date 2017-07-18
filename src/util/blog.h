#ifndef BLOG_HH
#define BLOG_HH

#include <boost/log/trivial.hpp>

//#define BOOST_LOG_DYN_LINK 1

#define LTRACE BOOST_LOG_TRIVIAL(trace)
#define LDBG BOOST_LOG_TRIVIAL(debug)
#define LINFO BOOST_LOG_TRIVIAL(info)
#define LWARN BOOST_LOG_TRIVIAL(warning)
#define LERR BOOST_LOG_TRIVIAL(error)
#define LFATAL BOOST_LOG_TRIVIAL(fatal)

#define Ltrace BOOST_LOG_TRIVIAL(trace)
#define Ldebug BOOST_LOG_TRIVIAL(debug)
#define Linfo BOOST_LOG_TRIVIAL(info)
#define Lwarn BOOST_LOG_TRIVIAL(warning)
#define Lerror BOOST_LOG_TRIVIAL(error)
#define Lfatal BOOST_LOG_TRIVIAL(fatal)

namespace SysUtil
{
	void blog_set_severity(int level = boost::log::trivial::info);
}

#endif // BLOG_HH
