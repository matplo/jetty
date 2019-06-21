// ALEPH_DATA RUN = 14209 EVENT  4218 ECM =   91.276 GEV
//  Primary vertex info flag =  4 vx = -0.0855 vy =  0.0485 ex =  0.0057 ey =  0.0000
// px=    0.001 py=    0.150 pz=   -0.126 m=    0.140 charge -1.0 pwflag    0 d0   0.051 z0  -0.359 ntpc   4 nitc   7 nvdet   2
// px=    3.001 py=   -2.409 pz=   -2.398 m=    0.140 charge  1.0 pwflag    0 d0  -0.013 z0  -0.393 ntpc  21 nitc   5 nvdet   2
// px=    4.377 py=   -3.291 pz=   -3.063 m=    0.140 charge -1.0 pwflag    0 d0   0.000 z0  -0.398 ntpc  18 nitc   3 nvdet   2
// px=    3.602 py=   -2.626 pz=   -2.855 m=    0.140 charge  1.0 pwflag    0 d0   0.001 z0  -0.394 ntpc  20 nitc   3 nvdet   2
// px=   -3.368 py=    2.556 pz=    1.994 m=    0.140 charge -1.0 pwflag    0 d0  -0.047 z0  -0.408 ntpc  21 nitc   0 nvdet   2
// px=    0.978 py=   -0.599 pz=   -0.757 m=    0.140 charge -1.0 pwflag    0 d0   0.000 z0  -0.357 ntpc  20 nitc   5 nvdet   2
// px=   -0.897 py=    0.522 pz=    0.484 m=    0.140 charge -1.0 pwflag    0 d0   0.006 z0  -0.396 ntpc  19 nitc   7 nvdet   2
// px=   -5.595 py=    3.948 pz=    3.960 m=    0.140 charge  1.0 pwflag    0 d0   0.396 z0  -0.340 ntpc  12 nitc   2 nvdet   2
// px=    0.755 py=   -0.068 pz=   -0.025 m=    0.140 charge -1.0 pwflag    0 d0  -0.005 z0  -0.417 ntpc  19 nitc   6 nvdet   2
// px=    5.527 py=   -3.759 pz=   -3.514 m=    0.140 charge -1.0 pwflag    0 d0   0.000 z0  -0.399 ntpc  19 nitc   4 nvdet   2
// px=   -0.324 py=    0.067 pz=    0.457 m=    0.140 charge  1.0 pwflag    0 d0  -0.036 z0  -0.363 ntpc  13 nitc   8 nvdet   0
// px=    7.434 py=   -5.007 pz=   -5.154 m=    0.000 charge  0.0 pwflag    4 d0  -1.000 z0  -1.000 ntpc  -1 nitc  -1 nvdet  -1
// px=    0.831 py=   -0.580 pz=   -0.457 m=    0.000 charge  0.0 pwflag    4 d0  -1.000 z0  -1.000 ntpc  -1 nitc  -1 nvdet  -1
// px=   -0.658 py=    0.505 pz=    0.500 m=    0.000 charge  0.0 pwflag    4 d0  -1.000 z0  -1.000 ntpc  -1 nitc  -1 nvdet  -1
// px=   -2.131 py=    1.651 pz=    1.799 m=    0.025 charge  0.0 pwflag    4 d0  -1.000 z0  -1.000 ntpc  -1 nitc  -1 nvdet  -1
// px=   -2.852 py=    2.115 pz=    2.425 m=    0.000 charge  0.0 pwflag    4 d0  -1.000 z0  -1.000 ntpc  -1 nitc  -1 nvdet  -1
// px=   -7.184 py=    4.718 pz=    4.988 m=    0.000 charge  0.0 pwflag    4 d0  -1.000 z0  -1.000 ntpc  -1 nitc  -1 nvdet  -1
// px=   -0.602 py=    0.368 pz=    0.525 m=    0.000 charge  0.0 pwflag    4 d0  -1.000 z0  -1.000 ntpc  -1 nitc  -1 nvdet  -1
// px=    1.634 py=   -1.048 pz=   -1.371 m=    0.000 charge  0.0 pwflag    4 d0  -1.000 z0  -1.000 ntpc  -1 nitc  -1 nvdet  -1
// px=    0.353 py=   -0.387 pz=   -0.731 m=    0.000 charge  0.0 pwflag    4 d0  -1.000 z0  -1.000 ntpc  -1 nitc  -1 nvdet  -1
// px=   -8.746 py=    7.889 pz=    7.007 m=    0.052 charge  0.0 pwflag    5 d0  -1.000 z0  -1.000 ntpc  -1 nitc  -1 nvdet  -1
// px=    1.140 py=   -0.210 pz=   -0.254 m=    0.015 charge  0.0 pwflag    5 d0  -1.000 z0  -1.000 ntpc  -1 nitc  -1 nvdet  -1
// px=    0.350 py=   -0.384 pz=   -0.726 m=    0.000 charge  0.0 pwflag    5 d0  -1.000 z0  -1.000 ntpc  -1 nitc  -1 nvdet  -1
// END_EVENT

#include <TString.h>
#include <TFile.h>
#include <TTree.h>

#include <jetty/util/blog.h>
#include <jetty/util/args.h>
#include <jetty/util/strutil.h>
#include <jetty/util/rstream/tstream.h>

#include <string>
#include <fstream>

namespace Aleph
{
	class Particle
	{
	public:
		Particle();
		Particle(const Particle &p);
		Particle(double px, double py, double pz, double m, double q, int pwflag, double d0, double z0, int ntpc, int nitc, int nvdet);

		double 	get_px() 	{return fpx;}
		double 	get_py() 	{return fpy;}
		double 	get_pz() 	{return fpz;}
		double 	get_m() 	{return fm;}
		double 	get_q() 	{return fq;}
		int 	get_pwflag(){return fpwflag;}
		double 	get_d0() 	{return fd0;}
		double 	get_z0() 	{return fz0;}
		int 	get_ntpc() 	{return fntpc;}
		int 	get_nitc() 	{return fnitc;}
		int 	get_nvdet() {return fnvdet;}

		~Particle() {;}

		void dump() const;

	private:
		double 	fpx;
		double 	fpy;
		double 	fpz;
		double 	fm;
		double 	fq;
		int    	fpwflag;
		double 	fd0;
		double 	fz0;
		int 	fntpc;
		int 	fnitc;
		int 	fnvdet;
	};

	Particle::Particle()
			: fpx(0), fpy(0), fpz(0), fm(0), fq(0), fpwflag(0), fd0(0), fz0(0), fntpc(0), fnitc(0), fnvdet(0)
	{
		;
	}

	Particle::Particle(const Particle &p)
			: fpx(p.fpx), fpy(p.fpy), fpz(p.fpz), fm(p.fm), fq(p.fq), fpwflag(p.fpwflag), fd0(p.fd0), fz0(p.fz0), fntpc(p.fntpc), fnitc(p.fnitc), fnvdet(p.fnvdet)
	{
		;
	}


	Particle::Particle(double px, double py, double pz, double m, double q, int pwflag, double d0, double z0, int ntpc, int nitc, int nvdet)
			: fpx(px), fpy(py), fpz(pz), fm(m), fq(q), fpwflag(pwflag), fd0(d0), fz0(z0), fntpc(ntpc), fnitc(nitc), fnvdet(nvdet)
	{
		;
	}

	void Particle::dump() const
	{
		Linfo << fpx << " " << fpy << " " << fpz << " " << fm << " " << fq << " " << fpwflag << " " << fd0 << " " << fz0 << " " << fntpc << " " << fnitc << " " << fnvdet;
	}

	class Event
	{
	public:
		Event();
		Event(int run, int n, double e, int vflag, double vx, double vy, double ex, double ey);
		void reset(int run, int n, double e, int vflag, double vx, double vy, double ex, double ey);

		int 	get_run() 	{return frun;}
		int 	get_n() 	{return fn;}
		double 	get_e() 	{return fe;}
		int 	get_vflag() {return fvflag;}
		double  get_vx() 	{return fvx;}
		double  get_vy() 	{return fvy;}
		double  get_ex() 	{return fex;}
		double  get_ey() 	{return fey;}

		std::vector<Particle> get_particles() const {return fparticles;}
		void add_particle(const Particle &p);
		void add_particle(double px, double py, double pz, double m, double q, int pwflag, double d0, double z0, int ntpc, int nitc, int nvdet);

		void reset();
		void clear();
		void dump(bool noparts = false) const;

		~Event() {;}

	private:
		int 	frun;
		int 	fn;
		double 	fe;
		int 	fvflag;
		double  fvx;
		double  fvy;
		double  fex;
		double  fey;

		std::vector<Particle> fparticles;
	};

	Event::Event()
		: frun(0), fn(0), fe(0), fvflag(0), fvx(0), fvy(0), fex(0), fey(0), fparticles()
	{
		;
	}

	Event::Event(int run, int n, double e, int vflag, double vx, double vy, double ex, double ey)
		: frun(run), fn(n), fe(e), fvflag(vflag), fvx(vx), fvy(vy), fex(ex), fey(ey), fparticles()
	{
		;
	}

	void Event::dump(bool noparts) const
	{
		Linfo << "Event: " << frun << " " << fn << " " << fe << " " << fvflag << " " << fvx << " " << fvy << " " << fex << " " << fey;
		if (noparts == false)
		{
			for (unsigned int i = 0; i < fparticles.size(); i++)
				fparticles[i].dump();
			Linfo << "---";
		}
	}

	void Event::add_particle(const Particle &p)
	{
		fparticles.push_back(p);
	}

	void Event::add_particle(double px, double py, double pz, double m, double q, int pwflag, double d0, double z0, int ntpc, int nitc, int nvdet)
	{
		Particle p(px, py, pz, m, q, pwflag, d0, z0, ntpc, nitc, nvdet);
		fparticles.push_back(p);
	}

	void Event::clear()
	{
		reset(0, 0, 0, 0, 0, 0, 0, 0);
	}

	void Event::reset(int run, int n, double e, int vflag, double vx, double vy, double ex, double ey)
	{
		frun   = run;
		fn     = n;
		fe     = e;
		fvflag = vflag;
		fvx    = vx;
		fvy    = vy;
		fex    = ex;
		fey    = ey;
		fparticles.clear();
	}

	class Reader
	{
	public:
		Reader();
		Reader(const char *fname);
		void open(const char *fname);
		void reset();
		bool read_next_event();
		const Event& get_event() {return fEvent;}
		~Reader();
	private:
		std::string 	fName;
		std::ifstream 	fStream;
		Event 			fEvent;
	};

	Reader::Reader()
		: fName(), fStream(), fEvent()
	{
		;
	}

	Reader::~Reader()
	{
		fEvent.clear();
		fStream.close();
	}

	Reader::Reader(const char *fname)
		: fName(fname), fStream(), fEvent()
	{
		fStream.open(fName);
	}

	void Reader::reset()
	{
		fEvent.clear();
		fStream.close();
	}

	void Reader::open(const char *fname)
	{
		fStream.close();
		fName = fname;
		fStream.open(fName);
	}

	bool Reader::read_next_event()
	{
		if (!fStream.good())
			return false;

		std::string line;

		bool reading_parts = false;
		bool reading_event = false;

		int run    = -99;
		int event  = -99;
		double ecm = -99.0;
		int vflag  = -99;
		double vx  = -99.0;
		double vy  = -99.0;
		double ex  = -99.0;
		double ey  = -99.0;

		std::map<std::string, std::vector<double>> parts;

		while (std::getline(fStream, line))
		{
			if (line.find("ALEPH_DATA", 0) != std::string::npos)
			{
				Ldebug << line << " - start of the event found";
				if (reading_event)
				{
					Lerror << "something went wrong : new event before finishing the last one" << event;
				}
				reading_event = true;
				StrUtil::replace_substring(line, " ", "");
				StrUtil::replace_substring(line, "EVENT", " EVENT=");
				StrUtil::replace_substring(line, "ECM", " ECM");
				StrUtil::replace_substring(line, "GEV", "");

				SysUtil::Args _args(line);
				run   = _args.getI("ALEPH_DATARUN", 0);
				event = _args.getI("EVENT", 0);
				ecm   = _args.getD("ECM", 0.0);

				fEvent.clear();
			}
			else
			{
				if (reading_event)
				{
					if (line.find("Primary vertex info flag", 0) != std::string::npos)
					{
						StrUtil::replace_substring(line, "Primary vertex info flag", "vflag");
						StrUtil::replace_substring(line, " ", "");
						StrUtil::replace_substring(line, "vx", " vx");
						StrUtil::replace_substring(line, "vy", " vy");
						StrUtil::replace_substring(line, "ex", " ex");
						StrUtil::replace_substring(line, "ey", " ey");
						reading_parts = true;
						Ldebug << line;
						SysUtil::Args _args(line);
						vflag = _args.getI("vflag", -99);
						vx    = _args.getD("vx", -99.);
						vy    = _args.getD("vy", -99.);
						ex    = _args.getD("ex", -99.);
						ey    = _args.getD("ey", -99.);
						Ldebug << vflag << " " << vx << " " << vy << " " << ex << " " << ey;
						fEvent.reset(run, event, ecm, vflag, vx, vy, ex, ey);
					}
					else if (reading_parts)
					{
						if (line.find("px=", 0) == 0)
						{
							StrUtil::replace_substring(line, " ", "");
							std::vector<std::string> vars{"px=", "py=", "pz=", "m=", "charge", "pwflag", "d0", "z0", "ntpc", "nitc", "nvdet"};
							std::vector<std::string> tovars{"px=", " py=", " pz=", " m=", " charge=", " pwflag=", " d0=", " z0=", " ntpc=", " nitc=", " nvdet="};
							for ( unsigned int i = 0; i < vars.size(); i++)
							{
								StrUtil::replace_substring(line, vars[i].c_str(), tovars[i].c_str());
							}
							SysUtil::Args _args(line);
							double px     = _args.getD("px", 0);
							double py     = _args.getD("py", 0);
							double pz     = _args.getD("pz", 0);
							double m      = _args.getD("m", 0);
							double q      = _args.getD("charge", 0);
							   int pwflag = _args.getI("pwflag", 0);
							double d0     = _args.getD("d0", 0);
							double z0     = _args.getD("z0", 0);
							   int ntpc   = _args.getI("ntpc", 0);
							   int nitc   = _args.getI("nitc", 0);
							   int nvdet  = _args.getI("nvdet", 0);
							Particle p(px, py, pz, m, q, pwflag, d0, z0, ntpc, nitc, nvdet);
							fEvent.add_particle(p);
						}
						else
						{
							if (line.find("END_EVENT", 0) != std::string::npos)
							{
								// this is ok
								Ldebug << line << " - end of the event found - while reading particles";
							}
							else
							{
								Lerror << "while reading particles - unknown tag found";
								Lerror << " - line is: " << line;
							}
						}
					}
				}
			}
			// Linfo << line;
			if (line.find("END_EVENT", 0) != std::string::npos)
			{
				Ldebug << line << " - end of the event found";
				if (reading_event && reading_parts)
				{
					reading_event = false;
					reading_parts = false;
					return true;
				}
				else
				{
					Lerror << "Something awfully wrong with the event structure...";
					return false;
					break;
				}
			}
		}
		fStream.close();
		return false;
	}

	int dump(const char *fname, int nevents, bool noparts)
	{
		int n = 0;
		Linfo << "Dumping file: " << fname;
		Reader r(fname);
		while (r.read_next_event())
		{
			auto &ev = r.get_event();
			ev.dump(noparts);
			n++;
			if (nevents > 0)
				if (n >= nevents)
					break;
		}
		return 0;
	}

	int make_a_tree(const char *fname = "")
	{
		TString sfname = TString::Format("%s.root", fname);
		TFile fout(sfname, "recreate");
		fout.cd();
		TTree t("t" ,"aleph data");

		RStream::TStream tstr("e", &t);

		bool reading_parts = false;
		bool reading_event = false;

		std::ifstream infile(fname);
		std::string line;

		int run    = -99;
		int event  = -99;
		double ecm = -99.0;
		int vflag  = -99;
		double vx  = -99.0;
		double vy  = -99.0;
		double ex  = -99.0;
		double ey  = -99.0;

		std::map<std::string, std::vector<double>> parts;

		while (std::getline(infile, line))
		{
			if (line.find("ALEPH_DATA", 0) != std::string::npos)
			{
				Ldebug << line << " - start of the event found";
				if (reading_event)
				{
					Lerror << "something went wrong : new event before finishing the last one" << event;
				}
				reading_event = true;
				StrUtil::replace_substring(line, " ", "");
				StrUtil::replace_substring(line, "EVENT", " EVENT=");
				StrUtil::replace_substring(line, "ECM", " ECM");
				StrUtil::replace_substring(line, "GEV", "");

				SysUtil::Args _args(line);
				run   = _args.getI("ALEPH_DATARUN", 0);
				event = _args.getI("EVENT", 0);
				ecm   = _args.getD("ECM", 0.0);
			}
			else
			{
				if (reading_event)
				{
					if (line.find("Primary vertex info flag", 0) != std::string::npos)
					{
						StrUtil::replace_substring(line, "Primary vertex info flag", "vflag");
						StrUtil::replace_substring(line, " ", "");
						StrUtil::replace_substring(line, "vx", " vx");
						StrUtil::replace_substring(line, "vy", " vy");
						StrUtil::replace_substring(line, "ex", " ex");
						StrUtil::replace_substring(line, "ey", " ey");
						reading_parts = true;

						SysUtil::Args _args(line);
						vflag = _args.getI("vflag", -99);
						vx    = _args.getD("vx", -99.);
						vy    = _args.getD("vy", -99.);
						ex    = _args.getD("ex", -99.);
						ey    = _args.getD("ey", -99.);
					}
					else if (reading_parts)
					{
						StrUtil::replace_substring(line, " ", "");
						std::vector<std::string> vars{"px=", "py=", "pz=", "m=", "charge", "pwflag", "d0", "z0", "ntpc", "nitc", "nvdet"};
						std::vector<std::string> tovars{"px=", " py=", " pz=", " m=", " charge=", " pwflag=", " d0=", " z0=", " ntpc=", " nitc=", " nvdet="};
						for ( unsigned int i = 0; i < vars.size(); i++)
						{
							StrUtil::replace_substring(line, vars[i].c_str(), tovars[i].c_str());
						}
						SysUtil::Args _args(line);
						for ( unsigned int i = 0; i < vars.size(); i++)
						{
							std::string _svar = StrUtil::replace_substring_copy(vars[i], "=", "");
							double val = _args.getD(_svar.c_str(), 0.0);
							parts[_svar].push_back(val);
						}
					}
				}
			}
			// Linfo << line;
			if (line.find("END_EVENT", 0) != std::string::npos)
			{
				Ldebug << line << " - end of the event found";
				if (reading_event && reading_parts)
				{
					tstr << "run" 	<< run;
					tstr << "event" << event;
					tstr << "ecm" 	<< ecm;

					tstr << "vflag" << vflag;

					std::vector<std::string> pvars{"px", "py", "pz", "m", "charge", "pwflag", "d0", "z0", "ntpc", "nitc", "nvdet"};
					for ( auto & v : pvars)
					{
						tstr << v.c_str() << parts[v];
					}

					tstr << std::endl;

					reading_event = false;
					reading_parts = false;

					run   = -99;
					event = -99;
					ecm   = -99.0;
					vflag = -99;
					vx    = -99.0;
					vy    = -99.0;
					ex    = -99.0;
					ey    = -99.0;

					parts.clear();

				}
				else
				{
					Lerror << "Something awfully wrong with the event structure...";
					break;
				}
			}
		}

		fout.Write();
		fout.Close();

		return 0;
	};
};
