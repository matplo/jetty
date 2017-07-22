#ifndef __PRESAMPLE_PYTHIA_HH
#define __PRESAMPLE_PYTHIA_HH

#include <string>
#include "util/pythia/crosssections.h"
#include "util/pythia/scanhard.h"

class PreSamplePythia
{
public:
	PreSamplePythia(const char *config);
	PreSamplePythia(const std::string &config);
	virtual ~PreSamplePythia();

	PyUtil::CrossSections* getXSections() {return _xsections;}
	PyUtil::CrossSections* getXsections() {return _xsections;}

	PyUtil::StatHardPythia* getStatHard() {return _stat_hard;}

	virtual void run();

	bool ready() {return _sampled;}

protected:

private:
	PreSamplePythia() { ; }
	PyUtil::CrossSections *_xsections;
	PyUtil::StatHardPythia *_stat_hard;
	std::string _config;
	bool _sampled;
};

#endif
