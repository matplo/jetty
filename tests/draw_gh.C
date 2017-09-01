void draw_gh()
{
	TFile *f = TFile::Open("gh_eA_2500_eB_2500_eCM_5000GeV_pTHatMin_12.0_pTHatMax_-1.0_nEv_100000.root");
	TNtuple *gn = (TNtuple*)f->Get("gntuple");
	TNtuple *ghn = (TNtuple*)f->Get("ghntuple");

	TFile *fout = TFile::Open("gh_histograms.root", "recreate");

	TCanvas *tc = new TCanvas("photons", "photons");
	tc->Divide(2,3);
	tc->cd(1);
	TH1F *hgpT = new TH1F("hgpT", "#gamma prompt |#eta| < 0.7;#gamma p_{T} (GeV/c);counts", 5, 5, 55);
	gn->Draw("gpT>>hgpT", "TMath::Abs(geta) < 0.7", "e");
	hgpT->Sumw2();

	tc->cd(2);
	TH1F *hgpTw = new TH1F("hgpTw", "#gamma prompt |#eta| < 0.7;#gamma p_{T} (GeV/c);#sigma (mb)", 5, 5, 55);
	gn->Draw("gpT>>hgpTw", "TMath::Abs(geta) < 0.7", "e");
	hgpTw->Sumw2();
	hgpTw->Scale(0.00028217 / 100000.);

	Int_t ib = hgpT->FindBin(20.);
	Stat_t number_of_photons_roi = hgpT->GetBinContent(ib);
	cout << "number of photons in the region of interest:" << number_of_photons_roi << endl;
	tc->cd(3);
	TH1F *hppT = new TH1F("hppT", "charged particles in events with #gamma w/ 15<p_{T}<25;particle p_{T} (GeV/c);counts per photon", 32, 0, 32);
	ghn->Draw("hpT>>hppT", "gpT>15 && gpT<25 && TMath::Abs(geta) < 0.7 && TMath::Abs(heta) < 1.0", "e");
	hppT->Sumw2();
	hppT->Scale(1./number_of_photons_roi);
	TH1F *hppTsignal = new TH1F("hppTsignal", "charged particles w/ #Delta #varphi > #pi/2;particle p_{T} (GeV/c);counts per photon", 32, 0, 32);
	ghn->Draw("hpT>>hppTsignal", "gpT>15 && gpT<25 && TMath::Abs(geta) < 0.7 && TMath::Abs(heta) < 1.0 && dphi > (TMath::Pi() / 2.)", "e");
	hppTsignal->Sumw2();
	hppTsignal->Scale(1./number_of_photons_roi);

	tc->cd(4);
	TH1F *hppTcoarse = new TH1F("hppTcoarse", "charged particles in events with #gamma w/ 15<p_{T}<25;particle p_{T} (GeV/c);counts per photon", 8, 0, 32);
	ghn->Draw("hpT>>hppTcoarse", "gpT>15 && gpT<25 && TMath::Abs(geta) < 0.7 && TMath::Abs(heta) < 1.0", "e");
	hppTcoarse->Sumw2();
	hppTcoarse->Scale(1./number_of_photons_roi);
	TH1F *hppTsignalcoarse = new TH1F("hppTsignalcoarse", "charged particles w/ #Delta #varphi > #pi/2;particle p_{T} (GeV/c);counts per photon", 8, 0, 32);
	ghn->Draw("hpT>>hppTsignalcoarse", "gpT>15 && gpT<25 && TMath::Abs(geta) < 0.7 && TMath::Abs(heta) < 1.0 && dphi > (TMath::Pi() / 2.)", "e");
	hppTsignalcoarse->Sumw2();
	hppTsignalcoarse->Scale(1./number_of_photons_roi);

	tc->cd(5);
	TH1F *hpdphi1 = new TH1F("hpdphi1", "#Delta #varphi for #gamma [p_{T} 15-25 GeV] & p_{T}^{hadron} > 1.0 GeV;#Delta #varphi;counts per photon", 8, 0, TMath::Pi());
	ghn->Draw("dphi>>hpdphi1", "gpT>15 && gpT<25 && TMath::Abs(geta) < 0.7 && TMath::Abs(heta) < 1.0 && hpT > 1.0", "e");
	hpdphi1->Sumw2();
	hpdphi1->Scale(1./number_of_photons_roi);

	TH1F *hpdphi5 = new TH1F("hpdphi5", "#Delta #varphi for #gamma [p_{T} 15-25 GeV] & p_{T}^{hadron} > 5.0 GeV;#Delta #varphi;counts per photon", 8, 0, TMath::Pi());
	ghn->Draw("dphi>>hpdphi5", "gpT>15 && gpT<25 && TMath::Abs(geta) < 0.7 && TMath::Abs(heta) < 1.0 && hpT > 5.0", "e");
	hpdphi5->Sumw2();
	hpdphi5->Scale(1./number_of_photons_roi);

	fout->Write();
	fout->Close();
	delete fout;
}
