#include <jetty/examples/et.h>

#include <jetty/util/pythia/pyargs.h>
#include <jetty/util/pythia/pyutil.h>
#include <jetty/util/pythia/outkinematics.h>
#include <jetty/util/pythia/crosssections.h>
#include <jetty/util/looputil.h>

#include <Pythia8/Pythia.h>

#include <TFile.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TMath.h>
#include <TNtuple.h>
#include <TProfile.h>

#include <string>
#include <iostream>

using namespace std;

int et (const std::string &s)
{
		// test(s); return;
		PyUtil::Args args(s);
		cout << args.asString("[pythia_run_wrapper:status]") << endl;
		if (args.isSet("--dry")) return 0;

		// create the output root file
		string outfname = args.get("--output");
		if (outfname.size() < 1)
		{
			outfname = "default_output.root";
		}
		TFile *fout = TFile::Open(outfname.c_str(), "RECREATE");
		fout->cd();
		TH1F *hpt = new TH1F("hpt", "pt;p_{T} (GeV/#it{c});counts", 100, 0, 100);

		TH1F *het = new TH1F("het", "het;E_{T} (GeV);counts", 100, 0, 100.);
		TH1F *hetmean = new TH1F("hetmean", "hetmean;E_{T}/2 (GeV);counts", 100, 0, 100.);
		TH1F *het_diff = new TH1F("het_diff", "het_diff;#Delta E_{T} (GeV);counts", 100, -100, 100.);

		TH2F *hetmean2d = new TH2F("hetmean2d", "hetmean2d;#sqrt{s};E_{T}/2 (GeV);counts", 100, 0, 12000., 100, 0, 100.);
		TH2F *hptmean2d = new TH2F("hptmean2d", "hptmean2d;#sqrt{s};p_{z}^{loss}/2 (GeV);counts", 100, 0, 12000., 100, 0, 100.);

		TNtuple *tn = new TNtuple("eloss", "eloss", "ecm:p0:p1:et:pid:pTHat:x1:x2");

		TProfile *p0 = new TProfile("p0", "p0", 12000, 0, 12000);
		TProfile *p1 = new TProfile("p1", "p1", 12000, 0, 12000);
		TProfile *et = new TProfile("et", "et", 12000, 0, 12000);

		TProfile *pmom[10];
		TProfile *pet[10];
		TProfile *petf[10];
		TString sname, stitle;
		for (int i = 100; i < 109; i++)
		{
			int profi = i - 100;
			sname = TString::Format("prof_pmom_%d", i);
			stitle = TString::Format("prof_pmom_%d", i);
			pmom[profi] = new TProfile(sname.Data(), stitle.Data(), 12000, 0, 12000);
			sname = TString::Format("prof_et_%d", i);
			stitle = TString::Format("prof_et_%d", i);
			pet[profi] = new TProfile(sname.Data(), stitle.Data(), 12000, 0, 12000);
			sname = TString::Format("prof_etf_%d", i);
			stitle = TString::Format("prof_etf_%d", i);
			petf[profi] = new TProfile(sname.Data(), stitle.Data(), 12000, 0, 12000);
		}

		// initialize pythia with a config and command line args
		Pythia8::Pythia *ppythia = PyUtil::make_pythia(args.asString());
		Pythia8::Pythia &pythia  = *ppythia;
		auto &event              = pythia.event;

		double ecm = args.getD("Beams:eCM");
		if (ecm<=0)
		{
			cout << "[e] ecm is " << ecm << " !?" << endl;
			return 1;
		}

		bool includeHard = args.isSet("--include-hard");

		// this is where the event loop section starts
		auto nEv = args.getI("Main:numberOfEvents");
		LoopUtil::TPbar pbar(nEv);
		for (int iE = 0; iE < nEv; iE++)
		{
			pbar.Update();
			if (pythia.next() == false) continue;
			double total_et = 0.0;
			// reject the hard subprocesses?
			double pTHat = -1;
			double x1 = -1;
			double x2 = -1;
			if (pythia.info.hasSub())
			{
				//cout << "event process: " << pythia.info.code()
				//	<< " has a hard subprocess: " << pythia.info.codeSub()
				//	<< " with pTHat = " << pythia.info.pTHat()
				//	<< endl;
				// total_et = total_et - 2. * pythia.info.pTHat();
				pTHat = pythia.info.pTHat();
				x1 = pythia.info.x1pdf();
				x2 = pythia.info.x2pdf();
				// the following does not give a good sqrt(s) behavior
				// if (pythia.info.pTHat() > 2)
				// 	continue;
			}
			else
			{
				//cout << "nothing hard in: " << pythia.info.code() << endl;
			}

			// loop over particles in the event
			for (int ip = 0; ip < event.size(); ip++)
			{
				if (event[ip].isFinal())
				{
					if (TMath::Abs(event[ip].eta()) < 1.)
						hpt->Fill(event[ip].pT(), 1./event[ip].pT());
					total_et += event[ip].eT();
				}
			}

			PyUtil::OutKinematics outk(pythia, includeHard);

			if (pythia.info.hasSub() && 0)
			{
				// PyUtil::PrintEventInfo(pythia);
				PyUtil::PrintParticle(event[3]);
				PyUtil::PrintParticle(event[4]);
				PyUtil::PrintParticle(event[5]);
				PyUtil::PrintParticle(event[6]);
				cout << "x1=" << pythia.info.x1pdf() << " ? " << event[3].pz() / event[1].pz() << endl;
				cout << "x2=" << pythia.info.x2pdf() << " ? " << event[4].pz() / event[2].pz() << endl;
				cout << "Et/2=" << total_et/2. << endl;
				cout << "<N_delta_p>=" << (outk.d_p_z[0] + outk.d_p_z[1])/2. << endl;
			}

			double mean_p_loss_perN = (outk.d_p_z[0] + outk.d_p_z[1])/2.;
			double mean_eT_perN = total_et/2.;
			double delta_means = mean_p_loss_perN - mean_eT_perN;
			//cout << "[i] mean_p_loss_perN = " << mean_p_loss_perN << " "
			//    << " tot_eT_perN = " << mean_eT_perN
			//    << " delta_means = " << delta_means
			//    << endl;
			het->Fill(total_et);
			hetmean->Fill(total_et/2.);
			het_diff->Fill(delta_means);

			hetmean2d->Fill(ecm, total_et/2.);
			hptmean2d->Fill(ecm, mean_p_loss_perN);

			int proc_id = pythia.info.code();
			tn->Fill(ecm, outk.d_p_z[0], outk.d_p_z[1], total_et/2., proc_id, pTHat, x1, x2);
			if (proc_id >= 101 && proc_id < 107)
			{
				pmom[0]->Fill(ecm, mean_p_loss_perN);
				pet[0]->Fill(ecm, mean_eT_perN);
				petf[0]->Fill(ecm, mean_eT_perN/ecm);

				pmom[proc_id - 100]->Fill(ecm, mean_p_loss_perN);
				pet[proc_id - 100]->Fill(ecm, mean_eT_perN);
				petf[proc_id - 100]->Fill(ecm, mean_eT_perN/ecm);

				// w/o non-diffractive
				if (proc_id > 101)
				{
					pmom[7]->Fill(ecm, mean_p_loss_perN);
					pet[7]->Fill(ecm, mean_eT_perN);
					petf[7]->Fill(ecm, mean_eT_perN/ecm);
				}

				// w/o non-diffractive and w/o elastic
				if (proc_id > 101 && proc_id > 102)
				{
					pmom[8]->Fill(ecm, mean_p_loss_perN);
					pet[8]->Fill(ecm, mean_eT_perN);
					petf[8]->Fill(ecm, mean_eT_perN/ecm);
				}
			}
			else
			{
				cout << "[w] this is designed for minbias - unknown process id occured : " << proc_id << endl;
			}
			p0->Fill(ecm, outk.d_p_z[0]);
			p1->Fill(ecm, outk.d_p_z[1]);
			et->Fill(ecm, mean_eT_perN);
		}
		pythia.stat();
		cout << "[i] Generation done." << endl;

		// remember to properly save/update and close the output file
		fout->Write();
		fout->Close();
		delete fout;

		string xsec_outfname = outfname + ".txt";
		PyUtil::CrossSections(pythia, xsec_outfname.c_str());

		// delete the pythia
		delete ppythia;
		return 0;
}
