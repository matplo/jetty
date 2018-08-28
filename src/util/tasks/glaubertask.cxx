#include <jetty/util/tasks/gentask.h>
#include <jetty/util/blog.h>
#include <jetty/util/strutil.h>
#include <jetty/util/tasks/glaubertask.h>
#include <jetty/util/pythia/param_sigmas.h>
#include <jetty/util/pythia/pyutil.h>
#include <jetty/util/tglaubermc/tglaubermc.h>

#include <jetty/util/rstream/tstream.h>

#include <TTree.h>
#include <TFile.h>
#include <TH2D.h>

#include <cstdio>
#include <string>

namespace GenUtil
{

	GlauberTask::~GlauberTask()
	{
		Ltrace << "destructor " << GetName();
	}

	unsigned int GlauberTask::FinalizeThis(const char *opt)
	{
		Ltrace << "destructor " << GetName();
		if (!fpGlauberMC)
		{
			Lwarn << GetName() << " GlauberMC not initialized. Nothing to be done.";
			return kDone;
		}

	    Linfo << GetName() << " Events " << GetNExecCalls();
	    Linfo << GetName() << " -> "
	    	<< " x-sect = " << fpGlauberMC->GetTotXSect()
	    	<< " +- " << fpGlauberMC->GetTotXSectErr() << " b ";
	    TString name = OutputFileName();
		Linfo << GetName() << " writing file: " << name;
		TFile out(name, "recreate", name,9);
		TNtuple  *nt = fpGlauberMC->GetNtuple();
		if (nt)
			nt->Write();
		else
			Lwarn << GetName() << " no glauber ntuple to write.";
		out.Close();
		if (fOutputFile)
		{
			fOutputFile->Write();
			fOutputFile->Close();
			delete fOutputFile;
			fOutputFile = 0;
		}
		Linfo << GetName() << " done.";
		return kDone;
	}

	const char * GlauberTask::OutputFileName()
	{
		if (!fpGlauberMC)
		{
			Lwarn << GetName() << " GlauberMC not initialized. Nothing to be done.";
			return "invalid.root";
		}

		string fname            = fArgs.get("--glauber-fname", "");
		const Double_t omega    = fArgs.getD("--glauber-omega", -1);
		const Double_t noded    = fArgs.getD("--glauber-noded", -1);
		TString om;
		if ((omega>=0) && (omega<=1))
		{
			om=Form("-om%.1f",omega);
		}
		TString name;
		if (fname.size() > 0)
		{
			name = fname.c_str();
		}
		else
		{
			TString nd;
			if (noded>0)
				nd=Form("-nd%.1f",noded);
			if (fArgs.isSet("--glauber-update-NNxsect"))
				name = Form("%s%s%s_xsectmod.root",fpGlauberMC->Str(),om.Data(),nd.Data());
			else
				name = Form("%s%s%s.root",fpGlauberMC->Str(),om.Data(),nd.Data());
		}
		return name.Data();
	}

	unsigned int GlauberTask::ExecThis(const char *opt)
	{
		Ltrace << "GlauberTask::ExecThis " << GetName() << " with option: " << opt;
		if (fpGlauberMC)
		{
			fpGlauberMC->Run(1, fFixedb);
			Ltrace << " - number of collisions: " << fpGlauberMC->GetCollisions().size() << " ?= " << fpGlauberMC->GetNcoll();
			auto colls = fpGlauberMC->GetCollisions();
			// for (auto &c : colls)
			// 	Ltrace << " -- " << c.GetA()->GetEnergy() << " - " << c.GetB()->GetEnergy();
			RStream::TStream &outT = *fTStream;
			Double_t _totalTArea = 0;
			Double_t _intArea = 0;
			const Int_t _nbins = 160;
			const Double_t _dArange = 16.;
			TH2D hA("hA", "hA", _nbins, -1. * _dArange, _dArange, _nbins, -1. * _dArange, _dArange);
			const Double_t _bsize = _dArange * 2. / _nbins;
			const Double_t _bsize2 = _bsize * _bsize;
			// Linfo << "grid size:" << _bsize;
			Double_t _rN = 0.0;
			Double_t _rN2 = 0.0;
			Double_t _intD = 0.0;
			Double_t _intD2 = 0.0;
			Double_t _maxArea = 0.0;
			Double_t _maxOverlapArea = 0.0;
			for (auto &c : colls)
			{
				if (_rN == 0.0)
				{
					Double_t signn = c.GetXsection(); // assume all collisions the same
				    _rN = TMath::Sqrt(signn/(TMath::Pi() * 10.)) / 2.;
				    Linfo << "sigma = " << signn << " r = " << _rN;
				    // _rN = 1.;
				    _rN2 = _rN * _rN;
				    outT << "r" << _rN;
				    outT << "sigma" << signn;
				    _intD = _rN * 2.;
				    _intD2 = _intD * _intD;
				    _maxOverlapArea = CalculateIntersectionArea(0, _rN, _rN);
				}
				Int_t ncells = 0;
				for (Double_t _x = -_dArange + _bsize / 2.; _x < _dArange + _bsize / 2.; _x = _x + _bsize)
				{
					for (Double_t _y = -_dArange + _bsize / 2.; _y < _dArange + _bsize / 2.; _y = _y + _bsize)
					{
						// use the radius and fill the histogram
						Double_t _dA = hypot(_x - c.GetA()->GetX(), _y - c.GetA()->GetY());
						Double_t _dA2 = _dA * _dA;
						if (_dA2 > _rN2)
							continue;
						Double_t _dB = hypot(_x - c.GetB()->GetX(), _y - c.GetB()->GetY());
						Double_t _dB2 = _dB * _dB;
						if (_dB2 > _rN2)
							continue;
						Int_t ibx = hA.GetXaxis()->FindBin(_x);
						Int_t iby = hA.GetYaxis()->FindBin(_y);

						// hA.Fill(_Xn, _Yn);
						hA.SetBinContent(ibx, iby, 1);
						ncells++;
					}
				}
				// analytic - sum of overlaps
				_totalTArea = _totalTArea + c.GetActiveTArea();
				_maxArea = _maxArea + _maxOverlapArea;
				// comment if not debugging
				// _intArea = hA.Integral() * _bsize2;
				// Linfo << "total : " << c.GetActiveTArea() << " int : " << _intArea << " : " << _intArea;
				// Linfo << "total : " << c.GetActiveTArea() << " int : " << ncells << " : " << ncells * _bsize2;
				// Linfo << " - ratio : " << (c.GetActiveTArea()) / (_intArea);
				// hA.Reset(); // - do not reset to get the area per event
			}
			// numerical sum of overlap
			_intArea = hA.Integral() * _bsize2;
			// Linfo << " summed : " << _totalTArea << " - no double counting : " << _intArea;
			outT << "ncoll" << fpGlauberMC->GetNcoll();
			outT << "npart" << fpGlauberMC->GetNpart();
			outT << "tarea" << _totalTArea;
			outT << "iarea" << _intArea;
			outT << "tmaxarea" << _maxArea;
			Linfo << _maxOverlapArea;
			outT << endl;
		}
		return kGood;
	}

	unsigned int GlauberTask::InitThis(const char *opt)
	{
		fArgs.merge(opt);
		Linfo << "GlauberTask::Init " << GetName() << " with opts: " << fArgs.asString();
		Linfo << "GlauberTask::Init " << GetName() << " fStatus: " << fStatus;

		fFixedb = fArgs.getD("--glauber-fixed-b", -1);

		string sysA             = fArgs.get("--glauber-A", "Pb");
		string sysB             = fArgs.get("--glauber-B", "Pb");
		Linfo << "GlauberTask::Init " << GetName() << " sysA: " << sysA << " sysB: " << sysB;

		Double_t eA = fArgs.getD("--eA", 0.0);
		Double_t eB = fArgs.getD("--eB", 0.0);
		if (eA == 0.0)
			eA = fArgs.getD("Beams:eA");
		if (eB == 0.0)
			eB = fArgs.getD("Beams:eB");
		Double_t eCM = fArgs.getD("Beams:eCM", 0.0);
		if ( eCM > 0)
		{
			if (eA == 0 && eB == 0)
			{
				eA = eCM / 2.;
				eB = eCM / 2.;
			}
		}
		else
		{
			if (eA > 0 || eB > 0)
			{
				eCM = PyUtil::sqrts(eA, eB);
				Lwarn << "adjusted eCM: eA = " << eA << " eB = " << eB << " << eCM = " << eCM;
			}
			else
			{
				eCM = 5020.; // LHC Run-2 energy for PbPb
				if (eA == 0 && eB == 0)
				{
					eA = eCM / 2.;
					eB = eCM / 2.;
				}
				Lwarn << "no sqrt(s) info given... running with default eCM = " << eCM << " GeV";
			}
		}

		Linfo << "eA = " << eA << " eB = " << eB << " << eCM = " << eCM;

		// const Int_t n           = 1;
		Double_t signn         = fArgs.getD("--glauber-signn", 0.0);
		if (signn == 0.0)
		{
			signn = 67.6;
			Linfo << "trying to figure out sigma nn (INEL) from sqrt(s)...";
			if (eCM > 0)
			{
				signn = PyUtil::ParamSigmas::Instance().Get(PyUtil::ParamSigmas::kINEL, eCM);
			}
			Lwarn << "adjusted signn = " << signn;
		}
		Double_t sigwidth      = fArgs.getD("--glauber-sigwidth", -1);
		Double_t mind          = fArgs.getD("--glauber-mind", 0.4);
		Double_t omega         = fArgs.getD("--glauber-omega", -1);
		Double_t noded         = fArgs.getD("--glauber-noded", -1);
		Bool_t   updateNNxsect = fArgs.isSet("--glauber-update-NNxsect");
		Double_t averageNucleonEloss = fArgs.getD("--glauber-average-N-eloss", 1.0);
		if (averageNucleonEloss < 0 || averageNucleonEloss > 1.0)
		{
			Lfatal << "averageNucleonEloss should be a number within [0,1] " << averageNucleonEloss << " was given.";
			return kError;
		}
		if (averageNucleonEloss < 1.0) updateNNxsect = kTRUE;
		else updateNNxsect = kFALSE;
		Linfo << "GlauberTask::Init " << GetName() << " updateNNxsect : " << updateNNxsect << " averageNucleonEloss : " << averageNucleonEloss;
		fpGlauberMC = new TGlauberMC(sysA.c_str(),sysB.c_str(),signn,sigwidth,updateNNxsect,averageNucleonEloss);
		fData->add(fpGlauberMC);

		Linfo << "GlauberTask::Init " << GetName() << " GlauberMC at: " << fpGlauberMC;

		fpGlauberMC->SetEnergyPerNucleon(eA, eB);

		fpGlauberMC->SetMinDistance(mind);
		fpGlauberMC->SetNodeDistance(noded);
		fpGlauberMC->SetCalcLength(fArgs.isSet("--glauber-calc-length"));
		fpGlauberMC->SetCalcArea(fArgs.isSet("--glauber-calc-area"));
		fpGlauberMC->SetDetail(fArgs.getI("--glauber-detail", 99));
		TString om;
		if ((omega>=0) && (omega<=1))
		{
			TF1 *f1 = getNNProf(signn, omega);
			fpGlauberMC->SetNNProf(f1);
			om=Form("-om%.1f",omega);
		}

		string slabel = StrUtil::sT(GetName()) + "_GlauberMC";

		if (fArgs.isSet("--write-collisions"))
		{
			string outfname = OutputFileName();
			StrUtil::replace_substring(outfname, ".root", "gtask.root");
			// outfname = // change name
			fOutputFile = new TFile(outfname.c_str(), "recreate");
			fCollisionsTree = new TTree("t", "t");
			fpGlauberMC->SetCollisionsTree(fCollisionsTree);

			fOutputTree = new TTree("tg", "tg");
			fTStream    = new RStream::TStream("_", fOutputTree);
		}

		return kGood;
	}
}
