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

#include <fastjet/PseudoJet.hh>

namespace GenUtil
{
	HepMCPSJUserInfo::HepMCPSJUserInfo(HepMC::GenParticle *p)
		: fastjet::PseudoJet::UserInfoBase::UserInfoBase()
		, fParticle(p)
		, fEvent(0)
		, fIndex(-1)
	{
		if (fParticle)
		{
			fEvent = fParticle->parent_event();
			fIndex = fParticle->barcode();
		}
	}

	HepMC::GenParticle* HepMCPSJUserInfo::getParticle()
	{
		if (fParticle)
			return fParticle;
		if (fEvent)
			return fEvent->barcode_to_particle(fIndex);
		else
			return 0x0;
	}

	HepMCEventWrapper::~HepMCEventWrapper()
	{
		if (fCleanUpEvent)
		{
			Ldebug << "cleanup of the allocated event";
			fCleanUpEvent->clear();
			delete fCleanUpEvent;
		}
		delete fIn;
		delete fOut;
	}

	HepMC::GenEvent* HepMCEventWrapper::GetEvent() {return fEvent;}

	void HepMCEventWrapper::SetEvent(HepMC::GenEvent *ev)
	{
		if (fCleanUpEvent)
		{
			fCleanUpEvent->clear();
			delete fCleanUpEvent;
			fCleanUpEvent = 0;
		}
		fEvent = ev;
	}

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
			{
				fEvent = new HepMC::GenEvent();
				fCleanUpEvent = fEvent;
			}
			else
			{
				fEvent->clear();
			}
			// ToHepMC.fill_next_event( &fPythia8->event, fEvent, -1, &fPythia8->info, &fPythia8->settings );
			ToHepMC.fill_next_event( *fPythia8, fEvent, fPythia8->info.getCounter(4));
			return true;
		}
		// read from a file - next event
		if (fIn)
		{
			fEvent->clear();
			delete fEvent;
			fEvent = fIn->read_next_event();
			fCleanUpEvent = fEvent;
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

	HepMC::GenCrossSection* HepMCEventWrapper::GetCrossSecion()
	{
		if (fEvent) return fEvent->cross_section();
		Lerror << "No event - unable to read cross section";
		return 0x0;
	}

	HepMC::PdfInfo* HepMCEventWrapper::GetPDFinfo()
	{
		if (fEvent) return fEvent->pdf_info();
		Lerror << "No event - unable to read PDF info";
		return 0x0;
	}

	HepMC::WeightContainer* HepMCEventWrapper::GetWeightContainer()
	{
		if (fEvent) return &(fEvent->weights());
		Lerror << "No event - unable to read PDF info";
		return 0x0;
	}

	std::list<HepMC::GenVertex*> HepMCEventWrapper::Vertices()
	{
		fVertices.clear();
		if (!fEvent)
			Lerror << "No event - unable to read vertices...";
		for ( HepMC::GenEvent::vertex_iterator v = fEvent->vertices_begin(); v != fEvent->vertices_end(); ++v )
		{
			fVertices.push_back(*v);
		}
		return fVertices;
	}

	std::vector<HepMC::GenParticle*> HepMCEventWrapper::HepMCParticles(bool only_final, int status)
	{
		if (only_final and status == 0)
		{
			// this does not make sense, so revert to default status == 1
			status = 1;
		}
		fParticles.clear();
		if (!fEvent)
			Lerror << "No event - unable to read particles...";
		for ( HepMC::GenEvent::particle_iterator p = fEvent->particles_begin();
			p != fEvent->particles_end(); ++p )
		{
			HepMC::GenParticle* pmc = *p;
			if (only_final)
			{
				if ( !pmc->end_vertex() && pmc->status() == status )
				{
					fParticles.push_back(pmc);
				}
			}
			else
			{
				if (status != 0)
				{
					if (pmc->status() == status)
					{
						fParticles.push_back(pmc);
					}
				}
				else
				{
					fParticles.push_back(pmc);
				}
			}
		}
		return fParticles;
	}

	std::vector<fastjet::PseudoJet> HepMCEventWrapper::PseudoJetParticles(bool only_final, int status)
	{
		if (only_final and status == 0)
		{
			// this does not make sense, so revert to default status == 1
			status = 1;
		}
		fPseudoJets.clear();
		if (!fEvent)
			Lerror << "No event - unable to read particles...";
		for ( HepMC::GenEvent::particle_iterator p = fEvent->particles_begin();
			p != fEvent->particles_end(); ++p )
		{
			HepMC::GenParticle* pmc = *p;
			HepMC::FourVector vpmc = pmc->momentum();
			if (only_final)
			{
				if ( !pmc->end_vertex() && pmc->status() == status )
				{
					fastjet::PseudoJet psj(vpmc.px(), vpmc.py(), vpmc.pz(), vpmc.e());
					psj.set_user_index(pmc->barcode());
					HepMCPSJUserInfo *uinfo = new HepMCPSJUserInfo(fEvent, pmc->barcode());
					psj.set_user_info(uinfo);
					fPseudoJets.push_back(psj);
				}
			}
			else
			{
				if (status != 0)
				{
					if (pmc->status() == status)
					{
						fParticles.push_back(pmc);
					}
				}
				else
				{
					fParticles.push_back(pmc);
				}
			}
		}
		return fPseudoJets;
	}
}
