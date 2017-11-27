#ifndef __OUTKINEMATICS__HH
#define __OUTKINEMATICS__HH

#include <Pythia8/Pythia.h>
#include <vector>
#include "pyutil.h"

#include <TTree.h>

namespace PyUtil
{
	class OutKinematics
	{
	public:
		OutKinematics();
		OutKinematics(const Pythia8::Pythia &pythia, bool includeHard = true);
		OutKinematics(const OutKinematics &o);
		~OutKinematics() { ; }

		double sqrts(double eA, double eB, double mA, double mB) const;
		double sqrts(double eA, double eB) const;
		double sqrts_i() const;
		double sqrts_f() const;

		std::vector<double> i_p_z;
		std::vector<double> f_p_z;
		std::vector<double> d_p_z;

		double mA;
		double mB;

		double f_et;

	private:

		void _calculate(const Pythia8::Pythia &pythia, bool includeHard);
	};
};

TTree & operator << (TTree & t, const PyUtil::OutKinematics &kine);

#endif // __OUTKINEMATICS__HH
