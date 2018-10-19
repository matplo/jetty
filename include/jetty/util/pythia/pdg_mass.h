#ifndef __JETTY_PYTHIA_PDG_MASS_HH
#define __JETTY_PYTHIA_PDG_MASS_HH

#include <map>

namespace PyUtil
{
	// some credit - quick copy&paste: http://www.cplusplus.com/forum/beginner/152735/
	// a util - get PID from a mass of a particle - to go deep into the subjet code - a test
	class PDGMass
	{
		public:
			static PDGMass& Instance()
			{
				static PDGMass _instance; // Guaranteed to be destroyed. // Instantiated on first use.
				return _instance;
			}

			void Add(double m, int pdg);
			double Mass(int pdg);
			int PDG(double m);

			virtual ~PDGMass();

		private:
			PDGMass(); // private construct
			PDGMass(const PDGMass&) = delete; // non-copyable
			PDGMass(PDGMass&&) = delete; // and non-movable
			// as there is only one object, assignment would always be assign to self
			PDGMass& operator= ( const PDGMass& ) = delete ;
			PDGMass& operator= ( PDGMass&& ) = delete ;

			std::map<int, double> fPDGM;
			std::map<double, int> fMPDG;
	};

}

#endif
