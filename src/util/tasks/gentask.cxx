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

	std::vector<GenTask*> GenTask::fTasks;

	GenTask *GenTask::GetTask(unsigned int id)
	{
		if (id >= fTasks.size())
		{
			Lfatal << "asked for a non-existing task ID: " << id;
			return 0x0;
		}
		return fTasks[id];
	}

	void GenTask::AddInputTask(GenTask *t)
	{
		fInputTasks.push_back(t);
	}

	void GenTask::DumpTaskListInfo()
	{
	    Linfo << "GenTask::DumpTaskListInfo() number of tasks: " << fTasks.size();
		for (auto &t : fTasks)
		{
			if (t)
				Linfo << " -- task id: " << t->GetId() << " " << t->GetName() << " status: " << t->GetStatus();
		}
	}


	void GenTask::Cleanup()
	{
		for (auto &t : fTasks)
		{
			if (t)
				delete t;
		}
		// this is not needed?
		// fTasks.clear();

		fShared->set_debug(1);
		delete fShared;
		fShared = 0;
	}

	GenTask::~GenTask()
	{
		Ltrace << GetName() << " destroy begin...";
		// fData->set_debug(1);
		delete fData;
		fData = 0;
		Ltrace << GetName() << " removing this " << this->GetId() << " one from the list of tasks";
		fTasks[this->GetId()] = 0x0;
		int counter = 0;
		for (auto &t : fTasks) if (t != 0) counter++;
		Ltrace << GetName() << " remaining tasks: " << counter << " out of " << fTasks.size() << " slots";
	    Ltrace << GetName() << " destroyed.";
	}

	GenTask::GenTask(const char *name)
		: fName(name), fArgs(), fSubtasks(), fParent(0), fStatus(kBeforeInit), fNExecCalls(0), fTaskId(0), fInputTasks(), fData(new Wrapper)
		, fOutputPath(""), fOutputFile(0), fOutputTree(0)
	{
		_instance_counter += 1;
		fTaskId = fTasks.size();
		fTasks.push_back(this);
	}

	GenTask::GenTask(const char *name, const char *params)
		: fName(name), fArgs(params), fSubtasks(), fParent(0), fStatus(kBeforeInit), fNExecCalls(0), fTaskId(0), fInputTasks(), fData(new Wrapper)
		, fOutputPath(""), fOutputFile(0), fOutputTree(0)
	{
		_instance_counter += 1;
		fTaskId = fTasks.size();
		fTasks.push_back(this);
		std::string _prefix = fArgs.get("--task-prefix");
		std::string _postfix = fArgs.get("--task-postfix");
		fName = _prefix + fName + _postfix;
		fOutputPath = fName + ".root";
	}

	GenTask::GenTask()
		: fName("GenTask"), fArgs(), fSubtasks(), fParent(0), fStatus(kBeforeInit), fNExecCalls(0), fTaskId(0), fInputTasks(), fData(new Wrapper)
		, fOutputPath(""), fOutputFile(0), fOutputTree(0)
	{
		_instance_counter += 1;
		std::string tmpname = "GenTask_";
		tmpname += StrUtil::sT(_instance_counter);
		SetName(tmpname);
		fOutputPath = GetName() + ".root";
		fTaskId = fTasks.size();
		fTasks.push_back(this);
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
		if (fStatus == kGood || fStatus == kSkipEvent || fStatus == kDefinedStop)
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
		if (fStatus == kSkipEvent)
		{
			// event skipped so we can reset to good...
			Ltrace << GetName() << "::Execute: reset status " << fStatus << " to " << kGood;
			fStatus = kGood;
		}
		if (fStatus != kDefinedStop)
		{
			fStatus = this->ExecThis(opt);
			if (fStatus != kGood)
			{
				iret = fStatus;
				Ltrace << GetName() << "::Execute: skipping deps of " << this->GetName() << " w/ status = " << fStatus;
				if (iret == kSkipEvent)
				{
					Ltrace << GetName() << "::Execute: reset status " << fStatus << " to " << kGood;
					fStatus = kGood;
				}
			}
			else
			{
				for(auto t : fSubtasks)
				{
					auto istat = t->GetStatus();
					if (istat != kGood && istat != kSkipEvent)
					{
						if (istat == kSkipEvent)
						{
							t->SetStatus(kGood);
							Ltrace << GetName() << "::Execute: reset status of " << t->GetName() << " from " << iret << " to " << t->GetStatus();
						}
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
		}
		return iret;
	}

	void GenTask::AddTask(GenTask *t)
	{
		t->SetParent(this);
		fSubtasks.push_back(t);
		Linfo << GetName() << "::AddTask : +" << t->GetName() << " (Nsubtasks=" << fSubtasks.size() << ")";
	}
}
