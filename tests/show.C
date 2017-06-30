{
	TProfile *p0 = (TProfile*)(TFile::Open("et_sum.root")->Get("p0"));
	TProfile *p1 = (TProfile*)(TFile::Open("et_sum.root")->Get("p1"));
	TProfile *pe = (TProfile*)(TFile::Open("et_sum.root")->Get("et"));

	TCanvas *tc = new TCanvas("tc", "tc");

	p0->Draw();
	//TF1 *fitp0 = new TF1("fitp0", "[0] + [1] * x[0]", 10, 10000);
	TF1 *fitp0 = new TF1("fitp0", "[0] + [1] * TMath::Power(x[0], [2])", 10, 10000);
	fitp0->SetParameter(0, 1);
	fitp0->SetParameter(1, 0.1);
	fitp0->SetParameter(2, 0.1);
	p0->Fit(fitp0, "RMN");
	fitp0->Draw("same");

	p1->Draw("same");
	TF1 *fitp1 = new TF1("fitp1", "[0] + [1] * x[0]", 100, 10000);
	fitp1->SetParameter(0, 1);
	fitp1->SetParameter(1, 0.1);
	p1->Fit(fitp1, "RMN");
	fitp1->Draw("same");
	fitp1->SetRange(10,10000);

	pe->Draw("same");
	TF1 *fitpe = new TF1("fitpe", "[0] + [1] * TMath::Power(x[0], [2])", 10, 10000);
	fitpe->SetParameter(0, 1);
	fitpe->SetParameter(1, 0.1);
	fitpe->SetParameter(2, 0.1);
	pe->Fit(fitpe, "RMN");
	fitpe->Draw("same");

	TF1 *fitpe2 = new TF1("fitpe2", "[0] + [1] * x[0] + [2] * x[0] * x[0]", 10, 10000);
	fitpe2->SetParameter(0, 1);
	fitpe2->SetParameter(1, 0.1);
	fitpe2->SetParameter(2, 0.1);
	pe->Fit(fitpe2, "RMN");
	fitpe2->Draw("same");

	gPad->SetLogx();

	TF1 *ref = new TF1("ref", "[0] + [1] * x[0]", 100, 10000);
	ref->SetParameter(0, +1.4);
	ref->SetParameter(1, +0.0025);

	TFile *fout = new TFile("fits.root", "recreate");
	fout->cd();
	p0->Write();
	p1->Write();
	pe->Write();
	fitp0->Write();
	fitp1->Write();
	fitpe->Write();
	fitpe2->Write();
	ref->Write();
	fout->Close();
}
