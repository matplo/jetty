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
		PyUtil::EventPool *GetEventPool() {return fEventPool;}
	protected:
		PyUtil::EventPool 		*fEventPool;
	};
}

#endif
