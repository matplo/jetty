#ifndef __JETTY_UTIL_GENTASK_HH
#define __JETTY_UTIL_GENTASK_HH

#include <vector>

#include <jetty/util/wrapper/wrapper.h>
#include <jetty/util/pythia/pyargs.h>

namespace GenUtil
{
	class GenTask
	{
	public:
		enum { kBeforeInit, kGood, kDefinedStop, kInactive, kError, kDone };
		GenTask(const char *name)
			: fName(name), fArgs(), fSubtasks(), fParent(0), fStatus(kBeforeInit), fNExecCalls(0)
			{;}
		GenTask(const char *name, const char *params)
			: fName(name), fArgs(params), fSubtasks(), fParent(0), fStatus(kBeforeInit), fNExecCalls(0)
			{;}
		GenTask();

		virtual 				~GenTask();

		virtual unsigned int 	ExecThis(const char *opt = "");
		virtual unsigned int 	InitThis(const char *opt = "");
		virtual unsigned int 	FinalizeThis(const char *opt = "");


		virtual unsigned int 	Execute(const char *opt = "");
		virtual unsigned int 	Init(const char *opt = "");
		virtual unsigned int 	Finalize(const char *opt = "");

		unsigned int 			GetStatus() {return fStatus;}
		void 					SetStatus(unsigned int st) {fStatus = st;}
		std::string 			GetName() const {return fName;}
		void 					SetName(const char *s) {fName = s;}
		void 					SetName(const std::string &s) {fName = s;}
		void 					AddTask(GenTask *t);
		void 					SetParent(GenTask *t) {fParent = t;}
		GenTask *				GetParent() {return fParent;}
		Wrapper * 				GetShared() {return fShared;}
		PyUtil::Args 			*GetArgs() {return &fArgs;}

		unsigned int 			GetNExecCalls() {return fNExecCalls;}

		void 					SetOutputPath(const char *s) {fOutputPath = s;}
		std::string 			GetOutputPath() {return fOutputPath;}

	protected:
		std::string 			fName;
		PyUtil::Args 			fArgs;
		std::vector<GenTask*> 	fSubtasks;
		GenTask *				fParent;
		unsigned int 			fStatus;
		unsigned int 			fNExecCalls;

		static unsigned int _instance_counter;
		static Wrapper *		fShared;

	private:
		std::string 			fOutputPath;

	};
};

#endif
