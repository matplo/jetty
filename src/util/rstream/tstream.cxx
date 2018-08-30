#include <jetty/util/rstream/tstream.h>
#include <jetty/util/pythia/pyutil.h>

#include <limits>

#include <TTree.h>
#include <TBranch.h>

using namespace std;

namespace RStream
{

	TStream::TStream(TTree *t)
		: fName("SE")
		, fCurrentName("")
		, fTree(t)
	{
		assert(t != NULL);
	}

	TStream::TStream(const char *name, TTree *t)
		: fName(name)
		, fCurrentName("")
		, fTree(t)
	{
		assert(t != NULL);
	}

	void TStream::FillBranch(const char *name, const vector<fastjet::PseudoJet> &in)
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
		FillBranch(s.c_str(), pt);
		s = name;
		s += "phi";
		FillBranch(s.c_str(), phi);
		s = name;
		s += "eta";
		FillBranch(s.c_str(), eta);
		s = name;
		s += "rap";
		FillBranch(s.c_str(), rap);
		s = name;
		s += "m";
		FillBranch(s.c_str(), m);
		s = name;
		s += "nc";
		FillBranch(s.c_str(), nc);
	}

	void TStream::FillBranch(const char *name, const vector<Pythia8::Particle> &in)
	{
		std::vector<double> pt;
		std::vector<double> phi;
		std::vector<double> eta;
		std::vector<double> y;
		std::vector<double> m;
		std::vector<int> id;
		std::vector<int> q;
		for (unsigned int i = 0; i < in.size(); i++)
		{
			pt.push_back(in[i].pT());
			phi.push_back(in[i].phi());
			eta.push_back(in[i].eta());
			y.push_back(in[i].y());
			m.push_back(in[i].m());
			id.push_back(in[i].id());
			if (in[i].isCharged())
				q.push_back(1);
			else
				q.push_back(0);
		}

		string s = name;

		s = name;
		s += "pt";
		FillBranch(s.c_str(), pt);
		s = name;
		s += "phi";
		FillBranch(s.c_str(), phi);
		s = name;
		s += "eta";
		FillBranch(s.c_str(), eta);
		s = name;
		s += "y";
		FillBranch(s.c_str(), y);
		s = name;
		s += "m";
		FillBranch(s.c_str(), m);
		s = name;
		s += "id";
		FillBranch(s.c_str(), id);
		s = name;
		s += "q";
		FillBranch(s.c_str(), q);
	}

	void TStream::FillBranch(const char *name, const fastjet::PseudoJet &in)
	{
		string s = name;
		s += "pt";
		FillBranch(s.c_str(), in.perp());
		s = name;
		s += "phi";
		FillBranch(s.c_str(), in.phi());
		s = name;
		s += "eta";
		FillBranch(s.c_str(), in.eta());
		s = name;
		s += "rap";
		FillBranch(s.c_str(), in.rap());
		s = name;
		s += "m";
		FillBranch(s.c_str(), in.m());
		s = name;
		s += "nc";
		if (in.has_constituents())
			FillBranch(s.c_str(), int(in.constituents().size()));
		else
			FillBranch(s.c_str(), int(0));
	}

	void TStream::FillBranch(const char *name, const vector<double> &in)
	{
		vector<double> *pv = 0;
		string sname = fName + "_" + name;
		TBranch *b = fTree->GetBranch(sname.c_str());
		if (b == 0)
		{
			b = fTree->Branch(sname.c_str(), &pv);
		}
		b->SetAddress(&pv);
		vector<double> &v  = *pv;
		for (unsigned int i = 0; i < in.size(); ++i)
		{
			double tpv(in[i]);
			v.push_back(tpv);
		}
		b->Fill();
		b->SetAddress(0x0);
		pv->clear();
		delete pv;
	}

	void TStream::FillBranch(const char *name, const vector<int> &in)
	{
		vector<int> *pv = 0;
		string sname = fName + "_" + name;
		TBranch *b = fTree->GetBranch(sname.c_str());
		if (b == 0)
		{
			b = fTree->Branch(sname.c_str(), &pv);
		}
		b->SetAddress(&pv);
		vector<int> &v  = *pv;
		for (unsigned int i = 0; i < in.size(); ++i)
		{
			int tpv(in[i]);
			v.push_back(tpv);
		}
		b->Fill();
		b->SetAddress(0x0);
		pv->clear();
		delete pv;
	}

	void TStream::FillBranch(const char *name, const double &in)
	{
		double v = in;
		string sname = fName + "_" + name;
		TBranch *b = fTree->GetBranch(sname.c_str());
		if (b == 0)
		{
			b = fTree->Branch(sname.c_str(), &v);
		}
		b->SetAddress(&v);
		b->Fill();
		b->SetAddress(0x0);
	}

	void TStream::FillBranch(const char *name, const int &in)
	{
		int v = in;
		string sname = fName + "_" + name;
		TBranch *b = fTree->GetBranch(sname.c_str());
		if (b == 0)
		{
			b = fTree->Branch(sname.c_str(), &v);
		}
		b->SetAddress(&v);
		b->Fill();
		b->SetAddress(0x0);
	}

	void TStream::FillBranch(const char *name, const unsigned long &in)
	{
		int v = in;
		string sname = fName + "_" + name;
		TBranch *b = fTree->GetBranch(sname.c_str());
		if (b == 0)
		{
			b = fTree->Branch(sname.c_str(), &v);
		}
		b->SetAddress(&v);
		b->Fill();
		b->SetAddress(0x0);
	}

	void TStream::FillTree()
	{
		Long64_t n = fTree->GetEntries();
		fTree->SetEntries(n + 1);
	}

	TStream& operator<<(TStream& out, const char *bname)
	{
	   out.fCurrentName = bname;
	   // std::cout << "current name is " << out.fCurrentName << std::endl;
	   return out;
	}

	TStream& operator<<(TStream& out, const std::string &bname)
	{
	   out.fCurrentName = bname;
	   // std::cout << "current name is " << out.fCurrentName << std::endl;
	   return out;
	}

	TStream& operator<<(TStream& out, const std::vector<fastjet::PseudoJet> &in)
	{
	   assert(out.fCurrentName.size() > 0);
	   out.FillBranch(out.fCurrentName.c_str(), in);
	   out.fCurrentName = "";
	   return out;
	}

	TStream& operator<<(TStream& out, const std::vector<Pythia8::Particle> &in)
	{
	   assert(out.fCurrentName.size() > 0);
	   out.FillBranch(out.fCurrentName.c_str(), in);
	   out.fCurrentName = "";
	   return out;
	}

	TStream& operator<<(TStream& out, const std::vector<double> &in)
	{
	   assert(out.fCurrentName.size() > 0);
	   out.FillBranch(out.fCurrentName.c_str(), in);
	   out.fCurrentName = "";
	   return out;
	}

	TStream& operator<<(TStream& out, const std::vector<int> &in)
	{
	   assert(out.fCurrentName.size() > 0);
	   out.FillBranch(out.fCurrentName.c_str(), in);
	   out.fCurrentName = "";
	   return out;
	}

	TStream& operator<<(TStream& out, const fastjet::PseudoJet &in)
	{
	   assert(out.fCurrentName.size() > 0);
	   out.FillBranch(out.fCurrentName.c_str(), in);
	   out.fCurrentName = "";
	   return out;
	}

	TStream& operator<<(TStream& out, const double &in)
	{
	   assert(out.fCurrentName.size() > 0);
	   out.FillBranch(out.fCurrentName.c_str(), in);
	   out.fCurrentName = "";
	   return out;
	}

	TStream& operator<<(TStream& out, const int &in)
	{
	   assert(out.fCurrentName.size() > 0);
	   out.FillBranch(out.fCurrentName.c_str(), in);
	   out.fCurrentName = "";
	   return out;
	}

	TStream& operator<<(TStream& out, const unsigned long &in)
	{
	   assert(out.fCurrentName.size() > 0);
	   out.FillBranch(out.fCurrentName.c_str(), in);
	   out.fCurrentName = "";
	   return out;
	}

	TStream& operator <<(TStream& out, std::ostream& (*os)(std::ostream&))
	{
		out.FillTree();
		return out;
	}
};


