#include <jetty/util/hepmc/readertask.h>
#include <jetty/util/hepmc/readfile.h>
#include <jetty/util/blog.h>
#include <jetty/util/strutil.h>

#include <cstdio>
#include <string>

namespace GenUtil
{
	HepMCReaderTask::~HepMCReaderTask()
	{
		Ltrace << "destructor " << GetName();
	}

	unsigned int HepMCReaderTask::ExecThis(const char *opt)
	{
		Ltrace << "HepMCReaderTask::ExecThis " << GetName() << " with option: " << opt;
		if (fReader->NextEvent() == false) return kDefinedStop;
		return kGood;
	}

	unsigned int HepMCReaderTask::InitThis(const char *opt)
	{
		unsigned int status = kError;
		fArgs.merge(opt);
		Linfo << "HepMCReaderTask::Init " << GetName() << " with opts: " << fArgs.asString();
		Linfo << "HepMCReaderTask::Init " << GetName() << " fStatus: " << fStatus;
		if (fStatus == kBeforeInit)
		{
			// check if this is a directory
			// then load files
			// then make sure you swap files when needed in ExecThis
			// add possibility to add text file with a list of files as input
			// need a new class? - extend ReadHepMCFile - this sounds reasonable...
			// read x-section from HEPMC
			fReader = new ReadHepMCFile(fArgs.get("--hepmc-input").c_str());

			if (!fReader)
			{
				Lfatal << GetName() << " unable to create new reader!";
				return kError;
			}

			fData->add(fReader);
			status = kGood;
		}
		Linfo << "HepMCReaderTask::Init " << GetName() << " reader at: " << fReader;
		return status;
	}
}
