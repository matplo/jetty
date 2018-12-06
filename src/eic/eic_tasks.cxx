#include <jetty/eic/eic_tasks.h>

#include <jetty/util/pythia/pythia_wrapper.h>
#include <jetty/util/pythia/pyargs.h>
#include <jetty/util/pythia/pyutil.h>
#include <jetty/util/pythia/crosssections.h>
#include <jetty/util/pythia/outkinematics.h>

#include <jetty/util/looputil.h>
#include <jetty/util/blog.h>

#include <jetty/util/rstream/hstream.h>

#include <TFile.h>
#include <TTree.h>
#include <TH1F.h>
#include <TMath.h>
#include <TNtuple.h>
#include <TVector3.h>

#include <fastjet/PseudoJet.hh>
#include <fastjet/ClusterSequence.hh>
namespace fj = fastjet;

#include <jetty/util/pythia/pyargs.h>
#include <jetty/util/pythia/pyutil.h>
#include <jetty/util/looputil.h>
#include <jetty/util/blog.h>
#include <jetty/util/strutil.h>
#include <jetty/util/wrapper/wrapper.h>

#include <jetty/util/tasks/gentask.h>
#include <jetty/util/tasks/glaubertask.h>
#include <jetty/util/tasks/pythiatask.h>
#include <jetty/util/tasks/pythiaAAtask.h>
#include <jetty/util/tasks/multiplicitytask.h>
#include <jetty/util/hepmc/readertask.h>
#include <jetty/util/hepmc/readfile.h>

#include <jetty/subjets/subjettask.h>

#include <jetty/util/pythia/event_pool.h>
#include <jetty/util/pythia/pythia_pool.h>
#include <jetty/util/tglaubermc/tglaubermc.h>

#include <TRandom.h>

#include <string>
#include <iostream>
using namespace std;

Long64_t n_events_hepmc(const char *s)
{
	Linfo << "figuring out number of hepmc events...";
	GenUtil::HepMCReaderTask r_hepmc("hepmc_reader_tmp", s);
	r_hepmc.Init();
	r_hepmc.DumpTaskListInfo();
	while (r_hepmc.GetStatus() == GenUtil::GenTask::kGood)
	{
		r_hepmc.Execute();
	}
	r_hepmc.Finalize();
	return r_hepmc.GetNExecCalls();
}

int eic_tasks (const std::string &s)
{
	Linfo << "This is run_ca_task...";

	PyUtil::Args args(s);

	GenUtil::GenTask *r = 0;

	Long64_t n_hepmc = 0;
	if (args.isSet("--hepmc-input"))
	{
		GenUtil::HepMCReaderTask *r_hepmc = new GenUtil::HepMCReaderTask("hepmc_reader", args.asString().c_str());
		r = r_hepmc;
		n_hepmc = n_events_hepmc(args.asString().c_str());
		Linfo << "found " << n_hepmc << " HEPMC events...";
	}
	else
	{
		GenUtil::PythiaTask *r_pythia = new GenUtil::PythiaTask("pythia_task", args.asString().c_str());
		r = r_pythia;
	}

	GenUtil::SubjetTask sj_ca_sjR10("subjets_ca_sjR10", (args.asString() + " --sjA=1 --sjR=0.10").c_str());
	sj_ca_sjR10.AddInputTask(r);
	r->AddTask(&sj_ca_sjR10);

	r->Init();
	r->DumpTaskListInfo();

	int nEv = args.getI("--nev", -1);
	if (nEv == -1)
	{
		if (n_hepmc > 0)
			nEv = n_hepmc;
		else
			nEv = 10;
	}
	if (nEv > n_hepmc && n_hepmc > 0)
		nEv = n_hepmc;
	if (args.isSet("-h") || args.isSet("--help"))
		nEv = 1;

	LoopUtil::TPbar pbar(nEv);
	for (int i = 0; i < nEv; i++)
	{
		pbar.Update();
		r->Execute("<an option>");
		if (r->GetStatus() != GenUtil::GenTask::kGood)
			break;
		GenUtil::ReadHepMCFile *f = r->GetData()->get<GenUtil::ReadHepMCFile>();
		Ldebug << f->CurrentEventNumber() << " number of particles: " << f->PseudoJetParticles(true).size();
	}

	r->Finalize();

	Linfo << "N exec calls: " << r->GetNExecCalls();
	// Linfo << "pythiaTAA N exec calls: " << pythiaTAA.GetNExecCalls();

	delete r;
	Linfo << "ca task done." << endl;
	return 0;
}
