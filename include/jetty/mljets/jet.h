#ifndef JETTY_MLJETS_JET_H_
#define JETTY_MLJETS_JET_H_

#include <vector>
#include <set>
#include <map>
#include <utility>
#include <algorithm>

#include <Math/SpecFuncMathCore.h>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmisleading-indentation"
#include <TDecompSVD.h>
#pragma GCC diagnostic pop
#include <TPolyLine.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Weffc++"
#pragma GCC diagnostic ignored "-Wshift-negative-value"
#pragma GCC diagnostic ignored "-Wnon-virtual-dtor"
#pragma GCC diagnostic ignored "-Wimplicit-fallthrough=0"
#pragma GCC diagnostic ignored "-Wuninitialized"
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/Voronoi_diagram_2.h>
#include <CGAL/Delaunay_triangulation_adaptation_traits_2.h>
#include <CGAL/Delaunay_triangulation_adaptation_policies_2.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/Boolean_set_operations_2.h>

#include <fastjet/PseudoJet.hh>
#include <fastjet/ClusterSequenceArea.hh>
#pragma GCC diagnostic pop

#include <jetty/mljets/special_function.h>
#include <jetty/mljets/emcal_cell.h>

namespace mljets {

    static const double emcal_voronoi_azimuth_0 = 0.5;

    typedef CGAL::Delaunay_triangulation_2<
        CGAL::Exact_predicates_exact_constructions_kernel>
    delaunay_triangulation_t;
    typedef delaunay_triangulation_t::Point point_2d_t;
    typedef CGAL::Voronoi_diagram_2<
        delaunay_triangulation_t,
        CGAL::Delaunay_triangulation_adaptation_traits_2<
            delaunay_triangulation_t>,
        CGAL::Delaunay_triangulation_caching_degeneracy_removal_policy_2<
            delaunay_triangulation_t> > voronoi_diagram_t;
    typedef CGAL::Polygon_2<
        CGAL::Exact_predicates_exact_constructions_kernel>
        polygon_t;
    typedef polygon_t::Point_2 point_2d_epeck_t;
    typedef CGAL::Polygon_with_holes_2<
        CGAL::Exact_predicates_exact_constructions_kernel>
        polygon_hole_t;

    void voronoi_insert_alice_tpc(
        voronoi_diagram_t &diagram,
        std::map<voronoi_diagram_t::Face_handle, size_t> &face_index,
        const std::vector<point_2d_t>
        particle_pseudorapidity_azimuth);

    void create_boundary(
        polygon_t &boundary_emcal,
        polygon_t &boundary_emcal_neg,
        polygon_t &boundary_dcal,
        const std::vector<point_2d_t> cell_emcal_dcal =
        std::vector<point_2d_t>());

    double eval_polygon_area(const std::list<polygon_hole_t> p);

    void voronoi_area_incident(
        std::vector<double> &particle_area,
        std::vector<std::set<size_t> > &particle_incident,
        const std::vector<point_2d_t> particle_pseudorapidity_azimuth,
        const std::vector<point_2d_t> cell_emcal_dcal =
        std::vector<point_2d_t>());

    std::pair<CGAL::Bbox_2, CGAL::Bbox_2>
    disjoined_dcal_bbox(const polygon_t b);

    bool fully_contained(const CGAL::Bbox_2 t, const CGAL::Bbox_2 b);

    void voronoi_polygon(
        std::vector<TPolyLine> &polyline,
        const std::vector<point_2d_t> &
        particle_pseudorapidity_azimuth,
        const std::vector<point_2d_t> cell_emcal_dcal =
        std::vector<point_2d_t>());

    double voronoi_area(const fastjet::PseudoJet jet,
                        const fastjet::ClusterSequenceArea
                        cluster_sequence,
                        const std::vector<double> particle_area);

    std::vector<double> rho_order_statistics(
        fastjet::ClusterSequenceArea cluster_sequence,
        std::vector<double> particle_area);

    void quantile_harrell_davis(double &q, double &q_se,
                                const std::vector<double> x,
                                double p);

    std::pair<std::pair<std::vector<double>, std::vector<double> >,
              double>
    ue_estimation_median(fastjet::ClusterSequenceArea
                         cluster_sequence,
                         std::vector<double> particle_area);

    void
    append_quantile(std::vector<fastjet::PseudoJet> &
                    constituent_truncated,
                    std::set<int> &constituent_truncated_user_index,
                    const std::vector<std::pair<
                    double, std::vector<fastjet::PseudoJet>::
                    const_iterator> > &rho_vs_jet_unsorted,
                    const fastjet::ClusterSequenceArea
                    cluster_sequence,
                    const std::vector<double> &particle_area,
                    double quantile);

    void constituent_quantile(
        std::vector<fastjet::PseudoJet> &constituent_truncated,
        std::set<int> &constituent_truncated_user_index,
        fastjet::ClusterSequenceArea cluster_sequence,
        std::vector<double> particle_area,
        size_t order_azimuth_fourier, double quantile);

    std::pair<std::vector<double>, std::vector<double> >
    ue_estimation_truncated_mean(
        fastjet::ClusterSequenceArea cluster_sequence,
        std::vector<double> particle_area,
        size_t order_pseudorapidity_chebyshev = 4,
        size_t order_azimuth_fourier = 3,
        double quantile = 0.5);

    std::set<int> ue_user_index_truncated_mean(
        fastjet::ClusterSequenceArea cluster_sequence,
        std::vector<double> particle_area,
        size_t order_pseudorapidity_chebyshev = 4,
        size_t order_azimuth_fourier = 3,
        double quantile = 0.5);

    double evaluate_ue(std::pair<std::vector<double>,
                       std::vector<double> > ue_estimate,
                       double pseudorapidity, double azimuth);

    double evaluate_ue_constant(std::pair<std::vector<double>,
                                std::vector<double> > ue_estimate);


    // fastjet::PseudoJet user indices -2 and -3 are used to tag the
    // EM particles/EMCAL clusters and muons. The index -1 is already
    // taken, being the fastjet::PseudoJet default initializer. After
    // the removal of EM and muons, -1 then implicitly means hadronic

    enum {
        USER_INDEX_DEFAULT_OR_TRACK     = -1,
        USER_INDEX_EM                   = -2,
        USER_INDEX_MUON                 = -3,
        USER_INDEX_PARTON_ALGORITHMIC_0 = -100,
        USER_INDEX_PARTON_PHYSICS_0     = -200,
    };

    double jet_emf(const std::vector<fastjet::PseudoJet> constituent,
                   double scale_em_ghost = 1);

    size_t jet_multiplicity(const std::vector<fastjet::PseudoJet>
                            constituent);

    double constituent_perp(const fastjet::PseudoJet constituent,
                            double scale_em_ghost = 1);

    double jet_ptd(const std::vector<fastjet::PseudoJet> constituent,
                   double scale_em_ghost = 1);

    void jet_width_sigma(double sigma[],
                         const fastjet::PseudoJet jet,
                         const std::vector<fastjet::PseudoJet>
                         constituent,
                         double scale_em_ghost = 1);

}

#define FILL_BRANCH_JET_TRUTH(t, s, jet_truth)                      \
    _branch_njet_ ## t ## _ ## s = 0;                               \
    if (mc_truth_event != NULL) {                                   \
        for (std::vector<fastjet::PseudoJet>::const_iterator        \
                 iterator_jet = jet_truth.begin();                  \
             iterator_jet != jet_truth.end(); iterator_jet++) {     \
            _branch_jet_ ## t ## _ ## s ## _e                       \
                [_branch_njet_ ## t ## _ ## s] =                    \
                half(iterator_jet->E());                            \
            _branch_jet_ ## t ## _ ## s ## _pt                      \
                [_branch_njet_ ## t ## _ ## s] =                    \
                half(iterator_jet->perp());                         \
            _branch_jet_ ## t ## _ ## s ## _eta                     \
                [_branch_njet_ ## t ## _ ## s] =                    \
                half(iterator_jet->pseudorapidity());               \
            _branch_jet_ ## t ## _ ## s ## _phi                     \
                [_branch_njet_ ## t ## _ ## s] =                    \
                half(iterator_jet->phi_std());                      \
            _branch_jet_ ## t ## _ ## s ## _area                    \
                [_branch_njet_ ## t ## _ ## s] =                    \
                half(iterator_jet->area());                         \
                                                                    \
            const std::vector<fastjet::PseudoJet> constituent =     \
                cluster_sequence_truth->                            \
                constituents(*iterator_jet);                        \
                                                                    \
            _branch_jet_ ## t ## _ ## s ## _emf                     \
                [_branch_njet_ ## t ## _ ## s] =                    \
                half(jet_emf(constituent));                         \
            _branch_jet_ ## t ## _ ## s ## _multiplicity            \
                [_branch_njet_ ## t ## _ ## s] =                    \
                jet_multiplicity(constituent);                      \
                                                                    \
            double sigma_d[2];                                      \
                                                                    \
            jet_width_sigma(sigma_d, *iterator_jet, constituent);   \
                                                                    \
            for (size_t i = 0; i < 2; i++) {                        \
                _branch_jet_ ## t ## _ ## s ## _width_sigma         \
                    [_branch_njet_ ## t ## _ ## s][i] =             \
                    half(sigma_d[i]);                               \
            }                                                       \
                                                                    \
            _branch_jet_ ## t ## _ ## s ## _ptd                     \
                [_branch_njet_ ## t ## _ ## s] =                    \
                half(jet_ptd(constituent));                         \
                                                                    \
            _branch_njet_ ## t ## _ ## s++;                         \
            if (_branch_njet_ ## t ## _ ## s >= NJET_MAX) {         \
                break;                                              \
            }                                                       \
        }                                                           \
    }

#define TAG_PARTICLE_RECO_JET_TRUTH(particle_reco_tagged,           \
                                    jet_truth, is_charged)          \
    if (mc_truth_event != NULL) {                                   \
        for (std::vector<fastjet::PseudoJet>::const_iterator        \
                 iterator_jet = jet_truth.begin();                  \
             iterator_jet != jet_truth.end(); iterator_jet++) {     \
            const std::vector<fastjet::PseudoJet> constituent =     \
                cluster_sequence_truth->                            \
                constituents(*iterator_jet);                        \
                                                                    \
            for (std::vector<fastjet::PseudoJet>::const_iterator    \
                     iterator_constituent = constituent.begin();    \
                 iterator_constituent != constituent.end();         \
                 iterator_constituent++) {                          \
                                                                    \
                particle_reco_tagged.push_back(                     \
                    *iterator_constituent * scale_ghost);           \
                /* Positive user indices are used to tag the truth  \
                 * jet, shifted by one bit to indicate whether this \
                 * is a charged truth jet tag */                    \
                particle_reco_tagged.back().set_user_index(         \
                    ((iterator_jet - jet_truth.begin()) << 1) |     \
                    is_charged);                                    \
            }                                                       \
        }                                                           \
    }

#define TAG_PARTICLE_RECO_PARTON(particle_reco_tagged, t, tt)       \
    if (mc_truth_event != NULL) {                                   \
        for (std::vector<Int_t>::const_iterator                     \
                 iterator_parton_index =                            \
                 reverse_stored_parton_ ## t ## _index.begin();     \
             iterator_parton_index !=                               \
                 reverse_stored_parton_ ## t ## _index.end();       \
             iterator_parton_index++) {                             \
            const AliMCParticle *p =                                \
                dynamic_cast<AliMCParticle *>(                      \
                    mc_truth_event->GetTrack(                       \
                        *iterator_parton_index));                   \
                                                                    \
            particle_reco_tagged.push_back(fastjet::PseudoJet(      \
                p->Px() * scale_ghost, p->Py() * scale_ghost,       \
                p->Pz() * scale_ghost, p->P() * scale_ghost));      \
            /* With USER_INDEX_PARTON_ALGORITHMIC_0 = -100, the     \
               user index range [-130, -70] are devoted to possible \
               partons, and +/-30 is sufficient to identify         \
               particles that are misclassified as partons */       \
            particle_reco_tagged.back().set_user_index(             \
                USER_INDEX_PARTON_ ## tt ## _0 +                    \
                std::max(-30, std::min(30, p->PdgCode())));         \
        }                                                           \
    }

#define FILL_BRANCH_JET(s, jet_reco, cluster_sequence_reco,         \
                        jet_reco_tagged,                            \
                        cluster_sequence_reco_tagged,               \
                        t, jet_truth, jet_charged_truth,            \
                        particle_reco_area, ue_estimate)            \
    _branch_njet_ ## s = 0;                                         \
    for (std::vector<fastjet::PseudoJet>::const_iterator            \
             iterator_jet = jet_reco.begin();                       \
         iterator_jet != jet_reco.end(); iterator_jet++) {          \
        std::vector<fastjet::PseudoJet>::const_iterator             \
            iterator_jet_tagged = jet_reco_tagged.end();            \
        double dr_2_min = INFINITY;                                 \
                                                                    \
        for (std::vector<fastjet::PseudoJet>::const_iterator        \
                 it = jet_reco_tagged.begin();                      \
             it != jet_reco_tagged.end(); it++) {                   \
            const double dr_2 =                                     \
                iterator_jet->squared_distance(*it);                \
            if (dr_2 < dr_2_min) {                                  \
                iterator_jet_tagged = it;                           \
                dr_2_min = dr_2;                                    \
            }                                                       \
        }                                                           \
                                                                    \
        _branch_debug_jet_ ## s ## _tag_dr_square                   \
            [_branch_njet_ ## s] = dr_2_min;                        \
                                                                    \
        if (!(iterator_jet->perp() >= _stored_jet_min_pt_raw)) {    \
            continue;                                               \
        }                                                           \
                                                                    \
        /* Jet quantities follow HEP convention (not ALICE so far): \
         * - Suffix _raw = raw, jet-uncalibrated detector quantity  \
         * - Suffix _charged = calibrated, "charged particle-level" \
         *   quantity                                               \
         * - No suffix = jet-calibrated, particle-level quantity */ \
                                                                    \
        _branch_jet_ ## s ## _e_raw[_branch_njet_ ## s] =           \
            half(iterator_jet->E());                                \
        _branch_jet_ ## s ## _e[_branch_njet_ ## s] = NAN;          \
                                                                    \
        std::vector<fastjet::PseudoJet> constituent =               \
            cluster_sequence_reco.constituents(*iterator_jet);      \
        double area = 0;                                            \
        double pt_raw_ue = 0;                                       \
                                                                    \
        for (std::vector<fastjet::PseudoJet>::const_iterator        \
                 iterator_constituent = constituent.begin();        \
             iterator_constituent != constituent.end();             \
             iterator_constituent++) {                              \
            const int index = iterator_constituent->user_index();   \
                                                                    \
            if (index >= 0 && static_cast<size_t>(index) <          \
                particle_reco_area.size()) {                        \
                area += particle_reco_area[index];                  \
                pt_raw_ue += evaluate_ue(                           \
                    ue_estimate.first,                              \
                    iterator_constituent->pseudorapidity(),         \
                    iterator_constituent->phi_std()) *              \
                    particle_reco_area[index];                      \
            }                                                       \
        }                                                           \
                                                                    \
        _branch_jet_ ## s ## _pt_raw_ue[_branch_njet_ ## s] =       \
            half(pt_raw_ue);                                        \
        _branch_jet_ ## s ## _pt_raw[_branch_njet_ ## s] =          \
            half(iterator_jet->perp() - pt_raw_ue);                 \
        _branch_jet_ ## s ## _pt[_branch_njet_ ## s] = NAN;         \
        _branch_jet_ ## s ## _e_charged[_branch_njet_ ## s] = NAN;  \
        _branch_jet_ ## s ## _pt_charged[_branch_njet_ ## s] = NAN; \
        _branch_jet_ ## s ## _eta_raw[_branch_njet_ ## s] =         \
            half(iterator_jet->pseudorapidity());                   \
        _branch_jet_ ## s ## _eta[_branch_njet_ ## s] =             \
            half(iterator_jet->pseudorapidity());                   \
        _branch_jet_ ## s ## _phi[_branch_njet_ ## s] =             \
            half(iterator_jet->phi_std());                          \
        _branch_jet_ ## s ## _area_raw[_branch_njet_ ## s] =        \
            half(area);                                             \
        _branch_jet_ ## s ## _area[_branch_njet_ ## s] =            \
            half(area);                                             \
                                                                    \
        /* Calculate the electro magnetic fraction (EMF), but       \
         * without a particle-flow-based removal of energy double   \
         * counting. Note the EM ghosts are scaled back here. */    \
                                                                    \
        _branch_jet_ ## s ## _emf_raw[_branch_njet_ ## s] = NAN;    \
        _branch_jet_ ## s ## _emf[_branch_njet_ ## s] = NAN;        \
        _branch_jet_ ## s ## _multiplicity_raw                      \
            [_branch_njet_ ## s] = 0;                               \
        _branch_jet_ ## s ## _multiplicity[_branch_njet_ ## s] =    \
            NAN;                                                    \
        std::fill(_branch_jet_ ## s ## _width_sigma_raw             \
                  [_branch_njet_ ## s],                             \
                  _branch_jet_ ## s ## _width_sigma_raw             \
                  [_branch_njet_ ## s] + 2, NAN);                   \
        std::fill(_branch_jet_ ## s ## _width_sigma                 \
                  [_branch_njet_ ## s],                             \
                  _branch_jet_ ## s ## _width_sigma                 \
                  [_branch_njet_ ## s] + 2, NAN);                   \
        _branch_jet_ ## s ## _ptd_raw[_branch_njet_ ## s] = NAN;    \
        _branch_jet_ ## s ## _ptd[_branch_njet_ ## s] = NAN;        \
                                                                    \
        if (iterator_jet_tagged != jet_reco_tagged.end()) {         \
            constituent = cluster_sequence_reco_tagged.             \
                constituents(*iterator_jet_tagged);                 \
                                                                    \
            _branch_jet_ ## s ## _emf_raw[_branch_njet_ ## s] =     \
                jet_emf(constituent, scale_ghost);                  \
            _branch_jet_ ## s ## _multiplicity_raw                  \
                [_branch_njet_ ## s] =                              \
                jet_multiplicity(constituent);                      \
                                                                    \
            double sigma_d[2];                                      \
                                                                    \
            jet_width_sigma(sigma_d, *iterator_jet, constituent);   \
                                                                    \
            for (size_t i = 0; i < 2; i++) {                        \
                _branch_jet_ ## s ## _width_sigma                   \
                    [_branch_njet_ ## s][i] = half(sigma_d[i]);     \
            }                                                       \
                                                                    \
            _branch_jet_ ## s ## _ptd_raw[_branch_njet_ ## s] =     \
                jet_ptd(constituent, scale_ghost);                  \
        }                                                           \
                                                                    \
        const size_t index_reco = iterator_jet - jet_reco.begin();  \
                                                                    \
        std::fill(_branch_jet_ ## s ## _truth_index_z_truth         \
                  [_branch_njet_ ## s],                             \
                  _branch_jet_ ## s ## _truth_index_z_truth         \
                  [_branch_njet_ ## s] + 2, -1);                    \
        std::fill(_branch_jet_ ## s ## _truth_z_truth               \
                  [_branch_njet_ ## s],                             \
                  _branch_jet_ ## s ## _truth_z_truth               \
                  [_branch_njet_ ## s] + 2, NAN);                   \
        std::fill(_branch_jet_ ## s ## _truth_index_z_reco          \
                  [_branch_njet_ ## s],                             \
                  _branch_jet_ ## s ## _truth_index_z_reco          \
                  [_branch_njet_ ## s] + 2, -1);                    \
        std::fill(_branch_jet_ ## s ## _truth_z_reco                \
                  [_branch_njet_ ## s],                             \
                  _branch_jet_ ## s ## _truth_z_reco                \
                  [_branch_njet_ ## s] + 2, NAN);                   \
                                                                    \
        std::fill(_branch_jet_ ## s ## _charged_truth_index_z_truth \
                  [_branch_njet_ ## s],                             \
                  _branch_jet_ ## s ## _charged_truth_index_z_truth \
                  [_branch_njet_ ## s] + 2, -1);                    \
        std::fill(_branch_jet_ ## s ## _charged_truth_z_truth       \
                  [_branch_njet_ ## s],                             \
                  _branch_jet_ ## s ## _charged_truth_z_truth       \
                  [_branch_njet_ ## s] + 2, NAN);                   \
        std::fill(_branch_jet_ ## s ## _charged_truth_index_z_reco  \
                  [_branch_njet_ ## s],                             \
                  _branch_jet_ ## s ## _charged_truth_index_z_reco  \
                  [_branch_njet_ ## s] + 2, -1);                    \
        std::fill(_branch_jet_ ## s ## _charged_truth_z_reco        \
                  [_branch_njet_ ## s],                             \
                  _branch_jet_ ## s ## _charged_truth_z_reco        \
                  [_branch_njet_ ## s] + 2, NAN);                   \
                                                                    \
        std::fill(_branch_jet_ ## s ## _pdg_code_algorithmic        \
                  [_branch_njet_ ## s],                             \
                  _branch_jet_ ## s ## _pdg_code_algorithmic        \
                  [_branch_njet_ ## s] + 2, 0);                     \
        std::fill(_branch_jet_ ## s ## _pdg_code_algorithmic_z      \
                  [_branch_njet_ ## s],                             \
                  _branch_jet_ ## s ## _pdg_code_algorithmic_z      \
                  [_branch_njet_ ## s] + 2, NAN);                   \
                                                                    \
        _branch_jet_ ## s ## _e_truth[_branch_njet_ ## s] = NAN;    \
        _branch_jet_ ## s ## _pt_truth[_branch_njet_ ## s] = NAN;   \
        _branch_jet_ ## s ## _eta_truth[_branch_njet_ ## s] = NAN;  \
        _branch_jet_ ## s ## _phi_truth[_branch_njet_ ## s] = NAN;  \
                                                                    \
        _branch_jet_ ## s ## _e_charged_truth                       \
            [_branch_njet_ ## s] = NAN;                             \
        _branch_jet_ ## s ## _pt_charged_truth                      \
            [_branch_njet_ ## s] = NAN;                             \
        _branch_jet_ ## s ## _eta_charged_truth                     \
            [_branch_njet_ ## s] = NAN;                             \
        _branch_jet_ ## s ## _phi_charged_truth                     \
            [_branch_njet_ ## s] = NAN;                             \
                                                                    \
        if (mc_truth_event != NULL &&                               \
            iterator_jet_tagged != jet_reco_tagged.end()) {         \
            std::map<int, double> z_ghost;                          \
            std::map<int, double> z_ghost_charged;                  \
            std::map<int, double> z_ghost_parton_algorithmic;       \
                                                                    \
            for (std::vector<fastjet::PseudoJet>::const_iterator    \
                     iterator_constituent = constituent.begin();    \
                 iterator_constituent != constituent.end();         \
                 iterator_constituent++) {                          \
                const int index_jet_truth =                         \
                    iterator_constituent->user_index() >= 0 &&      \
                    (iterator_constituent->user_index() & 1) == 0 ? \
                    (iterator_constituent->user_index() >> 1) : -1; \
                const int index_jet_charged_truth =                 \
                    iterator_constituent->user_index() >= 0 &&      \
                    (iterator_constituent->user_index() & 1) == 1 ? \
                    (iterator_constituent->user_index() >> 1) : -1; \
                const int parton_pdg_code_algorithmic =             \
                    iterator_constituent->user_index() -            \
                    USER_INDEX_PARTON_ALGORITHMIC_0;                \
                                                                    \
                if (index_jet_truth >= 0) {                         \
                    if (z_ghost.find(index_jet_truth) ==            \
                        z_ghost.end()) {                            \
                        z_ghost[index_jet_truth] =                  \
                            iterator_constituent->perp() /          \
                            scale_ghost;                            \
                    }                                               \
                    else {                                          \
                        z_ghost[index_jet_truth] +=                 \
                            iterator_constituent->perp() /          \
                            scale_ghost;                            \
                    }                                               \
                }                                                   \
                if (index_jet_charged_truth >= 0) {                 \
                    if (z_ghost.find(index_jet_charged_truth) ==    \
                        z_ghost.end()) {                            \
                        z_ghost[index_jet_charged_truth] =          \
                            iterator_constituent->perp() /          \
                            scale_ghost;                            \
                    }                                               \
                    else {                                          \
                        z_ghost[index_jet_charged_truth] +=         \
                            iterator_constituent->perp() /          \
                            scale_ghost;                            \
                    }                                               \
                }                                                   \
                else if (std::abs(parton_pdg_code_algorithmic) <=   \
                         21) {                                      \
                    if (z_ghost_parton_algorithmic.                 \
                        find(parton_pdg_code_algorithmic) ==        \
                        z_ghost_parton_algorithmic.end()) {         \
                        z_ghost_parton_algorithmic[                 \
                            parton_pdg_code_algorithmic] =          \
                            iterator_constituent->perp() /          \
                            scale_ghost;                            \
                    }                                               \
                    else {                                          \
                        z_ghost_parton_algorithmic[                 \
                            parton_pdg_code_algorithmic] += \
                            iterator_constituent->perp() /          \
                            scale_ghost;                            \
                    }                                               \
                }                                                   \
            }                                                       \
                                                                    \
            /* z_truth = fraction of truth constituents inside the  \
             * area of the reco jet, relative to the truth jet (not \
             * necessarily within the reco jet) */                  \
                                                                    \
            std::vector<std::pair<double, int> > z_truth;           \
                                                                    \
            for (std::map<int, double>::const_iterator iterator =   \
                     z_ghost.begin();                               \
                 iterator != z_ghost.end(); iterator++) {           \
                if (jet_truth[iterator->first].perp() > 0) {        \
                    z_truth.push_back(std::pair<double, int>(       \
                        iterator->second /                          \
                        jet_truth[iterator->first].perp(),          \
                        iterator->first));                          \
                }                                                   \
            }                                                       \
            std::sort(z_truth.begin(), z_truth.end());              \
                                                                    \
            /* Note that z_truth is now in *acending* order. Any    \
             * information beyond 2 truth -> 1 reco jet mapping is  \
             * not really useful, we only need to know how fuzzy    \
             * the mapping was */                                   \
                                                                    \
            for (size_t j = 0;                                      \
                 j < std::min(2UL, z_truth.size()); j++) {          \
                _branch_jet_ ## s ## _truth_z_truth                 \
                    [_branch_njet_ ## s][j] =                       \
                    half(z_truth.rbegin()[j].first);                \
                _branch_jet_ ## s ## _truth_index_z_truth           \
                    [_branch_njet_ ## s][j] =                       \
                    z_truth.rbegin()[j].second;                     \
            }                                                       \
                                                                    \
            /* z_reco = fraction of truth constituents inside the   \
             * area of the reco jet, relative to the total truth    \
             * particles inside the reco jet */                     \
                                                                    \
            double sum_z_ghost = 0;                                 \
                                                                    \
            for (std::map<int, double>::const_iterator iterator =   \
                     z_ghost.begin();                               \
                 iterator != z_ghost.end(); iterator++) {           \
                sum_z_ghost += iterator->second;                    \
            }                                                       \
                                                                    \
            std::vector<std::pair<double, int> > z_reco;            \
                                                                    \
            if (sum_z_ghost > 0) {                                  \
                for (std::map<int, double>::iterator iterator =     \
                         z_ghost.begin();                           \
                     iterator != z_ghost.end(); iterator++) {       \
                    iterator->second /= sum_z_ghost;                \
                }                                                   \
                for (std::map<int, double>::const_iterator          \
                         iterator = z_ghost.begin();                \
                     iterator != z_ghost.end(); iterator++) {       \
                    z_reco.push_back(std::pair<double, int>(        \
                        iterator->second, iterator->first));        \
                }                                                   \
            }                                                       \
            std::sort(z_reco.begin(), z_reco.end());                \
                                                                    \
            /* Note that z_reco is now in *acending* order */       \
                                                                    \
            for (size_t j = 0;                                      \
                 j < std::min(2UL, z_reco.size()); j++) {           \
                _branch_jet_ ## s ## _truth_z_reco                  \
                    [_branch_njet_ ## s][j] =                       \
                    half(z_reco.rbegin()[j].first);                 \
                _branch_jet_ ## s ## _truth_index_z_reco            \
                    [_branch_njet_ ## s][j] =                       \
                    z_reco.rbegin()[j].second;                      \
            }                                                       \
                                                                    \
            /* A simplified z_reco matching, which is a more        \
             * rigorous version of the CMS delta R < D matching,    \
             * for jet energy correction derivation. */             \
                                                                    \
            if (!z_reco.empty() &&                                  \
                z_reco.rbegin()[0].second >= 0 &&                   \
                static_cast<size_t>(z_reco.rbegin()[0].second) <    \
                _branch_njet_truth_ ## t) {                         \
                const size_t k = z_reco.rbegin()[0].second;         \
                                                                    \
                _branch_jet_ ## s ## _e_truth[_branch_njet_ ## s] = \
                    _branch_jet_truth_ ## t ## _e[k];               \
                _branch_jet_ ## s ## _pt_truth                      \
                    [_branch_njet_ ## s] =                          \
                    _branch_jet_truth_ ## t ## _pt[k];              \
                _branch_jet_ ## s ## _eta_truth                     \
                    [_branch_njet_ ## s] =                          \
                    _branch_jet_truth_ ## t ## _eta[k];             \
                _branch_jet_ ## s ## _phi_truth                     \
                    [_branch_njet_ ## s] =                          \
                    _branch_jet_truth_ ## t ## _phi[k];             \
            }                                                       \
                                                                    \
            /* Repeat for charged_truth, using z_ghost_charged */   \
                                                                    \
            z_truth.clear();                                        \
            for (std::map<int, double>::const_iterator iterator =   \
                     z_ghost_charged.begin();                       \
                 iterator != z_ghost_charged.end(); iterator++) {   \
                if (jet_charged_truth[iterator->first].perp() >     \
                    0) {                                            \
                    z_truth.push_back(std::pair<double, int>(       \
                        iterator->second /                          \
                        jet_charged_truth[iterator->first].perp(),  \
                        iterator->first));                          \
                }                                                   \
            }                                                       \
            std::sort(z_truth.begin(), z_truth.end());              \
            for (size_t j = 0;                                      \
                 j < std::min(2UL, z_truth.size()); j++) {          \
                _branch_jet_ ## s ## _charged_truth_z_truth         \
                    [_branch_njet_ ## s][j] =                       \
                    half(z_truth.rbegin()[j].first);                \
                _branch_jet_ ## s ## _charged_truth_index_z_truth   \
                    [_branch_njet_ ## s][j] =                       \
                    z_truth.rbegin()[j].second;                     \
            }                                                       \
            sum_z_ghost = 0;                                        \
            for (std::map<int, double>::const_iterator iterator =   \
                     z_ghost_charged.begin();                       \
                 iterator != z_ghost_charged.end(); iterator++) {   \
                sum_z_ghost += iterator->second;                    \
            }                                                       \
            z_reco.clear();                                         \
            if (sum_z_ghost > 0) {                                  \
                for (std::map<int, double>::iterator iterator =     \
                         z_ghost_charged.begin();                   \
                     iterator != z_ghost_charged.end();             \
                     iterator++) {                                  \
                    iterator->second /= sum_z_ghost;                \
                }                                                   \
                for (std::map<int, double>::const_iterator          \
                         iterator = z_ghost_charged.begin();        \
                     iterator != z_ghost_charged.end();             \
                     iterator++) {                                  \
                    z_reco.push_back(std::pair<double, int>(        \
                        iterator->second, iterator->first));        \
                }                                                   \
            }                                                       \
            std::sort(z_reco.begin(), z_reco.end());                \
            for (size_t j = 0;                                      \
                 j < std::min(2UL, z_reco.size()); j++) {           \
                _branch_jet_ ## s ## _charged_truth_z_reco          \
                    [_branch_njet_ ## s][j] =                       \
                    half(z_reco.rbegin()[j].first);                 \
                _branch_jet_ ## s ## _charged_truth_index_z_reco    \
                    [_branch_njet_ ## s][j] =                       \
                    z_reco.rbegin()[j].second;                      \
            }                                                       \
            if (!z_reco.empty() &&                                  \
                z_reco.rbegin()[0].second >= 0 &&                   \
                static_cast<size_t>(z_reco.rbegin()[0].second) <    \
                _branch_njet_charged_truth_ ## t) {                 \
                const size_t k = z_reco.rbegin()[0].second;         \
                                                                    \
                _branch_jet_ ## s ## _e_charged_truth               \
                    [_branch_njet_ ## s] =                          \
                    _branch_jet_charged_truth_ ## t ## _e[k];       \
                _branch_jet_ ## s ## _pt_charged_truth              \
                    [_branch_njet_ ## s] =                          \
                    _branch_jet_charged_truth_ ## t ## _pt[k];      \
                _branch_jet_ ## s ## _eta_charged_truth             \
                    [_branch_njet_ ## s] =                          \
                    _branch_jet_charged_truth_ ## t ## _eta[k];     \
                _branch_jet_ ## s ## _phi_charged_truth             \
                    [_branch_njet_ ## s] =                          \
                    _branch_jet_charged_truth_ ## t ## _phi[k];     \
            }                                                       \
                                                                    \
            /* The flavor tagging is a variant of the z_reco        \
               matching above */                                    \
                                                                    \
            double sum_z_ghost_parton_algorithmic = 0;              \
                                                                    \
            for (std::map<int, double>::const_iterator iterator =   \
                     z_ghost_parton_algorithmic.begin();            \
                 iterator != z_ghost_parton_algorithmic.end();      \
                 iterator++) {                                      \
                sum_z_ghost_parton_algorithmic += iterator->second; \
            }                                                       \
                                                                    \
            std::vector<std::pair<double, int> >                    \
                z_reco_parton_algorithmic;                          \
                                                                    \
            if (sum_z_ghost_parton_algorithmic > 0) {               \
                for (std::map<int, double>::iterator iterator =     \
                         z_ghost_parton_algorithmic.begin();        \
                     iterator != z_ghost_parton_algorithmic.end();  \
                     iterator++) {                                  \
                    iterator->second /=                             \
                        sum_z_ghost_parton_algorithmic;             \
                }                                                   \
                for (std::map<int, double>::const_iterator          \
                         iterator =                                 \
                         z_ghost_parton_algorithmic.begin();        \
                     iterator != z_ghost_parton_algorithmic.end();  \
                     iterator++) {                                  \
                    z_reco_parton_algorithmic.                      \
                        push_back(std::pair<double, int>(           \
                            iterator->second, iterator->first));    \
                }                                                   \
            }                                                       \
            std::sort(z_reco_parton_algorithmic.begin(),            \
                      z_reco_parton_algorithmic.end());             \
                                                                    \
            /* Note that z_truth is now in *acending* order */      \
                                                                    \
            for (size_t j = 0;                                      \
                 j <                                                \
                 std::min(2UL, z_reco_parton_algorithmic.size());   \
                 j++) {                                             \
                _branch_jet_ ## s ## _pdg_code_algorithmic_z        \
                    [_branch_njet_ ## s][j] =                       \
                    half(z_reco_parton_algorithmic.                 \
                         rbegin()[j].first);                        \
                _branch_jet_ ## s ## _pdg_code_algorithmic          \
                    [_branch_njet_ ## s][j] =                       \
                    z_reco_parton_algorithmic.                      \
                    rbegin()[j].second;                             \
            }                                                       \
                                                                    \
        }                                                           \
        _branch_njet_ ## s++;                                       \
        if (_branch_njet_ ## s >= NJET_MAX) {                       \
            break;                                                  \
        }                                                           \
    }


#endif // JET_H_
