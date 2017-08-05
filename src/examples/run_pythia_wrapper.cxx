#include "run_pythia_wrapper.h"

#include "util/pythia/pythia_wrapper.h"
#include "util/pythia/pyargs.h"
#include "util/pythia/pyutil.h"
#include "util/pythia/crosssections.h"
#include "util/pythia/outkinematics.h"

#include "util/looputil.h"
#include "util/blog.h"

#include "util/rstream/hstream.h"

#include <TFile.h>
#include <TTree.h>
#include <TH1F.h>
#include <TMath.h>

#include <string>
#include <iostream>

using namespace std;

int run_pythia_wrapper (const std::string &s)
{
	PyUtil::Args args(s);
	if (args.isSet("--invalid"))
	{
		Linfo << "invalid parameters. stop here.";
		return 0;
	}
	Linfo << args.asString("[pythia_run_wrapper:status]");
	if (args.isSet("--dry")) return 0;

	PyUtil::PythiaWrapper pywrap(args.asString());
	if (!pywrap.initialized())
	{
		Lwarn << "PYTHIA not initialized. Stop Here.";
		return 1;
	}
	PyUtil::Args &pyargs      = *pywrap.args();
	Pythia8::Pythia &pythia = *pywrap.pythia();
	auto &event             = pythia.event;

	pywrap.outputFile()->cd();
	TTree *tk = new TTree("kine", "kine");
	TH1F *hpT = new TH1F("hpT", "pT;p_{T} (GeV/#it{c});counts", 50, 0, 100);

	RStream::HStream hstream;
	if (args.isSet("--hconfig"))
	{
		hstream.Init("other_name", args.get("--hconfig").c_str(), true, pywrap.outputFile());
		hstream.SkipUndefined(!args.isSet("--debug"));
	}

	// this is where the event loop section starts
	auto nEv = pyargs.getI("Main:numberOfEvents");
	LoopUtil::TPbar pbar(nEv);
	for (unsigned int iE = 0; iE < nEv; iE++)
	{
		pbar.Update();
		if (pywrap.next() == false) continue;

		// some kinematics info
		*tk << PyUtil::OutKinematics(pythia);

		// loop over particles in the event
		for (unsigned int ip = 0; ip < event.size(); ip++)
		{
			if (event[ip].isFinal())
			{
				if (TMath::Abs(event[ip].eta()) < 1.)
					hpT->Fill(event[ip].pT(), 1./event[ip].pT());
				hstream << "part_" << event[ip];
				hstream << "undefined_" << event[ip];
				double pt_eta[] = {event[ip].pT(), event[ip].eta()};
				hstream << "part_pt_eta" << pt_eta;
			}
		}
	}
	hstream.Scale(pythia.info.sigmaGen() / pythia.info.weightSum());
	pythia.stat();
	Linfo << "Generation done.";

	return 0;
}
