#include <jetty/util/pythia/param_sigmas.h>
#include <jetty/util/pythia/pyutil.h>
#include <jetty/util/blog.h>

#include <TF1.h>

namespace PyUtil
{
	ParamSigmas::ParamSigmas()
		: fNames()
		, fXsections()
	{
		fNames[kTOTAL] = "TOTAL";
		fNames[kND]    = "ND";
		fNames[kEL]    = "EL";
		fNames[kSDXB]  = "SDXB";
		fNames[kSDAX]  = "SDAX";
		fNames[kDD]    = "DD";
		fNames[kINEL]  = "INEL";

		for (int i = 0; i < kMax; i++)
		{
			TF1 *f = new TF1(fNames[i].c_str(), "[0]+[1]*TMath::Log(x[0]*x[0]*[2])+[3]*TMath::Sqrt(x[0])", 10, 14000.);
			switch (i)
			{
				case kTOTAL:
					f->SetParameter(0, 51.6389472848);
					f->SetParameter(1, 3.05613665785);
					f->SetParameter(2, 1.0884771575e-05);
					f->SetParameter(3, 0.237484785118);
					break;
				case kND:
					f->SetParameter(0, 13.9458383924);
					f->SetParameter(1, 1.30765188439);
					f->SetParameter(2, 0.482746511151);
					f->SetParameter(3, 0.17585155832);
					break;
				case kEL:
					f->SetParameter(0, 3.4829688946);
					f->SetParameter(1, 0.682186029981);
					f->SetParameter(2, 0.0583341374122);
					f->SetParameter(3, 0.0678140660216);
					break;
				case kSDXB:
					f->SetParameter(0, 4.26269098963);
					f->SetParameter(1, 0.273015705424);
					f->SetParameter(2, 0.000110832307191);
					f->SetParameter(3, -0.00495655964888);
					break;
				case kSDAX:
					f->SetParameter(0, 2.13245329645);
					f->SetParameter(1, 0.273557270118);
					f->SetParameter(2, 0.281858024986);
					f->SetParameter(3, -0.00520758649784);
					break;
				case kDD:
					f->SetParameter(0, 0.454483509367);
					f->SetParameter(1, 0.532155223171);
					f->SetParameter(2, 0.027704438448);
					f->SetParameter(3, 0.00159635726621);
					break;
				case kINEL:
					f->SetParameter(0, 13.1350155312);
					f->SetParameter(1, 2.31312978595);
					f->SetParameter(2, 3.20896552109);
					f->SetParameter(3, 0.172282595553);
					break;
				default:
					Lfatal << "No cross section for process " << i;
					f->SetParameter(0, 0.0);
					f->SetParameter(1, 0.0);
					f->SetParameter(2, 0.0);
					f->SetParameter(3, 0.0);
					break;
			}
			fXsections[i] = f;
		}
	}

	ParamSigmas::~ParamSigmas()
	{
		for (int i = 0; i < 6; i++)
		{
			delete fXsections[i];
		}
		Ltrace << "destroyed.";
	}

	TF1 *ParamSigmas::Get(int iwhich)
	{
		if (iwhich < kMax && iwhich >= 0)
		{
			return fXsections[iwhich];
		}
		else
		{
			Lerror << "requested unknown xsection: " << iwhich;
		}
		return 0x0;
	}

	TF1 *ParamSigmas::Get(const char *str_which)
	{
		for (int i = 0; i < 6; i++)
		{
			if (fNames[i] == str_which)
			{
				return fXsections[i];
			}
			else
			{
				Lerror << "requested unknown xsection: " << str_which;
			}
		}
		return 0x0;
	}

	double ParamSigmas::Get(int iwhich, double eCM)
	{
		TF1 *f = Get(iwhich);
		if (f)
		{
			return f->Eval(eCM);
		}
		return 0;
	}

	double ParamSigmas::Get(int iwhich, double eA, double eB)
	{
		double eCM = PyUtil::sqrts(eA, eB);
		return Get(iwhich, eCM);
	}

	double ParamSigmas::Get(const char *str_which, double eCM)
	{
		TF1 *f = Get(str_which);
		if (f)
		{
			return f->Eval(eCM);
		}
		return 0;
	}

	double ParamSigmas::Get(const char *str_which, double eA, double eB)
	{
		double eCM = PyUtil::sqrts(eA, eB);
		return Get(str_which, eCM);
	}

}
