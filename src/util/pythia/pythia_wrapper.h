#ifndef PYTHIAWRAPPER_HH
#define PYTHIAWRAPPER_HH

#include <Pythia8/Pythia.h>
#include <string>
#include "util/wrapper/wrapper.h"

class TFile;
class TNtuple;
namespace PyUtil
{
	class Args;

	class PythiaWrapper
	{
	public:
		PythiaWrapper(const char* params);
		PythiaWrapper(const std::string& params);
		PythiaWrapper(int argc, char *argv[]);

		virtual ~PythiaWrapper();

		void 				readConfig(const char *fname);

		bool 				next();
		Pythia8::Pythia* 	pythia();
		TFile*				outputFile();
		PyUtil::Args*     	args();

		bool 				initialized() {return _is_initialized;}

	private:
		PythiaWrapper() {;}
		std::string _outputFileName();
		bool _checkOutput();
		void _initPythia();
		void _initOutput();
		void _storeAverages();
		Wrapper *fWrapper;
		TNtuple *fNtuple;
		bool _is_initialized;
	};

};

#endif // PYTHIAWRAPPER_HH
