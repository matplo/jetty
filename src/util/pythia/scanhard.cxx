#include "scanhard.h"

#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics.hpp>
#include <boost/accumulators/statistics/variance.hpp>
#include <algorithm>
#include <functional>
#include <cmath>

#include "util/blog.h"

//using namespace boost::accumulators;
namespace accm = boost::accumulators;

namespace PyUtil
{
	ProcStats::ProcStats(int procid)
		: _v()
		, _std_dev(0)
		, _mean(0)
		, _proc_id (procid)
		, _recalc(true)
	{
		;
	}

	ProcStats::~ProcStats()
	{
		;
	}

	void ProcStats::add(double v)
	{
		_v.push_back(v);
		_recalc = true;
	}

	void ProcStats::calculate()
	{
		if (_recalc)
		{
			accm::accumulator_set<double, accm::stats<accm::tag::variance(accm::lazy)>> acc;
			for_each(_v.begin(), _v.end(), std::bind<void>(std::ref(acc), std::placeholders::_1));
			_mean =  accm::mean(acc);
			_std_dev = sqrt(accm::variance(acc));
			Ltrace << "_mean = " << _mean << " " << " _std_dev = " << _std_dev;
		}
		_recalc = false;
	}

	double ProcStats::std_dev()
	{
		calculate();
		return _std_dev;
	}

	double ProcStats::mean()
	{
		calculate();
		return _mean;
	}


	StatHardPythia::StatHardPythia()
		: _stats()
	{
		;
	}
	StatHardPythia::~StatHardPythia()
	{
		;
	}

	void StatHardPythia::add_event(const Pythia8::Pythia &pythia)
	{
		int icode = pythia.info.code();
		if (icode == 101 || (icode >= 111 && icode <= 124) || (icode >=201 && icode <= 205))
		{
			Ltrace << "[StatHardPythia::add_event] event code = " << icode << " ok";
		}
		else
		{
			Ldebug << "[StatHardPythia::add_event] event code = " << icode << " unknown";
			return;
		}

		auto it = _stats.find(icode);
		if (it == _stats.end())
		{
		  	_stats.insert (_stats.begin(), std::pair<int, ProcStats>(icode, ProcStats(icode)));
		}
		it = _stats.find(icode);
		if (it == _stats.end())
		{
			Lfatal << "[StatHardPythia::add_event] something went reall wrong with insertion... code = " << icode;
		}
		else
		{
			// add pT's of the outgoing partons from the hard event
			double _max = std::max(pythia.event[5].pT(), pythia.event[6].pT());
			it->second.add(_max);
			Ltrace << "hard max: " << _max << " mean = " << mean(pythia) << " std_dev = " << std_dev(pythia);
			//it->second.add(pythia.event[5].pT());
			//it->second.add(pythia.event[6].pT());
		}
	}

	bool StatHardPythia::accept(const Pythia8::Pythia &pythia, double n_std_dev)
	{
		return false;
	}

	double StatHardPythia::std_dev(const Pythia8::Pythia &pythia)
	{
		int icode = pythia.info.code();
		auto it = _stats.find(icode);
		if (it == _stats.end())
		{
			Ltrace << "[StatHardPythia::std_dev] unkown event code = " << icode;
			return 0.0;
		}
		return it->second.std_dev();
	}

	double StatHardPythia::mean(const Pythia8::Pythia &pythia)
	{
		int icode = pythia.info.code();
		auto it = _stats.find(icode);
		if (it == _stats.end())
		{
			Ltrace << "[StatHardPythia::std_dev] unkown event code = " << icode;
			return 0.0;
		}
		return it->second.mean();
	}

	double StatHardPythia::n_std_dev(const Pythia8::Pythia &pythia)
	{
		auto _std_dev = std_dev(pythia);
		if (_std_dev != 0.0)
		{
			double _max = std::max(pythia.event[5].pT(), pythia.event[6].pT());
			return (_max - mean(pythia) ) / _std_dev;
		}
		return 0.0;
	}

};
