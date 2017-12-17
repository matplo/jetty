#include <jetty/util/args.h>
#include <jetty/util/strutil.h>
#include <jetty/util/blog.h>

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>

#include <algorithm>
#include <fstream>
#include <iostream>
using namespace std;

namespace SysUtil
{
	unsigned int Args::_instance_counter = 0;

	void Args::_log_argument(const char *what)
	{
		string swhat = what;
		for (auto s : _args_logged)
		{
			if (s == swhat)
				return;
		}
		_args_logged.push_back(swhat);
	}

	void Args::_dump_logged_arguments()
	{
		if (isSet("--dump-args-log") || isSet("--debug") || isSet("--help") || isSet("-h"))
		{
			sort(_args_logged.begin(), _args_logged.end());
			cout << asString("[i] argument log : known via ::isSet|add|set|get - cmnd was:");
			for (auto s : _args_logged)
			{
				cout << "    " << s << endl;
			}
		}
	}

	std::vector<std::string> Args::_args_logged;

	Args::~Args()
	{
		_instance_counter -= 1;
		if (_instance_counter == 0)
			_dump_logged_arguments();
	}

	Args::Args(int argc, char **argv)
		: _args()
	{
		_convert(argc, argv);
		_init_logging();
	}

	Args::Args()
		: _args()
	{
		_init_logging();
	}

	Args::Args(const vector<string> &v)
		: _args(v)
	{
		_init_logging();
	}

	Args::Args(const string &s, const char bchar)
		: _args(breakup(s.c_str(), bchar))
	{
		_init_logging();
	}

	Args::Args(const char * s, const char bchar)
		: _args(breakup(s, bchar))
	{
		_init_logging();
	}

	Args::Args(const Args &v)
		: _args(v._args)
	{
		_init_logging();
	}

	bool Args::isSet(const char *what) const
	{
		_log_argument(what);
		string swhat(what);
		string s = boost::trim_left_copy(swhat);
		auto prs = pairs();
		for (auto &p : prs)
			if (p.first == s) return true;
		return false;
	}

	bool Args::isSet(const string &what) const
	{
		return isSet(what.c_str());
	}

	string Args::get(const char *what) const
	{
		_log_argument(what);
		string sret("");
		string swhat(what);
		auto prs = pairs();
		for (auto &p : prs)
			if (p.first == swhat)
			{
				sret = p.second;
			}
		return sret;
	}

	string Args::get(const char *what, const char *defret) const
	{
		string sret = get(what);
		if (sret.size() <= 0)
			sret = defret;
		return sret;
	}

	string Args::get(const string &what, const string &defret) const
	{
		string sret = get(what);
		if (sret.size() <= 0)
			sret = defret;
		return sret;
	}

	string Args::get(const string &what) const
	{
		return get(what.c_str());
	}

	double Args::getD(const char *what, const double defret) const
	{
		if (isSet(what) == false) return defret;
		return StrUtil::str_to_double(get(what).c_str(), defret);
	}

	double Args::getD(const string &what, const double defret) const
	{
		return getD(what.c_str(), defret);
	}

	int Args::getI(const char *what, const int defret) const
	{
		if (isSet(what) == false) return defret;
		return StrUtil::str_to_int(get(what).c_str(), defret);
	}

	int Args::getI(const string &what, const int defret) const
	{
		return getI(what.c_str(), defret);
	}

	void Args::add(const char *what)
	{
		string swhat(what);
		if (swhat.find("=") != std::string::npos)
		{
			auto f = swhat.find("=");
			auto s = swhat.substr(0, f);
			_log_argument(s.c_str());
			remove(s);
		}
		else
		{
			_log_argument(what);
			remove(what);
		}
		string s(what);
		_args.push_back(s);
	}

	void Args::add(const string &what)
	{
		add(what.c_str());
	}

	void Args::remove(const char *what)
	{
		string swhat(what);
		remove(swhat);
	}

	void Args::remove(const string &swhat)
	{
		string s = boost::trim_left_copy(swhat);
		auto prs = pairs();
		for (auto &p : prs)
		{
			if (p.first == s)
			{
				Ldebug << "removing arg:" << p.first;
				p.first = "";
				p.second = "";
			}
		}
		_args.clear();
		for (auto &p : prs)
		{
			if (p.first.size() > 0)
			{
				string s = p.first;
				if (p.second.size() > 0)
					s = boost::str(boost::format("%s=%s") % p.first % p.second);
				_args.push_back(s);
			}
		}
	}

	vector<pair <string, string>> Args::pairs() const
	{
		vector<pair <string, string>> retv;
		for (unsigned int i = 0; i < _args.size(); i++)
		{
			string s1 = boost::trim_left_copy(_args[i]);
			auto feq = s1.find("=");
			string s2 = "";
			if (feq != string::npos)
			{
				s2 = s1.substr(feq+1);
				boost::trim_left(s2);
				s1 = s1.substr(0, feq);
			}
			auto p = make_pair(s1, s2);
			retv.push_back(p);
		}
		return retv;
	}

	string Args::pairsString() const
	{
		string rets;
		auto prs = pairs();
		for (unsigned int i = 0; i < prs.size(); i++)
		{
			cout << "[passing] paired arg: #" << i << " " << prs[i].first << " " << prs[i].second << endl;
			string spypar = prs[i].first;
			string scnd = boost::trim_left_copy(prs[i].second);
			if (scnd.size() > 0)
				spypar += "=" + scnd;
			rets += spypar + " ";
		}
		return rets;
	}

	void Args::_convert(int argc, char **argv)
	{
		ostringstream ss;
		for (int i = 0; i < argc; i++)
			ss << argv[i] << " ";
		istringstream iss(ss.str());
		copy(istream_iterator<string>(iss), istream_iterator<string>(), back_inserter(_args));
	}

	// static
	vector<string> Args::breakup(const char *cs, const char marker)
	{
		vector<string> v;
		istringstream f(cs);
	    string s;
    	while (getline(f, s, marker))
    	{
			s.erase(std::remove(s.begin(), s.end(), '\n'), s.end());
    		if (s.size() > 0)
    		{
	        	v.push_back(s);
    		}
    	}
		return v;
	}

	void Args::merge(const Args &args)
	{
		std::vector<string> v = breakup(args.asString().c_str(), ' ');
		for (auto &s : v)
		{
			Ldebug << "merge.. " << s;
			add(s);
		}
	}

	string Args::asString(const char *pre, bool breaklines) const
	{
		ostringstream ss;
		if (breaklines)
		{
			ss << pre;
			for (unsigned int i = 0; i < _args.size(); i++)
			{
				ss << _args[i] << endl;
			}
		}
		else
		{
			ss << pre;
			for (unsigned int i = 0; i < _args.size(); i++)
			{
				ss << " " << _args[i];
			}
			ss << endl;
		}
		return ss.str();
	}

	void Args::readConfig(const char *fname)
	{
		Ldebug << "[Args::readConfig] present:" << asString();
		Ldebug << "[Args::readConfig] file " << fname;
		std::string str;
		std::ifstream fin(fname);
		if (!fin)
		{
			Lwarn << "[Args::readConfig] unable to read from config file:" << fname;
			return;
		}
		while (std::getline(fin, str))
		{
			std::string s = boost::trim_left_copy(str);
			auto found = s.find("!");
			if (found != std::string::npos)
				s = s.substr(0, found);
			boost::erase_all(s, " ");
			if (s.size() > 0)
			{
				std::string sarg = s;
				auto eqf = sarg.find("=");
				if (eqf != std::string::npos)
					sarg = sarg.substr(0, eqf);
				sarg = boost::trim_left_copy(sarg);
				if (isSet(sarg))
				{
					Ldebug << " - setting already present - ignoring entry: " << s << std::endl;
				}
				else
				{
					Ldebug << " - adding setting " << sarg << " " << s << std::endl;
					add(s);
				}
			}
		}
	}

	void Args::readConfigLines(const char *fname)
	{
		Ldebug << "[Args::readConfig] present:" << asString();
		Ldebug << "[Args::readConfig] file " << fname;
		std::string str;
		std::ifstream fin(fname);
		if (!fin)
		{
			Lwarn << "[Args::readConfig] unable to read from config file:" << fname;
			return;
		}
		while (std::getline(fin, str))
		{
			std::string s = boost::trim_left_copy(str);
			auto found = s.find("!");
			if (found != std::string::npos)
				s = s.substr(0, found);
			//boost::erase_all(s, " ");
			if (s.size() > 0)
			{
				std::string sarg = s;
				auto eqf = sarg.find("=");
				if (eqf != std::string::npos)
					sarg = sarg.substr(0, eqf);
				sarg = boost::trim_left_copy(sarg);
				if (isSet(sarg))
				{
					Ldebug << " - setting already present - ignoring entry: " << s << std::endl;
				}
				else
				{
					Ldebug << " - adding setting " << sarg << " " << s << std::endl;
					string sval = "";
					auto pieces = breakup(s.c_str(), '=');
					if (pieces.size() > 0)
						sarg = pieces[0];
					if (pieces.size() > 1)
						sval = pieces[1];
					boost::trim_left(sarg);
					boost::trim_left(sval);
					Ldebug << " - sarg = " << sarg << " val = " << sval;
					if (sarg.size() > 0)
					{
						if (sval.size() > 0)
						{
							add(sarg, sval);
						}
						else
						{
							set(sarg);
						}
					}
				}
			}
		}
	}

	void Args::_init_logging()
	{
		_instance_counter += 1;

		// cout << "[Args::_init_logging] instance:" << _instance_counter << " current severity: " << LogUtil::current_blog_severity() << endl;

		LogUtil::blog_set_severity(LogUtil::current_blog_severity());

		if (isSet("--debug"))
		{
			if (LogUtil::current_blog_severity() > boost::log::trivial::debug)
				LogUtil::blog_set_severity(boost::log::trivial::debug);
		}

		if (isSet("--trace"))
			if (LogUtil::current_blog_severity() > boost::log::trivial::trace)
				LogUtil::blog_set_severity(boost::log::trivial::trace);
	}

};
