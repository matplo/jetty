#ifndef __JETTY_UTIL_SUBJETTASK_HH
#define __JETTY_UTIL_SUBJETTASK_HH

#include <jetty/util/tasks/eventpooltask.h>
#include <fastjet/JetDefinition.hh>

#include <string>
class TH1F;
class TFile;
class TTree;
class TParticle;

namespace RStream
{
	class TStream;
}

namespace GenUtil
{
	class SubjetTask : public EventPoolTask
	{
	public:
		class Settings
		{
		public:
			Settings();
			~Settings () {;}

			std::string str();
			void setup_from_string(const char *s, const char *comment = 0x0);

			double R;
			fastjet::JetAlgorithm    A; //algorithm
			double maxEta;
			double jptcut;
			double jptcutmax;
			double sjR;
			fastjet::JetAlgorithm    sjA; //algorithm
			double sd_z_cut;
			double sd_beta;
			double sd_r_jet;
		};

		SubjetTask(const char *name) : EventPoolTask(name), fOutput(0), fOutputTree(0), fTStream(0), fSettings() {;}
		SubjetTask(const char *name, const char *params) : EventPoolTask(name, params), fOutput(0), fOutputTree(0), fTStream(0), fSettings() {;}
		SubjetTask() : EventPoolTask(), fOutput(0), fOutputTree(0), fTStream(0), fSettings() {;}
		virtual 			~SubjetTask();
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

#endif // __JETTY_UTIL_SUBJETTASK_HH
