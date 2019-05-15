#include <jetty/util/tasks/pythiatask.h>
#include <jetty/util/blog.h>
#include <jetty/util/strutil.h>
#include <jetty/util/pythia/event_pool.h>

#include <Pythia8/Pythia.h>

#include <cstdio>
#include <string>

namespace GenUtil
{
	PythiaTask::~PythiaTask()
	{
		Ltrace << "destructor " << GetName();
	}

	unsigned int PythiaTask::ExecThis(const char *opt)
	{
		Ltrace << "PythiaTask::ExecThis " << GetName() << " with option: " << opt;
		while (fpPythia->next() == false) continue;
		fEventPool->Clear();
		fEventPool->AddEvent(fpPythia->event);
		return kGood;
	}

	unsigned int PythiaTask::InitThis(const char *opt)
	{
		unsigned int status = kError;
		fArgs.merge(opt);
		Linfo << "PythiaTask::Init " << GetName() << " with opts: " << fArgs.asString();
		Linfo << "PythiaTask::Init " << GetName() << " fStatus: " << fStatus;
		if (fStatus == kBeforeInit)
		{
			fpPythia = new Pythia8::Pythia();

			if (!fpPythia)
			{
				Lfatal << GetName() << " unable to create new pythia!";
				return kError;
			}

			double eA = fArgs.getD("Beams:eA");
			double eB = fArgs.getD("Beams:eB");
			if (eA == eB)
			{
				fArgs.set("Beams:frameType=1");
			}
			else
			{
				fArgs.set("Beams:frameType=2");
			}
			// http://home.thep.lu.se/~torbjorn/pythia81html/BeamParameters.html
			// NOTE: option 3 : the beams are not back-to-back, and therefore the three-momentum of each incoming particle
			// needs to be specified, see Beams:pxA through Beams:pzB below.

			auto pairs = fArgs.pairs();
			for (unsigned int i = 0; i < pairs.size(); i++)
			{
				if (pairs[i].second.size() < 1) continue;
				Linfo << GetName() << " [init pythia] paired arg: #" << i << " " << pairs[i].first << " " << pairs[i].second;
				std::string spypar = pairs[i].first + " = " + pairs[i].second;
				fpPythia->readString(spypar.c_str());
			}

			if (fpPythia->init())
			{
				std::string slabel = StrUtil::sT(GetName()) + "_Pythia";
				// fShared->add(fpPythia, slabel.c_str());
				// fShared->list();
				fData->add(fpPythia);
				status = kGood;
			}
		}
		Linfo << "PythiaTask::Init " << GetName() << " pythia at: " << fpPythia;
		return status;
	}

	Pythia8::Pythia * PythiaTask::GetPythia()
	{
		return fData->get<Pythia8::Pythia>();
	}

	unsigned int PythiaTask::FinalizeThis(const char *opt)
	{
		if (fpPythia)
		{
			fpPythia->stat();
		}
		return kGood;
	}

	///---
	SpectraPtHatBins::~SpectraPtHatBins()
	{
		;
	}

	unsigned int SpectraPtHatBins::InitThis(const char *opt)
	{
		fpPythia = fShared->get<Pythia8::Pythia>();
		Linfo << "SpectraPtHatBins::InitThis " << GetName() << " pythia at: " << fpPythia;
		return kGood;
	}

	unsigned int SpectraPtHatBins::ExecThis(const char *opt)
	{
		Ltrace << "SpectraPtHatBins::ExecThis " << GetName() << " with option: " << opt;
		Ltrace << "SpectraPtHatBins::ExecThis " << GetName() << " pythia at: " << fpPythia;
		Ltrace << "SpectraPtHatBins::ExecThis " << GetName() << " number of particles " << fpPythia->event.size();
		return kGood;
	}
}
