#include <string>

#include <jetty/util/pythia/pyargs.h>
#include <jetty/util/pythia/pyutil.h>
#include <jetty/util/pythia/gentask.h>
#include <jetty/util/looputil.h>
#include <jetty/util/blog.h>


using namespace std;

int gentasks (const std::string &s)
{
    // test(s); return;
    PyUtil::Args args(s);

    PyUtil::PythiaTask pythiaT("pythia", "pythia", args.asString().c_str());
    PyUtil::SpectraPtHatBins* aTask = new PyUtil::SpectraPtHatBins();
    PyUtil::SpectraPtHatBins* aTask2 = new PyUtil::SpectraPtHatBins();

	Ltrace << "tasks setup";

    pythiaT.Add(aTask);

	Ltrace << "added aTask";

    aTask->Add(aTask2);

	Ltrace << "added aTask2";

	Ltrace << "tasks add done";

    int nEv = 1000;
    LoopUtil::TPbar pbar(nEv);
    for (int i = 0; i < nEv; i++)
    {
    	pbar.Update();
    	pythiaT.ExecuteTask("<an option>");
    }
}
