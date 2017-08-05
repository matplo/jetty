#include "hstream.h"

#include <limits>

#include <TList.h>
#include <TH1.h>
#include <TH2F.h>
#include <TH1F.h>
#include <TFile.h>
#include <TClass.h>
#include <TSystem.h>

#include "util/pythia/pyutil.h"
#include "util/blog.h"
#include "util/strutil.h"
#include "util/sysutil.h"

#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;

#include <boost/algorithm/string.hpp>
namespace balgor = boost::algorithm;

using namespace std;

namespace RStream
{
	HStream::HStream()
		: fName("h")
		, fList()
		, fCurrentName("")
		, fConfigString("")
		, fOutputFile(0)
		, fFileOwner(false)
		, fInit(false)
		, fSkipUndefined(true)
	{

	}

	HStream::HStream(const char *sconfig, bool file_config, TFile *fout)
		: fName("h")
		, fList()
		, fCurrentName("")
		, fConfigString(sconfig)
		, fOutputFile(0)
		, fFileOwner(false)
		, fInit(false)
		, fSkipUndefined(true)
	{
		fInit = Init(fName.c_str(), sconfig, file_config, fout);
	}

	HStream::HStream(const char *name, const char *sconfig, bool file_config, TFile *fout)
		: fName(name)
		, fList()
		, fCurrentName("")
		, fConfigString(sconfig)
		, fOutputFile(0)
		, fFileOwner(false)
		, fInit(false)
		, fSkipUndefined(true)
	{
		fInit = Init(name, sconfig, file_config, fout);
	}

	bool HStream::Init(const char *sconfig, bool file_config, TFile *fout)
	{
		return Init(fName.c_str(), sconfig, file_config, fout);
	}

	bool HStream::Init(const char *name, const char *sconfig, bool file_config, TFile *fout)
	{
		fInit = false;
		fName = name;
		fConfigString = sconfig;
		if (file_config)
		{
			SysUtil::Args args;
			args.readConfig(sconfig);
			fConfigString = args.asString();
		}
		SysUtil::Args args(fConfigString);
		if (args.isSet("name"))
		{
			if (fName == "h")
				fName = args.get("name");
		}
		if (fout)
			fOutputFile = fout;
		else
		{
			Linfo << "creating new histogram file:" << args.get("output_file");
			fOutputFile = new TFile(args.get("output_file").c_str(), "recreate");
			fFileOwner = true;
		}
		if (fOutputFile)
		{
			fList = new TList();
			fList->SetOwner(kFALSE);
			fInit = true;
			CreateHistograms();
		}
		else
		{
			Lerror << "creating new histogram file failed:" << args.get("output_file");
		}
		return fInit;
	}

	HStream::~HStream()
	{
		if (fFileOwner)
		{
			fOutputFile->cd();
			fList->Write();
		}
		if (fList)
		{
			delete fList;
		}
		if (fOutputFile && fFileOwner)
		{
			fOutputFile->Write();
			fOutputFile->Close();
			delete fOutputFile;
		}
	}

	void HStream::Scale(double scale)
	{
		for (unsigned int i = 0; i < fList->GetEntries(); i++)
		{
			TH1 *h = (TH1*)fList->At(i);
			h->Sumw2();
			h->Scale(scale);
		}
	}

	void HStream::CreateHistograms()
	{
		if (!fInit) return;
		SysUtil::Args args(fConfigString);
		for (auto &p : args.pairs())
		{
			if (p.first.size() < 1) continue;
			if (p.first[0] != '#')
			{
				Ldebug << "first:" << p.first;
				auto h = CreateH(p.first);
				if (h)
					Linfo << "created " << h->GetName() << " : " << p.second << " at " << h;
				auto h2 = CreateH2(p.first);
				if (h2)
					Linfo << "created " << h2->GetName() << " : " << p.second << " at " << h2;
			}
		}
	}

	TH1 *HStream::CloneH1FromFile(const char* sname, const char* setting)
	{
		TH1 *h = 0;
		string _setting(setting);
		vector<string> _settings;
		balgor::split(_settings, _setting, boost::is_any_of(":"));
		if (_settings.size() < 3)
			return h;
		auto _stitle   = _settings[0];
		string _sfname = (gSystem->ExpandPathName(_settings[1].c_str()));
		auto _shname   = _settings[2];
		if (!fs::exists(fs::path(_sfname)))
		{
			Ldebug << "[" << _sfname << "] path does not exist ";
			return h;
		}
		_sfname = fs::canonical(_sfname).string();
		string _sname = fName + "_" + sname;
		vector<string> _hpaths;
		balgor::split(_hpaths, _shname, boost::is_any_of("/"));
		Ltrace << "getting ... h: " << _shname << " from: " << _sfname;
		TFile *_fin = new TFile(_sfname.c_str());
		if (!_fin)
		{
			Ldebug << "unable to create TFile from " << _sfname;
			return h;
		}
		TH1 *_hread = 0;
		TDirectory *_d = (TDirectoryFile*)_fin;
		TObject *_o = 0;
		for ( auto &_s : _hpaths)
		{
			_o =_d->Get(_s.c_str());
			if (!_o)
			{
				Ltrace << "object: " << _s.c_str() << " not in TFile " << _fin->GetName();
				break;
			}
			string _oclass = _o->IsA()->GetName();
			Ltrace << _o << " " << _oclass;
			if (_oclass == "TDirectoryFile")
			{
				_d = (TDirectoryFile*)_d->Get(_s.c_str());
				continue;
			}
			if (balgor::contains(_oclass, "TH1"))
			{
				_hread = (TH1*)_d->Get(_s.c_str());
			}
		}
		if (_hread)
		{
			fOutputFile->cd();
			h = (TH1*)_hread->Clone(_sname.c_str());
			h->SetDirectory(fOutputFile);
			fList->Add(h);
		}
		if (_fin)
		{
			_fin->Close();
			delete _fin;
		}
		return h;
	}

	TH1 *HStream::CreateH(const char *sname)
	{
		TH1 *h = 0;
		if (!fInit) return h;
		SysUtil::Args args(fConfigString);
		if (args.isSet(sname) == false)
		{
			Lwarn << sname << " histogram is not configured";
			return h;
		}
		vector<string> settings;
		string setting = args.get(sname);
		balgor::split(settings, setting, boost::is_any_of("#"));
		if (settings.size()<1)
		{
			Ldebug << "commented ? " << setting;
			return h;
		}
		setting = settings[0];
		if (balgor::contains(setting, ":"))
		{
			h = CloneH1FromFile(sname, setting.c_str());
			if (!h)
				Ldebug << "failed: h from " << setting << " at " << h;
			return h;
		}
		balgor::split(settings, setting, boost::is_any_of(","));
		if (settings.size() != 4)
		{
			Ldebug << "1D: ignored definition when building histograms : " << args.get(sname);
			return h;
		}
		string _sname = fName + "_" + sname;
		fOutputFile->cd();
		auto _stitle = settings[0];
		auto _nbins  = StrUtil::str_to_int   (settings[1].c_str(), 1);
		auto _xmin   = StrUtil::str_to_double(settings[2].c_str(), 0);
		auto _xmax   = StrUtil::str_to_double(settings[3].c_str(), 1);
		Linfo << "creating 1D histogram for " << sname << " ...";
		h = new TH1F(_sname.c_str(), _stitle.c_str(), _nbins, _xmin, _xmax);
		fList->Add(h);
		return h;
	}

	TH2 *HStream::CreateH2(const char *sname)
	{
		TH2 *h = 0;
		if (!fInit) return h;
		SysUtil::Args args(fConfigString);
		if (args.isSet(sname) == false)
		{
			Lwarn << sname << " histogram is not configured";
			return h;
		}
		vector<string> settings;
		string setting = args.get(sname);
		balgor::split(settings, setting, boost::is_any_of("#"));
		if (settings.size()<1)
		{
			Ldebug << "commented ? " << setting;
			return h;
		}
		setting = settings[0];
		balgor::split(settings, setting, boost::is_any_of(","));
		if (settings.size() != 7)
		{
			Ldebug << "2D: ignored definition when building histograms : " << args.get(sname);
			return h;
		}
		string _sname = fName + "_" + sname;
		fOutputFile->cd();
		auto _stitle = settings[0];
		auto _nbins  = StrUtil::str_to_int   (settings[1].c_str(), 1);
		auto _xmin   = StrUtil::str_to_double(settings[2].c_str(), 0);
		auto _xmax   = StrUtil::str_to_double(settings[3].c_str(), 1);
		auto _nbinsy = StrUtil::str_to_int   (settings[4].c_str(), 1);
		auto _ymin   = StrUtil::str_to_double(settings[5].c_str(), 0);
		auto _ymax   = StrUtil::str_to_double(settings[6].c_str(), 1);
		Linfo << "creating 2D histogram for " << sname << " ...";
		h = new TH2F(_sname.c_str(), _stitle.c_str(), _nbins, _xmin, _xmax, _nbinsy, _ymin, _ymax);
		fList->Add(h);
		return h;
	}
	void HStream::FillHranch(const char *name, const vector<fastjet::PseudoJet> &in)
	{
		Ltrace << "FillHranch... " << name;
		if (!fInit) return;
		std::vector<double> pt;
		std::vector<double> phi;
		std::vector<double> eta;
		std::vector<double> rap;
		std::vector<double> m;
		std::vector<int> nc;
		for (unsigned int i = 0; i < in.size(); i++)
		{
			pt.push_back(in[i].perp());
			phi.push_back(in[i].phi());
			eta.push_back(in[i].eta());
			rap.push_back(in[i].rap());
			m.push_back(in[i].m());
			if (in[i].has_constituents())
				nc.push_back(in[i].constituents().size());
			else
				nc.push_back(0);
		}

		string s = name;

		s = name;
		s += "pt";
		FillHranch(s.c_str(), pt);
		s = name;
		s += "phi";
		FillHranch(s.c_str(), phi);
		s = name;
		s += "eta";
		FillHranch(s.c_str(), eta);
		s = name;
		s += "rap";
		FillHranch(s.c_str(), rap);
		s = name;;
		s += "m";
		FillHranch(s.c_str(), m);
		s = name;
		s += "nc";
		FillHranch(s.c_str(), nc);
	}

	void HStream::FillHranch(const char *name, const fastjet::PseudoJet &in)
	{
		Ltrace << "FillHranch... " << name;
		if (!fInit) return;
		string s = name;
		s += "pt";
		FillHranch(s.c_str(), in.perp());
		s = name;
		s += "phi";
		FillHranch(s.c_str(), in.phi());
		s = name;
		s += "eta";
		FillHranch(s.c_str(), in.eta());
		s = name;
		s += "rap";
		FillHranch(s.c_str(), in.rap());
		s = name;
		s += "m";
		FillHranch(s.c_str(), in.m());
		s = name;
		s += "nc";
		if (in.has_constituents())
			FillHranch(s.c_str(), int(in.constituents().size()));
		else
			FillHranch(s.c_str(), int(0));
	}

	void HStream::FillHranch(const char *name, const Pythia8::Particle &in)
	{
		Ltrace << "FillHranch... " << name;
		if (!fInit) return;
		string s = name;
		s += "pt";
		FillHranch(s.c_str(), in.pT());
		s = name;
		s += "phi";
		FillHranch(s.c_str(), in.phi());
		s = name;
		s += "eta";
		FillHranch(s.c_str(), in.eta());
		s = name;
		s += "rap";
		FillHranch(s.c_str(), in.y());
		s = name;
		s += "m";
		FillHranch(s.c_str(), in.m());
	}

	void HStream::FillHranch(const char *name, const vector<double> &in)
	{
		Ltrace << "FillHranch... " << name;
		if (!fInit) return;
		string sname = fName + "_" + name;
		TH1 *b = (TH1*)fList->FindObject(sname.c_str());
		if (b == 0 && fSkipUndefined == false)
		{
			b = CreateH(name);
		}
		if (b == 0) return;
		for (unsigned int i = 0; i < in.size(); ++i)
		{
			b->Fill(in[i]);
		}
	}

	void HStream::FillHranch(const char *name, const vector<int> &in)
	{
		Ltrace << "FillHranch... " << name;
		if (!fInit) return;
		string sname = fName + "_" + name;
		TH1 *b = (TH1*)fList->FindObject(sname.c_str());
		if (b == 0 && fSkipUndefined == false)
		{
			b = CreateH(name);
		}
		if (b == 0) return;
		for (unsigned int i = 0; i < in.size(); ++i)
		{
			b->Fill(in[i]);
		}
	}

	void HStream::FillHranch(const char *name, const double &in)
	{
		Ltrace << "FillHranch... " << name;
		if (!fInit) return;
		string sname = fName + "_" + name;
		TH1 *b = (TH1*)fList->FindObject(sname.c_str());
		if (b == 0 && fSkipUndefined == false)
		{
			b = CreateH(name);
		}
		if (b == 0) return;
		b->Fill(in);
	}

	void HStream::FillHranch(const char *name, const double in[2])
	{
		Ltrace << "FillHranch... " << name;
		if (!fInit) return;
		string sname = fName + "_" + name;
		TH2 *b = (TH2*)fList->FindObject(sname.c_str());
		if (b == 0 && fSkipUndefined == false)
		{
			b = CreateH2(name);
		}
		if (b == 0) return;
		b->Fill(in[0], in[1]);
	}

	void HStream::FillHranch(const char *name, const int &in)
	{
		Ltrace << "FillHranch... " << name;
		if (!fInit) return;
		string sname = fName + "_" + name;
		TH1 *b = (TH1*)fList->FindObject(sname.c_str());
		if (b == 0 && fSkipUndefined == false)
		{
			b = CreateH(name);
		}
		if (b == 0) return;
		b->Fill(in);
	}

	void HStream::FillHranch(const char *name, const unsigned long &in)
	{
		Ltrace << "FillHranch... " << name;
		if (!fInit) return;
		string sname = fName + "_" + name;
		TH1 *b = (TH1*)fList->FindObject(sname.c_str());
		if (b == 0 && fSkipUndefined == false)
		{
			b = CreateH(name);
		}
		if (b == 0) return;
		b->Fill(in);
	}

	HStream& operator<<(HStream& out, const char *bname)
	{
		out.fCurrentName = bname;
		// Ltrace << "current name is " << out.fCurrentName;
		return out;
	}

	HStream& operator<<(HStream& out, const std::string &bname)
	{
		out.fCurrentName = bname;
		// Ltrace << "current name is " << out.fCurrentName;
		return out;
	}

	HStream& operator<<(HStream& out, const std::vector<fastjet::PseudoJet> &in)
	{
		if (out.fCurrentName.size() < 1) return out;
		out.FillHranch(out.fCurrentName.c_str(), in);
		out.fCurrentName = "";
		return out;
	}

	HStream& operator<<(HStream& out, const std::vector<double> &in)
	{
		if (out.fCurrentName.size() < 1) return out;
		out.FillHranch(out.fCurrentName.c_str(), in);
		out.fCurrentName = "";
		return out;
	}

	HStream& operator<<(HStream& out, const std::vector<int> &in)
	{
		if (out.fCurrentName.size() < 1) return out;
		out.FillHranch(out.fCurrentName.c_str(), in);
		out.fCurrentName = "";
		return out;
	}

	HStream& operator<<(HStream& out, const fastjet::PseudoJet &in)
	{
		if (out.fCurrentName.size() < 1) return out;
		out.FillHranch(out.fCurrentName.c_str(), in);
		out.fCurrentName = "";
		return out;
	}

	HStream& operator<<(HStream& out, const Pythia8::Particle &in)
	{
		if (out.fCurrentName.size() < 1) return out;
		out.FillHranch(out.fCurrentName.c_str(), in);
		out.fCurrentName = "";
		return out;
	}

	HStream& operator<<(HStream& out, const double &in)
	{
		if (out.fCurrentName.size() < 1) return out;
		out.FillHranch(out.fCurrentName.c_str(), in);
		out.fCurrentName = "";
		return out;
	}

	HStream& operator<<(HStream& out, const double in[2])
	{
		if (out.fCurrentName.size() < 1) return out;
		out.FillHranch(out.fCurrentName.c_str(), in);
		out.fCurrentName = "";
		return out;
	}

	HStream& operator<<(HStream& out, const int &in)
	{
		if (out.fCurrentName.size() < 1) return out;
		out.FillHranch(out.fCurrentName.c_str(), in);
		out.fCurrentName = "";
		return out;
	}

	HStream& operator<<(HStream& out, const unsigned long &in)
	{
		if (out.fCurrentName.size() < 1) return out;
		out.FillHranch(out.fCurrentName.c_str(), in);
		out.fCurrentName = "";
		return out;
	}

	HStream& operator <<(HStream& out, std::ostream& (*os)(std::ostream&))
	{
		return out;
	}
};
