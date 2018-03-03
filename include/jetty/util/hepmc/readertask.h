#ifndef __HEPMC_UTIL_READERTASK_HH
#define __HEPMC_UTIL_READERTASK_HH

#include <vector>

#include <jetty/util/wrapper/wrapper.h>
#include <jetty/util/pythia/pyargs.h>
#include <jetty/util/tasks/gentask.h>

namespace GenUtil
{
	class ReadHepMCFile;

	// a convenience class
	// init creates pythia or grabs last instance from the fShared
	class HepMCReaderTask : public GenTask
	{
	public:
		HepMCReaderTask(const char *name) : GenTask(name), fReader(0) {;}
		HepMCReaderTask(const char *name, const char *params) : GenTask(name, params), fReader(0) {;}
		HepMCReaderTask() : GenTask(), fReader(0) {;}
		virtual 			~HepMCReaderTask();
		virtual unsigned int InitThis(const char *opt = "");
		virtual unsigned int ExecThis(const char *opt = "");
	protected:
		ReadHepMCFile 		*fReader;
	};
}

#endif