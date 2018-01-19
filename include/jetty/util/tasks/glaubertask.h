#ifndef __JETTY_UTIL_GLAUBERTASK_HH
#define __JETTY_UTIL_GLAUBERTASK_HH

#include <vector>

#include <jetty/util/wrapper/wrapper.h>
#include <jetty/util/tasks/gentask.h>

class TGlauberMC;

namespace GenUtil
{
	class GlauberTask : public GenTask
	{
	public:
		GlauberTask(const char *name) : GenTask(name), fpGlauberMC(0), fFixedb(-1.) {;}
		GlauberTask(const char *name, const char *params) : GenTask(name, params), fpGlauberMC(0), fFixedb(-1.) {;}
		GlauberTask() : GenTask(), fpGlauberMC(0), fFixedb(-1.) {;}
		virtual 			~GlauberTask();
		virtual unsigned int InitThis(const char *opt = "");
		virtual unsigned int ExecThis(const char *opt = "");
		virtual unsigned int FinalizeThis(const char *opt = "");

		TGlauberMC 	*GetGlauberMC() {return fpGlauberMC;}

	protected:
		TGlauberMC 		*fpGlauberMC;
		double 			 fFixedb;
	};
}

#endif
