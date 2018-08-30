#ifndef __JETTY_UTIL_GENTASK_HH
#define __JETTY_UTIL_GENTASK_HH

#include <vector>

#include <jetty/util/wrapper/wrapper.h>
#include <jetty/util/pythia/pyargs.h>

class TFile;
class TTree;

namespace GenUtil
{
	class GenTask
	{
	public:
		enum { kBeforeInit, kGood, kDefinedStop, kSkipEvent, kInactive, kError, kDone };
		GenTask(const char *name);
		GenTask(const char *name, const char *params);
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

		unsigned int 			GetId() {return fTaskId;}
		GenTask *				GetTask(unsigned int id);
		const std::vector<GenTask*> GetTasks() {return fTasks;}

		static void 			DumpTaskListInfo();
		static void 			Cleanup();

		void 					AddInputTask(GenTask *t);

		Wrapper * 				GetData() {return fData;}

	protected:
		std::string 			fName;
		PyUtil::Args 			fArgs;
		std::vector<GenTask*> 	fSubtasks;
		GenTask *				fParent;
		unsigned int 			fStatus;
		unsigned int 			fNExecCalls;
		unsigned int 			fTaskId;
		std::vector<GenTask*> 	fInputTasks;
		Wrapper *				fData;

		static unsigned int 			_instance_counter;
		static Wrapper *				fShared;
		static std::vector<GenTask*> 	fTasks;

		std::string 			fOutputPath;
		TFile 				   *fOutputFile;
		TTree 				   *fOutputTree;

	private:
	};
};

#endif
