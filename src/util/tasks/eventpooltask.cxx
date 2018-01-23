#include <jetty/util/tasks/eventpooltask.h>
#include <jetty/util/tasks/glaubertask.h>
#include <jetty/util/blog.h>
#include <jetty/util/strutil.h>
#include <jetty/util/pythia/event_pool.h>

#include <jetty/util/tglaubermc/tglaubermc.h>

#include <cstdio>
#include <string>

namespace GenUtil
{
	EventPoolTask::EventPoolTask(const char *name) : GenTask(name), fEventPool(new PyUtil::EventPool), fpGlauberMC(0)
	{
		fData->add(fEventPool);
	}

	EventPoolTask::EventPoolTask(const char *name, const char *params) : GenTask(name, params), fEventPool(new PyUtil::EventPool), fpGlauberMC(0)
	{
		fData->add(fEventPool);
	}

	EventPoolTask::EventPoolTask() : GenTask(), fEventPool(new PyUtil::EventPool), fpGlauberMC(0)
	{
		fData->add(fEventPool);
	}

	unsigned int EventPoolTask::Init(const char *opt)
	{
		for (auto &t : fInputTasks)
		{
			auto *glt = (GlauberTask*)t;
			fpGlauberMC = glt->GetGlauberMC();
			if (fpGlauberMC)
				Linfo << "got GlauberMC at " << fpGlauberMC << " from task : " << t->GetName();
		}
		return GenTask::Init(opt);
	}

	EventPoolTask::~EventPoolTask()
	{
		Ltrace << "destructor " << GetName();
	}

}
