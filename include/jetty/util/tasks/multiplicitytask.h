#ifndef __JETTY_UTIL_MULTIPLICITYTASK_HH
#define __JETTY_UTIL_MULTIPLICITYTASK_HH

#include <jetty/util/tasks/eventpooltask.h>

#include <string>
class TH1F;
class TFile;
class TParticle;

namespace GenUtil
{
	class MultiplicityEstimator;

	// a convenience class
	// init creates pythia or grabs last instance from the fShared
	class MultiplicityTask : public EventPoolTask
	{
	public:
		MultiplicityTask(const char *name) : EventPoolTask(name), fMult(0) {;}
		MultiplicityTask(const char *name, const char *params) : EventPoolTask(name, params), fMult(0) {;}
		MultiplicityTask() : EventPoolTask(), fMult(0) {;}
		virtual 			~MultiplicityTask();
		virtual unsigned int InitThis(const char *opt = "");
		virtual unsigned int ExecThis(const char *opt = "");
		virtual unsigned int FinalizeThis(const char *opt = "");
		MultiplicityEstimator *GetEstimator() {return fMult;}
	protected:
		MultiplicityEstimator *fMult;
	};

	class MultiplicityEstimator
	{
	public:
		enum { 	kFS = 0, kFSCh,
				kFSInt, kFSChInt,
				kFSPerEv, kFSChPerEv,
				kMEMax};
		const char *GetEstimatorName(int i)
		{
			switch (i)
			{
				case kFSChPerEv 	: return "kFSChPerEv"; break;
				case kFSPerEv 		: return "kFSPerEv"; break;
				case kFSChInt 		: return "kFSChInt"; break;
				case kFSInt 		: return "kFSInt"; break;
				case kFSCh 			: return "kFSCh"; break;
				case kFS 			: return "kFS"; break;
				default:
					return 0x0;
			}
		}

		MultiplicityEstimator(const char *name);
		virtual ~MultiplicityEstimator();

		void AddParticle(const TParticle *p, const double scale = 1.);
		template <class T> void AddParticle(const T &p, const double scale = 1.);
		void AddParticle(double eta, bool isCharged, const double scale = 1.);

		template <class E> void AddEventParticles(const E &e, const double scale = 1.);
		template <class E> void AddEvent(const E &e, const double scale = 1.);

		void NotifyEvent();
		void Write(TFile *fout = 0);
		double GetMultiplicity(unsigned int which, double etamin = -1., double etamax = 1);

		unsigned int Nevents() {return fNevent;}
		TH1F *GetEstimator(int i) { if (i >= 0 && i <= kMEMax) return fHist[i]; return 0x0;}


	protected:
		std::string fName;
		unsigned int fNevent;
		TH1F *fHist[kMEMax];
		bool fEventDone;
	private:
		MultiplicityEstimator() {;}
	};
}

#endif // __JETTY_UTIL_MULTIPLICITYTASK_HH
