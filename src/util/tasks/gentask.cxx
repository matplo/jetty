#include <jetty/util/tasks/gentask.h>
#include <jetty/util/blog.h>
#include <jetty/util/strutil.h>

#include <Pythia8/Pythia.h>

#include <cstdio>
#include <string>

namespace GenUtil
{
	unsigned int 	GenTask::_instance_counter 	= 0;
	Wrapper * 		GenTask::fShared 			= new Wrapper();

	GenTask::~GenTask()
	{
		_instance_counter -= 1;
		if (fShared && _instance_counter == 0)
		{
			delete fShared;
			fShared = 0;
		}
	}

	GenTask::GenTask()
		: fName("GenTask"), fArgs(), fSubtasks(), fParent(0), fStatus(0), fNExecCalls(0)
	{
		_instance_counter += 1;
		std::string tmpname = "GenTask_";
		tmpname += StrUtil::sT(_instance_counter);
		SetName(tmpname);
	}

	unsigned int GenTask::ExecThis(const char *opt)
	{
		Ltrace << "GenTask::ExecThis " << GetName() << " with options: " << opt;
		return kGood;
	}

	unsigned int GenTask::InitThis(const char *opt)
	{
		Ltrace << "GenTask::InitThis " << GetName() << " with options: " << opt;
		return kGood;
	}

	unsigned int GenTask::FinalizeThis(const char *opt)
	{
		Ltrace << "GenTask::FinalizeThis " << GetName() << " with options: " << opt;
		return kDone;
	}

	unsigned int GenTask::Init(const char *opt)
	{
		Ltrace << "GenTask::Init " << GetName() << " with options: " << opt;
		if (fStatus == kBeforeInit)
		{
			fStatus = InitThis(opt);
		}
		for(auto t : fSubtasks)
		{
			Ltrace << " -- call ::Init of " << t->GetName();
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
		fNExecCalls+=1;
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
