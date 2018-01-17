#ifndef __JETTY_UTIL_PYTHIATASK_HH
#define __JETTY_UTIL_PYTHIATASK_HH

#include <vector>

#include <jetty/util/wrapper/wrapper.h>
#include <jetty/util/pythia/pyargs.h>
#include <jetty/util/tasks/gentask.h>

namespace Pythia8
{
	class Pythia;
};

namespace GenUtil
{
	// a convenience class
	// init creates pythia or grabs last instance from the fShared
	class PythiaTask : public GenTask
	{
	public:
		PythiaTask(const char *name) : GenTask(name), fpPythia(0) {;}
		PythiaTask(const char *name, const char *params) : GenTask(name, params), fpPythia(0) {;}
		PythiaTask() : GenTask(), fpPythia(0) {;}
		virtual 			~PythiaTask();
		virtual unsigned int Init(const char *opt = "");
		virtual unsigned int ExecThis(const char *opt = "");
	protected:
		Pythia8::Pythia 		*fpPythia;
	};

	class SpectraPtHatBins : public PythiaTask
	{
	public:
		SpectraPtHatBins(const char *name) : PythiaTask(name), fvPtHatMin() {;}
		SpectraPtHatBins(const char *name, const char *params) : PythiaTask(name, params), fvPtHatMin() {;}
		SpectraPtHatBins() : PythiaTask(), fvPtHatMin() {;}
		virtual 			~SpectraPtHatBins();
		virtual unsigned int ExecThis(const char *opt = "");
		virtual unsigned int InitThis(const char *opt = "");
	protected:
		std::vector<double> fvPtHatMin;
	};
}

#endif
