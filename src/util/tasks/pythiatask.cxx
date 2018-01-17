#include <jetty/util/tasks/pythiatask.h>
#include <jetty/util/blog.h>
#include <jetty/util/strutil.h>

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
		return kGood;
	}

	unsigned int PythiaTask::Init(const char *opt)
	{
		fArgs.merge(opt);
		Linfo << "PythiaTask::Init " << GetName() << " with opts: " << fArgs.asString();
		Linfo << "PythiaTask::Init " << GetName() << " fStatus: " << fStatus;
		if (fStatus == kBeforeInit)
		{
			if (fArgs.isSet("new"))
			{
				fpPythia = new Pythia8::Pythia();
				std::string slabel = StrUtil::sT(GetName()) + "_Pythia";
				fShared->add(fpPythia, slabel.c_str());
				fShared->list();
				fArgs.remove("new");
			}
			else
			{
				fpPythia = fShared->get<Pythia8::Pythia>();
			}
		}
		Linfo << "PythiaTask::Init " << GetName() << " pythia at: " << fpPythia;
		return GenTask::Init(opt);
	}

	///---
	SpectraPtHatBins::~SpectraPtHatBins()
	{
		;
	}

	unsigned int SpectraPtHatBins::InitThis(const char *opt)
	{
		Linfo << "SpectraPtHatBins::InitThis " << GetName() << " pythia at: " << fpPythia;
		return kGood;
	}

	unsigned int SpectraPtHatBins::ExecThis(const char *opt)
	{
		Ltrace << "SpectraPtHatBins::ExecThis " << GetName() << " with option: " << opt;
		Ltrace << "SpectraPtHatBins::ExecThis " << GetName() << " pythia at: " << fpPythia;
		return kGood;
	}
}
