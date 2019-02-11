#ifndef __JETTY_UTIL_GENTASKOUTPUT_HH
#define __JETTY_UTIL_GENTASKOUTPUT_HH

class TList;
class TFile;
class TObject;

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

			TFile 	*GetOutput(const char *fname);
			TObject *GetOutputObject(const char *name);
			TObject *RegisterOutputObject(TObject *o);

		private:
			GenTaskOutput(); // private construct
			GenTaskOutput(const GenTaskOutput&) = delete; // non-copyable
			GenTaskOutput(GenTaskOutput&&) = delete; // and non-movable
			// as there is only one object, assignment would always be assign to self
			GenTaskOutput& operator= ( const GenTaskOutput& ) = delete ;
			GenTaskOutput& operator= ( GenTaskOutput&& ) = delete ;

			TList* 					fListOfFiles;
			TList* 					fListOfObjects;
	};
}

#endif
