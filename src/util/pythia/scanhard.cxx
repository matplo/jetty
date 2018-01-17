#include <jetty/util/pythia/scanhard.h>

#include <boost/accumulators/accumulators.hpp>
#include <boost/serialization/array_wrapper.hpp> // boost 1.64 needs this (changed from 1.63 - known issue - check later)
#include <boost/accumulators/statistics.hpp>
#include <boost/accumulators/statistics/variance.hpp>
#include <algorithm>
#include <functional>
#include <cmath>

#include <jetty/util/blog.h>

//using namespace boost::accumulators;
namespace accm = boost::accumulators;

#include <fastjet/PseudoJet.hh>
#include <fastjet/ClusterSequence.hh>
namespace fj = fastjet;

namespace PyUtil
{
	double total_eT(const Pythia8::Pythia &pythia)
	{
		auto &event             = pythia.event;
		double sum = 0;
		for (int ip = 0; ip < event.size(); ip++)
		{
			if (event[ip].isFinal())
			{
				sum += event[ip].eT();
			}
		}
		return sum;
	}

	static bool psj_pt_compare(const fj::PseudoJet &a, const fj::PseudoJet &b)
	{
		return (a.perp() < b.perp());
	}

	double estim_eT_jets(const Pythia8::Pythia &pythia)
	{
		auto &event             = pythia.event;
		// double sum = 0;
		std::vector<fj::PseudoJet> parts;
		for (int ip = 0; ip < event.size(); ip++)
		{
			if (event[ip].isFinal())
			{
				fj::PseudoJet p(event[ip].px(), event[ip].py(), event[ip].pz(), event[ip].e());
				p.set_user_index(ip);
				parts.push_back(p);
			}
		}
		fj::JetDefinition jet_def(fj::antikt_algorithm, 0.4);
		fj::ClusterSequence cs(parts, jet_def);
		auto jets = fj::sorted_by_pt(cs.inclusive_jets());
		//std::vector<double> vpt;
		//for (auto j : jets)
		//{
		//	vpt.push_back(j.perp());
		//	if (std::fabs(j.eta()) < 2.)
		//		sum += j.perp();
		//}
		////return sum;
		//auto max_element = std::max_element(vpt.begin(), vpt.end(), psj_pt_compare);
		if (jets.size() > 0)
			return jets[0].perp();
		return 0;
	}

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

	void StatHardPythia::add_event(Pythia8::Pythia &pythia)
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
			//double _max = std::max(pythia.event[5].pT(), pythia.event[6].pT());
			//double _max = pythia.event[5].pT() + pythia.event[6].pT();
			//double _max = total_eT(pythia);
			double _max = estim_eT_jets(pythia);
			//double _max = (pythia.event[5].pT() + pythia.event[6].pT()) / 2.; // * pythia.info.sigmaGen();
			it->second.add(_max);
			Ltrace << "hard max: " << _max << " mean = " << mean(pythia) << " std_dev = " << std_dev(pythia);
			//it->second.add(pythia.event[5].pT());
			//it->second.add(pythia.event[6].pT());
		}
	}

	bool StatHardPythia::accept(const Pythia8::Pythia &/*pythia*/, double /*n_std_dev*/)
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
			//double _max = std::max(pythia.event[5].pT(), pythia.event[6].pT());
			//double _max = pythia.event[5].pT() + pythia.event[6].pT();
			//double _max = total_eT(pythia);
			double _max = estim_eT_jets(pythia);
			//double _max = (pythia.event[5].pT() + pythia.event[6].pT()) / 2.;
			return (_max - mean(pythia) ) / _std_dev;
		}
		return 0.0;
	}

};
