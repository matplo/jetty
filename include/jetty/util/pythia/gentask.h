#ifndef __JETTY_UTIL_PYTHIA_GENTASK_HH
#define __JETTY_UTIL_PYTHIA_GENTASK_HH

#include <vector>
#include <TTask.h>

#include <jetty/util/pythia/pyargs.h>
#include <Pythia8/Pythia.h>

namespace PyUtil
{
	class GenTask : public TTask
	{
	public:
		GenTask(const char *name, const char *title) : TTask(name, title), fArgs() {;}
		GenTask(const char *name, const char *title, const char *params) : TTask(name, title), fArgs(params) {;}
		GenTask();
		virtual ~GenTask() {;}
	private:
		Args fArgs;
	};

	class PythiaTask : public GenTask
	{
	public:
		PythiaTask(const char *name, const char *title) : GenTask(name, title), fCurrentPythia(0) {;}
		PythiaTask(const char *name, const char *title, const char *params) : GenTask(name, title, params), fCurrentPythia(0) {;}
		PythiaTask() : GenTask(), fCurrentPythia(0) {;}
		virtual ~PythiaTask();
		virtual void Exec(Option_t *option);
	private:
		Pythia8::Pythia *fCurrentPythia; // useful in case of using a pool
	};

	class SpectraPtHatBins : public PythiaTask
	{
		public:
			SpectraPtHatBins(const char *name, const char *title) : PythiaTask(name, title), fvPtHatMin() {;}
			SpectraPtHatBins(const char *name, const char *title, const char *params) : PythiaTask(name, title, params), fvPtHatMin() {;}
			SpectraPtHatBins() : PythiaTask(), fvPtHatMin() {;}
			virtual ~SpectraPtHatBins();
			virtual void Exec(Option_t *option);
		private:
			std::vector<double> fvPtHatMin;
	};
};
#endif
