#include <jetty/util/tasks/gentaskoutput.h>
#include <jetty/util/blog.h>

#include <TList.h>
#include <TFile.h>

namespace GenUtil
{
	GenTaskOutput::GenTaskOutput()
		: fListOfFiles(0)
		, fListOfObjects(0)
	{
		fListOfFiles = new TList();
		fListOfFiles->SetOwner(kTRUE);

		fListOfObjects = new TList();
		fListOfObjects->SetOwner(kFALSE);
	}

	GenTaskOutput::~GenTaskOutput()
	{
		Linfo << "~GenTaskOutput() : number of files to write = " << fListOfFiles->GetSize();
		// TIter next(fListOfFiles);
		// while (( TFile *f = dynamic_cast<TFile*>(next()) ))
		for(const auto&& o: *fListOfFiles)
		{
			TFile *f = dynamic_cast<TFile*>(o);
			if (f)
			{
				f->Write();
				f->Close();
				Linfo << "~GenTaskOutput() : written: " << f->GetName();
			}
		}
		delete fListOfFiles;
		delete fListOfObjects;
		Ltrace << "GenTaskOutput destroyed.";
	}

	TFile *GenTaskOutput::GetOutput(const char *fname)
	{
		TFile *f = 0x0;
		if (fname)
		{
			f = dynamic_cast<TFile*>(fListOfFiles->FindObject(fname));
			if (!f)
			{
				Lwarn << "new file in recreate mode: " << fname;
				f = new TFile(fname, "recreate");
				if (f)
				{
					fListOfFiles->Add(f);
					return f;
				}
			}
		}
		return f;
	}

	TObject *GenTaskOutput::GetOutputObject(const char *name)
	{
		return fListOfObjects->FindObject(name);
	}

	TObject *GenTaskOutput::RegisterOutputObject(TObject *o)
	{
		if (o) fListOfObjects->Add(o);
		TObject *reto = GetOutputObject(o->GetName());
		return reto;
	}
}
