#ifndef __JETTY_UTIL_GLAUBERTASK_HH
#define __JETTY_UTIL_GLAUBERTASK_HH

#include <vector>

#include <jetty/util/wrapper/wrapper.h>
#include <jetty/util/tasks/gentask.h>

#include <TString.h>

class TGlauberMC;
class TTree;
class TFile;

namespace RStream
{
	class TStream;
}

namespace GenUtil
{
	class GlauberTask : public GenTask
	{
	public:
		GlauberTask(const char *name) :
			GenTask(name), fpGlauberMC(0), fFixedb(-1.), fCollisionsTree(0), fTStream(0) {;}
		GlauberTask(const char *name, const char *params) :
			GenTask(name, params), fpGlauberMC(0), fFixedb(-1.), fCollisionsTree(0), fTStream(0) {;}
		GlauberTask() :
			GenTask(), fpGlauberMC(0), fFixedb(-1.), fCollisionsTree(0), fTStream(0) {;}
		virtual 			~GlauberTask();
		virtual unsigned int InitThis(const char *opt = "");
		virtual unsigned int ExecThis(const char *opt = "");
		virtual unsigned int FinalizeThis(const char *opt = "");

		TGlauberMC 	*GetGlauberMC() {return fpGlauberMC;}

	protected:
		TString 		OutputFileName();
		TGlauberMC 		*fpGlauberMC;
		double 			 fFixedb;

		TTree 			*fCollisionsTree;

		RStream::TStream *fTStream;
	};
}

#endif
