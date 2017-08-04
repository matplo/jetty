#include "hstream.h"

#include <limits>

#include <TList.h>
#include <TH1F.h>
#include <TFile.h>

#include "util/pythia/pyutil.h"
#include "util/blog.h"

using namespace std;

namespace RStream
{
	HStream::HStream()
		: fName("h")
		, fList()
		, fCurrentName("")
		, fConfigString("")
		, fOutputFile(0)
		, fInit(false)
	{

	}
	HStream::HStream(const char *sconfig, bool file_config)
		: fName("h")
		, fList()
		, fCurrentName("")
		, fConfigString(sconfig)
		, fOutputFile(0)
	{
		fInit = Init(fName.c_str(), sconfig, file_config);
	}

	HStream::HStream(const char *name, const char *sconfig, bool file_config)
		: fName(name)
		, fList()
		, fCurrentName("")
		, fConfigString(sconfig)
		, fOutputFile(0)
	{
		fInit = Init(name, sconfig, file_config);
	}

	bool HStream::Init(const char *sconfig, bool file_config)
	{
		return Init(fName.c_str(), sconfig, file_config);
	}

	bool HStream::Init(const char *name, const char *sconfig, bool file_config)
	{
		fName = name;
		fConfigString = sconfig;
		if (file_config)
		{
			SysUtil::Args args;
			args.readConfig(sconfig);
			fConfigString = args.asString();
		}
		SysUtil::Args args(fConfigString);
		fOutputFile = new TFile(args.get("output_file").c_str(), "recreate");
		if (!fOutputFile)
			return false;
		return true;
	}

	HStream::~HStream()
	{
		if (fOutputFile)
		{
			fOutputFile->Write();
			fOutputFile->Close();
			delete fOutputFile;
		}
	}

	TH1 *HStream::CreateH(const char *sname)
	{
		TH1 *h = 0;
		SysUtil::Args args(fConfigString);
		if (args.isSet(sname) == false)
		{
			Lwarn << sname << " histogram is not configured";
			return (TH1*)0x0;
		}
		auto _sname = args.get(sname);
		auto _stitle = args.get(_sname + "_title");
		auto _nbins = args.getI(_sname + "_nbins");
		auto _xmin = args.getD(_sname + "_xmin");
		auto _xmax = args.getD(_sname + "_xmax");

		fOutputFile->cd();
		h = new TH1F(_sname.c_str(), _stitle.c_str(), _nbins, _xmin, _xmax);
		fList->Add(h);
		return h;
	}

	void HStream::FillHranch(const char *name, const vector<fastjet::PseudoJet> &in)
	{
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

	void HStream::FillHranch(const char *name, const vector<double> &in)
	{
		string sname = fName + "_" + name;
		TH1 *b = (TH1*)fList->FindObject(sname.c_str());
		if (b == 0)
		{
			b = CreateH(sname);
		}
		if (b == 0) return;
		for (unsigned int i = 0; i < in.size(); ++i)
		{
			b->Fill(in[i]);
		}
	}

	void HStream::FillHranch(const char *name, const vector<int> &in)
	{
		string sname = fName + "_" + name;
		TH1 *b = (TH1*)fList->FindObject(sname.c_str());
		if (b == 0)
		{
			b = CreateH(sname);
		}
		if (b == 0) return;
		for (unsigned int i = 0; i < in.size(); ++i)
		{
			b->Fill(in[i]);
		}
	}

	void HStream::FillHranch(const char *name, const double &in)
	{
		string sname = fName + "_" + name;
		TH1 *b = (TH1*)fList->FindObject(sname.c_str());
		if (b == 0)
		{
			b = CreateH(sname);
		}
		if (b == 0) return;
		b->Fill(in);
	}

	void HStream::FillHranch(const char *name, const int &in)
	{
		string sname = fName + "_" + name;
		TH1 *b = (TH1*)fList->FindObject(sname.c_str());
		if (b == 0)
		{
			b = CreateH(sname);
		}
		if (b == 0) return;
		b->Fill(in);
	}

	void HStream::FillHranch(const char *name, const unsigned long &in)
	{
		string sname = fName + "_" + name;
		TH1 *b = (TH1*)fList->FindObject(sname.c_str());
		if (b == 0)
		{
			b = CreateH(sname);
		}
		if (b == 0) return;
		b->Fill(in);
	}

	HStream& operator<<(HStream& out, const char *bname)
	{
	   out.fCurrentName = bname;
	   // std::cout << "current name is " << out.fCurrentName << std::endl;
	   return out;
	}

	HStream& operator<<(HStream& out, const std::string &bname)
	{
	   out.fCurrentName = bname;
	   // std::cout << "current name is " << out.fCurrentName << std::endl;
	   return out;
	}

	HStream& operator<<(HStream& out, const std::vector<fastjet::PseudoJet> &in)
	{
	   assert(out.fCurrentName.size() > 0);
	   out.FillHranch(out.fCurrentName.c_str(), in);
	   out.fCurrentName = "";
	   return out;
	}

	HStream& operator<<(HStream& out, const std::vector<double> &in)
	{
	   assert(out.fCurrentName.size() > 0);
	   out.FillHranch(out.fCurrentName.c_str(), in);
	   out.fCurrentName = "";
	   return out;
	}

	HStream& operator<<(HStream& out, const std::vector<int> &in)
	{
	   assert(out.fCurrentName.size() > 0);
	   out.FillHranch(out.fCurrentName.c_str(), in);
	   out.fCurrentName = "";
	   return out;
	}

	HStream& operator<<(HStream& out, const fastjet::PseudoJet &in)
	{
	   assert(out.fCurrentName.size() > 0);
	   out.FillHranch(out.fCurrentName.c_str(), in);
	   out.fCurrentName = "";
	   return out;
	}

	HStream& operator<<(HStream& out, const double &in)
	{
	   assert(out.fCurrentName.size() > 0);
	   out.FillHranch(out.fCurrentName.c_str(), in);
	   out.fCurrentName = "";
	   return out;
	}

	HStream& operator<<(HStream& out, const int &in)
	{
	   assert(out.fCurrentName.size() > 0);
	   out.FillHranch(out.fCurrentName.c_str(), in);
	   out.fCurrentName = "";
	   return out;
	}

	HStream& operator<<(HStream& out, const unsigned long &in)
	{
	   assert(out.fCurrentName.size() > 0);
	   out.FillHranch(out.fCurrentName.c_str(), in);
	   out.fCurrentName = "";
	   return out;
	}

	HStream& operator <<(HStream& out, std::ostream& (*os)(std::ostream&))
	{
		return out;
	}
};
