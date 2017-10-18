#ifndef __HEPMCUTIL_READFILE__hh
#define __HEPMCUTIL_READFILE__hh

#include <HepMC/IO_GenEvent.h>
#include <HepMC/GenEvent.h>

#include <vector>
#include <list>

#include <TParticle.h>
#include <fastjet/PseudoJet.hh>

class ReadHepMCFile
{
public:
	ReadHepMCFile(const char *fname);
	virtual ~ReadHepMCFile();

	bool 								NextEvent();

	HepMC::GenCrossSection* 			GetCrossSecion();
	HepMC::PdfInfo* 					GetPDFinfo();
	HepMC::WeightContainer*  			GetWeightContainer();
	std::list<HepMC::GenVertex*> 		Vertices();
	std::vector<HepMC::GenParticle*> 	HepMCParticles(bool only_final = true);

private:
	HepMC::IO_GenEvent fIn;
	HepMC::GenEvent* fEvent;
	std::list<HepMC::GenVertex*> fVertices;
	std::vector<HepMC::GenParticle*> fParticles;
	std::vector<TParticle> fTParticles;
	std::vector<fastjet::PseudoJet> fPseudoJets;
	long int fCurrentEvent;
};

#endif
