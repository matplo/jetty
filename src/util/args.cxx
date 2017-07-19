#include "args.h"
#include "strutil.h"
#include "blog.h"
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>

#include <fstream>

#include <iostream>
using namespace std;

namespace SysUtil
{
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

	Args::Args(const string &s)
		: _args(breakup(s.c_str(), ' '))
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
		string swhat(what);
		auto prs = pairs();
		for (auto &p : prs)
			if (p.first == swhat) return true;
		return false;
	}

	bool Args::isSet(const string &what) const
	{
		return isSet(what.c_str());
	}

	string Args::get(const char *what) const
	{
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

	double Args::getD(const char *what, const double defret) const
	{
		if (isSet(what) == false) return defret;
		return StrUtil::str_to_double(get(what).c_str(), defret);
	}

	int Args::getI(const char *what, const int defret) const
	{
		if (isSet(what) == false) return defret;
		return StrUtil::str_to_int(get(what).c_str(), defret);
	}

	void Args::add(const char *what)
	{
		string swhat(what);
		if (swhat.find("=") != std::string::npos)
		{
			auto f = swhat.find("=");
			auto s = swhat.substr(0, f);
			remove(s);
		}
		else
		{
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
		auto prs = pairs();
		for (auto &p : prs)
		{
			if (p.first == swhat)
			{
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

	string Args::asString(const char *pre, bool breaklines) const
	{
		ostringstream ss;
		if (breaklines)
		{
			for (unsigned int i = 0; i < _args.size(); i++)
			{
				if (_args[i][0]=='-' || i == 0)
				{
					ss << endl;
					ss << pre << " " << _args[i];
				}
				else
				{
					ss << " " << _args[i];
				}
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
			Ldebug << "[Args::readConfig:e] unable to read from config file:" << fname;
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

	void Args::_init_logging()
	{
		LogUtil::blog_set_severity();

		if (isSet("--debug"))
			LogUtil::blog_set_severity(boost::log::trivial::debug);

		if (isSet("--trace"))
			LogUtil::blog_set_severity(boost::log::trivial::trace);
	}

};
