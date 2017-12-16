#include <jetty/util/tasks/gentask.h>
#include <jetty/util/blog.h>
#include <jetty/util/strutil.h>

#include <Pythia8/Pythia.h>

#include <cstdio>
#include <string>

namespace GenUtil
{
	unsigned int GenTask::_instance_counter = 0;

	GenTask::GenTask()
		: fName("GenTask"), fArgs(), fSubtasks(), fParent(0), fStatus(0)
	{
		_instance_counter += 1;
		std::string tmpname = "GenTask_";
		tmpname += StrUtil::sT(_instance_counter);
		SetName(tmpname);
	}

	unsigned int GenTask::ExecThis(const char *opt)
	{
		Ltrace << GetName() << "::ExecThis with options: " << opt;
		return kGood;
	}

	unsigned int GenTask::InitThis(const char *opt)
	{
		Ltrace << GetName() << "::InitThis with options: " << opt;
		return kGood;
	}

	unsigned int GenTask::FinalizeThis(const char *opt)
	{
		Ltrace << GetName() << "::FinalizeThis with options: " << opt;
		return kDone;
	}

	unsigned int GenTask::Init(const char *opt)
	{
		if (fStatus == kBeforeInit)
		{
			fStatus = InitThis(opt);
		}
		for(auto t : fSubtasks)
		{
			auto iret_tmp = t->Init(opt);
			t->SetStatus(iret_tmp);
		}
		return fStatus;
	}

	unsigned int GenTask::Finalize(const char *opt)
	{
		if (fStatus == kGood)
		{
			fStatus = FinalizeThis(opt);
			if (fStatus != kDone)
			{
				Lwarn << GetName() << "::FinalizeThis did not result in kDone. Forcing it.";
				fStatus = kDone;
			}
		}
		for(auto t : fSubtasks)
		{
			auto iret_tmp = t->Finalize(opt);
			t->SetStatus(iret_tmp);
		}
		return fStatus;
	}

	unsigned int GenTask::Execute(const char *opt)
	{
		unsigned int iret = kGood;
		if (fStatus == kError)
		{
			Lfatal << GetName() << "::Execute: status is kError. Stop here.";
			return kError;
		}
		if (fStatus == kDone)
		{
			Lfatal << GetName() << "::Execute: is marked as done. Stop here.";
			fStatus = kError;
			return kError;
		}
		if (fStatus == kBeforeInit)
		{
			Lfatal << GetName() << "::Execute: not initialized. Stop here.";
			fStatus = kError;
			return kError;
		}
		if (fStatus != kDefinedStop)
		{
			fStatus = this->ExecThis(opt);
			for(auto t : fSubtasks)
			{
				auto istat = t->GetStatus();
				if (istat != kGood)
				{
					Ltrace << GetName() << "::Execute: skipping " << t->GetName() << " w/ status = " << t->GetStatus();
					continue;
				}
				auto iret_tmp = t->Execute(opt);
				t->SetStatus(iret_tmp);
				if (iret == kDefinedStop)
				{
					fStatus = kDefinedStop;
					iret = fStatus;
				}
				if (iret_tmp == kError)
				{
					Lfatal << GetName() << "::Execute: " << t->GetName() << " returned with kError.";
					fStatus = iret_tmp;
					iret = fStatus;
					break;
				}
			}
		}
		return iret;
	}

	void GenTask::AddTask(GenTask *t)
	{
		t->SetParent(this);
		fSubtasks.push_back(t);
		Linfo << GetName() << "::AddTask : +" << t->GetName() << " (Nsubtasks=" << fSubtasks.size() << ")";
	}

	///---

	PythiaTask::~PythiaTask()
	{
		Ltrace << "destructor " << GetName();
	}

	unsigned int PythiaTask::ExecThis(const char *opt)
	{
		Ltrace << "PythiaTask " << GetName() << " with option: " << opt;
		return kGood;
	}

	SpectraPtHatBins::~SpectraPtHatBins()
	{
		;
	}

	unsigned int SpectraPtHatBins::ExecThis(const char *opt)
	{
		Ltrace << "SpectraPtHatBins " << GetName() << " with option: " << opt;
		return kGood;
	}
}
