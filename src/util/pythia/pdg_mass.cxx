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

	double PDGMass::beta(double pt, double eta, double phi, double m)
	{
		TLorentzVector tlv;
		tlv.SetPtEtaPhiM(pt, eta, phi, m);
		return tlv.Beta();
	}

	double PDGMass::gamma(double beta) // relat gamma
	{
		return 1.0 / TMath::Sqrt(1 - beta*beta);
	}

	double PDGMass::beta(double m, double p) // relat beta
	{
		return (p / TMath::Sqrt(p*p + m*m)); // beta = p / E
	}

	double PDGMass::gamma(double m, double p) // relat gamma
	{
		Double_t b = beta(m, p);
		return 1.0 / TMath::Sqrt(1 - b*b);
	}

	double PDGMass::tof(double m, double p, double L) // tof = L / beta
	{
		return L / beta(m, p);
	}

}
