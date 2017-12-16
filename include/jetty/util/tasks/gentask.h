#ifndef __JETTY_UTIL_GENTASK_HH
#define __JETTY_UTIL_GENTASK_HH

#include <vector>

#include <jetty/util/wrapper/wrapper.h>
#include <jetty/util/pythia/pyargs.h>

#include <Pythia8/Pythia.h>

namespace GenUtil
{
	class GenTask
	{
	public:
		enum { kBeforeInit, kGood, kDefinedStop, kInactive, kError, kDone };
		GenTask(const char *name)
			: fName(name), fArgs(), fSubtasks(), fParent(0), fStatus(kBeforeInit)
			{;}
		GenTask(const char *name, const char *params)
			: fName(name), fArgs(params), fSubtasks(), fParent(0), fStatus(kBeforeInit)
			{;}
		GenTask();

		virtual 				~GenTask();

		virtual unsigned int 	ExecThis(const char *opt = "");
		virtual unsigned int 	InitThis(const char *opt);
		virtual unsigned int 	FinalizeThis(const char *opt);


		unsigned int 			Execute(const char *opt = "");
		unsigned int 			Init(const char *opt = "");
		unsigned int 			Finalize(const char *opt = "");

		unsigned int 			GetStatus() {return fStatus;}
		void 					SetStatus(unsigned int st) {fStatus = st;}
		std::string 			GetName() const {return fName;}
		void 					SetName(const char *s) {fName = s;}
		void 					SetName(const std::string &s) {fName = s;}
		void 					AddTask(GenTask *t);
		void 					SetParent(GenTask *t) {fParent = t;}
		GenTask *				GetParent() {return fParent;}

	private:
		std::string 			fName;
		PyUtil::Args 			fArgs;
		std::vector<GenTask*> 	fSubtasks;
		GenTask *				fParent;
		unsigned int 			fStatus;

		static unsigned int _instance_counter;

	protected:

		static Wrapper *		fExchangeWrapper;

	};

	class PythiaTask : public GenTask
	{
	public:
		PythiaTask(const char *name) : GenTask(name), fCurrentPythia(0) {;}
		PythiaTask(const char *name, const char *params) : GenTask(name, params), fCurrentPythia(0) {;}
		PythiaTask() : GenTask(), fCurrentPythia(0) {;}
		virtual ~PythiaTask();
		virtual unsigned int ExecThis(const char *opt = "");
		virtual unsigned int 	InitThis(const char *opt);
	private:
		Pythia8::Pythia *fCurrentPythia; // useful in case of using a pool
	};

	class SpectraPtHatBins : public PythiaTask
	{
		public:
			SpectraPtHatBins(const char *name) : PythiaTask(name), fvPtHatMin() {;}
			SpectraPtHatBins(const char *name, const char *params) : PythiaTask(name, params), fvPtHatMin() {;}
			SpectraPtHatBins() : PythiaTask(), fvPtHatMin() {;}
			virtual ~SpectraPtHatBins();
			virtual unsigned int ExecThis(const char *opt = "");
		virtual unsigned int 	InitThis(const char *opt);
		private:
			std::vector<double> fvPtHatMin;
	};
};

#endif
