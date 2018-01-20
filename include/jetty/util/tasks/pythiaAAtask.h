#ifndef __JETTY_UTIL_PYTHIAAATASK_HH
#define __JETTY_UTIL_PYTHIAAATASK_HH

#include <vector>

#include <jetty/util/wrapper/wrapper.h>
#include <jetty/util/pythia/pyargs.h>
#include <jetty/util/tasks/eventpooltask.h>

namespace GenUtil
{
	class PythiaAATask : public EventPoolTask
	{
	public:
		PythiaAATask(const char *name) : EventPoolTask(name), fFixedNcoll(0), fEA(0), fEB(0) {;}
		PythiaAATask(const char *name, const char *params) : EventPoolTask(name, params), fFixedNcoll(0), fEA(0), fEB(0) {;}
		PythiaAATask() : EventPoolTask(), fFixedNcoll(0), fEA(0), fEB(0) {;}
		virtual 			~PythiaAATask();
		virtual unsigned int InitThis(const char *opt = "");
		virtual unsigned int ExecThis(const char *opt = "");
	protected:
		int			  	   fFixedNcoll;
		double 			   fEA;
		double 			   fEB;
	};
}

#endif
