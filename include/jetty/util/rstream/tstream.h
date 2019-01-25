#ifndef __TSTREAM_HH
#define __TSTREAM_HH

#include <cassert>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <fastjet/PseudoJet.hh>
#include <Pythia8/Event.h>

class TTree;
class TBranch;

namespace RStream
{
	class TStream
	{
	public:
		TStream(TTree *t);
		TStream(const char *name, TTree *t);

		void FillBranch(const char *name, const fastjet::PseudoJet &in);
		void FillBranch(const char *name, const double &in);
		void FillBranch(const char *name, const int &in);
		void FillBranch(const char *name, const unsigned long &in);
		void FillBranch(const char *name, const std::vector<fastjet::PseudoJet> &in);
		void FillBranch(const char *name, const std::vector<double> &in);
		void FillBranch(const char *name, const std::vector<int> &in);
		void FillTree();

		friend TStream& operator<<(TStream& out, const char *bname);
		friend TStream& operator<<(TStream& out, const std::string &bname);
		friend TStream& operator<<(TStream& out, const std::vector<fastjet::PseudoJet> &in);
		friend TStream& operator<<(TStream& out, const std::vector<double> &in);
		friend TStream& operator<<(TStream& out, const std::vector<int> &in);
		friend TStream& operator<<(TStream& out, const fastjet::PseudoJet &in);
		friend TStream& operator<<(TStream& out, const double &in);
		friend TStream& operator<<(TStream& out, const int &in);
		friend TStream& operator<<(TStream& out, const unsigned long &in);
		// template<class T>
		// friend TStream& operator<<(TStream& out, const T &val);
		friend TStream& operator<<(TStream& out, const double &val);

		friend TStream& operator <<(TStream& out, std::ostream& (*os)(std::ostream&));

		void FillBranch(const char *name, const std::vector<Pythia8::Particle> &in);
		friend TStream& operator<<(TStream& out, const std::vector<Pythia8::Particle> &in);

		friend TStream& operator<<(TStream& out, const std::ostringstream &ss);

	   std::string CurrentBranchName() const {return fCurrentName;}

	protected:
		std::string fName;
		std::string fCurrentName;
		TTree *fTree;
	};
};

namespace PyUtil
{
	class CrossSections;
	class OutKinematics;
}

TTree & operator << (TTree & t, const PyUtil::CrossSections &xs);
TTree & operator << (TTree & t, const PyUtil::OutKinematics &kine);

#endif // __TSTREAM_HH

