#ifndef __JETTY_PYTHIA_UTIL_PYTHIA_POOL_HH
#define __JETTY_PYTHIA_UTIL_PYTHIA_POOL_HH

class TH2I;

#include <vector>
#include <Pythia8/Pythia.h>

#include <jetty/util/pythia/pyargs.h>

namespace PyUtil
{
	// some credit - quick copy&paste: http://www.cplusplus.com/forum/beginner/152735/
	class PythiaPool
	{
		public:
			static PythiaPool& Instance()
			{
				static PythiaPool _instance; // Guaranteed to be destroyed. // Instantiated on first use.
				return _instance;
			}
			void SetCommonSettings(const char *s);
			void SetupECMs(double eA, double eB, int ndiv = 100);
			Pythia8::Pythia * GetPythia(const PyUtil::Args &new_settings);
			Pythia8::Pythia * GetPythia(double eA, double eB, const char *new_settings = 0);
			Pythia8::Pythia * GetPythiaSettingsInsensitive(double eA, double eB, const char *new_settings);

			virtual ~PythiaPool();
			void WriteECMsToFile(const char *fname);

		private:
			PythiaPool()
				: _pythia_pool()
				, _pythia_pool_settings()
				, _pythia_pool_settings_hashes()
				, _eAeBmap(0)
				, _eAeBmapW(0)
			{;} // private construct
			PythiaPool(const PythiaPool&) = delete; // non-copyable
			PythiaPool(PythiaPool&&) = delete; // and non-movable
			// as there is only one object, assignment would always be assign to self
			PythiaPool& operator= ( const PythiaPool& ) = delete ;
			PythiaPool& operator= ( PythiaPool&& ) = delete ;

			Pythia8::Pythia * CreatePythia(double eA, double eB, const char *s);
			Pythia8::Pythia * CreatePythia(const PyUtil::Args &args);

			void DumpInfo();

			std::vector<Pythia8::Pythia*> _pythia_pool;
			std::vector<PyUtil::Args> 	  _pythia_pool_settings; // store settings - will create new in place if settings changed
			std::vector<int> 	  		  _pythia_pool_settings_hashes; // store settings - will create new in place if settings changed
			std::string 				  _common_settings;
			TH2I 						  *_eAeBmap; // store indexes eA+eB system in a binned histogram
			TH2I 						  *_eAeBmapW; // store indexes eA+eB system in a binned histogram
	};

}

#endif
