#ifndef __JETTY_EIC_EICSPECTRATASK_HH
#define __JETTY_EIC_EICSPECTRATASK_HH

#include <jetty/util/tasks/eventpooltask.h>

#include <string>
class TH1F;
class TFile;
class TTree;
class TParticle;

namespace RStream
{
	class TStream;
}

namespace EIC
{
	class SpectraTask : public GenUtil::EventPoolTask
	{
	public:
		class Settings
		{
		public:
			Settings();
			~Settings () {;}

			std::string str();
			void setup_from_string(const char *s, const char *comment = 0x0);

			double R1;
			double R2;
			double maxEta;
		};

		SpectraTask(const char *name) : EventPoolTask(name), fOutput(0), fOutputTree(0), fTStream(0), fSettings() {;}
		SpectraTask(const char *name, const char *params) : EventPoolTask(name, params), fOutput(0), fOutputTree(0), fTStream(0), fSettings() {;}
		SpectraTask() : EventPoolTask(), fOutput(0), fOutputTree(0), fTStream(0), fSettings() {;}
		virtual 			~SpectraTask();
		virtual unsigned int InitThis(const char *opt = "");
		virtual unsigned int ExecThis(const char *opt = "");
		virtual unsigned int FinalizeThis(const char *opt = "");
	protected:
		TFile *fOutput;
		TTree *fOutputTree;
		RStream::TStream *fTStream;
		Settings fSettings;
	};

}

#endif // __JETTY_EIC_EICSPECTRATASK_HH
