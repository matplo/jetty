#include <jetty/util/tasks/multiplicitytask.h>
#include <jetty/util/tglaubermc/tglaubermc.h>
#include <jetty/util/pythia/event_pool.h>
#include <jetty/util/rstream/tstream.h>

#include <jetty/util/blog.h>
#include <algorithm>
#include <type_traits>

#include <TH1F.h>
#include <TFile.h>
#include <TParticle.h>
#include <TParticlePDG.h>

namespace GenUtil
{
	MultiplicityTask::~MultiplicityTask()
	{
		Ltrace << "destructor " << GetName();
	}

	unsigned int MultiplicityTask::InitThis(const char *opt)
	{
		fArgs.merge(opt);
		string estimName = GetName();
		estimName += "_ME";
		fMult = new MultiplicityEstimator(estimName.c_str());
		fData->add(fMult); // no need to destroy

		fStoreParticles = fArgs.isSet("--store-particles");

		fOutputFile = new TFile(fOutputPath.c_str(), "recreate");
		fOutputTree = new TTree("tAA", "tAA");
		fNNTree     = new TTree("tNN", "tNN");
		return kGood;
	}

	unsigned int MultiplicityTask::ExecThis(const char * /*opt*/)
	{
		int npart = 2;
		Ldebug << "fpGlauberMC: " << fpGlauberMC;
		if (fpGlauberMC)
		{
			npart = fpGlauberMC->GetNpart();
		}
		for (auto &t : fInputTasks)
		{
			auto evpool = t->GetData()->get<PyUtil::EventPool>();
			if (!evpool) continue;
			fMult->AddEvent(evpool->GetFinalParticles(), 1./(npart/2.));
			// write a tree for each collision
			// and
			// write a tree for each event - AA
			if (fStoreParticles)
			{
				auto pyevents = evpool->GetPool();
				Ldebug << "number of pythia events: " << pyevents.size();
				std::vector<Pythia8::Particle> AAEventParticles;
				auto AAstream = RStream::TStream("ev", fOutputTree);
				auto NNstream = RStream::TStream("ev", fNNTree);
				for (unsigned int ie = 0; ie < pyevents.size(); ie++)
				{
					Ldebug << " - pythia event : " << ie << "number of pythia particles: " << pyevents[ie].size();
					std::vector<Pythia8::Particle> ppEventParticles;
					for (int ip = 0; ip < pyevents[ie].size(); ip++)
					{
						if (pyevents[ie][ip].isFinal())
						{
							auto p = pyevents[ie][ip];
							ppEventParticles.push_back(p);
							AAEventParticles.push_back(p);
						}
					}
					// fill the pp collisions tree
					NNstream << "p_" << ppEventParticles << endl;
				}
				// fill the AA collisions tree
				AAstream << "p_" << AAEventParticles << endl;

				// now fill the glauber information
				if (fpGlauberMC)
				{
					auto colls = fpGlauberMC->GetCollisions();
				}
			}
			// auto fstate_parts = evpool->GetFinalParticles();
			// std::vector<double> etas;
			// for ( auto & p : fstate_parts )
			// {
			// 	if (p.isCharged())
			// 		etas.push_back(p.eta());
			// }
			// // Linfo << "from : " << t->GetName() << " number of final state parts: " << fstate_parts.size();
			// unsigned int dNdeta = std::count_if(etas.begin(), etas.end(), [] (double _eta) {return fabs(_eta) < 1.;});
			// try to do something like this... - needs new event structure... get another task going...
			// unsigned int dNdeta = std::count_if(fstate_parts.begin(), fstate_parts.end(), [] (TPartile p) {return fabs(p.Eta()) < 1.;});
			//Linfo << "from : " << t->GetName() << " number of final state charged parts: " << etas.size();
			//Linfo << "from : " << t->GetName() << " dN/dEta in abs(eta) < 1: " << dNdeta / 2.;
			//Linfo << "from : " << t->GetName() << " dN/dEta in abs(eta) < 1 per Npart / 2.: " << dNdeta / 2. / npart / 2.
			//	<< " npart = " << npart;
		}
		// Ldebug << "charged particle multiplicity in this event #" << fNExecCalls << " = "  << fMult->GetMultiplicity(MultiplicityEstimator::kFSChPerEv, -1, 1);
		// Ldebug << "    charged dN/deta in abs(1) in this event #" << fNExecCalls << " = "  << fMult->GetMultiplicity(MultiplicityEstimator::kFSChPerEv, -1, 1) / 2.;
		// Ldebug << "  total particle multiplicity in this event #" << fNExecCalls << " = "  << fMult->GetMultiplicity(MultiplicityEstimator::kFSPerEv, -1, 1);
		// Ldebug << "      total dN/deta in abs(1) in this event #" << fNExecCalls << " = "  << fMult->GetMultiplicity(MultiplicityEstimator::kFSPerEv, -1, 1) / 2.;

		fMult->NotifyEvent();
		return kGood;
	}

	unsigned int MultiplicityTask::FinalizeThis(const char *opt)
	{
		// fArgs.merge(opt);
		// if (fArgs.isSet("--write"))
		// {
		if (fMult)
			fMult->Write();
		if (fOutputFile)
		{
			Linfo << GetName() << "writing " << fOutputFile->GetPath();
			fOutputFile->Write();
			fOutputFile->Close();
			delete fOutputFile;
		}
		//}
		return kDone;
	}

	// multiplicity estimators with histograms
	MultiplicityEstimator::MultiplicityEstimator(const char *name)
		: fName(name)
		, fNevent(0)
		, fHist()
		, fEventDone(false)
	{
		for (unsigned int i = 0; i < kMEMax; i++)
		{
			string sname = fName + GetEstimatorName(i);
			fHist[i] = new TH1F(sname.c_str(), sname.c_str(), 600, -30, 30);
			fHist[i]->SetDirectory(0);
			fHist[i]->Sumw2();
		}
		Ldebug << "MultiplicityEstimator " << fName << " at " << this;
	}

	MultiplicityEstimator::~MultiplicityEstimator()
	{
		Ldebug << "MultiplicityEstimator::~ " << fName << " at " << this;
		for (unsigned int i = 0; i < kMEMax; i++)
			delete fHist[i];
	}

	void MultiplicityEstimator::AddParticle(const TParticle *p, double scale)
	{
		AddParticle(p->Eta(), (p->GetPDG()->Charge() != 0), scale);
	}

	template <class T> void MultiplicityEstimator::AddParticle(const T &p, const double scale)
	{
		if (std::is_same<T, Pythia8::Particle>::value)
		{
			if (!p.isFinal())
			{
				if (fEventDone == true)
				{
					fHist[kFSChPerEv]->Reset();
					fHist[kFSPerEv]->Reset();
					fEventDone = false;
				}
				return;
			}
		}
		AddParticle(p.eta(), p.isCharged(), scale);
	}

	void MultiplicityEstimator::AddParticle(double eta, bool isCharged, const double scale)
	{
		if (fEventDone == true)
		{
			fHist[kFSChPerEv]->Reset();
			fHist[kFSPerEv]->Reset();
			fEventDone = false;
			// Ldebug << "charged particle multiplicity in this event #" << fNevent << " = "  << GetMultiplicity(kFSChPerEv, -1, 1);
		}
		for (unsigned int i = 0; i < kMEMax; i++)
		{
			if ((isCharged == false) && (i % 2 == 1))
				continue;
			double _scale = 1.;
			if (i == kFSCh || i == kFS)
				_scale = scale;
			fHist[i]->Fill(eta, _scale);
		}
	}

	template <class E> void MultiplicityEstimator::AddEventParticles(const E &e, const double scale)
	{
		AddEvent(e, scale);
		NotifyEvent();
	}

	template <class E> void MultiplicityEstimator::AddEvent(const E &e, const double scale)
	{
		for ( auto & p : e )
		{
			AddParticle(p, scale);
		}
	}

	double MultiplicityEstimator::GetMultiplicity(unsigned int which, double etamin, double etamax)
	{
		if (which >= kMEMax)
		{
			Lerror << "asking for multplicity measure" << which << " range is: 0-" << kMEMax - 1;
			return -1;
		}
		int blow = fHist[which]->FindBin(etamin);
		int bhigh = fHist[which]->FindBin(etamax);
		return fHist[which]->Integral(blow, bhigh);
	}

	void MultiplicityEstimator::NotifyEvent()
	{
		fNevent++;
		fEventDone = true;
	}

	void MultiplicityEstimator::Write(TFile *fout)
	{
		Bool_t own_file = false;
		if (!fout)
		{
			string fname = fName;
			fname += ".root";
			fout = new TFile(fname.c_str(), "recreate");
			own_file = true;
		}
		if (fout)
		{
			fout->cd();
			for (unsigned int i = 0; i < kMEMax; i++)
			{
				string new_name = "h";
				new_name += fHist[i]->GetName();
				auto tmp = (TH1F*)fHist[i]->Clone(new_name.c_str());
				if (i != kFSPerEv && i != kFSChPerEv)
				{
					tmp->Scale(1./fNevent, "width");
					tmp->Write();
					if (i == 0) Linfo << fName << " writing to file: " << fout->GetName();
					if (i == 0) Linfo << fName << " per event scale: 1./" << fNevent;
				}
			}
			if (own_file)
			{
				fout->Close();
				delete fout;
			}
		}
	}
}
