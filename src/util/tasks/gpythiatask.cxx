#include <jetty/util/tasks/gpythiatask.h>
#include <jetty/util/blog.h>
#include <jetty/util/strutil.h>
#include <jetty/util/tglaubermc/tglaubermc.h>
#include <jetty/util/tasks/glaubertask.h>
#include <jetty/util/tasks/multiplicitytask.h>
#include <jetty/util/pythia/pyutil.h>
#include <jetty/util/rstream/tstream.h>

#include <cstdio>
#include <string>

namespace GenUtil
{

	GPythiaTask::~GPythiaTask()
	{
		Ltrace << "destructor " << GetName();
	}

	unsigned int GPythiaTask::FinalizeThis(const char *opt)
	{
		if (fOutput)
		{
			fOutput->cd();
			fOutputTree->Write();
			fOutput->Write();
			Linfo << GetName() << " Finalize " << " file written: " << fOutput->GetName();
		}
		//}
		return kDone;
	}

	unsigned int GPythiaTask::ExecThis(const char *opt)
	{
		Ltrace << "GPythiaTask::ExecThis " << GetName() << " with option: " << opt;

		RStream::TStream &gt = *fTStream;

		GenUtil::GlauberTask *glauber = 0;
		GenUtil::MultiplicityTask *mult = 0;
		for (auto &t : fInputTasks)
		{
			// get the glauber task
			if (!glauber)
			{
				if (GenUtil::GlauberTask *gtmp = dynamic_cast<GenUtil::GlauberTask*>(t))
				{
					glauber = gtmp;
					Ldebug << GetName() << " glauber task : " << glauber->GetName();
				}
			}
			// get the multiplicity task
			if (!mult)
			{
				if (GenUtil::MultiplicityTask *mtmp = dynamic_cast<GenUtil::MultiplicityTask*>(t))
				{
					mult = mtmp;
					Ldebug << GetName() << "    mult task : " << mult->GetName();
				}
			}
		}

		Ldebug << GetName() << " glauber task : " << glauber;
		Ldebug << GetName() << "    mult task : " << mult;

		if (!glauber || !mult)
		{
			Lfatal << GetName() << " glauber task : " << glauber;
			Lfatal << GetName() << "    mult task : " << mult;
			return kError;
		}

		// write both into to a tree
		// multiplicity in eta ranges; b, Ncol, Npart

		auto npart = glauber->GetGlauberMC()->GetNpart();
		gt << "ncol" << glauber->GetGlauberMC()->GetNcoll();
		gt << "npart" << npart;
		gt << "b" << glauber->GetGlauberMC()->GetB();
		gt << "m10" << mult->GetEstimator()->GetMultiplicity(MultiplicityEstimator::kFSPerEv, -10, 10);
		gt << "m10ch" << mult->GetEstimator()->GetMultiplicity(MultiplicityEstimator::kFSChPerEv, -10, 10);
		gt << "m1" << mult->GetEstimator()->GetMultiplicity(MultiplicityEstimator::kFSPerEv, -1, 1);
		gt << "m1ch" << mult->GetEstimator()->GetMultiplicity(MultiplicityEstimator::kFSChPerEv, -1, 1);
		gt << "m1x2" << mult->GetEstimator()->GetMultiplicity(MultiplicityEstimator::kFSPerEv, -2, 1);
		gt << "m1x2ch" << mult->GetEstimator()->GetMultiplicity(MultiplicityEstimator::kFSChPerEv, -2, 1);
		gt << "mV0A" << mult->GetEstimator()->GetMultiplicity(MultiplicityEstimator::kFSPerEv, 2.8, 5.);
		gt << "mV0Ach" << mult->GetEstimator()->GetMultiplicity(MultiplicityEstimator::kFSChPerEv, 2.8, 5.);
		gt << "mV0C" << mult->GetEstimator()->GetMultiplicity(MultiplicityEstimator::kFSPerEv, -3.7, -1.7);
		gt << "mV0Cch" << mult->GetEstimator()->GetMultiplicity(MultiplicityEstimator::kFSChPerEv, -3.7, -1.7);

		gt << endl;
		return kGood;
	}

	unsigned int GPythiaTask::InitThis(const char *opt)
	{
		fArgs.merge(opt);
		Linfo << "GPythiaTask::Init " << GetName() << " with opts: " << fArgs.asString();
		Linfo << "GPythiaTask::Init " << GetName() << " fStatus: " << fStatus;

		std::ostringstream _ss;
		_ss << GetName() << "_" << "gpythia.root";
		string outname = _ss.str();
		fOutput = new TFile(outname.c_str(), "recreate");
		if (!fOutput)
		{
			Lfatal << "unable to create output file: " << outname;
			return kError;
		}

		fOutput->cd();
		fOutputTree = new TTree("t", "t");
		fTStream    = new RStream::TStream("gpy", fOutputTree);

		return kGood;
	}
}
