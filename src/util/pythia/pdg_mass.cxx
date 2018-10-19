#include <jetty/util/pythia/pdg_mass.h>
#include <jetty/util/pythia/pyutil.h>
#include <jetty/util/blog.h>

#include <TF1.h>

namespace PyUtil
{
	PDGMass::PDGMass()
		: fPDGM()
		, fMPDG()
	{
		;
	}

	PDGMass::~PDGMass()
	{
		Ltrace << "destroyed.";
	}

	void PDGMass::Add(double m, int pdg)
	{
		fPDGM[pdg] = m;
		fMPDG[m] = pdg;
	}

	double PDGMass::Mass(int pdg)
	{
		return fPDGM[pdg];
	}

	int PDGMass::PDG(double m)
	{
		return fMPDG[m];
	}

}
