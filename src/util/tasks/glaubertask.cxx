#include <jetty/util/tasks/gentask.h>
#include <jetty/util/blog.h>
#include <jetty/util/strutil.h>
#include <jetty/util/tglaubermc/tglaubermc.h>
#include <jetty/util/tasks/glaubertask.h>

#include <cstdio>
#include <string>

namespace GenUtil
{

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

	unsigned int GlauberTask::InitThis(const char *opt)
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

		return kGood;
	}
}
