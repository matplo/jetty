#ifndef __JETTY_PYTHIA_PARAMETRIZED_XSECTIONS_HH
#define __JETTY_PYTHIA_PARAMETRIZED_XSECTIONS_HH

class TF1;
#include <string>

namespace PyUtil
{
	// some credit - quick copy&paste: http://www.cplusplus.com/forum/beginner/152735/
	class ParamSigmas
	{
		public:
			enum Process {kTOTAL, kND, kEL, kSDXB, kSDAX, kDD, kINEL, kMax};

			static ParamSigmas& Instance()
			{
				static ParamSigmas _instance; // Guaranteed to be destroyed. // Instantiated on first use.
				return _instance;
			}

			TF1 *Get(int iwhich);
			TF1 *Get(const char *str_which);

			double Get(int iwhich, double eCM);
			double Get(int iwhich, double eA, double eB);

			double Get(const char *str_which, double eCM);
			double Get(const char *str_which, double eA, double eB);

			virtual ~ParamSigmas();

		private:
			ParamSigmas(); // private construct
			ParamSigmas(const ParamSigmas&) = delete; // non-copyable
			ParamSigmas(ParamSigmas&&) = delete; // and non-movable
			// as there is only one object, assignment would always be assign to self
			ParamSigmas& operator= ( const ParamSigmas& ) = delete ;
			ParamSigmas& operator= ( ParamSigmas&& ) = delete ;

			std::string fNames[kMax];
			TF1 *fXsections[kMax];
	};

}

#endif
