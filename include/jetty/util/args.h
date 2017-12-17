#ifndef __SARGS__HH
#define __SARGS__HH

#include <string>
#include <vector>
#include <sstream>

namespace SysUtil
{
	class Args
	{
	public:
		Args(int argc, char **argv);
		Args(const std::vector<std::string> &v);
		Args(const std::string &s, const char bchar = ' ');
		Args(const char *s, const char bchar = ' ');
		Args(const Args &v);
		Args();
		virtual ~Args();

		bool 			isSet(const char *what) const;
		bool 			isSet(const std::string &what) const;

		std::string     get (const char *what) const;
		std::string     get (const char *what, const char *defret) const;
		double 			getD(const char *what, const double defret = 0.0) const;
		int 			getI(const char *what, const int 	defret = 0  ) const;

		std::string     get (const std::string &what) const;
		std::string     get (const std::string &what, const std::string &defret) const;
		double 			getD(const std::string &what, const double 	defret = 0.0) const;
		int 			getI(const std::string &what, const int 	defret = 0  ) const;

		template <class T>
		void 			add(const char *what, const T &value);
		void 			add(const char *what);
		template <class T>
		void 			add(const std::string &what, const T &value);
		void 			add(const std::string &what);

		void 			merge(const Args &args);

		void 			set(const char *what) 		 { add(what); }
		void 			set(const std::string &what) { add(what.c_str()); }

		template <class T>
		void 			set(const char *what, const T &value);
		template <class T>
		void 			set(const std::string &what, const T &value);

		void 			remove(const std::string &what);
		void 			remove(const char *what);

		std::vector<std::pair <std::string, std::string>> pairs() const;
		std::string 	pairsString() const;
		std::string 	asString(const char *pre = "", bool breaklines = false) const;

		static std::vector<std::string> breakup(const char *cs, const char marker = ',');

		void 			readConfig(const char *fname);
		void 			readConfigLines(const char *fname);

	protected:
		void				_convert(int argc, char **argv);
		void 				_init_logging();

		static void 		_log_argument(const char *what);
		void 				_dump_logged_arguments();
		static unsigned int _instance_counter;

		std::vector<std::string> _args; // keep all in a string
		static std::vector<std::string> _args_logged;
	};

	template <class T>
	void Args::set(const char *what, const T &value)
	{
		// remove(what); // done in ::add
		add(what, value);
	}

	template <class T>
	void Args::set(const std::string &what, const T &value)
	{
		set(what.c_str(), value);
	}

	template <class T>
	void Args::add(const char *what, const T &value)
	{
		std::ostringstream ss;
		ss << what << "=" << value;
		std::string s = ss.str();
		add(s);
	}

	template <class T>
	void Args::add(const std::string &what, const T &value)
	{
		add(what.c_str(), value);
	}
};
#endif
