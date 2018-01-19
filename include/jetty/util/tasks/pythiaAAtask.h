#ifndef __JETTY_UTIL_PYTHIAAATASK_HH
#define __JETTY_UTIL_PYTHIAAATASK_HH

#include <vector>

#include <jetty/util/wrapper/wrapper.h>
#include <jetty/util/pythia/pyargs.h>
#include <jetty/util/tasks/gentask.h>

namespace PyUtil
{
	class EventPool;
}

namespace GenUtil
{
	class PythiaAATask : public GenTask
	{
	public:
		PythiaAATask(const char *name) : GenTask(name), fEventPool(0), fFixedNcoll(0), fEA(0), fEB(0) {;}
		PythiaAATask(const char *name, const char *params) : GenTask(name, params), fEventPool(0), fFixedNcoll(0), fEA(0), fEB(0) {;}
		PythiaAATask() : GenTask(), fEventPool(0), fFixedNcoll(0), fEA(0), fEB(0) {;}
		virtual 			~PythiaAATask();
		virtual unsigned int InitThis(const char *opt = "");
		virtual unsigned int ExecThis(const char *opt = "");
		PyUtil::EventPool *GetEventPool() {return fEventPool;}
	protected:
		PyUtil::EventPool *fEventPool;
		int			  	   fFixedNcoll;
		double 			   fEA;
		double 			   fEB;
	};
}

#endif
