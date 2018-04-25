#ifndef __JETTY_UTIL_GPYTHIA_HH
#define __JETTY_UTIL_GPYTHIA_HH

#include <jetty/util/tasks/gentask.h>

class TFile;
class TTree;
namespace RStream
{
	class TStream;
}

namespace GenUtil
{
	class GPythiaTask : public GenTask
	{
	public:
		GPythiaTask(const char *name) : GenTask(name), fOutput(0), fOutputTree(0), fTStream(0) {;}
		GPythiaTask(const char *name, const char *params) : GenTask(name, params), fOutput(0), fOutputTree(0), fTStream(0) {;}
		GPythiaTask() : GenTask(), fOutput(0), fOutputTree(0), fTStream(0) {;}
		virtual 			~GPythiaTask();
		virtual unsigned int InitThis(const char *opt = "");
		virtual unsigned int ExecThis(const char *opt = "");
		virtual unsigned int FinalizeThis(const char *opt = "");

	protected:
		TFile *fOutput;
		TTree *fOutputTree;
		RStream::TStream *fTStream;
	};
}

#endif
