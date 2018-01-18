#include <jetty/util/tasks/gentask.h>
#include <jetty/util/blog.h>
#include <jetty/util/strutil.h>
#include <jetty/util/tglaubermc/tglaubermc.h>
#include <jetty/util/tasks/glaubertask.h>
#include <jetty/util/pythia/param_sigmas.h>
#include <jetty/util/pythia/pyutil.h>

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
			Ltrace << " - number of collisions: " << fpGlauberMC->GetCollisions().size() << " ?= " << fpGlauberMC->GetNcoll();
			auto colls = fpGlauberMC->GetCollisions();
			for (auto &c : colls)
				Ltrace << " -- " << c.GetA()->GetEnergy() << " - " << c.GetB()->GetEnergy();
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

		Double_t eA = fArgs.getD("--eA", 0.0);
		Double_t eB = fArgs.getD("--eB", 0.0);
		if (eA == 0.0)
			eA = fArgs.getD("Beams:eA");
		if (eB == 0.0)
			eB = fArgs.getD("Beams:eB");
		Double_t eCM = fArgs.getD("Beams:eCM", 0.0);
		if ( eCM > 0)
		{
			if (eA == 0 && eB == 0)
			{
				eA = eCM / 2.;
				eB = eCM / 2.;
			}
		}
		else
		{
			if (eA > 0 || eB > 0)
			{
				eCM = PyUtil::sqrts(eA, eB);
				Lwarn << "adjusted eCM: eA = " << eA << " eB = " << eB << " << eCM = " << eCM;
			}
			else
			{
				eCM = 5020.; // LHC Run-2 energy for PbPb
				if (eA == 0 && eB == 0)
				{
					eA = eCM / 2.;
					eB = eCM / 2.;
				}
				Lwarn << "no sqrt(s) info given... running with default eCM = " << eCM << " GeV";
			}
		}

		Linfo << "eA = " << eA << " eB = " << eB << " << eCM = " << eCM;

		// const Int_t n           = 1;
		Double_t signn         = fArgs.getD("--glauber-signn", 0.0);
		if (signn == 0.0)
		{
			signn = 67.6;
			Linfo << "trying to figure out sigma nn (INEL) from sqrt(s)...";
			if (eCM > 0)
			{
				signn = PyUtil::ParamSigmas::Instance().Get(PyUtil::ParamSigmas::kINEL, eCM);
			}
			Lwarn << "adjusted signn = " << signn;
		}
		Double_t sigwidth      = fArgs.getD("--glauber-sigwidth", -1);
		Double_t mind          = fArgs.getD("--glauber-mind", 0.4);
		Double_t omega         = fArgs.getD("--glauber-omega", -1);
		Double_t noded         = fArgs.getD("--glauber-noded", -1);
		Bool_t   updateNNxsect = fArgs.isSet("--glauber-update-NNxsect");
		fpGlauberMC = new TGlauberMC(sysA.c_str(),sysB.c_str(),signn,sigwidth,updateNNxsect);
		Linfo << "GlauberTask::Init " << GetName() << " GlauberMC at: " << fpGlauberMC;

		fpGlauberMC->SetEnergyPerNucleon(eA, eB);

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
