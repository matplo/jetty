#include <jetty/util/pythia/outkinematics.h>

#include <Pythia8/Pythia.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <string>

#include <jetty/util/rstream/tstream.h>

namespace PyUtil
{
	// OutKinematics implementation
	OutKinematics::OutKinematics(const Pythia8::Pythia &pythia, bool includeHard)
	: i_p_z({pythia.event[1].pz(), pythia.event[2].pz()})
	, f_p_z({0, 0})
	, d_p_z({0, 0})
	, mA(0.93827)
	, mB(0.93827)
	{
		_calculate(pythia, includeHard);
	}

	OutKinematics::OutKinematics()
	: i_p_z({0, 0})
	, f_p_z({0, 0})
	, d_p_z({0, 0})
	, mA(0.93827)
	, mB(0.93827)
	{
		;
	}
	OutKinematics::OutKinematics(const OutKinematics &o)
	: i_p_z({0, 0})
	, f_p_z({0, 0})
	, d_p_z({0, 0})
	, mA(0.93827)
	, mB(0.93827)
	{
		i_p_z = o.i_p_z;
		f_p_z = o.f_p_z;
		d_p_z = o.d_p_z;
		mA = o.mA;
		mB = o.mB;
	}

	void OutKinematics::_calculate(const Pythia8::Pythia &pythia, bool includeHard)
	{
		auto &event = pythia.event;
		for (unsigned int i = 0; i < event.size(); i++)
		{
			auto &p = event[i];
			auto im = has_beam_mother(p);
			if (im > 0)
			{
				// cout << i << " " << pypart_to_str(p) << endl;
				if (im == 1 || im == 3)
				{
					// cout << " mother: " << 1 << " " << pypart_to_str(event[1]) << endl;
					f_p_z[0] += p.pz();
				}
				if (im == 2 || im == 3)
				{
					// cout << " mother: " << 2 << " " << pypart_to_str(event[2]) << endl;
					f_p_z[1] += p.pz();
				}
			}
		}
		if (pythia.info.code() == 101) // non-diffractive
		{
			if (pythia.info.hasSub() && includeHard == true)
			{
				//f_p_z[0] += event[3].pz(); //incoming partons to the hard process
				//f_p_z[1] += event[4].pz(); //incoming partons to the hard process

				f_p_z[0] += event[5].pT(); //outgoing partons to the hard process
				f_p_z[1] += event[6].pT(); //outgoing partons to the hard process
			}
		}
		for ( int i : {0, 1})
		{
			d_p_z[i] = std::fabs(i_p_z[i] - f_p_z[i]);
			// std::cout << "=> remaining p_z_" << i+1 << " = " << f_p_z[i] << " delta p_z = " << d_p_z[i] << std::endl;
		}
	}

	double OutKinematics::sqrts(double eA, double eB, double _mA, double _mB) const
	{
		double _eA = std::fabs(eA);
		double _eB = std::fabs(eB);
		double _pA = std::sqrt(_eA * _eA - _mA * _mA);
		double _pB = std::sqrt(_eB * _eB - _mB * _mB);
		double eCM = std::sqrt( std::pow(_eA + _eB, 2.) - std::pow(_pA + (-1. * _pB), 2.) );
		return eCM;
	}

	double OutKinematics::sqrts(double eA, double eB) const
	{
		return sqrts(eA, eB, mA, mB);
	}

	double OutKinematics::sqrts_i() const
	{
		return sqrts(i_p_z[0], i_p_z[1]);
	}

	double OutKinematics::sqrts_f() const
	{
		return sqrts(f_p_z[0], f_p_z[1]);
	}

};

TTree & operator << (TTree & t, const PyUtil::OutKinematics &kine)
{
	RStream::TStream e("e", &t);
	e << "Ai" << kine.i_p_z[0];
	e << "Bi" << kine.i_p_z[1];
	e << "CMi" << kine.sqrts_i();

	e << "Af" << kine.f_p_z[0];
	e << "Bf" << kine.f_p_z[1];
	e << "CMf" << kine.sqrts_f();

	e << std::endl;
	return t;
}
