#ifndef __JETTY_UTIL_PYTHIAAATASK_HH
#define __JETTY_UTIL_PYTHIAAATASK_HH

#include <vector>

#include <jetty/util/wrapper/wrapper.h>
#include <jetty/util/pythia/pyargs.h>
#include <jetty/util/tasks/eventpooltask.h>

class TGlauNucleon;

namespace GenUtil
{
	class PythiaAATask : public EventPoolTask
	{
	public:
		PythiaAATask(const char *name) : EventPoolTask(name), fFixedNcoll(0), fEA(0), fEB(0), fISR(0) , fFSR(0) {;}
		PythiaAATask(const char *name, const char *params) : EventPoolTask(name, params), fFixedNcoll(0), fEA(0), fEB(0), fISR(0) , fFSR(0) {;}
		PythiaAATask() : EventPoolTask(), fFixedNcoll(0), fEA(0), fEB(0), fISR(0) , fFSR(0) {;}
		virtual 			~PythiaAATask();
		virtual unsigned int InitThis(const char *opt = "");
		virtual unsigned int ExecThis(const char *opt = "");
	protected:
		std::string 	   SetupXSR(TGlauNucleon *nA, TGlauNucleon *nB);
		int			  	   fFixedNcoll;
		double 			   fEA;
		double 			   fEB;
		int 			   fISR; // control ISR for wounded nuncleons: 0 (default) - no XSR for wounded only; -1 - always off (even for non-wounded); 1 - always on - also for wounded
		int 			   fFSR; // control ISR for wounded nuncleons: 0 (default) - no XSR for wounded only; -1 - always off (even for non-wounded); 1 - always on - also for wounded
	};
}

#endif
