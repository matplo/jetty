#ifndef __JETTY_UTIL_GENTASKOUTPUT_HH
#define __JETTY_UTIL_GENTASKOUTPUT_HH

class TList;
class TFile;
class TObject;
class Wrapper;

namespace GenUtil
{
	class GenTaskOutput
	{
		public:
			static GenTaskOutput& Instance()
			{
				static GenTaskOutput _instance; // Guaranteed to be destroyed. // Instantiated on first use.
				return _instance;
			}

			virtual ~GenTaskOutput();

			Wrapper *GetWrapper() {return fWrapper;}

			TFile *GetOutput(const char *fname);

		private:
			GenTaskOutput(); // private construct
			GenTaskOutput(const GenTaskOutput&) = delete; // non-copyable
			GenTaskOutput(GenTaskOutput&&) = delete; // and non-movable
			// as there is only one object, assignment would always be assign to self
			GenTaskOutput& operator= ( const GenTaskOutput& ) = delete ;
			GenTaskOutput& operator= ( GenTaskOutput&& ) = delete ;

			Wrapper *fWrapper;
	};
}

#endif
