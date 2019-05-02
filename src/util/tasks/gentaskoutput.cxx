#include <jetty/util/tasks/gentaskoutput.h>
#include <jetty/util/blog.h>
#include <jetty/util/wrapper/wrapper.h>

#include <TList.h>
#include <TFile.h>

namespace GenUtil
{
	GenTaskOutput::GenTaskOutput()
		: fWrapper(new Wrapper)
	{
		;
	}

	GenTaskOutput::~GenTaskOutput()
	{
		Linfo << "GenTaskOutput::~GenTaskOutput : deleting the wrapper container";
		delete fWrapper;
	}

	TFile *GenTaskOutput::GetOutput(const char *fname)
	{
		TFile *f = 0x0;
		if (fname)
		{
			f = fWrapper->get<TFile>(fname);
			if (!f)
			{
				Lwarn << "new file in recreate mode: " << fname;
				f = new TFile(fname, "recreate");
				if (f)
				{
					fWrapper->add(f, fname);
					return f;
				}
			}
		}
		return f;
	}
}
