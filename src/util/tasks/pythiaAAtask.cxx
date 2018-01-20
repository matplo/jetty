#include <jetty/util/tasks/pythiaAAtask.h>
#include <jetty/util/tasks/glaubertask.h>
#include <jetty/util/tglaubermc/tglaubermc.h>
#include <jetty/util/pythia/event_pool.h>
#include <jetty/util/pythia/pythia_pool.h>
#include <jetty/util/blog.h>
#include <jetty/util/strutil.h>

#include <Pythia8/Pythia.h>

#include <cstdio>
#include <string>

namespace GenUtil
{
	PythiaAATask::~PythiaAATask()
	{
		Ltrace << "destructor " << GetName();
	}

	unsigned int PythiaAATask::ExecThis(const char *opt)
	{
		fEventPool->Clear();
		Ltrace << "PythiaAATask::ExecThis " << GetName() << " with option: " << opt;
		PyUtil::PythiaPool &pypool = PyUtil::PythiaPool::Instance();
		if (fFixedNcoll > 0)
		{
			for (int ic = 0; ic < fFixedNcoll; ic++)
			{
				auto ppythia = pypool.GetPythia(fEA, fEB);
				if (!ppythia)
				{
					Lfatal << GetName() << " unable to get pythia with eA=" << fEA << " eB=" << fEB;
					return kError;
				}
				Pythia8::Pythia &pythia = *ppythia;

				int failed_counter = 0;
				while (pythia.next() == false)
				{
					failed_counter++;
					continue;
					if (failed_counter > 10)
					{
						Lwarn << GetName() << " pythia event generation failed - something is off... count: " << failed_counter;
						return kError;
					}
				}
				auto &event             = pythia.event;
				fEventPool->AddEvent(event);
			}
		}
		else
		{
			TGlauberMC *glauber = 0x0;
			auto *glt = (GlauberTask*)fParent;
			if (glt)
				glauber = glt->GetGlauberMC();
			if (!glauber)
			{
				Lfatal << GetName() << " no ncoll specified and no glauber-parent present...";
				Lfatal << GetName() << " parent is at: " << fParent;
				if (fParent)
					Lfatal << GetName() << " parent name is: " << fParent->GetName();
				Lfatal << GetName() << " stop here.";
				return kError;
			}
			auto collisions = glauber->GetCollisions();
			int ncoll = collisions.size();
			Ltrace << GetName() << "generating ncoll = " << ncoll;
			for (auto &c : collisions)
			{
				double tmp_fEA = c.GetA()->GetEnergy();
				double tmp_fEB = c.GetB()->GetEnergy();
				if (tmp_fEB == 0 && tmp_fEA == 0)
				{
					tmp_fEA = fEA;
					tmp_fEB = fEB;
				}
				auto ppythia = pypool.GetPythia(tmp_fEA, tmp_fEB);
				if (!ppythia)
				{
					Lfatal << GetName() << " unable to get pythia with eA=" << tmp_fEA << " eB=" << tmp_fEB;
					return kError;
				}
				Pythia8::Pythia &pythia = *ppythia;
				int failed_counter = 0;
				while (pythia.next() == false)
				{
					failed_counter++;
					continue;
					if (failed_counter > 10)
					{
						Lwarn << GetName() << " pythia event generation failed - something is off... count: " << failed_counter;
						return kError;
					}
				}
				auto &event = pythia.event;
				fEventPool->AddEvent(event);
			}
		}
		return kGood;
	}

	unsigned int PythiaAATask::InitThis(const char *opt)
	{
		fArgs.merge(opt);
		if (fStatus == kBeforeInit)
		{
			Linfo << "PythiaAATask::InitThis " << GetName() << " with opts: " << fArgs.asString();
			Linfo << "PythiaAATask::InitThis " << GetName() << " fStatus: " << fStatus;

			fEA = fArgs.getD("Beams:eA", 2510.);
			fEB = fArgs.getD("Beams:eB", 2510.);
			fArgs.set("Beams:eA", fEA);
			fArgs.set("Beams:eB", fEB);

			PyUtil::PythiaPool &pypool = PyUtil::PythiaPool::Instance();
			pypool.SetCommonSettings(fArgs.asString().c_str());
			pypool.SetupECMs(fEA, fEB, fArgs.getD("--pythia-pool-ndiv", 50));
		}
		fFixedNcoll = fArgs.getI("--AA-fixed-ncoll", 0);
		Linfo << "PythiaAATask::Init " << GetName() << " done.";
		return kGood;
	}
}
