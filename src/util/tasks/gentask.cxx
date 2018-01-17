#include <jetty/util/tasks/gentask.h>
#include <jetty/util/blog.h>
#include <jetty/util/strutil.h>
#include <jetty/util/tglaubermc/tglaubermc.h>

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
		PyUtil::Args _opts(opt);
		Linfo << "PythiaTask::Init " << GetName() << " with opts: " << _opts.asString();
		Linfo << "PythiaTask::Init " << GetName() << " fStatus: " << fStatus;
		if (fStatus == kBeforeInit)
		{
			if (_opts.isSet("new"))
			{
				fpPythia = new Pythia8::Pythia();
				std::string slabel = StrUtil::sT(GetName()) + "_Pythia";
				fShared->add(fpPythia, slabel.c_str());
				fShared->list();
				_opts.remove("new");
			}
			else
			{
				fpPythia = fShared->get<Pythia8::Pythia>();
			}
		}
		Linfo << "PythiaTask::Init " << GetName() << " pythia at: " << fpPythia;
		return GenTask::Init(_opts.asString().c_str());
	}

	///---

	GlauberTask::~GlauberTask()
	{
		;
	}

	unsigned int GlauberTask::FinalizeThis(const char *opt)
	{
		Ltrace << "destructor " << GetName();
		if (!fpGlauberMC)
		{
			Lwarn << GetName() << " GlauberMC not initialized. Nothing to be done.";
			return kDone;
		}

	    Linfo << GetName() << " Events " << GetNExecCalls();
	    Linfo << GetName() << " -> "
	    	<< " x-sect = " << fpGlauberMC->GetTotXSect()
	    	<< " +- " << fpGlauberMC->GetTotXSectErr() << " b ";
		string fname            = fArgs.get("--glauber-fname", "");
		const Double_t omega    = fArgs.getD("--glauber-omega", -1);
		const Double_t noded    = fArgs.getD("--glauber-noded", -1);
		TString om;
		if ((omega>=0) && (omega<=1))
		{
			om=Form("-om%.1f",omega);
		}
		TString name;
		if (fname.size() > 0)
		{
			name = fname.c_str();
		}
		else
		{
			TString nd;
			if (noded>0)
				nd=Form("-nd%.1f",noded);
			name = Form("%s%s%s.root",fpGlauberMC->Str(),om.Data(),nd.Data());
		}
		Linfo << GetName() << " writing file: " << name;
		TFile out(name,"recreate",name,9);
		TNtuple  *nt = fpGlauberMC->GetNtuple();
		if (nt)
			nt->Write();
		else
			Lwarn << GetName() << " no glauber ntuple to write.";
		out.Close();
		Linfo << GetName() << " done.";
		return kDone;
	}

	unsigned int GlauberTask::ExecThis(const char *opt)
	{
		Ltrace << "GlauberTask::ExecThis " << GetName() << " with option: " << opt;
		if (fpGlauberMC)
		{
			fpGlauberMC->Run(1, fFixedb);
		}
		return kGood;
	}

	unsigned int GlauberTask::Init(const char *opt)
	{
		fArgs.merge(opt);
		Linfo << "GlauberTask::Init " << GetName() << " with opts: " << fArgs.asString();
		Linfo << "GlauberTask::Init " << GetName() << " fStatus: " << fStatus;

		fFixedb = fArgs.getD("--glauber-fixed-b", -1);

		string sysA             = fArgs.get("--glauber-A", "Pb");
		string sysB             = fArgs.get("--glauber-B", "Pb");
		Linfo << "GlauberTask::Init " << GetName() << " sysA: " << sysA << " sysB: " << sysB;

		// const Int_t n           = 1;
		const Double_t signn    = fArgs.getD("--glauber-signn", 67.6);
		const Double_t sigwidth = fArgs.getD("--glauber-sigwidth", 1);
		const Double_t mind     = fArgs.getD("--glauber-mind", 0.4);
		const Double_t omega    = fArgs.getD("--glauber-omega", -1);
		const Double_t noded    = fArgs.getD("--glauber-noded", -1);

		fpGlauberMC = new TGlauberMC(sysA.c_str(),sysB.c_str(),signn,sigwidth);
		Linfo << "GlauberTask::Init " << GetName() << " GlauberMC at: " << fpGlauberMC;

		fpGlauberMC->SetMinDistance(mind);
		fpGlauberMC->SetNodeDistance(noded);
		fpGlauberMC->SetCalcLength(fArgs.isSet("--glauber-calc-length"));
		fpGlauberMC->SetCalcArea(fArgs.isSet("--glauber-calc-area"));
		fpGlauberMC->SetDetail(fArgs.getI("--glauber-detail", 99));
		TString om;
		if ((omega>=0) && (omega<=1))
		{
			TF1 *f1 = getNNProf(signn, omega);
			fpGlauberMC->SetNNProf(f1);
			om=Form("-om%.1f",omega);
		}

		string slabel = StrUtil::sT(GetName()) + "_GlauberMC";
		fShared->add(fpGlauberMC, slabel.c_str());
		fShared->list();

		// fpGlauberMC->Run(n);

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
