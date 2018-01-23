#ifndef __JETTY_UTIL_EVENTPOOLTASK_HH
#define __JETTY_UTIL_EVENTPOOLTASK_HH

#include <vector>

#include <jetty/util/wrapper/wrapper.h>
#include <jetty/util/pythia/pyargs.h>
#include <jetty/util/tasks/gentask.h>

namespace PyUtil
{
	class EventPool;
}

class TGlauberMC;

namespace GenUtil
{
	// a convenience class
	// init creates pythia or grabs last instance from the fShared
	class EventPoolTask : public GenTask
	{
	public:
		EventPoolTask(const char *name);
		EventPoolTask(const char *name, const char *params);
		EventPoolTask();
		virtual 			~EventPoolTask();
		virtual unsigned int Init(const char *opt = "");

		PyUtil::EventPool 	*GetEventPool() {return fEventPool;}
		TGlauberMC 			*GetGlauberMC() {return fpGlauberMC;}
	protected:
		PyUtil::EventPool 		*fEventPool;
		TGlauberMC 				*fpGlauberMC;
	};
}

#endif
