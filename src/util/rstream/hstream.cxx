#include "hstream.h"

#include <limits>

#include <TList.h>
#include <TH1F.h>
#include <TFile.h>

#include "util/pythia/pyutil.h"
#include "util/blog.h"
#include "util/strutil.h"

#include <boost/algorithm/string.hpp>

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
		, fSkipUndefined(false)
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
		, fSkipUndefined(false)
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
		, fSkipUndefined(false)
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

	void HStream::CreateHistograms()
	{
		if (!fInit) return;
		SysUtil::Args args(fConfigString);
		for (auto &p : args.pairs())
		{
			auto h = CreateH(p.first);
			if (h)
				Linfo << "created " << h->GetName() << " : " << p.second << " at " << h;
		}
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
		else
		{
			Linfo << "creating histogram for " << sname;
		}
		vector<string> settings;
		string setting = args.get(sname);
		boost::algorithm::split(settings, setting, boost::is_any_of(","));
		if (settings.size() < 4)
		{
			Linfo << "ignored definition when building histograms : " << args.get(sname);
			return h;
		}
		auto _stitle = settings[0];
		auto _nbins  = StrUtil::str_to_int   (settings[1].c_str(), 1);
		auto _xmin   = StrUtil::str_to_double(settings[2].c_str(), 0);
		auto _xmax   = StrUtil::str_to_double(settings[3].c_str(), 1);

		string _sname = fName + "_" + sname;
		fOutputFile->cd();
		h = new TH1F(_sname.c_str(), _stitle.c_str(), _nbins, _xmin, _xmax);
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
		s = name;
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
