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
#include <jetty/util/tasks/gpythiatask.h>

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

void test_args_merge();

int gentasks_debug (const std::string &s)
{
	PyUtil::Args args(s);
	PyUtil::PythiaPool &pypool = PyUtil::PythiaPool::Instance();
	pypool.SetCommonSettings(s.c_str());
	pypool.SetupECMs(1000, 1000, 50);
	pypool.GetPythia(900, 900);
	// pypool.GetPythia(1002, 1002, s.c_str());
	Linfo << "just get the pool and done...";
	LogUtil::cout_sink _cout_sink;
	LogUtil::cerr_sink _cerr_sink;
	return 1;
}

int gentasks (const std::string &s)
{
	// test(s); return;
	PyUtil::Args args(s);

	if (args.isSet("--random-seed"))
	{
		gRandom->SetSeed(args.getI("--random-seed", 0));
	}
	Linfo << "random seed is: " << gRandom->GetSeed();

	// PyUtil::Args args1("Beams:eB=20 Beams:eA=25 Beams:eB=15 Beams:eB=201");
	// PyUtil::Args args2("Beams:eA=25 Beams:eB=20");
	// Linfo << args1.asString();
	// Linfo << args1.get("Beams:eA");
	// Linfo << args1.get("Beams:eB");
	// Linfo << args2.asString();
	// Linfo << args2.get("Beams:eA");
	// Linfo << args2.get("Beams:eB");
	// Linfo << (args1==args2);
	// Linfo << args1.asString();
	// Linfo << args2.asString();
	// Linfo << args1.pairs().size() << " " << args2.pairs().size();
	// return 0;

	GenUtil::GlauberTask g0("glauber", args.asString().c_str());
	GenUtil::PythiaAATask pythiaTAA("pythiaAA", args.asString().c_str());
	g0.AddTask(&pythiaTAA);
	pythiaTAA.AddInputTask(&g0);
	// GenUtil::PythiaTask pythiaT("pythia", args.asString().c_str());
	// g0.AddTask(&pythiaT);
	GenUtil::MultiplicityTask mult("mult", args.asString().c_str());
	g0.AddTask(&mult);
	mult.AddInputTask(&pythiaTAA);
	// mult.AddInputTask(&pythiaT);
	mult.AddInputTask(&g0);

	g0.Init();

	pythiaTAA.DumpTaskListInfo();

	int nEv = args.getI("--nev", 5);
	if (args.isSet("-h") || args.isSet("--help"))
		nEv = 1;

	LoopUtil::TPbar pbar(nEv);
	for (int i = 0; i < nEv; i++)
	{
		pbar.Update();
		g0.Execute("<an option>");
		if (g0.GetStatus() == GenUtil::GenTask::kError)
			break;
		Ldebug << "number of collisions: " << g0.GetGlauberMC()->GetNcoll();
		// Ldebug << " -> number of final state particles: " << pythiaTAA.GetEventPool()->GetFinalParticles().size();
	}

	g0.Finalize();

	Linfo << "glauber N exec calls: " << g0.GetNExecCalls();
	// Linfo << "pythiaTAA N exec calls: " << pythiaTAA.GetNExecCalls();

	Linfo << "gentasks is done." << endl;
	return 0;
}

int gpythia (const std::string &s)
{
	// test(s); return;
	PyUtil::Args args(s);

	if (args.isSet("--random-seed"))
	{
		gRandom->SetSeed(args.getI("--random-seed", 0));
	}
	Linfo << "random seed is: " << gRandom->GetSeed();

	GenUtil::GlauberTask g0("glauber", args.asString().c_str());
	GenUtil::PythiaAATask pythiaTAA("pythiaAA", args.asString().c_str());
	g0.AddTask(&pythiaTAA);
	pythiaTAA.AddInputTask(&g0);
	GenUtil::MultiplicityTask mult("mult", args.asString().c_str());
	g0.AddTask(&mult);
	mult.AddInputTask(&pythiaTAA);
	mult.AddInputTask(&g0);

	GenUtil::GPythiaTask gpythia("gpythia", args.asString().c_str());
	g0.AddTask(&gpythia);
	gpythia.AddInputTask(&g0);
	gpythia.AddInputTask(&mult);

	g0.Init();
	g0.DumpTaskListInfo();

	int nEv = args.getI("--nev", 5);
	if (args.isSet("-h") || args.isSet("--help"))
		nEv = 1;

	LoopUtil::TPbar pbar(nEv);
	for (int i = 0; i < nEv; i++)
	{
		pbar.Update();
		g0.Execute("<an option>");
		if (g0.GetStatus() == GenUtil::GenTask::kError)
			break;
		// Ldebug << "number of collisions: " << g0.GetGlauberMC()->GetNcoll();
		// Ldebug << " -> number of final state particles: " << pythiaTAA.GetEventPool()->GetFinalParticles().size();
	}

	g0.Finalize();

	Linfo << "glauber N exec calls: " << g0.GetNExecCalls();
	// Linfo << "pythiaTAA N exec calls: " << pythiaTAA.GetNExecCalls();

	Linfo << "gentasks is done." << endl;
	return 0;
}

int gentasks_hepmc (const std::string &s)
{
	// test(s); return;
	PyUtil::Args args(s);

	GenUtil::HepMCReaderTask r("hepmc_reader", args.asString().c_str());

	GenUtil::SubjetTask sj_kt("subjets_kt", (args.asString() + "--sjA=0").c_str());
	sj_kt.AddInputTask(&r);
	r.AddTask(&sj_kt);

	GenUtil::SubjetTask sj_ca("subjets_ca", (args.asString() + "--sjA=1").c_str());
	sj_ca.AddInputTask(&r);
	r.AddTask(&sj_ca);

	GenUtil::SubjetTask sj_akt("subjets_akt", (args.asString() + "--sjA=2").c_str());
	sj_akt.AddInputTask(&r);
	r.AddTask(&sj_akt);

	GenUtil::SubjetTask sj_akt_sjR15("subjets_akt_sjR15", (args.asString() + "--sjA=2 --sjR=0.15").c_str());
	sj_akt_sjR15.AddInputTask(&r);
	r.AddTask(&sj_akt_sjR15);

	GenUtil::SubjetTask sj_akt_sjR20("subjets_akt_sjR20", (args.asString() + "--sjA=2 --sjR=0.2").c_str());
	sj_akt_sjR20.AddInputTask(&r);
	r.AddTask(&sj_akt_sjR20);

	r.Init();
	r.DumpTaskListInfo();

	int nEv = args.getI("--nev", 5);
	if (args.isSet("-h") || args.isSet("--help"))
		nEv = 1;

	LoopUtil::TPbar pbar(nEv);
	for (int i = 0; i < nEv; i++)
	{
		pbar.Update();
		r.Execute("<an option>");
		if (r.GetStatus() != GenUtil::GenTask::kGood)
			break;
		GenUtil::ReadHepMCFile *f = r.GetData()->get<GenUtil::ReadHepMCFile>();
		Ldebug << f->CurrentEventNumber() << " number of particles: " << f->PseudoJetParticles(true).size();
	}

	r.Finalize();

	Linfo << "N exec calls: " << r.GetNExecCalls();
	// Linfo << "pythiaTAA N exec calls: " << pythiaTAA.GetNExecCalls();

	Linfo << "gentasks_hepmc is done." << endl;
	return 0;

}


int gentasks_test (const std::string &s)
{
	// test(s); return;
	PyUtil::Args args(s);

	GenUtil::GlauberTask g0("glauber", args.asString().c_str());

	GenUtil::PythiaTask pythiaT("pythia", args.asString().c_str());

	GenUtil::SpectraPtHatBins task1;
	GenUtil::SpectraPtHatBins task2;

	g0.AddTask(&pythiaT);
	pythiaT.AddTask(&task1);
	task1.AddTask(&task2);

	g0.Init("new");

	int nEv = args.getI("--nev", 5);
	if (args.isSet("-h") || args.isSet("--help"))
		nEv = 1;

	LoopUtil::TPbar pbar(nEv);
	for (int i = 0; i < nEv; i++)
	{
		pbar.Update();
		g0.Execute("<an option>");
	}

	g0.Finalize();

	Linfo << "glauber N exec calls: " << g0.GetNExecCalls();
	Linfo << "pythiaT N exec calls: " << pythiaT.GetNExecCalls();
	Linfo << "task1 N exec calls: " << task1.GetNExecCalls();
	Linfo << "task2 N exec calls: " << task2.GetNExecCalls();

	if (args.isSet("--test-args-merge")) test_args_merge();

	return 0;
}

void test_args_merge()
{
	Ldebug << "-----";
	PyUtil::Args a1("--out=None");
	Linfo << a1.asString("args - 1:");

	Ldebug << "-----";
	PyUtil::Args a2("--out=ala.root");
	a1.merge(a2);
	Linfo << a1.asString("args - 2:");

	Ldebug << "-----";
	PyUtil::Args a3("--out=bela.root");
	a1.merge(a3);
	Linfo << a1.asString("args - 3:");
}

//int wrap_tests()
//{
//    Wrapper w;
//    w.set_debug(1);
//    w.add(pythiaT, "pythiaT");
//    w.add(aTask2);
//    w.add(aTask, "aTaskLabel");
//    w.list();
//
//    auto p = w.get<GenUtil::PythiaTask>("pythiaT");
//    cout << p->GetName() << endl;
//    auto px = w.get<GenUtil::SpectraPtHatBins>("aTaskLabel");
//    cout << px->GetName() << endl;
//    auto px2 = w.get<GenUtil::SpectraPtHatBins>();
//    cout << px2->GetName() << endl;
//    auto px3 = w.get<GenUtil::SpectraPtHatBins>("no_label");
//    cout << px3->GetName() << endl;
//}
