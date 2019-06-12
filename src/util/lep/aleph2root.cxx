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

#include <string>
#include <fstream>

namespace Aleph
{

	void make_a_tree(const char *fname = "")
	{
		TString sfname = TString::Format("%s.root", fname);
		TFile fout(sfname, "recreate");
		fout.cd();
		TTree t("t");

		std::ifstream infile(fname);
		std::string line;
		while (std::getline(infile, line))
		{
			Ldebug << line;
		}

		fout.Write();
		fout.Close();
	};
};
