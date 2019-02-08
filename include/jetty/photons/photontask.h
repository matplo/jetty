#ifndef __JETTY_PHOTONS_PHOTONTASK_HH
#define __JETTY_PHOTONS_PHOTONTASK_HH

#include <jetty/util/tasks/eventpooltask.h>

#include <string>
class TH1F;
class TFile;
class TTree;
class TParticle;
class TF1;

namespace RStream
{
	class TStream;
}

namespace GenUtil
{
	class HepMCEventWrapper;
}

namespace Photons
{
	class PhotonTask : public GenUtil::EventPoolTask
	{
	public:
		class Settings
		{
		public:
			Settings();
			~Settings () {;}

			std::string str();
			void setup_from_string(const char *s, const char *comment = 0x0);
			double maxEta;
			double jetR;
		};

		PhotonTask(const char *name) : EventPoolTask(name), fOutput(0), fOutputTree(0), fTStream(0), fMCEvWrapper(0), fSettings() {;}
		PhotonTask(const char *name, const char *params) : EventPoolTask(name, params), fOutput(0), fOutputTree(0), fTStream(0), fMCEvWrapper(0), fSettings() {;}
		PhotonTask() : EventPoolTask(), fOutput(0), fOutputTree(0), fTStream(0), fMCEvWrapper(0), fSettings() {;}
		virtual 			~PhotonTask();
		virtual unsigned int InitThis(const char *opt = "");
		virtual unsigned int ExecThis(const char *opt = "");
		virtual unsigned int FinalizeThis(const char *opt = "");
	protected:
		TFile *fOutput;
		TTree *fOutputTree;
		RStream::TStream *fTStream;
		GenUtil::HepMCEventWrapper *fMCEvWrapper;
		Settings fSettings;
	};

}

#endif // __JETTY_EIC_EICSPECTRATASK_HH
