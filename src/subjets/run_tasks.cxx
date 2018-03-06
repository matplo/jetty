#include <jetty/subjets/run_tasks.h>
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

int run_tasks (const std::string &s)
{
	// test(s); return;
	PyUtil::Args args(s);

	GenUtil::GenTask *r = 0;

	if (args.isSet("--hepmc-input"))
	{
		GenUtil::HepMCReaderTask *r_hepmc = new GenUtil::HepMCReaderTask("hepmc_reader", args.asString().c_str());
		r = r_hepmc;
	}
	else
	{
		GenUtil::PythiaTask *r_pythia = new GenUtil::PythiaTask("pythia_task", args.asString().c_str());
		r = r_pythia;
	}

	GenUtil::SubjetTask sj_kt("subjets_kt", (args.asString() + "--sjA=0").c_str());
	sj_kt.AddInputTask(r);
	r->AddTask(&sj_kt);

	GenUtil::SubjetTask sj_ca("subjets_ca", (args.asString() + "--sjA=1").c_str());
	sj_ca.AddInputTask(r);
	r->AddTask(&sj_ca);

	GenUtil::SubjetTask sj_akt("subjets_akt", (args.asString() + "--sjA=2").c_str());
	sj_akt.AddInputTask(r);
	r->AddTask(&sj_akt);

	GenUtil::SubjetTask sj_akt_sjR15("subjets_akt_sjR15", (args.asString() + "--sjA=2 --sjR=0.15").c_str());
	sj_akt_sjR15.AddInputTask(r);
	r->AddTask(&sj_akt_sjR15);

	GenUtil::SubjetTask sj_akt_sjR20("subjets_akt_sjR20", (args.asString() + "--sjA=2 --sjR=0.2").c_str());
	sj_akt_sjR20.AddInputTask(r);
	r->AddTask(&sj_akt_sjR20);

	r->Init();
	r->DumpTaskListInfo();

	int nEv = args.getI("--nev", 5);
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
	Linfo << "gentasks_hepmc is done." << endl;
	return 0;

}

