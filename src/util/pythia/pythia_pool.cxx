#include "pythia_pool.h"
#include "pyargs.h"

#include <algorithm>

#include <util/blog.h>
#include <util/wrapper/wrapper.h>
#include <TH2I.h>

using namespace std;

namespace PyUtil
{
	void PythiaPool::DumpInfo()
	{
		Linfo << "dumping pool info...";
		Linfo << "... common settings: " << _common_settings;
		if (_eAeBmap)
		{
			Linfo << "... listing entries in the pool map: ";
			for (Int_t ibx = 1; ibx < _eAeBmap->GetXaxis()->GetNbins(); ibx++)
				for (Int_t iby = 1; iby < _eAeBmap->GetYaxis()->GetNbins(); iby++)
				{
					if (_eAeBmap->GetBinContent(ibx, iby) >0 || _eAeBmapW->GetBinContent(ibx, iby) > 0)
					{
						Linfo << "    ... : beam energies: " << _eAeBmap->GetXaxis()->GetBinCenter(ibx) << " >< " << _eAeBmap->GetYaxis()->GetBinCenter(iby);
						Linfo << "    ...   n inst: " << _eAeBmap->GetBinContent(ibx, iby);
						if (_eAeBmapW)
							Linfo << "    ...   counts: " << _eAeBmapW->GetBinContent(ibx, iby);
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
		// setup generator
		Pythia8::Pythia *ppythia = new Pythia8::Pythia();

		Args args(s);
		if (eA == eB)
		{
			args.set("Beams:frameType=1");
		}
		else
		{
			args.set("Beams:frameType=2");
		}
		//args.set("Beams:eA", eA);
		//args.set("Beams:eB", eB);

		auto pairs = args.pairs();
		for (unsigned int i = 0; i < pairs.size(); i++)
		{
			if (pairs[i].second.size() < 1) continue;
			Linfo << "[init pythia] paired arg: #" << i << " " << pairs[i].first << " " << pairs[i].second;
			string spypar = pairs[i].first + " = " + pairs[i].second;
			ppythia->readString(spypar.c_str());
		}
		bool _is_initialized = ppythia->init();
		if (_is_initialized)
		{
			Linfo << "new pythia is at " << ppythia << " with settings: " << args.asString();
		}
		else
		{
			Lerror << "pythia initialization failed with settings: " << args.asString();
			delete ppythia;
			ppythia = 0;
		}
		return ppythia;
	}

	Pythia8::Pythia * PythiaPool::GetPythia(double eA, double eB, const char *new_settings)
	{
		Pythia8::Pythia *ret_pythia = 0x0;
		if (!_eAeBmap)
		{
			double eMax = max(eA, eB);
			int nbinsA = 100;
			int nbinsB = 100;
			if (eA > eB)
			{
				nbinsA = eA / (eB / 100.);
			}
			_eAeBmap = new TH2I("_eAeBmap", "_eAeBmap", nbinsA, eA/2. + 1.0, eA  + 1.0, nbinsB, eB/2. + 1.0, eB  + 1.0);
			_eAeBmap->SetDirectory(0);
			_eAeBmapW = new TH2I("_eAeBmapW", "_eAeBmapW", nbinsA, eA/2. + 1.0, eA + 1.0, nbinsB, eB/2. + 1.0, eB + 1.0);
			_eAeBmapW->SetDirectory(0);

			Linfo << "setting up energy map with eA: " << eA << " eB: " << eB;
			Linfo << "                           nA: " << nbinsA << " nB: " << nbinsB;
			Linfo << " ... known range eA: " << _eAeBmap->GetXaxis()->GetXmin() << " - " << _eAeBmap->GetXaxis()->GetXmax();
			Linfo << " ... known range eB: " << _eAeBmap->GetYaxis()->GetXmin() << " - " << _eAeBmap->GetYaxis()->GetXmax();
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
		Linfo << "binx : " << binx << " biny : " << biny;
		//if (ibin < 1 || ibin > _eAeBmap->GetNbinsX() * _eAeBmap->GetNbinsY())
		//{
		//	Lfatal << "pythia requested outside the initial allocation - " << eA << " : " << eB << " - this will not work...";
		//	Lfatal << " ... got ibin = " << ibin << " allowed X*Y: " << _eAeBmap->GetNbinsX() * _eAeBmap->GetNbinsY();
		//	Lfatal << " ... known range eA: " << _eAeBmap->GetXaxis()->GetXmin() << " - " << _eAeBmap->GetXaxis()->GetXmax();
		//	Lfatal << " ... known range eB: " << _eAeBmap->GetYaxis()->GetXmin() << " - " << _eAeBmap->GetYaxis()->GetXmax();
		//	ret_pythia = 0x0;
		//	return ret_pythia;
		//}

		if (pyindex < 0)
		{
			// init new pythia here
			ret_pythia = CreatePythia(eA, eB, settings.c_str());
			if (ret_pythia != 0)
			{
				_pythia_pool.push_back(ret_pythia);
				pyindex = _pythia_pool.size();
				_eAeBmap->SetBinContent(binx, biny, pyindex);
				_eAeBmapW->Fill(eA, eB);
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
				Linfo << "pythia instance found at index: " << pyindex;
				Linfo << "... eA: " << ret_pythia->parm("Beams:eA");
				Linfo << "... eB: " << ret_pythia->parm("Beams:eB");

				double tmp_eA = ret_pythia->parm("Beams:eA");
				double tmp_eB = ret_pythia->parm("Beams:eB");
				Int_t binx    = _eAeBmap->GetXaxis()->FindBin(tmp_eA);
				Int_t biny    = _eAeBmap->GetYaxis()->FindBin(tmp_eB);
				Int_t ibin    = _eAeBmap->GetBin(binx,biny);
				if (ibin < 1 || ibin > _eAeBmap->GetNbinsX() * _eAeBmap->GetNbinsY())
				{
					Lfatal << "pythia requested outside the initial allocation - " << tmp_eA << " : " << tmp_eB << " - this will not work...";
					Linfo << " ... known range eA: " << _eAeBmap->GetXaxis()->GetXmin() << " - " << _eAeBmap->GetXaxis()->GetXmax();
					Linfo << " ... known range eB: " << _eAeBmap->GetYaxis()->GetXmin() << " - " << _eAeBmap->GetYaxis()->GetXmax();
					ret_pythia = 0x0;
					return ret_pythia;
				}
				_eAeBmapW->Fill(eA, eB);
			}
		}

		return ret_pythia;
	}


};
