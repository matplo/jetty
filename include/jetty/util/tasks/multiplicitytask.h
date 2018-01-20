#ifndef __JETTY_UTIL_MULTIPLICITYTASK_HH
#define __JETTY_UTIL_MULTIPLICITYTASK_HH

#include <jetty/util/tasks/eventpooltask.h>

namespace GenUtil
{
	// a convenience class
	// init creates pythia or grabs last instance from the fShared
	class MultiplicityTask : public EventPoolTask
	{
	public:
		MultiplicityTask(const char *name) : EventPoolTask(name) {;}
		MultiplicityTask(const char *name, const char *params) : EventPoolTask(name, params) {;}
		MultiplicityTask() : EventPoolTask() {;}
		virtual 			~MultiplicityTask();
		virtual unsigned int InitThis(const char *opt = "");
		virtual unsigned int ExecThis(const char *opt = "");
	protected:
	};
}

#endif // __JETTY_UTIL_MULTIPLICITYTASK_HH
