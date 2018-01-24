#include <jetty/util/tasks/multiplicitytask.h>
#include <jetty/util/tglaubermc/tglaubermc.h>
#include <jetty/util/pythia/event_pool.h>

#include <jetty/util/blog.h>

#include <algorithm>

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
		int npart = 2;
		Ldebug << "fpGlauberMC: " << fpGlauberMC;
		if (fpGlauberMC)
			npart = fpGlauberMC->GetNpart();
		for (auto &t : fInputTasks)
		{
			auto evpool = t->GetData()->get<PyUtil::EventPool>();
			if (!evpool) continue;
			auto fstate_parts = evpool->GetFinalParticles();
			std::vector<double> etas;
			for ( auto & p : fstate_parts )
			{
				if (p.isCharged())
					etas.push_back(p.eta());
			}
			Linfo << "from : " << t->GetName() << " number of final state parts: " << fstate_parts.size();
			unsigned int dNdeta = std::count_if(etas.begin(), etas.end(), [] (double _eta) {return fabs(_eta) < 1.;});
			// try to do something like this... - needs new event structure... get another task going...
			// unsigned int dNdeta = std::count_if(fstate_parts.begin(), fstate_parts.end(), [] (TPartile p) {return fabs(p.Eta()) < 1.;});
			Linfo << "from : " << t->GetName() << " number of final state charged parts: " << etas.size();
			Linfo << "from : " << t->GetName() << " dN/dEta in abs(eta) < 1: " << dNdeta / 2.;
			Linfo << "from : " << t->GetName() << " dN/dEta in abs(eta) < 1 per Npart: " << dNdeta / 2. / npart
				<< " npart = " << npart;
		}
		return kGood;
	}
}
