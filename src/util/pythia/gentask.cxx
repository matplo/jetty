#include <jetty/util/pythia/gentask.h>
#include <jetty/util/blog.h>

#include <Pythia8/Pythia.h>

#include <cstdio>
#include <string>

namespace PyUtil
{
	GenTask::GenTask() : TTask(), fArgs()
	{
		std::string tmpname = std::tmpnam(nullptr);
		SetName(tmpname.c_str());
		SetTitle(tmpname.c_str());
	}

	PythiaTask::~PythiaTask()
	{
		Ltrace << "destructor " << GetName();
	}

	void PythiaTask::Exec(Option_t *option)
	{
		Ltrace << "PythiaTask " << GetName() << " with option: " << option;
	}

	SpectraPtHatBins::~SpectraPtHatBins()
	{
		;
	}

	void SpectraPtHatBins::Exec(Option_t *option)
	{
		Ltrace << "SpectraPtHatBins " << GetName() << " with option: " << option;
	}
}
