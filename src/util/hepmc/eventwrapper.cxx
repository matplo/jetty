#include <jetty/util/hepmc/eventwrapper.h>
#include <jetty/util/blog.h>

#include <HepMC/IO_GenEvent.h>
#include <HepMC/GenEvent.h>
#include <HepMC/GenCrossSection.h>
#include <HepMC/PdfInfo.h>
#include <HepMC/WeightContainer.h>

#include <algorithm>
#include <list>

#include <iostream>
using namespace std;

#include <Pythia8/Pythia.h>
#include <Pythia8Plugins/HepMC2.h>

namespace GenUtil
{
	HepMCEventWrapper::~HepMCEventWrapper()
	{
		delete fEvent;
		delete fIn;
		delete fOut;
	}

	HepMC::GenEvent* HepMCEventWrapper::GetEvent() {return fEvent;}

	void HepMCEventWrapper::SetPythia8(Pythia8::Pythia *pythia)
	{
		if (fPythia8 && fPythia8 != pythia)
		{
			Lwarn << "changing pythia 8 generator";
		}
		fPythia8 = pythia;
	}

	void HepMCEventWrapper::SetInputFile(const char *fname, bool force)
	{
		if (fIn)
		{
			if (force)
			{
				Lwarn << "changing hepmc input file to " << fname;
				delete fIn;
				fIn = 0;
			}
		}
		if (!fIn)
		{
			fIn = new HepMC::IO_GenEvent(fname, std::ios::in);
			if ( fIn->rdstate() == std::ios::failbit )
			{
				Lfatal << "unable to read from: " << fname << endl;
				delete fIn;
				fIn = 0;
			}
		}
	}

	void HepMCEventWrapper::SetOutputFile(const char *fname, bool force)
	{
		if (fOut)
		{
			Lwarn << "changing hepmc output file to " << fname;
			if (force)
			{
				delete fOut;
				fOut = 0;
			}
		}
		if (!fOut)
		{
			fOut = new HepMC::IO_GenEvent(fname, std::ios::out);
			if ( fOut->rdstate() == std::ios::failbit )
			{
				Lfatal << "unable to read from: " << fname << endl;
				delete fOut;
				fOut = 0;
			}
		}
	}

	bool HepMCEventWrapper::FillEvent()
	{
		if (fPythia8 && fIn)
		{
			Lfatal << "should not read from both file and pythia generator. stop here.";
			return false;
		}
		// depending on the settings
		// read from pythia8 that is set
		if (fPythia8)
		{
			HepMC::Pythia8ToHepMC ToHepMC;
			ToHepMC.set_print_inconsistency(true);
			// ToHepMC.set_free_parton_warnings(true);
			// ToHepMC.set_crash_on_problem(false);
			ToHepMC.set_convert_gluon_to_0(false);
			ToHepMC.set_store_pdf(true);
			ToHepMC.set_store_proc(true);
			ToHepMC.set_store_xsec(true);

			if (!fEvent)
				fEvent = new HepMC::GenEvent();
			else
				fEvent->clear();
			// ToHepMC.fill_next_event( &fPythia8->event, fEvent, -1, &fPythia8->info, &fPythia8->settings );
			ToHepMC.fill_next_event( *fPythia8, fEvent, fPythia8->info.getCounter(4));
			return true;
		}
		// read from a file - next event
		if (fIn)
		{
			delete fEvent;
			fEvent = fIn->read_next_event();
			return true;
		}
		return false;
	}

	bool HepMCEventWrapper::WriteEvent()
	{
		if (fOut)
		{
			HepMC::IO_GenEvent &rfOut = *fOut;
			rfOut << fEvent;
			return true;
		}
		return false;
	}
}
