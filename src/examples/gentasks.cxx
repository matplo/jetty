#include <jetty/util/pythia/pyargs.h>
#include <jetty/util/pythia/pyutil.h>
#include <jetty/util/tasks/gentask.h>
#include <jetty/util/looputil.h>
#include <jetty/util/blog.h>
#include <jetty/util/strutil.h>

#include <jetty/util/wrapper/wrapper.h>

#include <string>
#include <iostream>
using namespace std;

int gentasks (const std::string &s)
{
    // test(s); return;
    PyUtil::Args args(s);

    GenUtil::GlauberTask g0;

    GenUtil::PythiaTask pythiaT("pythia", args.asString().c_str());

    GenUtil::SpectraPtHatBins task1;
    GenUtil::SpectraPtHatBins task2;

    pythiaT.AddTask(&task1);
    task1.AddTask(&task2);
    task1.AddTask(&g0);

    pythiaT.Init("new");

    int nEv = args.getI("--nev", 5);
    LoopUtil::TPbar pbar(nEv);
    for (int i = 0; i < nEv; i++)
    {
    	pbar.Update();
    	pythiaT.Execute("<an option>");
    }

    pythiaT.Finalize();

    Linfo << "pythiaT N exec calls: " << pythiaT.GetNExecCalls();
    Linfo << "task1 N exec calls: " << task1.GetNExecCalls();
    Linfo << "task2 N exec calls: " << task2.GetNExecCalls();

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
    return 0;
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
