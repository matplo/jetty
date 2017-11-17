#ifndef BLOG_HH
#define BLOG_HH

#include <boost/log/trivial.hpp>
#include <string>

//#define BOOST_LOG_DYN_LINK 1


namespace LogUtil
{
	boost::log::trivial::severity_level current_blog_severity();
	void blog_set_severity(boost::log::trivial::severity_level level = boost::log::trivial::info);
	std::string env_path_jetty(const char *fname);
}

#define LTRACE BOOST_LOG_TRIVIAL(trace) << "(" << LogUtil::env_path_jetty( __FILE__ ) << ", " << __LINE__ << ") "
#define LDEBUG BOOST_LOG_TRIVIAL(debug)   << "(" << LogUtil::env_path_jetty( __FILE__ ) << ", " << __LINE__ << ") "
#define LINFO  BOOST_LOG_TRIVIAL(info)
#define LWARN  BOOST_LOG_TRIVIAL(warning)
#define LERROR BOOST_LOG_TRIVIAL(error)  << "(" << LogUtil::env_path_jetty( __FILE__ ) << ", " << __LINE__ << ") "
#define LFATAL BOOST_LOG_TRIVIAL(fatal)  << "(" << LogUtil::env_path_jetty( __FILE__ ) << ", " << __LINE__ << ") "

#define Ltrace BOOST_LOG_TRIVIAL(trace) << "(" << __FILE__ << ", " << __LINE__ << ") "
#define Ldebug BOOST_LOG_TRIVIAL(debug) << "(" << __FILE__ << ", " << __LINE__ << ") "
#define Linfo  BOOST_LOG_TRIVIAL(info)
#define Lwarn  BOOST_LOG_TRIVIAL(warning)
#define Lerror BOOST_LOG_TRIVIAL(error)  << "(" << __FILE__ << ", " << __LINE__ << ") "
#define Lfatal BOOST_LOG_TRIVIAL(fatal)  << "(" << __FILE__ << ", " << __LINE__ << ") "

#endif // BLOG_HH
