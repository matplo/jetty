#include "util/pythia/pyargs.h"
#include "util/pythia/pyutil.h"
#include "util/pythia/crosssections.h"
#include "util/looputil.h"
#include "util/blog.h"

#include <Pythia8/Pythia.h>
#include <Pythia8Plugins/HepMC2.h>

#include <TFile.h>
#include <TH1F.h>
#include <TMath.h>

#include <string>
#include <iostream>

#include <HepMC/IO_GenEvent.h>
#include <HepMC/GenEvent.h>

using namespace std;

int main ( int argc, char *argv[] )
{
	PyUtil::Args args(argc, argv);
	cout << args.asString("[pythia_hepmc:status]") << endl;
	if (args.isSet("--dry")) return 0;

	// create the output root file
	string outfname = args.get("--output");
	HepMC::IO_GenEvent hepmc_output(outfname.c_str(), std::ios::out);

	// initialize pythia with a config and command line args
	Pythia8::Pythia *ppythia = PyUtil::make_pythia(args.asString());
	Pythia8::Pythia &pythia  = *ppythia;
	auto &event              = pythia.event;

	HepMC::Pythia8ToHepMC pyhepmc;
	// this is where the event loop section starts
	auto nEv = args.getI("Main:numberOfEvents");
	LoopUtil::TPbar pbar(nEv);
	for (unsigned int iE = 0; iE < nEv; iE++)
	{
		pbar.Update();
		if (pythia.next() == false) continue;

		HepMC::GenEvent* hepmc_event = new HepMC::GenEvent();
    	// ToHepMC.fill_next_event( pythia, hepmcevt );
		bool _filled = pyhepmc.fill_next_event( pythia.event, hepmc_event, iE, &pythia.info, &pythia.settings);
		hepmc_output << hepmc_event;
		delete hepmc_event;
	}
	pythia.stat();
	Linfo << "[i] Generation done." << endl;

	string xsec_outfname = outfname + ".txt";
	PyUtil::CrossSections(pythia, xsec_outfname.c_str());

	// delete the pythia
	delete ppythia;
	return 0;
}
