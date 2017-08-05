#ifndef __HSTREAM_HH
#define __HSTREAM_HH

#include <cassert>
#include <vector>
#include <string>
#include <iostream>
#include <fastjet/PseudoJet.hh>
#include <Pythia8/Pythia.h>

class TList;
class TH1;
class TFile;

namespace RStream
{
	class HStream
	{
	public:
		HStream();
		HStream(const char *sconfig, bool file_config, TFile *fout = 0);
		HStream(const char *name, const char *sconfig, bool file_config, TFile *fout = 0);

		bool Init(const char *sconfig, bool file_config, TFile *fout = 0);
		bool Init(const char *name, const char *sconfig, bool file_config, TFile *fout = 0);

		bool Initialized() {return fInit;}

		virtual ~HStream();

		TH1* CreateH(const char *sname);
		TH1* CreateH(const std::string &sname) {return CreateH(sname.c_str());}

		void FillHranch(const char *name, const Pythia8::Particle &in);
		void FillHranch(const char *name, const fastjet::PseudoJet &in);
		void FillHranch(const char *name, const double &in);
		void FillHranch(const char *name, const int &in);
		void FillHranch(const char *name, const unsigned long &in);
		void FillHranch(const char *name, const std::vector<fastjet::PseudoJet> &in);
		void FillHranch(const char *name, const std::vector<double> &in);
		void FillHranch(const char *name, const std::vector<int> &in);

		friend HStream& operator<<(HStream& out, const char *bname);
		friend HStream& operator<<(HStream& out, const std::string &bname);
		friend HStream& operator<<(HStream& out, const std::vector<fastjet::PseudoJet> &in);
		friend HStream& operator<<(HStream& out, const std::vector<double> &in);
		friend HStream& operator<<(HStream& out, const std::vector<int> &in);
		friend HStream& operator<<(HStream& out, const fastjet::PseudoJet &in);
		friend HStream& operator<<(HStream& out, const Pythia8::Particle &in);
		friend HStream& operator<<(HStream& out, const double &in);
		friend HStream& operator<<(HStream& out, const int &in);
		friend HStream& operator<<(HStream& out, const unsigned long &in);
		// template<class T>
		// friend HStream& operator<<(HStream& out, const T &val);
		friend HStream& operator<<(HStream& out, const double &val);

		friend HStream& operator <<(HStream& out, std::ostream& (*os)(std::ostream&));

		std::string CurrentHranchName() const {return fCurrentName;}

	protected:
		TList *fList;
		std::string fName;
		std::string fCurrentName;
		std::string fConfigString;
		TFile *fOutputFile;
		bool fFileOwner;
		bool fInit;
		void CreateHistograms();
	};
};

// namespace PyUtil
// {
// 	class CrossSections;
// 	class OutKinematics;
// }
//
// TTree & operator << (TTree & t, const PyUtil::CrossSections &xs);
// TTree & operator << (TTree & t, const PyUtil::OutKinematics &kine);

#endif // __HSTREAM_HH

