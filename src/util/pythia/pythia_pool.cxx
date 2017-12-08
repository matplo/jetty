#include <jetty/util/pythia/pythia_pool.h>
#include <jetty/util/pythia/pyargs.h>
#include <jetty/util/blog.h>
#include <jetty/util/wrapper/wrapper.h>

#include <algorithm>

#include <TH2I.h>
#include <TFile.h>

using namespace std;

namespace PyUtil
{
	PythiaPool::~PythiaPool()
	{
		DumpInfo();
	}

	void PythiaPool::WriteECMsToFile(const char *fname)
	{
		TFile *fout = new TFile(fname, "recreate");
		if (fout)
		{
			fout->cd();
			if (_eAeBmap && _eAeBmapW)
			{
				_eAeBmap->Write();
				_eAeBmapW->Write();
				Linfo << "writing maps to " << fname;
			}
			else
			{
				Lerror << "unable to write the maps - not initialized?";
			}
			fout->Write();
			fout->Close();
			delete fout;
		}
	}

	void PythiaPool::DumpInfo()
	{
		Linfo << "dumping pool info...";
		Linfo << "... common settings: " << _common_settings;
		if (_eAeBmap)
		{
			Linfo << "... listing entries in the pool map: ";
			for (Int_t ibx = 1; ibx <= _eAeBmap->GetXaxis()->GetNbins(); ibx++)
				for (Int_t iby = 1; iby <= _eAeBmap->GetYaxis()->GetNbins(); iby++)
				{
					if (_eAeBmap->GetBinContent(ibx, iby) > 0 || _eAeBmapW->GetBinContent(ibx, iby) > 0)
					{
						double bwx = _eAeBmap->GetXaxis()->GetBinWidth(ibx);
						double bwy = _eAeBmap->GetXaxis()->GetBinWidth(iby);
						Linfo << "    ... : beam energies - central value: "
							<< _eAeBmap->GetXaxis()->GetBinCenter(ibx)
							<< " >< "
							<< _eAeBmap->GetYaxis()->GetBinCenter(iby);
						Linfo << "    ... : beam energies - range: "
							<< _eAeBmap->GetXaxis()->GetBinLowEdge(ibx) << "-" << _eAeBmap->GetXaxis()->GetBinLowEdge(ibx) + bwx
							<< " >< "
							<< _eAeBmap->GetYaxis()->GetBinLowEdge(iby) << "-" << _eAeBmap->GetXaxis()->GetBinLowEdge(iby) + bwy;
						Linfo << "    ...   instance #: " << _eAeBmap->GetBinContent(ibx, iby);
						if (_eAeBmapW)
							Linfo << "    ...   used N: " << _eAeBmapW->GetBinContent(ibx, iby);
					}
				}
			Linfo << "... done.";
		}
	}

	void PythiaPool::SetCommonSettings(const char *s)
	{
		_common_settings = s;
	}

	Pythia8::Pythia * PythiaPool::CreatePythia(double eA, double eB, const char *s)
	{
		Args args(s);

		// setup generator
		Pythia8::Pythia *ppythia = 0x0;
		if (args.isSet("--silent-pythia-init"))
		{
			LogUtil::cout_sink _cout_sink;
			LogUtil::cerr_sink _cerr_sink;
			ppythia = new Pythia8::Pythia();
		}
		else
		{
			ppythia = new Pythia8::Pythia();
		}

		if (!ppythia)
		{
			Lfatal << "unable to create new pythia!";
			return 0x0;
		}

		if (eA == eB)
		{
			args.set("Beams:frameType=1");
		}
		else
		{
			args.set("Beams:frameType=2");
		}
		// http://home.thep.lu.se/~torbjorn/pythia81html/BeamParameters.html
		// NOTE: option 3 : the beams are not back-to-back, and therefore the three-momentum of each incoming particle
		// needs to be specified, see Beams:pxA through Beams:pzB below.
		args.set("Beams:eA", eA);
		args.set("Beams:eB", eB);

		auto pairs = args.pairs();
		for (unsigned int i = 0; i < pairs.size(); i++)
		{
			if (pairs[i].second.size() < 1) continue;
			Linfo << "[init pythia] paired arg: #" << i << " " << pairs[i].first << " " << pairs[i].second;
			string spypar = pairs[i].first + " = " + pairs[i].second;
			ppythia->readString(spypar.c_str());
		}
		bool _is_initialized = false;
		string pythia_init_message;
		if (args.isSet("--silent-pythia-init"))
		{
			LogUtil::cout_sink _cout_sink;
			LogUtil::cerr_sink _cerr_sink;
			_is_initialized = ppythia->init();
			pythia_init_message = _cerr_sink.get_buffer()->str();
			pythia_init_message += _cout_sink.get_buffer()->str();
		}
		else
		{
			_is_initialized = ppythia->init();
		}
		if (_is_initialized)
		{
			Ldebug << "new pythia is at " << ppythia << " with settings: " << args.asString();
		}
		else
		{
			Lerror << "pythia initialization failed with settings: " << args.asString();
			Lerror << pythia_init_message;
			delete ppythia;
			ppythia = 0;
		}
		return ppythia;
	}

	void PythiaPool::SetupECMs(double eA, double eB, int ndiv) // ndiv is approximate! - will try to make the bin sizes similar
	{
		if (_eAeBmap)
		{
			Lerror << "ECMs map already setup";
			DumpInfo();
			return;
		}
		int nbinsA = ndiv;
		int nbinsB = ndiv;
		if (eA > eB)
		{
			nbinsA = eA / (eB / ndiv);
		}
		else
		{
			nbinsB = eB / (eA / ndiv);
		}
		double lowCutoffFraction = 0.; // 0 is no cut off
		double bwA = (eA - eA * lowCutoffFraction) / nbinsA / 2.;
		double bwB = (eB - eB * lowCutoffFraction) / nbinsB / 2.;
		_eAeBmap = new TH2I("_eAeBmap", "_eAeBmap",
		                    nbinsA, eA * lowCutoffFraction + bwA, eA  + bwA,
		                    nbinsB, eB * lowCutoffFraction + bwB, eB  + bwB);
		_eAeBmap->SetDirectory(0);
		_eAeBmapW = new TH2I("_eAeBmapW", "_eAeBmapW",
		                     nbinsA, eA * lowCutoffFraction + bwA, eA + bwA,
		                     nbinsB, eB * lowCutoffFraction + bwB, eB + bwB);
		_eAeBmapW->SetDirectory(0);

		Linfo << "setting up energy map with eA: " << eA << " eB: " << eB;
		Linfo << "                           nA: " << nbinsA << " nB: " << nbinsB;
		Linfo << " ... known range eA: " << _eAeBmap->GetXaxis()->GetXmin() << " - " << _eAeBmap->GetXaxis()->GetXmax();
		Linfo << " ... known range eB: " << _eAeBmap->GetYaxis()->GetXmin() << " - " << _eAeBmap->GetYaxis()->GetXmax();
	}

	Pythia8::Pythia * PythiaPool::GetPythia(double eA, double eB, const char *new_settings)
	{
		Pythia8::Pythia *ret_pythia = 0x0;
		if (!_eAeBmap)
		{
			SetupECMs(eA, eB);
		}

		// settings ...
		string settings = _common_settings;
		if (new_settings != 0)
		{
			settings = new_settings;
			if (_common_settings.size() == 0)
				_common_settings = settings;
		}

		// map exists...
		Int_t binx    = _eAeBmap->GetXaxis()->FindBin(eA);
		Int_t biny    = _eAeBmap->GetYaxis()->FindBin(eB);
		Int_t pyindex = _eAeBmap->GetBinContent(binx,biny) - 1;
		Int_t ibin    = _eAeBmap->GetBin(binx,biny);
		Ldebug << "eA = " << eA << " is bin : " << binx;
		Ldebug << "eB = " << eB << " is bin : " << biny;
		if (binx <= 0 || binx > _eAeBmap->GetXaxis()->GetNbins() ||
		    biny <= 0 || biny > _eAeBmap->GetYaxis()->GetNbins())
		{
			Lfatal << "pythia requested outside the initial allocation - eA=" << eA << " && eB=" << eB << " - this will not work...";
			Lfatal << " ... got A-bin = " << binx << " allowed: 1-" << _eAeBmap->GetXaxis()->GetNbins();
			Lfatal << " ... known range eA: " << _eAeBmap->GetXaxis()->GetXmin() << " - " << _eAeBmap->GetXaxis()->GetXmax();
			Lfatal << " ... got B-bin = " << biny << " allowed: 1-" << _eAeBmap->GetYaxis()->GetNbins();
			Lfatal << " ... known range eB: " << _eAeBmap->GetYaxis()->GetXmin() << " - " << _eAeBmap->GetYaxis()->GetXmax();
			ret_pythia = 0x0;
			return ret_pythia;
		}

		if (pyindex < 0 || _pythia_pool.size() == 0)
		{
			// init new pythia here
			Ldebug << "create new pythia instance ...";
			ret_pythia = CreatePythia(eA, eB, settings.c_str());
			if (ret_pythia != 0)
			{
				_pythia_pool.push_back(ret_pythia);
				pyindex = _pythia_pool.size();
				_eAeBmap->SetBinContent(binx, biny, pyindex);
				return GetPythia(eA, eB, settings.c_str());
			}
		}
		else
		{
			ret_pythia = _pythia_pool[pyindex];
			if (!ret_pythia)
			{
				Lfatal << "pythia instance not found at index: " << pyindex;
				Args tmp_args(settings.c_str());
				Lfatal << " - with arguments: eA: " << eA << " eB:" << eB << " [assumed] settings: " << tmp_args.asString();
			}
			else
			{
				Ldebug << "pythia instance found at index: " << pyindex;
				Ldebug << "... eA: " << ret_pythia->parm("Beams:eA");
				Ldebug << "... eB: " << ret_pythia->parm("Beams:eB");

				double tmp_eA = ret_pythia->parm("Beams:eA");
				double tmp_eB = ret_pythia->parm("Beams:eB");
				Int_t binx    = _eAeBmap->GetXaxis()->FindBin(tmp_eA);
				Int_t biny    = _eAeBmap->GetYaxis()->FindBin(tmp_eB);
				Int_t ibin    = _eAeBmap->GetBin(binx,biny);
				if (binx <= 0 || binx > _eAeBmap->GetXaxis()->GetNbins() ||
				    biny <= 0 || biny > _eAeBmap->GetYaxis()->GetNbins())
				{
					Lfatal << "pythia retrieved outside the initial allocation - eA=" << tmp_eA << " && eB=" << tmp_eB << " - this will not work...";
					Lfatal << " ... got A-bin = " << binx << " allowed: 1-" << _eAeBmap->GetXaxis()->GetNbins();
					Lfatal << " ... known range eA: " << _eAeBmap->GetXaxis()->GetXmin() << " - " << _eAeBmap->GetXaxis()->GetXmax();
					Lfatal << " ... got B-bin = " << biny << " allowed: 1-" << _eAeBmap->GetYaxis()->GetNbins();
					Lfatal << " ... known range eB: " << _eAeBmap->GetYaxis()->GetXmin() << " - " << _eAeBmap->GetYaxis()->GetXmax();
					ret_pythia = 0x0;
					return ret_pythia;
				}
				_eAeBmapW->Fill(eA, eB);
			}
		}

		return ret_pythia;
	}


};
