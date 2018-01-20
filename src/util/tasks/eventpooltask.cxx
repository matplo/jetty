#include <jetty/util/tasks/eventpooltask.h>
#include <jetty/util/blog.h>
#include <jetty/util/strutil.h>
#include <jetty/util/pythia/event_pool.h>

#include <cstdio>
#include <string>

namespace GenUtil
{
	EventPoolTask::EventPoolTask(const char *name) : GenTask(name), fEventPool(new PyUtil::EventPool)
	{
		fData->add(fEventPool);
	}
	EventPoolTask::EventPoolTask(const char *name, const char *params) : GenTask(name, params), fEventPool(new PyUtil::EventPool)
	{
		fData->add(fEventPool);
	}
	EventPoolTask::EventPoolTask() : GenTask(), fEventPool(new PyUtil::EventPool)
	{
		fData->add(fEventPool);
	}

	EventPoolTask::~EventPoolTask()
	{
		Ltrace << "destructor " << GetName();
	}

}
