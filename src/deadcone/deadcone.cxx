#include <jetty/deadcone/deadcone.h>
#include <jetty/util/pythia/pyargs.h>
#include <jetty/util/looputil.h>
#include <jetty/util/blog.h>

#include <TFile.h>
#include <TTree.h>
#include <TNtuple.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TMath.h>

#include <Pythia8/Pythia.h>

#include <fastjet/PseudoJet.hh>
#include <fastjet/ClusterSequence.hh>
namespace fj = fastjet;

#include <string>
#include <iostream>
using namespace std;

int deadcone (const std::string &s)
{
	Linfo << "This is stand alone deadcone...";

	PyUtil::Args args(s);

	// analysis parameters
	double max_part_eta = args.getD("--maxeta", 3.5);
	double jetR = args.getD("--R", 0.4);
	double max_jet_eta = args.getD("--max-jet-eta", max_part_eta - jetR - 0.1);
	double min_jet_pt = args.getD("--min-jet-pt", 10.);
	string outfname = args.get("--output", "deadcone.root");
	Linfo << "pythia args:" << args.asString();

	// intialize PYTHIA
	Pythia8::Pythia pythia;
	auto pairs = args.pairs();
	for (unsigned int i = 0; i < pairs.size(); i++)
	{
		if (pairs[i].second.size() < 1) continue;
		Linfo << "[init pythia] paired arg: #" << i << " " << pairs[i].first << " " << pairs[i].second;
		string spypar = pairs[i].first + " = " + pairs[i].second;
		pythia.readString(spypar.c_str());
	}
	bool _is_initialized = pythia.init();
	if (_is_initialized == false)
	{
		Lerror << "Unable to initialize pythia. Stop.";
		return -1;
	}

	//initialize output
	TFile fout(outfname.c_str(), "recreate");
	if (!fout.IsOpen())
	{
		Lerror << "unable to open output: " << fout.GetName();
		return -1;
	}
	fout.cd();
	TNtuple *tne = new TNtuple("tne", "tne", "n:pid:xsec");
	TNtuple *tnj = new TNtuple("tnj", "tnj", "pt:e:eta:phi:lpid:nsplits");
	TNtuple *tnd = new TNtuple("tnd", "tnd", "pt:e:eta:phi:lpid:lund_dR:lund_pt:lund_e:lund_pt1:lund_pt2:lund_lpid:nsplits");

	int nEv = args.getI("--nev", -1);
	LoopUtil::TPbar pbar(nEv);
	for (int i = 0; i < nEv; i++)
	{
		pbar.Update();
		if (pythia.next())
		{
			tne->Fill(i, pythia.info.code(), pythia.info.sigmaGen());

			// analyze the event
			std::vector<fj::PseudoJet> parts;
			for (unsigned int ip = 0; ip < pythia.event.size(); ip++)
			{
				if (pythia.event[ip].isFinal() && TMath::Abs(pythia.event[ip].eta()) < max_part_eta)
				{
					fj::PseudoJet pfj;
					pfj.reset_momentum(pythia.event[ip].px(), pythia.event[ip].py(), pythia.event[ip].pz(), pythia.event[ip].e());
					pfj.set_user_index(ip);
					parts.push_back(pfj);
				}
			}
			Ldebug << "number of particles - final : " << parts.size();

			// jet finding and lund
			fj::JetDefinition jet_def(fj::antikt_algorithm, jetR);
			fj::ClusterSequence ca(parts, jet_def);
			auto jets = ca.inclusive_jets(min_jet_pt);
			for (const auto & jakt: jets)
			{
				if (TMath::Abs(jakt.eta()) > max_jet_eta) continue;
				int lidx = fj::SelectorNHardest(1)(jakt.constituents())[0].user_index();
				int lpid = pythia.event[lidx].id();
				int nsplits = 0;
				fj::JetDefinition decl_jet_def(fj::cambridge_algorithm, 1.0);
				fj::ClusterSequence decl_ca(jakt.constituents(), decl_jet_def);
				for (const auto & j : decl_ca.inclusive_jets(0))
				{
					fj::PseudoJet jj, j1, j2;
					jj = j;
					while (jj.has_parents(j1,j2))
					{
						nsplits++;
						// make sure j1 is always harder branch
						if (j1.pt2() < j2.pt2()) swap(j1,j2);
						// collect info and fill in the histogram
						double delta_R = j1.delta_R(j2);
						int _lidx = fj::SelectorNHardest(1)(jj.constituents())[0].user_index();
						int _lpid = pythia.event[_lidx].id();
						tnd->Fill(jakt.perp(), jakt.e(), jakt.eta(), jakt.phi(), lpid,
						         delta_R, jj.perp(), jj.e(), j1.perp(), j2.perp(), _lpid, nsplits);
						// // double delta_R_norm = delta_R / jet_def.R();
						// double delta_R_norm = delta_R; // MP
						// double z = j2.pt()/(j1.pt() + j2.pt());
						// double y = log(1.0 / delta_R_norm);
						// // there is an ambiguity here: can use z or j2.pt() / j.pt()
						// double lnpt_rel = log(z * delta_R_norm);
						// double lnpt_abs = log(j2.pt()/j.pt() * delta_R_norm);
						// // hists_2d["lund-zrel"].add_entry(y, lnpt_rel, evwgt);
						// // hists_2d["lund-zabs"].add_entry(y, lnpt_abs, evwgt);
						// double lnpt = log(j2.pt() * delta_R);
						// // hists_2d["lund-lnpt"].add_entry(y, lnpt, evwgt);
						// follow harder branch
						jj = j1;
					}
				}
				tnj->Fill(jakt.perp(), jakt.e(), jakt.eta(), jakt.phi(), lpid, nsplits);
			}
		}
		else
		{
			Lerror << "Pythia next failed. Stop.";
			return -1;
		}
	}
	fout.Write();
	fout.Close();
	Linfo << "written: " << fout.GetName();
	Linfo << "... deadcone done." << endl;
	return 0;
}
