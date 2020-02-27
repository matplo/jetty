#include <TFile.h>
#include <TTree.h>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmisleading-indentation"
#include <TLorentzVector.h>
#pragma GCC diagnostic pop

#include <TROOT.h>
#include <TApplication.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TH2D.h>
#include <TProfile.h>

#include <fastjet/PseudoJet.hh>
#include <fastjet/JetDefinition.hh>
#include <fastjet/AreaDefinition.hh>
#include <fastjet/ClusterSequenceArea.hh>

#ifdef FASTJET_MEDIAN
#include <fastjet/tools/JetMedianBackgroundEstimator.hh>
#include <fastjet/tools/Subtractor.hh>
#endif // FASTJET_MEDIAN

#include <jetty/util/args.h>
#include <jetty/util/blog.h>
#include <jetty/util/strutil.h>
#include <jetty/util/looputil.h>

// #include <jet.h>
// #ifdef WITH_EFP7
// #include <einstein_sum.h>
// #include <efp7.cc>
// #endif // WITH_EFP7

#include <jetty/mljets/jet.h>
#include <jetty/mljets/einstein_sum.h>
#include <jetty/mljets/efp7.h>
#include <jetty/mljets/fill_efp7.h>
using namespace mljets;

#define NTRACK_MAX (1U << 17)

inline float half(const float x)
{
    return x;
}

int jet_efp(int argc, char *argv[])
{
    if (argc < 2) {
        exit(EXIT_FAILURE);
    }

    int dummyc = 1;
    char **dummyv = new char *[1];

    dummyv[0] = strdup("main");

#if 0
    TApplication application("", &dummyc, dummyv);
    TCanvas canvas("canvas", "", 960 + 4, 720 + 28);
    TH1D histogram0("histogram0", "", 120, -2, 2);
    TH1D histogram1("histogram1", "", 120, -2, 2);

    canvas.SetLogy();
    histogram0.Sumw2();
    histogram0.SetMarkerStyle(20);
    histogram0.SetMarkerColor(EColor::kBlack);
    histogram0.SetLineColor(EColor::kBlack);
    histogram0.SetXTitle("\\Delta p_T\\:(\\mathrm{GeV}/c)");
    histogram1.Sumw2();
    histogram1.SetMarkerStyle(25);
    histogram1.SetMarkerColor(EColor::kBlack);
    histogram1.SetLineColor(EColor::kBlack);
    histogram1.SetXTitle("\\Delta p_T\\:(\\mathrm{GeV}/c)");
#endif

#pragma GCC diagnostic ignored "-Wunused-but-set-variable"

    UInt_t _branch_njet_ak04tpc;
    Float_t _branch_jet_ak04tpc_pt_raw[32768];
    Float_t _branch_jet_ak04tpc_eta_raw[32768];
    Float_t _branch_jet_ak04tpc_phi[32768];
    Float_t _branch_jet_ak04tpc_efp_raw[32768][489];

    SysUtil::Args args(argc, argv);
    if (args.isSet("-f"))
    {
        Linfo << "file argument set... ok.";
    }
    else
    {
        Lerror << "file argument -f not set";
        return -1;
    }

    int findex = 1;
    for (int iarg = argc - 1; iarg > 0; iarg--) {
        std::string s(argv[iarg]);
        if (s.rfind("-f", 0) == 0) {
            continue;
        }
        findex = iarg++;
        break;
    }
    Linfo << "using findex at position" << findex;
    for (int iarg = findex; iarg < argc; iarg++) {

        TFile *file = TFile::Open(argv[iarg]);

        if (file == NULL) {
            exit(EXIT_FAILURE);
        }

        TDirectoryFile *ntgj = dynamic_cast<TDirectoryFile *>
            (file->Get("AliAnalysisTaskNTGJ"));
        TDirectoryFile *pwghf = dynamic_cast<TDirectoryFile *>
            (file->Get("PWGHF_TreeCreator"));
        TTree *t = NULL;

        UInt_t ntrack;
        Float_t track_e[NTRACK_MAX];
        Float_t track_pt[NTRACK_MAX];
        Float_t track_eta[NTRACK_MAX];
        Float_t track_phi[NTRACK_MAX];
        UChar_t track_quality[NTRACK_MAX];
        UInt_t track_event_id;

        if (ntgj != NULL) {
            t = dynamic_cast<TTree *>(ntgj->Get("_tree_event"));

            t->SetBranchAddress("ntrack", &ntrack);
            t->SetBranchAddress("track_e", track_e);
            t->SetBranchAddress("track_pt", track_pt);
            t->SetBranchAddress("track_eta", track_eta);
            t->SetBranchAddress("track_phi", track_phi);
            t->SetBranchAddress("track_quality", track_quality);
        }
        else if (pwghf != NULL) {
            t = dynamic_cast<TTree *>(pwghf->Get("tree_Particle"));

            t->SetBranchAddress("ParticlePt", track_pt);
            t->SetBranchAddress("ParticleEta", track_eta);
            t->SetBranchAddress("ParticlePhi", track_phi);
            t->SetBranchAddress("ev_id", &track_event_id);
        }

#ifdef FASTJET_MEDIAN
        fastjet::JetMedianBackgroundEstimator fastjet_ue_estimator(
            fastjet::SelectorAbsRapMax(0.9),
            fastjet::JetDefinition(fastjet::kt_algorithm, 0.3),
            fastjet::AreaDefinition(
                fastjet::active_area,
                fastjet::GhostedAreaSpec(0.9)));
#endif // FASTJET_MEDIAN

        TLorentzVector last_v;
        Long64_t nentries = t->GetEntries();
        LoopUtil::TPbar pbar(nentries);
        for (Long64_t i = 0; i < nentries; i++) {
            t->GetEntry(i);
            pbar.Update();
            std::vector<fastjet::PseudoJet> particle_reco_tpc;
            std::vector<point_2d_t> particle_reco_area_estimation;

            if (ntgj != NULL) {
                for (UInt_t j = 0; j < ntrack; j++) {
                    if ((track_quality[j] & 3) != 0 &&
                        std::isfinite(track_eta[j]) &&
                        std::isfinite(track_phi[j])) {
                        TLorentzVector v;

                        v.SetPtEtaPhiE(track_pt[j], track_eta[j],
                                       track_phi[j], track_e[j]);
                        particle_reco_tpc.push_back(fastjet::PseudoJet(
                            v.Px(), v.Py(), v.Pz(), v.P()));
                        particle_reco_area_estimation.push_back(
                            point_2d_t(track_eta[j], track_phi[j]));
                    }
                }
            }
            else if (pwghf != NULL) {
#if 1
                if (i > 0) {
                    particle_reco_tpc.push_back(fastjet::PseudoJet(
                        last_v.Px(), last_v.Py(), last_v.Pz(),
                        last_v.P()));
                }

                const UInt_t current_event_id = track_event_id;

                while (true) {
                    // http://pdg.lbl.gov/2019/listings/
                    // rpp2019-list-pi-plus-minus.pdf
                    static const double piplusminus_mass =
                        139.57061e-3;
                    TLorentzVector v;

                    v.SetPtEtaPhiM(track_pt[0], track_eta[0],
                                   track_phi[0], piplusminus_mass);
                    particle_reco_tpc.push_back(fastjet::PseudoJet(
                        v.Px(), v.Py(), v.Pz(), v.P()));
                    t->GetEntry(++i);
                    if (i >= t->GetEntries() ||
                        track_event_id != current_event_id) {
                        v.SetPtEtaPhiM(track_pt[0], track_eta[0],
                                       track_phi[0], piplusminus_mass);
                        last_v = v;
                        break;
                    }
                }
#endif
            }

            // fprintf(stderr, "%s:%d: %lu\n", __FILE__, __LINE__, particle_reco_tpc.size());

            for (size_t j = 0; j < particle_reco_tpc.size(); j++) {
                particle_reco_tpc[j].set_user_index(static_cast<int>(j));
            }

            std::vector<double> particle_reco_area;
            std::vector<std::set<size_t> > particle_reco_incident;

            voronoi_area_incident(particle_reco_area,
                                  particle_reco_incident,
                                  particle_reco_area_estimation);

            double sum_area = 0;

            for (size_t j = 0; j < particle_reco_area.size(); j++) {
                sum_area += particle_reco_area[j];
            }

            static const double jet_kt_d_ue_estimation = 0.3;
            const fastjet::ClusterSequenceArea
                cluster_sequence_ue_estimation(
                    particle_reco_tpc,
                    fastjet::JetDefinition(fastjet::JetDefinition(
                        fastjet::kt_algorithm, jet_kt_d_ue_estimation)),
                    fastjet::VoronoiAreaSpec());

            std::pair<std::vector<double>, std::vector<double> >
                ue_estimate = ue_estimation_truncated_mean(
                    cluster_sequence_ue_estimation, particle_reco_area);

            const fastjet::ClusterSequenceArea
                cluster_sequence(
                    particle_reco_tpc,
                    fastjet::JetDefinition(fastjet::JetDefinition(
                        fastjet::antikt_algorithm, 0.4)),
                    fastjet::AreaDefinition(
                        fastjet::active_area,
                        fastjet::GhostedAreaSpec(0.9)));
            const std::vector<fastjet::PseudoJet> jet_with_ue =
                cluster_sequence.inclusive_jets(0);

#ifdef FASTJET_MEDIAN
            fastjet_ue_estimator.set_particles(particle_reco_tpc);
#endif // FASTJET_MEDIAN

            _branch_njet_ak04tpc = jet_with_ue.size();

            for (std::vector<fastjet::PseudoJet>::const_iterator
                     iterator_jet = jet_with_ue.begin();
                 iterator_jet != jet_with_ue.end(); iterator_jet++) {
                const std::vector<fastjet::PseudoJet> constituent =
                    cluster_sequence.constituents(*iterator_jet);
                double pt_raw_ue = 0;

                for (std::vector<fastjet::PseudoJet>::const_iterator
                         iterator_constituent = constituent.begin();
                     iterator_constituent != constituent.end();
                     iterator_constituent++) {
                    const int index = iterator_constituent->user_index();

                    if (index >= 0 && static_cast<size_t>(index) <
                        particle_reco_area.size()) {
                        pt_raw_ue += evaluate_ue(
                            ue_estimate,
                            iterator_constituent->pseudorapidity(),
                            iterator_constituent->phi_std()) *
                            particle_reco_area[index];
                    }
                }

                const size_t index_jet = iterator_jet - jet_with_ue.begin();

                _branch_jet_ak04tpc_pt_raw[index_jet] =
                    iterator_jet->perp() - pt_raw_ue;
                _branch_jet_ak04tpc_eta_raw[index_jet] =
                    iterator_jet->pseudorapidity();
                _branch_jet_ak04tpc_phi[index_jet] =
                    iterator_jet->phi_std();

#if 0
                if (fabs(iterator_jet->pseudorapidity()) < 0.4) {
                    if (std::isfinite(pt_raw_ue)) {
                        histogram0.Fill(iterator_jet->perp() - pt_raw_ue);
                    }
#ifdef FASTJET_MEDIAN
                    histogram1.Fill(iterator_jet->perp() -
                                    fastjet_ue_estimator.rho(*iterator_jet) *
                                    iterator_jet->area());
#endif // FASTJET_MEDIAN
                }
#endif
            }

//#ifdef WITH_EFP7
//#define FILL_EFP7_NO_TRUTH 1
//#include <fill_efp7.cc>
//#undef FILL_EFP7_NO_TRUTH
//#endif // WITH_EFP7

//#ifdef WITH_EFP7
#define FILL_EFP7_NO_TRUTH 1
FILL_EFP7(efp_raw, particle_reco_tpc, jet_ak04tpc, eta_raw, 0.4);
#ifndef FILL_EFP7_NO_TRUTH
FILL_EFP7(efp, particle_truth, jet_truth_ak04, eta, 0.4);
FILL_EFP7(efp, particle_charged_truth, jet_charged_truth_ak04, eta, 0.4);
#endif // FILL_EFP7_NO_TRUTH
#undef FILL_EFP7_NO_TRUTH
//#endif // WITH_EFP7



#if 0
            if (i % 100 == 0) {
                histogram0.Draw("e1x0");
                histogram1.Draw("e1x0same");
                canvas.Update();
                canvas.SaveAs("sub.eps");
            }
#endif
        }
    }

#if 0
    histogram0.Draw("e1x0");
    histogram1.Draw("e1x0same");
    canvas.Update();

    application.Run();
#endif

    return EXIT_SUCCESS;
}
