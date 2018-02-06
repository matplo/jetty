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
			if (!fpGlauberMC)
			{
				Lfatal << GetName() << " no ncoll specified and no fpGlauberMC set...";
				Lfatal << GetName() << " stop here.";
				return kError;
			}
			auto collisions = fpGlauberMC->GetCollisions();
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
				string pysetup = SetupXSR(c.GetA(), c.GetB());
				auto ppythia = pypool.GetPythia(tmp_fEA, tmp_fEB, pysetup.c_str());
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

	std::string PythiaAATask::SetupXSR(TGlauNucleon *nA, TGlauNucleon *nB)
	{
		PyUtil::Args args;
		args.merge(fArgs);

		if (fISR == -1)
		{
			args.add("PartonLevel:ISR=off");
		}
		if (fISR == 1)
		{
			args.add("PartonLevel:ISR=on");
		}
		if (fISR == 0)
		{
			if (nA->GetNColl()-1 > 0 || nB->GetNColl()-1 > 0)
				args.add("PartonLevel:ISR=off");
			else
				args.add("PartonLevel:ISR=on");
		}

		if (fFSR == -1)
		{
			args.add("PartonLevel:FSR=off");
			args.add("PartonLevel:FSRinProcess=off");
			args.add("PartonLevel:FSRinResonances=off");
		}
		if (fFSR == 1)
		{
			args.add("PartonLevel:FSR=on");
			args.add("PartonLevel:FSRinProcess=on");
			args.add("PartonLevel:FSRinResonances=on");
		}
		if (fFSR == 0)
		{
			if (nA->GetNColl()-1 > 0 || nB->GetNColl()-1 > 0)
			{
				Ltrace << "since one of the nucleons is wounded: " << (nA->GetNColl()-1 > 0) << " ? " << (nB->GetNColl()-1 > 0) << " turning off the FSR";
				args.add("PartonLevel:FSR=off");
				args.add("PartonLevel:FSRinProcess=off");
				args.add("PartonLevel:FSRinResonances=off");
			}
			else
			{
				args.add("PartonLevel:FSR=on");
				args.add("PartonLevel:FSRinProcess=on");
				args.add("PartonLevel:FSRinResonances=on");
			}
		}

		return args.asString();
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
		fISR = fArgs.getI("--AA-ISR", 0); // XSR only for nucleons that are not wounded
		fFSR = fArgs.getI("--AA-FSR", 0); // XSR only for nucleons that are not wounded
		Linfo << "PythiaAATask::Init " << GetName() << " done.";
		return kGood;
	}
}
