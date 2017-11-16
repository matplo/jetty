#ifndef __JETTY_PYTHIA_UTIL_PYTHIA_POOL_HH
#define __JETTY_PYTHIA_UTIL_PYTHIA_POOL_HH

class TH2I;

#include <vector>

#include <Pythia8/Pythia.h>

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
			Pythia8::Pythia * GetPythia(double eA, double eB, const char *new_settings = 0);

			virtual ~PythiaPool() { DumpInfo(); }
		private:
			PythiaPool() {;} // private construct
			PythiaPool(const PythiaPool&) = delete; // non-copyable
			PythiaPool(PythiaPool&&) = delete; // and non-movable
			// as there is only one object, assignment would always be assign to self
			PythiaPool& operator= ( const PythiaPool& ) = delete ;
			PythiaPool& operator= ( PythiaPool&& ) = delete ;

			Pythia8::Pythia * CreatePythia(double eA, double eB, const char *s);

			void DumpInfo();

			std::vector<Pythia8::Pythia*> _pythia_pool;
			std::string 				  _common_settings;
			TH2I 						  *_eAeBmap; // store indexes eA+eB system in a binned histogram
			TH2I 						  *_eAeBmapW; // store indexes eA+eB system in a binned histogram

	};

}

#endif
