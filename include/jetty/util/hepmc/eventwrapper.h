#ifndef __HEPMCUTIL_EVENTWRAPPER__hh
#define __HEPMCUTIL_EVENTWRAPPER__hh

namespace HepMC
{
	class IO_GenEvent;
	class IO_GenEvent;
	class GenEvent;
}

namespace Pythia8
{
	class Pythia;
}

namespace GenUtil
{
	class HepMCEventWrapper
	{
	public:
		HepMCEventWrapper() : fIn(0), fOut(0), fEvent(0), fPythia8(0) {;}
		virtual ~HepMCEventWrapper();
		HepMC::GenEvent* GetEvent();
		void SetPythia8(Pythia8::Pythia *pythia);
		void SetInputFile(const char *fname, bool force = false);
		void SetOutputFile(const char *fname, bool force = false);
		bool FillEvent();
		bool WriteEvent();
	private:
		HepMC::IO_GenEvent* fIn;
		HepMC::IO_GenEvent* fOut;
		HepMC::GenEvent* fEvent;
		Pythia8::Pythia* fPythia8;
	};
}

#endif
