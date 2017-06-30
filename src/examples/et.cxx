#include "et.h"

#include <util/pyargs.h>
#include <util/pyutil.h>
#include <util/looputil.h>
#include <util/outkinematics.h>

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

        TNtuple *tn = new TNtuple("eloss", "eloss", "ecm/F:p0/F:p1/F:et/F");

        TProfile *p0 = new TProfile("p0", "p0", 12000, 0, 12000);
        TProfile *p1 = new TProfile("p1", "p1", 12000, 0, 12000);
        TProfile *et = new TProfile("et", "et", 12000, 0, 12000);

        // initialize pythia with a config and command line args
		Pythia8::Pythia *ppythia = PyUtil::make_pythia(args.asString());
		Pythia8::Pythia &pythia  = *ppythia;
		auto &event              = pythia.event;

        double ecm = args.getD("Beams:eCM");
		// this is where the event loop section starts
        auto nEv = args.getI("Main:numberOfEvents");
        LoopUtil::TPbar pbar(nEv);
        for (unsigned int iE = 0; iE < nEv; iE++)
        {
        	pbar.Update();
            if (pythia.next() == false) continue;
            double total_et = 0.0;
            // loop over particles in the event
            for (unsigned int ip = 0; ip < event.size(); ip++)
            {
                if (event[ip].isFinal())
                {
                    if (TMath::Abs(event[ip].eta()) < 1.)
                    	hpt->Fill(event[ip].pT(), 1./event[ip].pT());
                    total_et += event[ip].eT();
                }
            }
            PyUtil::OutKinematics outk(event);
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

            tn->Fill(ecm, outk.d_p_z[0], outk.d_p_z[1], total_et/2.);
            p0->Fill(ecm, outk.d_p_z[0]);
            p1->Fill(ecm, outk.d_p_z[1]);
            et->Fill(ecm, total_et/2.);
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
