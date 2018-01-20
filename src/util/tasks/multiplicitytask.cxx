#include <jetty/util/tasks/multiplicitytask.h>
#include <jetty/util/pythia/event_pool.h>

#include <jetty/util/blog.h>

namespace GenUtil
{

	MultiplicityTask::~MultiplicityTask()
	{
		Ltrace << "destructor " << GetName();
	}

	unsigned int MultiplicityTask::InitThis(const char *opt)
	{
		return kGood;
	}

	unsigned int MultiplicityTask::ExecThis(const char *opt)
	{
		for (auto &t : fInputTasks)
		{
			auto evpool = t->GetData()->get<PyUtil::EventPool>();
			auto fstate_parts = evpool->GetFinalParticles();
			Linfo << "from : " << t->GetName() << " number of final state parts: " << fstate_parts.size();
		}
		return kGood;
	}
}
