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

#include <jetty/util/pythia/event_pool.h>
#include <jetty/util/tglaubermc/tglaubermc.h>

#include <string>
#include <iostream>
using namespace std;

void test_args_merge();

int gentasks (const std::string &s)
{
    // test(s); return;
    PyUtil::Args args(s);

    GenUtil::GlauberTask g0("glauber", args.asString().c_str());
    GenUtil::PythiaAATask pythiaTAA("pythiaAA", args.asString().c_str());
    g0.AddTask(&pythiaTAA);

    GenUtil::PythiaTask pythiaT("pythia", args.asString().c_str());
    g0.AddTask(&pythiaT);

    GenUtil::MultiplicityTask mult("mult");
    mult.AddInputTask(&pythiaTAA);
    mult.AddInputTask(&pythiaT);
    g0.AddTask(&mult);

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
        Ldebug << " -> number of final state particles: " << pythiaTAA.GetEventPool()->GetFinalParticles().size();
    }

    g0.Finalize();

    Linfo << "glauber N exec calls: " << g0.GetNExecCalls();
    Linfo << "pythiaTAA N exec calls: " << pythiaTAA.GetNExecCalls();

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
