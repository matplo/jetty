#include <jetty/eic/eic_tasks.h>
#include <jetty/eic/eicspectratask.h>

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

	GenUtil::SubjetTask sj_ca_sjR10("eic_subjets_ca_sjR10", (args.asString() + " --sjA=1 --sjR=0.10").c_str());
	sj_ca_sjR10.AddInputTask(r);
	r->AddTask(&sj_ca_sjR10);

	EIC::SpectraTask eic_spectra_tof0("eic_spectra_tof0", (args.asString() + " --tofR1=0.05 --tofR2=1.0 --eta=10. --deltat=10.").c_str());
	eic_spectra_tof0.AddInputTask(r);
	r->AddTask(&eic_spectra_tof0);

	EIC::SpectraTask eic_spectra_tof0a("eic_spectra_tof0a", (args.asString() + " --tofR1=0.05 --tofR2=1.0 --eta=10. --deltat=50.").c_str());
	eic_spectra_tof0a.AddInputTask(r);
	r->AddTask(&eic_spectra_tof0a);

	EIC::SpectraTask eic_spectra_tof0b("eic_spectra_tof0b", (args.asString() + " --tofR1=0.05 --tofR2=1.0 --eta=10. --deltat=100.").c_str());
	eic_spectra_tof0b.AddInputTask(r);
	r->AddTask(&eic_spectra_tof0b);

	EIC::SpectraTask eic_spectra_tof1("eic_spectra_tof1", (args.asString() + " --tofR1=0.05 --tofR2=5.0 --eta=10. --deltat=10.").c_str());
	eic_spectra_tof1.AddInputTask(r);
	r->AddTask(&eic_spectra_tof1);

	EIC::SpectraTask eic_spectra_tof1x("eic_spectra_tof1x", (args.asString() + " --tofR1=0.05 --tofR2=5.0 --eta=10. --deltat=1000.").c_str());
	eic_spectra_tof1x.AddInputTask(r);
	r->AddTask(&eic_spectra_tof1x);

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
