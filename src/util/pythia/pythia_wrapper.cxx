#include "pythia_wrapper.h"
#include "util/pythia/pyargs.h"
#include "util/pythia/crosssections.h"
#include "util/blog.h"

#include <TNtuple.h>
#include <TH1F.h>
#include <TFile.h>
#include <TSystem.h>

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>

using namespace std;

namespace PyUtil
{
	PythiaWrapper::PythiaWrapper(const string& params)
		: fWrapper(new Wrapper())
		, _is_initialized(false)
	{
		fWrapper->set_debug(1);
		PyUtil::Args *args = new PyUtil::Args(params);
		fWrapper->add(args);
		if (_checkOutput()) _initPythia();
	}

	PythiaWrapper::PythiaWrapper(const char* params)
		: fWrapper(new Wrapper())
		, _is_initialized(false)
	{
		PyUtil::Args *args = new PyUtil::Args(params);
		fWrapper->add(args);
		if (_checkOutput()) _initPythia();
	}

	PythiaWrapper::PythiaWrapper(int argc, char *argv[])
		: fWrapper(new Wrapper())
		, _is_initialized(false)
	{
		PyUtil::Args *args = new PyUtil::Args(argc, argv);
		fWrapper->add(args);
		if (_checkOutput()) _initPythia();
	}

	void PythiaWrapper::readConfig(const char *fname)
	{
		Linfo << "[PythiaWrapper::readConfig] file " << fname;
		auto args = fWrapper->get<PyUtil::Args>();
		args->readConfig(fname);
	}

	void PythiaWrapper::_initPythia()
	{
		auto args = fWrapper->get<PyUtil::Args>();

		// setup generator
		Pythia8::Pythia *ppythia = new Pythia8::Pythia();

		double eA       = args->getD("Beams:eA");
		double eB       = args->getD("Beams:eB");
		if (eA == eB)
			args->set("Beams:frameType=1");

		auto pairs = args->pairs();
		for (unsigned int i = 0; i < pairs.size(); i++)
		{
			if (pairs[i].second.size() < 1) continue;
			Linfo << "[init pythia] paired arg: #" << i << " " << pairs[i].first << " " << pairs[i].second;
			string spypar = pairs[i].first + " = " + pairs[i].second;
			ppythia->readString(spypar.c_str());
		}
		_is_initialized = ppythia->init();
		if (_is_initialized)
		{
			Linfo << "pythia is at " << ppythia;
			fWrapper->add(ppythia); // no need to remember to delete - just delete the pywrapp
			_initOutput();
			args->set("PyInit=1");
		}
		else
		{
			args->set("PyInit=0");
			Lerror << "pythia initialization failed.";
			delete ppythia;
			ppythia = 0;
		}
		fWrapper->list();
	}

	bool PythiaWrapper::_checkOutput()
	{
		auto args = fWrapper->get<PyUtil::Args>();
		string fname = _outputFileName();
		if (args->isSet("--force"))
		{
			return true;
		}
		// strange but yeah... kFALSE if the file exists
		if (gSystem->AccessPathName(fname.c_str(), kFileExists) == kFALSE)
		{
			Lwarn << "output file " << fname.c_str() << " already exists";
			if (args->isSet("--overwrite"))
				Linfo << "overwrite is set - continuing" << endl;
			else
			{
				Linfo << "setting pythia Beams:eCM=0 ... - this should stop execution.";
				args->set("Beams:eCM=0");
				return false;
			}
		}
		return true;
	}

	string PythiaWrapper::_outputFileName()
	{
		auto args = fWrapper->get<PyUtil::Args>();
		string outfname = args->get("--out");
		if (outfname.size() == 0) outfname = "default.root";
		int nEvent      = args->getI("Main:numberOfEvents");
		double pTHatMin = args->getD("PhaseSpace:pTHatMin");
		double pTHatMax = args->getD("PhaseSpace:pTHatMax");
		double mHatMin  = args->getD("PhaseSpace:mHatMin");
		double mHatMax  = args->getD("PhaseSpace:mHatMax");
		double eCM      = args->getD("Beams:eCM");
		double eA       = args->getD("Beams:eA");
		double eB       = args->getD("Beams:eB");
		if (args->getI("Beams:frameType") == 1 || eA==eB)
		{
			eA = eCM / 2.;
			eB = eCM / 2.;
		}
		string smodif = boost::str(boost::format("_eA_%1.0f_eB_%1.0f_eCM_XXXGeV_pTHatMin_%2.1f_pTHatMax_%2.1f_nEv_%d.root")
		                           % eA % eB % pTHatMin % pTHatMax % nEvent);
		if (args->isSet("--z0"))
		{
			smodif = boost::str(boost::format("_eA_%1.0f_eB_%1.0f_eCM_XXXGeV_mHatMin_%2.1f_mHatMax_%2.1f_nEv_%d.root")
			                    % eA % eB % mHatMin % mHatMax % nEvent);
		}
		if (args->isSet("--minbias"))
		{
			smodif = boost::str(boost::format("_eA_%1.0f_eB_%1.0f_eCM_XXXGeV_minbias_nEv_%d.root") % eA % eB % nEvent);
		}
		// smodif = smodif.ReplaceAll("_eCM_XXX", TString::Format("_eCM_%1.0f", eCM));
		boost::replace_all(smodif, "_eCM_XXX", boost::str(boost::format("_eCM_%1.0f") % eCM));
		boost::replace_all(outfname, ".root", smodif);
		if (args->isSet("--nsd"))
			boost::replace_all(outfname, ".root", "_nsd.root");
		return outfname;
	}

	void PythiaWrapper::_initOutput()
	{
		string outfname = _outputFileName();
		Linfo << "Output goes to: " << outfname;
		TFile *fout = new TFile(outfname.c_str(), "RECREATE");
		fout->cd();
		fNtuple = new TNtuple("pystats", "pystats", "pxsec:pcode");
		fWrapper->add(fout);
	}

	bool PythiaWrapper::next()
	{
		auto py = fWrapper->get<Pythia8::Pythia>();
		bool retval = py->next();
		if (!retval) return retval;

		auto tn     = fWrapper->get<TNtuple>();
		int xcode   = py->info.code();
		double xsec = py->info.sigmaGen(xcode);
		fNtuple->Fill(xsec, xcode);

		return retval;
	}

	Pythia8::Pythia* PythiaWrapper::pythia()
	{
		return fWrapper->get<Pythia8::Pythia>();
	}

	TFile *PythiaWrapper::outputFile()
	{
		return fWrapper->get<TFile>();
	}

	PyUtil::Args* PythiaWrapper::args()
	{
		return fWrapper->get<PyUtil::Args>();
	}

	void PythiaWrapper::_storeAverages()
	{
		auto fout = fWrapper->get<TFile>();
		fout->cd();
		auto tdir = new TDirectory("histograms", "histograms");
		auto h = new TH1F("pcodexsec", "pcodexsec", 500, -0.5, 500 - 0.5);

		// deal with the process codes
		auto py    = fWrapper->get<Pythia8::Pythia>();
		auto codes = py->info.codesHard();
		codes.push_back(0);
		for ( auto &xcode : codes)
		{
			auto xsec     = py->info.sigmaGen(xcode);
			auto xsec_err = py->info.sigmaErr(xcode);
			auto ib       = h->FindBin(xcode);
			h->SetBinContent(ib, xsec);
			h->SetBinError(ib, xsec_err);
			Linfo << "code " << xcode << " xsec=" << xsec << " +- " << xsec_err;
		}

		//same as above but to a txt file
		string xsec_outfname = fout->GetName();
		xsec_outfname += ".txt";
		PyUtil::CrossSections(*py, xsec_outfname.c_str());
	}

	PythiaWrapper::~PythiaWrapper()
	{
		auto fout = fWrapper->get<TFile>();
		if (fout)
		{
			_storeAverages();
			fout->Write();
			fout->Close();
		}
		delete fWrapper;
	}
};
