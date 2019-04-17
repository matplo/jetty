#include <jetty/util/pythia/presample.h>
#include <jetty/util/pythia/pythia_wrapper.h>
#include <jetty/util/pythia/pyargs.h>

#include <jetty/util/blog.h>
#include <jetty/util/looputil.h>

#include <boost/filesystem/path.hpp>
// #include <boost/progress.hpp>


PreSamplePythia::PreSamplePythia(const char *config)
	: _xsections(0)
	, _stat_hard(0)
	, _config(config)
	, _sampled(false)
{
	;
}

PreSamplePythia::PreSamplePythia(const std::string &config)
	: _xsections(0)
	, _stat_hard(0)
	, _config(config)
	, _sampled(false)
{
	;
}

PreSamplePythia::~PreSamplePythia()
{
	delete _xsections;
	delete _stat_hard;
}

void PreSamplePythia::run()
{
	PyUtil::Args args(_config);
	if (args.isSet("--invalid"))
	{
		Linfo << "invalid parameters. stop here.";
		return;
	}
	Linfo << args.asString("[pythia_run_wrapper:status]");
	if (args.isSet("--dry")) return;

	boost::filesystem::path sout = "presample.root";
	if (args.isSet("--out"))
	{
		boost::filesystem::path p(args.get("--out"));
		auto dname = p.root_directory();
		Ldebug << "filename and extension : " << p.filename();
		Ldebug << "filename only          : " << p.stem();
		Ldebug << "dname only             : " << dname;
		sout = dname;
		sout += boost::filesystem::path("presample_");
		sout += p.stem();
		sout += ".root";
	}

	args.set("--out", sout.c_str());
	args.add("--overwrite");

	PyUtil::PythiaWrapper pywrap(args.asString());
	if (pywrap.initialized() == false)
	{
		Lwarn << "Pythia not initialized. Stop here.";
		return; // this is a normal termination
	}
	PyUtil::Args &pyargs      = *pywrap.args();
	Pythia8::Pythia &pythia = *pywrap.pythia();

	_stat_hard = new PyUtil::StatHardPythia();

	// this is where the event loop section starts
	auto nEv = pyargs.getI("--presample", 0);
	if (nEv == 0) nEv = 1000;
	//boost::progress_display show_progress(nEv);
	LoopUtil::TPbar pbar(nEv);
	for (int iE = 0; iE < nEv; iE++)
	{
		pbar.Update();
		//++show_progress;
		if (pywrap.next() == false) continue;
		_stat_hard->add_event(pythia);
	}
	pythia.stat();
	Linfo << "Generation done.";
	_xsections = new PyUtil::CrossSections(pythia);
	Linfo << "[i] Xsections saved.";
	_sampled = true;
}
