#ifndef __SCANHARD_HH
#define __SCANHARD_HH

#include <Pythia8/Pythia.h>
#include <map>

namespace PyUtil
{
	class ProcStats
	{
	public:
		ProcStats(int procid = 0);
		virtual ~ProcStats();
		void add(double v);
		void calculate();
		double std_dev();
		double mean();
	private:
		std::vector<double> _v;
		double _std_dev;
		double _mean;
		int _proc_id;
		bool _recalc;
	};

	class StatHardPythia
	{
	public:
		StatHardPythia();
		virtual ~StatHardPythia();
		void add_event(const Pythia8::Pythia &pythia);
		bool accept(const Pythia8::Pythia &pythia, double n_std_dev);
		double std_dev(const Pythia8::Pythia &pythia);
		double mean(const Pythia8::Pythia &pythia);
		double n_std_dev(const Pythia8::Pythia &pythia);
	private:
		//std::vector<ProcStats> _stats;
		std::map<int, ProcStats> _stats;
	};
};

#endif // __SCANHARD_HH
