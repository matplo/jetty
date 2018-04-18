#include <jetty/util/hepmc/readertask.h>
#include <jetty/util/hepmc/readfile.h>
#include <jetty/util/blog.h>
#include <jetty/util/strutil.h>
#include <jetty/util/sysutil.h>

#include <cstdio>
#include <string>

namespace GenUtil
{
	HepMCReaderTask::~HepMCReaderTask()
	{
		Ltrace << "destructor " << GetName();
	}

	unsigned int HepMCReaderTask::ExecThis(const char *opt)
	{
		Ltrace << "HepMCReaderTask::ExecThis " << GetName() << " with option: " << opt;
		if (fReader->NextEvent() == false)
		{
			if (fFileList.empty())
			{
				return kDefinedStop;
			}
			else
			{
				fData->remove(fReader);
				// delete fReader;
				std::string _stmp = fFileList.back();
				Lwarn <<  "HepMCReaderTask::ExecThis " << GetName() << " file : " << _stmp;
				fReader = new ReadHepMCFile(_stmp.c_str());
				fFileList.pop_back();
				if (fReader)
				{
					fData->add(fReader);
					return ExecThis(opt);
				}
				else
				{
					Lfatal << "HepMCReaderTask::Init " << GetName() << " unable to create the hepmc file reader.";
					return kError;
				}
			}
		}
		return kGood;
	}

	unsigned int HepMCReaderTask::InitThis(const char *opt)
	{
		unsigned int status = kError;
		fArgs.merge(opt);
		Linfo << "HepMCReaderTask::Init " << GetName() << " with opts: " << fArgs.asString();
		Linfo << "HepMCReaderTask::Init " << GetName() << " fStatus: " << fStatus;
		if (fStatus == kBeforeInit)
		{
			// check if this is a directory
			std::string _hepmc_input = fArgs.get("--hepmc-input");
			std::string _hepmc_input_fpattern = fArgs.get("--hepmc-input-pattern");
			if (_hepmc_input.size() > 0)
			{
				if (SysUtil::is_file(_hepmc_input))
				{
					Linfo << "HepMCReaderTask::Init " << GetName() << " input file : " << _hepmc_input;
					fFileList.push_back(_hepmc_input);
				}
				else
				{
					// assume it is a directory...
					if (SysUtil::is_directory(_hepmc_input))
					{
						if (_hepmc_input_fpattern.size() > 0)
						{
							fFileList = SysUtil::find_files_exact(_hepmc_input.c_str(), _hepmc_input_fpattern.c_str());
							Linfo << "HepMCReaderTask::Init " << GetName() << " found " << fFileList.size() << " files in " << _hepmc_input << " with pattern " << _hepmc_input_fpattern;
						}
						else
						{
							fFileList = SysUtil::find_files_ext(_hepmc_input.c_str(), ".hepmc");
							Linfo << "HepMCReaderTask::Init " << GetName() << " found " << fFileList.size() << " files in " << _hepmc_input << " with extention .hepmc";
						}
					}
				}
			}

			if (fFileList.empty())
			{
				Lfatal << "HepMCReaderTask::Init " << GetName() << " no files to read from....";
				return kError;
			}
			else
			{
				std::string _stmp = fFileList.back();
				Lwarn <<  "HepMCReaderTask::InitThis " << GetName() << " file : " << _stmp;
				fReader = new ReadHepMCFile(_stmp.c_str());
				fFileList.pop_back();
			}

			if (!fReader)
			{
				Lfatal << "HepMCReaderTask::Init " << GetName() << " unable to create the hepmc file reader.";
				return kError;
			}

			fData->add(fReader);
			status = kGood;
		}
		Linfo << "HepMCReaderTask::Init " << GetName() << " reader at: " << fReader;
		return status;
	}
}
