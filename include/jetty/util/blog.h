#ifndef BLOG_HH
#define BLOG_HH

#include <boost/log/trivial.hpp>
#include <string>

//#define BOOST_LOG_DYN_LINK 1

#include <streambuf>
#include <sstream>
#include <iostream>

namespace LogUtil
{
	boost::log::trivial::severity_level current_blog_severity();
	void blog_set_severity(boost::log::trivial::severity_level level = boost::log::trivial::info);
	std::string env_path_jetty(const char *fname);

	// https://stackoverflow.com/questions/5419356/redirect-stdout-stderr-to-a-string

	class cout_redirect
	{
	public:
		cout_redirect( std::streambuf * new_buffer )
			: old( std::cout.rdbuf( new_buffer ) )
		{ }

		~cout_redirect( )
		{
			std::cout.rdbuf( old );
		}

	private:
		std::streambuf * old;
	};

	class cerr_redirect
	{
	public:
		cerr_redirect( std::streambuf * new_buffer )
			: old( std::cerr.rdbuf( new_buffer ) )
		{ }

		~cerr_redirect( )
		{
			std::cout.rdbuf( old );
		}

	private:
		std::streambuf * old;
	};

	class cout_sink
	{
	public:
		cout_sink()
			: buffer()
			, sink(buffer.rdbuf())
		{
			;
		}
		~cout_sink() { ; }
		std::stringstream * get_buffer() {return &buffer;}
	private:
		std::stringstream buffer;
		cout_redirect 	  sink;
	};

	class cerr_sink
	{
	public:
		cerr_sink()
			: buffer()
			, sink(buffer.rdbuf())
		{
			;
		}
		~cerr_sink() { ; }
		std::stringstream * get_buffer() {return &buffer;}
	private:
		std::stringstream buffer;
		cerr_redirect 	  sink;
	};

}

#define COLWARN "\033[33m"
#define COLERR "\033[31m"
#define COLDEF "\033[0m"

#define LTRACE BOOST_LOG_TRIVIAL(trace) 	<< "(" << LogUtil::env_path_jetty( __FILE__ ) << ", " << __LINE__ << ") "
#define LDEBUG BOOST_LOG_TRIVIAL(debug) 	<< "(" << LogUtil::env_path_jetty( __FILE__ ) << ", " << __LINE__ << ") "
#define LINFO  BOOST_LOG_TRIVIAL(info)
#define LWARN  BOOST_LOG_TRIVIAL(warning)	<< COLWARN << "[!] " << COLDEF
#define LERROR BOOST_LOG_TRIVIAL(error)  	<< COLERR << "(" << LogUtil::env_path_jetty( __FILE__ ) << ", " << __LINE__ << ") " << COLDEF
#define LFATAL BOOST_LOG_TRIVIAL(fatal)  	<< COLERR << "(" << LogUtil::env_path_jetty( __FILE__ ) << ", " << __LINE__ << ") " << COLDEF

#define Ltrace BOOST_LOG_TRIVIAL(trace) 	<< "(" << __FILE__ << ", " << __LINE__ << ") "
#define Ldebug BOOST_LOG_TRIVIAL(debug) 	<< "(" << __FILE__ << ", " << __LINE__ << ") "
#define Linfo  BOOST_LOG_TRIVIAL(info)
#define Lwarn  BOOST_LOG_TRIVIAL(warning) 	<< COLWARN << "[!] " << COLDEF
#define Lerror BOOST_LOG_TRIVIAL(error)  	<< COLERR << "(" << __FILE__ << ", " << __LINE__ << ") " << COLDEF
#define Lfatal BOOST_LOG_TRIVIAL(fatal)  	<< COLERR << "(" << __FILE__ << ", " << __LINE__ << ") " << COLDEF

#endif // BLOG_HH
