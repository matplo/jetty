#ifndef __HEPMCUTIL_EVENTWRAPPER__hh
#define __HEPMCUTIL_EVENTWRAPPER__hh

namespace HepMC
{
	class IO_GenEvent;
	class GenEvent;
	class GenVertex;
	class GenParticle;
	class GenCrossSection;
	class PdfInfo;
	class WeightContainer;
}

namespace Pythia8
{
	class Pythia;
}

#include <list>
#include <vector>

#include <fastjet/PseudoJet.hh>

namespace GenUtil
{
	class HepMCPSJUserInfo : public fastjet::PseudoJet::UserInfoBase
	{
	public:
		HepMCPSJUserInfo()
			: fastjet::PseudoJet::UserInfoBase::UserInfoBase(), fParticle(0), fEvent(0), fIndex(-1)
			{;}
		HepMCPSJUserInfo(HepMC::GenEvent *ev, int x)
			: fastjet::PseudoJet::UserInfoBase::UserInfoBase(), fParticle(0), fEvent(ev), fIndex(x)
			{fParticle = getParticle();}
		HepMCPSJUserInfo(HepMC::GenParticle *p);

		virtual ~HepMCPSJUserInfo() {;}

		void setIndex(int x) {fIndex = x;}
		int getIndex(){return fIndex;}
		void setEvent(HepMC::GenEvent* ev) {fEvent = ev;}
		HepMC::GenEvent* getEvent() {return fEvent;}
		HepMC::GenParticle* getParticle();

	private:
		HepMC::GenParticle *fParticle;
		HepMC::GenEvent* 	fEvent;
		int 			 	fIndex;
	};

	class HepMCEventWrapper
	{
	public:
		HepMCEventWrapper() : fIn(0), fOut(0), fEvent(0), fCleanUpEvent(0), fPythia8(0), fVertices(), fParticles(), fPseudoJets() {;}
		virtual ~HepMCEventWrapper();
		HepMC::GenEvent* GetEvent();
		void SetEvent(HepMC::GenEvent *ev);

		void SetPythia8(Pythia8::Pythia *pythia);
		void SetInputFile(const char *fname, bool force = false);
		void SetOutputFile(const char *fname, bool force = false);
		bool FillEvent();
		bool WriteEvent();

		HepMC::GenCrossSection* 			GetCrossSecion();
		HepMC::PdfInfo* 					GetPDFinfo();
		HepMC::WeightContainer*  			GetWeightContainer();
		std::list<HepMC::GenVertex*> 		Vertices();
		std::vector<HepMC::GenParticle*> 	HepMCParticles(bool only_final = true, int status = 0);
		std::vector<fastjet::PseudoJet> 	PseudoJetParticles(bool only_final = true, int status = 0);

	private:
		HepMC::IO_GenEvent* fIn;
		HepMC::IO_GenEvent* fOut;
		HepMC::GenEvent* fEvent;
		HepMC::GenEvent* fCleanUpEvent;
		Pythia8::Pythia* fPythia8;

		std::list<HepMC::GenVertex*> fVertices;
		std::vector<HepMC::GenParticle*> fParticles;
		std::vector<fastjet::PseudoJet> fPseudoJets;
	};
}

#endif
