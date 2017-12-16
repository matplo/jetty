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

    GenUtil::PythiaTask pythiaT("pythia", args.asString().c_str());
    GenUtil::SpectraPtHatBins aTask;
    GenUtil::SpectraPtHatBins aTask2;

    pythiaT.AddTask(&aTask);
    aTask.AddTask(&aTask2);

    pythiaT.Init();

    Wrapper w;
    w.set_debug(1);
    w.add(pythiaT, "pythiaT");
    w.add(aTask2);
    w.add(aTask, "aTaskLabel");
    w.list();

    auto p = w.get<GenUtil::PythiaTask>("pythiaT");
    cout << p->GetName() << endl;
    auto px = w.get<GenUtil::SpectraPtHatBins>("aTaskLabel");
    cout << px->GetName() << endl;
    auto px2 = w.get<GenUtil::SpectraPtHatBins>();
    cout << px2->GetName() << endl;
    auto px3 = w.get<GenUtil::SpectraPtHatBins>("no_label");
    cout << px3->GetName() << endl;

    int nEv = args.getI("--nev", 5);
    LoopUtil::TPbar pbar(nEv);
    for (int i = 0; i < nEv; i++)
    {
    	pbar.Update();
    	pythiaT.Execute("<an option>");
    }

    pythiaT.Finalize();

    return 0;
}
