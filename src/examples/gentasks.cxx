#include <string>

#include <jetty/util/pythia/pyargs.h>
#include <jetty/util/pythia/pyutil.h>
#include <jetty/util/tasks/gentask.h>
#include <jetty/util/looputil.h>
#include <jetty/util/blog.h>


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

    int nEv = args.getI("--nev", 5);
    LoopUtil::TPbar pbar(nEv);
    for (int i = 0; i < nEv; i++)
    {
    	pbar.Update();
    	pythiaT.Execute("<an option>");
    }

    pythiaT.Finalize();
}
