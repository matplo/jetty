#include <jetty/mljets/jet.h>

namespace mljets {

    void voronoi_insert_alice_tpc(
        voronoi_diagram_t &diagram,
        std::map<voronoi_diagram_t::Face_handle, size_t> &face_index,
        const std::vector<point_2d_t>
        particle_pseudorapidity_azimuth)
    {
        static const double pseudorapidity_limit = 0.9;

        for (std::vector<point_2d_t>::const_iterator iterator =
                 particle_pseudorapidity_azimuth.begin();
             iterator != particle_pseudorapidity_azimuth.end();
             iterator++) {
            // Reflect at ALICE TPC boundary of |eta| = 0.9, to
            // cut-off the tesselation at the boundary condition via
            // "mirror tracks"
            for (int j = -1; j <= 1; j++) {
                // Make two additional replicas with azimuth +/- 2 pi
                // (and use only the middle) to mimick the cyclical
                // boundary condition
                for (int k = -1; k <= 1; k++) {
                    const point_2d_t
                        p(iterator->x() * (1 - 2 * (j & 1)) +
                          j * (2 * pseudorapidity_limit),
                          angular_range_reduce(
                              CGAL::to_double(iterator->y()) -
                              emcal_voronoi_azimuth_0) +
                          k * (2 * M_PI));
                    const voronoi_diagram_t::Face_handle
                        handle = diagram.insert(p);

                    face_index[handle] = iterator -
                        particle_pseudorapidity_azimuth.begin();
                }
            }
        }
    }


    void create_boundary(
        polygon_t &boundary_emcal,
        polygon_t &boundary_emcal_neg,
        polygon_t &boundary_dcal,
        const std::vector<point_2d_t> cell_emcal_dcal)
    {
        if (!cell_emcal_dcal.empty()) {
            for (unsigned short i = 0; i < nboundary_emcal; i++) {
                boundary_emcal.push_back(point_2d_epeck_t(
                    cell_emcal_dcal[boundary_emcal_cell_id[i]].x(),
                    angular_range_reduce(
                        CGAL::to_double(
                            cell_emcal_dcal[boundary_emcal_cell_id
                                            [i]].y()) -
                        emcal_voronoi_azimuth_0)));
                // Note the -2 pi is outside the angular range
                // reduction
                boundary_emcal_neg.push_back(point_2d_epeck_t(
                    cell_emcal_dcal[boundary_emcal_cell_id[i]].x(),
                    angular_range_reduce(
                        CGAL::to_double(
                            cell_emcal_dcal[boundary_emcal_cell_id
                                            [i]].y()) -
                        emcal_voronoi_azimuth_0) -
                    2 * M_PI));
            }
            for (unsigned short i = 0; i < nboundary_dcal; i++) {
                boundary_dcal.push_back(point_2d_epeck_t(
                    cell_emcal_dcal[boundary_dcal_cell_id[i]].x(),
                    angular_range_reduce(
                        CGAL::to_double(
                            cell_emcal_dcal[boundary_dcal_cell_id
                                            [i]].y()) -
                        emcal_voronoi_azimuth_0)));
            }
        }
    }

    double eval_polygon_area(const std::list<polygon_hole_t> p)
    {
        double area = 0;

        // For each resulting polygon, add the outer boundary area,
        // and subtract all hole areas
        for (std::list<polygon_hole_t>::const_iterator iterator =
                 p.begin();
             iterator != p.end(); iterator++) {
            area +=
                CGAL::to_double(iterator->outer_boundary().area());
            for (polygon_hole_t::Hole_const_iterator
                     iterator_hole = iterator->holes_begin();
                 iterator_hole != iterator->holes_end();
                 iterator_hole++) {
                area -= CGAL::to_double(iterator_hole->area());
            }
        }

        return area;
    }

    void voronoi_area_incident(
        std::vector<double> &particle_area,
        std::vector<std::set<size_t> > &particle_incident,
        const std::vector<point_2d_t> particle_pseudorapidity_azimuth,
        const std::vector<point_2d_t> cell_emcal_dcal)
    {
        voronoi_diagram_t diagram;
        std::map<voronoi_diagram_t::Face_handle, size_t> face_index;

        // For the moment, insert EMCAL/DCAL clusters like TPC tracks,
        // to avoid open polygons with rays (the polygons will be cut
        // on the exact cell boundary later)
        voronoi_insert_alice_tpc(diagram, face_index,
                                 particle_pseudorapidity_azimuth);

        particle_area.clear();
        particle_incident = std::vector<std::set<size_t> >(
            particle_pseudorapidity_azimuth.size(),
            std::set<size_t>());

        // Initialize the (event-by-event) EMCAL and DCAL boundary
        // polygons
        polygon_t boundary_emcal;
        polygon_t boundary_emcal_neg;
        polygon_t boundary_dcal;

        create_boundary(boundary_emcal, boundary_emcal_neg,
                        boundary_dcal, cell_emcal_dcal);

        // Extract the Voronoi cells as polygon and calculate the
        // area associated with individual particles

        for (std::vector<point_2d_t>::const_iterator iterator =
                 particle_pseudorapidity_azimuth.begin();
             iterator != particle_pseudorapidity_azimuth.end();
             iterator++) {
            const point_2d_t
                p(iterator->x(),
                  iterator->y() - emcal_voronoi_azimuth_0);
            const voronoi_diagram_t::Locate_result result =
                diagram.locate(p);
            const voronoi_diagram_t::Face_handle *face =
                boost::get<voronoi_diagram_t::Face_handle>(&result);
            double polygon_area;

            if (face != NULL) {
                voronoi_diagram_t::Ccb_halfedge_circulator
                    circulator_start = (*face)->outer_ccb();
                bool unbounded = false;
                polygon_t polygon;

                voronoi_diagram_t::Ccb_halfedge_circulator
                    circulator = circulator_start;

                // Circle around the edges and extract the polygon
                // vertices
                do {
                    if (circulator->has_target()) {
                        polygon.push_back(point_2d_epeck_t(
                            circulator->target()->point().x(),
                            circulator->target()->point().y()));
                        particle_incident[face_index[*face]].insert(
                            face_index[circulator->twin()->face()]);
                    }
                    else {
                        unbounded = true;
                        break;
                    }
                }
                while (++circulator != circulator_start);
                if (!cell_emcal_dcal.empty()) {
                    const bool is_emcal =
                        angular_range_reduce(
                            CGAL::to_double(iterator->y()) -
                            emcal_voronoi_azimuth_0) >= 0;
                    std::list<polygon_hole_t> polygon_boundary;

                    // Cut to the detector boundary. Decide by the
                    // sign of the particle phi ("y()") if the
                    // boundary is that of the EMCAL or DCAL.
                    CGAL::intersection(
                        polygon,
                        is_emcal ? boundary_emcal : boundary_dcal,
                        std::back_inserter(polygon_boundary));
                    if (is_emcal) {
                        CGAL::intersection(
                            polygon, boundary_emcal_neg,
                            std::back_inserter(polygon_boundary));
                    }
                    polygon_area =
                        eval_polygon_area(polygon_boundary);
                }
                else {
                    polygon_area = unbounded ?
                        INFINITY : CGAL::to_double(polygon.area());
                }
            }
            else {
                polygon_area = NAN;
            }
            particle_area.push_back(fabs(polygon_area));
        }
    }

    std::pair<CGAL::Bbox_2, CGAL::Bbox_2>
    disjoined_dcal_bbox(const polygon_t b)
    {
        // Cut at eta = -0.7 (which lies between -0.7180 of SM 16, 17,
        // and -0.6781 of the SM 18, 19)
        polygon_t cut;

        cut.push_back(point_2d_epeck_t( 0.9, -1.8));
        cut.push_back(point_2d_epeck_t( 0.9, -0.7));
        cut.push_back(point_2d_epeck_t(-0.9, -0.7));
        cut.push_back(point_2d_epeck_t(-0.9, -1.8));

        std::list<polygon_hole_t> dcal_cut;

        CGAL::intersection(b, cut, std::back_inserter(dcal_cut));

        if (dcal_cut.size() == 2) {
            return std::pair<CGAL::Bbox_2, CGAL::Bbox_2>(
                dcal_cut.front().bbox(), dcal_cut.back().bbox());
        }
        else {
            polygon_t far_away;

            far_away.push_back(point_2d_epeck_t(6, 0));
            far_away.push_back(point_2d_epeck_t(6, 1));
            far_away.push_back(point_2d_epeck_t(5, 1));
            far_away.push_back(point_2d_epeck_t(5, 0));

            return std::pair<CGAL::Bbox_2, CGAL::Bbox_2>(
                far_away.bbox(), far_away.bbox());
        }
    }

    bool fully_contained(const CGAL::Bbox_2 t, const CGAL::Bbox_2 b)
    {
        // Safe distance to the edge, being at least the size of one
        // cell (0.0143) plus non-projectiveness in pseudorapidity
        // (0.0072)
        static const double border = 0.022;

        return (t.xmin() > b.xmin() + border &&
                t.xmax() < b.xmax() - border &&
                t.ymin() > b.ymin() + border &&
                t.ymax() < b.ymax() - border);
    }

    void voronoi_polygon(
        std::vector<TPolyLine> &polyline,
        const std::vector<point_2d_t> &
        particle_pseudorapidity_azimuth,
        const std::vector<point_2d_t> cell_emcal_dcal)
    {
        voronoi_diagram_t diagram;
        std::map<voronoi_diagram_t::Face_handle, size_t> face_index;

        voronoi_insert_alice_tpc(diagram, face_index,
                                 particle_pseudorapidity_azimuth);

        polygon_t boundary_emcal;
        polygon_t boundary_emcal_neg;
        polygon_t boundary_dcal;

        create_boundary(boundary_emcal, boundary_emcal_neg,
                        boundary_dcal, cell_emcal_dcal);

        std::pair<CGAL::Bbox_2, CGAL::Bbox_2> dcal_bbox =
            disjoined_dcal_bbox(boundary_dcal);

        for (std::vector<point_2d_t>::const_iterator iterator =
                 particle_pseudorapidity_azimuth.begin();
             iterator != particle_pseudorapidity_azimuth.end();
             iterator++) {
            const point_2d_t
                p(iterator->x(),
                  iterator->y() - emcal_voronoi_azimuth_0);
            const voronoi_diagram_t::Locate_result result =
                diagram.locate(p);
            const voronoi_diagram_t::Face_handle *face =
                boost::get<voronoi_diagram_t::Face_handle>(&result);

            if (face != NULL) {
                voronoi_diagram_t::Ccb_halfedge_circulator
                    circulator_start = (*face)->outer_ccb();
                polygon_t polygon;

                voronoi_diagram_t::Ccb_halfedge_circulator
                    circulator = circulator_start;

                // Circle around the edges and extract the polygon
                // vertices
                do {
                    if (circulator->has_target()) {
                        polygon.push_back(point_2d_epeck_t(
                            circulator->target()->point().x(),
                            circulator->target()->point().y()));
                    }
                }
                while (++circulator != circulator_start);
                if (!cell_emcal_dcal.empty()) {
                    const bool is_emcal =
                        angular_range_reduce(
                            CGAL::to_double(iterator->y()) -
                            emcal_voronoi_azimuth_0) >= 0;
                    std::list<polygon_hole_t> polygon_boundary;

                    const polygon_t &boundary = is_emcal ?
                        boundary_emcal : boundary_dcal;

                    if (is_emcal &&
                        fully_contained(polygon.bbox(),
                                        boundary_emcal.bbox())) {
                        polygon_boundary.push_back(
                            polygon_hole_t(polygon));
                    }
                    else if (!is_emcal &&
                             (fully_contained(polygon.bbox(),
                                              dcal_bbox.first) ||
                              fully_contained(polygon.bbox(),
                                              dcal_bbox.second))) {
                        polygon_boundary.push_back(
                            polygon_hole_t(polygon));
                    }
                    else if (CGAL::do_overlap(polygon.bbox(),
                                              boundary.bbox())) {
                        CGAL::intersection(
                            polygon, boundary,
                            std::back_inserter(polygon_boundary));
                    }
                    if (is_emcal &&
                        CGAL::do_overlap(polygon.bbox(),
                                         boundary_emcal_neg.bbox())) {
                        CGAL::intersection(
                            polygon, boundary_emcal_neg,
                            std::back_inserter(polygon_boundary));
                    }

                    for (std::list<polygon_hole_t>::const_iterator
                             iterator_polygon =
                             polygon_boundary.begin();
                         iterator_polygon != polygon_boundary.end();
                         iterator_polygon++) {
                        const polygon_t outer =
                            iterator_polygon->outer_boundary();
                        std::vector<double> x;
                        std::vector<double> y;

                        for (polygon_t::Vertex_const_iterator
                                 iterator_vertex = outer.vertices_begin();
                             iterator_vertex != outer.vertices_end();
                             iterator_vertex++) {
                            x.push_back(
                                CGAL::to_double(iterator_vertex->x()));
                            y.push_back(
                                CGAL::to_double(iterator_vertex->y()) +
                                emcal_voronoi_azimuth_0);
                        }
                        if (!x.empty()) {
                            x.push_back(x.front());
                            y.push_back(y.front());
                        }
                        polyline.push_back(TPolyLine(
                            x.size(), &x[0], &y[0]));
                    }
                }
                else {
                    std::vector<double> x;
                    std::vector<double> y;

                    for (polygon_t::Vertex_const_iterator
                             iterator_vertex = polygon.vertices_begin();
                         iterator_vertex != polygon.vertices_end();
                         iterator_vertex++) {
                        x.push_back(
                            CGAL::to_double(iterator_vertex->x()));
                        y.push_back(
                            CGAL::to_double(iterator_vertex->y()) +
                            emcal_voronoi_azimuth_0);
                    }
                    if (!x.empty()) {
                        x.push_back(x.front());
                        y.push_back(y.front());
                    }
                    polyline.push_back(TPolyLine(
                        x.size(), &x[0], &y[0]));
                }
            }
        }
    }

    double voronoi_area(const fastjet::PseudoJet jet,
                        const fastjet::ClusterSequenceArea
                        cluster_sequence,
                        const std::vector<double> particle_area)
    {
        std::vector<fastjet::PseudoJet> constituent =
            cluster_sequence.constituents(jet);
        double sum_area = 0;
        double sum_area_kahan_error = 0;

        for (std::vector<fastjet::PseudoJet>::const_iterator
                 iterator_constituent = constituent.begin();
             iterator_constituent != constituent.end();
             iterator_constituent++) {
            const int index = iterator_constituent->user_index();

            if (index >= 0 && static_cast<size_t>(index) <
                particle_area.size() &&
                std::isfinite(particle_area[index])) {
                kahan_sum(sum_area, sum_area_kahan_error,
                          particle_area[index]);
            }
        }

        return sum_area;
    }

    std::vector<double> rho_order_statistics(
        fastjet::ClusterSequenceArea cluster_sequence,
        std::vector<double> particle_area)
    {
        const std::vector<fastjet::PseudoJet> jet =
            cluster_sequence.inclusive_jets(0);
        std::vector<double> rho;

        for (std::vector<fastjet::PseudoJet>::const_iterator
                 iterator = jet.begin();
             iterator != jet.end(); iterator++) {
            const double area =
                voronoi_area(*iterator, cluster_sequence,
                             particle_area);

            if (area > 0) {
                rho.push_back(iterator->perp() / area);
            }
        }
        std::sort(rho.begin(), rho.end());

        return rho;
    }

    void quantile_harrell_davis(double &q, double &q_se,
                                const std::vector<double> x,
                                double p)
    {
        // See F. E. Harrell, C. E. Davis, "A new distribution-free
        // quantile estimator", Biometrika 69(3), 635--640,
        // https://doi.org/10.1093/biomet/69.3.635 , p. 646, eq. (2)
        // and (3); and Harrell's implementation for R at
        // https://github.com/harrelfe/Hmisc/blob/master/R/Misc.s

        // CEPHES' incbet(), wrapped by ROOT, is already the
        // regularized form, or beta distribution, denoted I_x(a, b)
        // in NBS AMS 55 6.6.2 and NIST DLMF 8.17.2
        // https://dlmf.nist.gov/8.17#E2
        double (*incbeta)(double, double, double) =
            &ROOT::Math::inc_beta;
        const double n = x.size();
        const double m = n + 1; // Following Harrell's Misc.s
        const double p_m = p * m;
        const double not_p_m = (1 - p) * m;

        q = 0;

        double q_kahan_error = 0;
        double b0 = 0;

        for (size_t i = 0; i < x.size(); i++) {
            const double b1 =
                incbeta((static_cast<double>(i) + 1) / n,
                        p_m, not_p_m);
            const double w = b1 - b0;

            kahan_sum(q, q_kahan_error, w * x[i]);
            b0 = b1;
        }

        // Jackknifed standard error

        // Calculate the jackknifed weights
        const double l = n - 1; // Following Harrell's Misc.s
        std::vector<double> w;

        b0 = 0;
        for (size_t i = 1; i < x.size(); i++) {
            const double b1 =
                incbeta((static_cast<double>(i)) / l,
                        p_m, not_p_m);

            w.push_back(b1 - b0);
            b0 = b1;
        }

        // Calculate the jackknifed L-statistic and its mean
        std::vector<double> s;
        double s_mean = 0;
        double s_mean_kahan_error = 0;

        for (size_t i = 0; i < x.size(); i++) {
            s.push_back(0);

            double s_kahan_error = 0;

            for (size_t j = 1; j < x.size(); j++) {
                kahan_sum(s.back(), s_kahan_error,
                          w[j - 1] * (j < i ? x[j - 1] : x[j]));
            }
            kahan_sum(s_mean, s_mean_kahan_error, s.back());
        }
        s_mean /= n;

        // Calculate the variance
        double u2 = 0;
        double u2_kahan_error = 0;

        for (std::vector<double>::const_iterator iterator =
                 s.begin();
             iterator != s.end(); iterator++) {
            kahan_sum(u2, u2_kahan_error,
                      std::pow(*iterator - s_mean, 2));
        }
        q_se = sqrt(l * u2) / n;
    }

    std::pair<std::pair<std::vector<double>, std::vector<double> >,
              double>
    ue_estimation_median(fastjet::ClusterSequenceArea
                         cluster_sequence,
                         std::vector<double> particle_area)
    {
        const std::vector<double> rho_order_statistics_ =
            rho_order_statistics(cluster_sequence,
                                 particle_area);
        double rho_median = 0;
        double rho_median_standard_error = 0;

        if (!rho_order_statistics_.empty()) {
            quantile_harrell_davis(rho_median,
                                   rho_median_standard_error,
                                   rho_order_statistics_, 0.5);
        }

        const std::vector<double>
            pseudorapidity_dependence(1, rho_median);
        const std::vector<double> azimuth_dependence(1, 1);

        return std::pair<std::pair<std::vector<double>,
                                   std::vector<double> >, double>(
            std::pair<std::vector<double>, std::vector<double> >(
                pseudorapidity_dependence, azimuth_dependence),
            rho_median_standard_error);
    }

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
                    double quantile)
    {
        if (rho_vs_jet_unsorted.empty()) {
            return;
        }

        std::vector<std::pair<
            double, std::vector<fastjet::PseudoJet>::
            const_iterator> > rho_vs_jet = rho_vs_jet_unsorted;

        std::sort(rho_vs_jet.begin(), rho_vs_jet.end());

        const size_t iterator_margin =
            floor(0.5 * (1 - quantile) * rho_vs_jet.size());

        for (std::vector<std::pair<
                 double, std::vector<fastjet::PseudoJet>::
                 const_iterator> >::const_iterator
                 iterator_rho_vs_jet =
                 rho_vs_jet.begin() + iterator_margin;
             iterator_rho_vs_jet !=
                 rho_vs_jet.end() - iterator_margin;
             iterator_rho_vs_jet++) {
            std::vector<fastjet::PseudoJet> constituent =
                cluster_sequence.
                constituents(*iterator_rho_vs_jet->second);

            for (std::vector<fastjet::PseudoJet>::const_iterator
                     iterator_constituent = constituent.begin();
                 iterator_constituent != constituent.end();
                 iterator_constituent++) {
                const int index = iterator_constituent->user_index();

                if (index >= 0 && static_cast<size_t>(index) <
                    particle_area.size() &&
                    std::isfinite(particle_area[index]) &&
                    constituent_truncated_user_index.find(index) ==
                    constituent_truncated_user_index.end()) {
                    constituent_truncated.push_back(
                        *iterator_constituent);
                    constituent_truncated_user_index.insert(index);
                }
            }
        }
    }

    void constituent_quantile(
        std::vector<fastjet::PseudoJet> &constituent_truncated,
        std::set<int> &constituent_truncated_user_index,
        fastjet::ClusterSequenceArea cluster_sequence,
        std::vector<double> particle_area,
        size_t order_azimuth_fourier, double quantile)
    {
        const std::vector<fastjet::PseudoJet> jet =
            cluster_sequence.inclusive_jets(0);

        static const unsigned int nwindow_azimuth_max = 24U;

        // Since the windows are staggered by 2x, there are 2 *
        // quantile * jet.size() / nwindow per window;

        const unsigned int nwindow_azimuth =
            std::max(1U, std::min(
                nwindow_azimuth_max, static_cast<unsigned int>(
                    floor(quantile * jet.size()))));
        const double azimuth_window_width =
            2 * M_PI / nwindow_azimuth;

        for (size_t i = 0; i < nwindow_azimuth; i++) {
            const double azimuth_window_center =
                i * (2 * M_PI / nwindow_azimuth) - M_PI;
            std::vector<std::pair<
                double, std::vector<fastjet::PseudoJet>::
                const_iterator> > rho_vs_jet;

            for (std::vector<fastjet::PseudoJet>::const_iterator
                     iterator = jet.begin();
                 iterator != jet.end(); iterator++) {
                if (fabs(angular_range_reduce(
                        iterator->phi_std() - azimuth_window_center)) <
                    azimuth_window_width) {
                    const double area =
                        voronoi_area(*iterator, cluster_sequence,
                                     particle_area);

                    rho_vs_jet.push_back(
                        std::pair<double, std::vector<
                        fastjet::PseudoJet>::const_iterator>(
                            iterator->perp() / area,
                            iterator));
                }
            }
            append_quantile(constituent_truncated,
                            constituent_truncated_user_index,
                            rho_vs_jet, cluster_sequence,
                            particle_area, quantile);
        }

        const size_t nwindow_azimuth_nyquist =
            4 * order_azimuth_fourier;
        const double azimuth_window_width_nyquist =
            2 * M_PI / nwindow_azimuth_nyquist;
        std::vector<fastjet::PseudoJet> constituent_guard;

        for (size_t i = 0; i < nwindow_azimuth_nyquist; i++) {
            const double azimuth_window_center =
                i * (2 * M_PI / nwindow_azimuth_nyquist) - M_PI;
            size_t count = 0;

            for (std::vector<fastjet::PseudoJet>::const_iterator
                     iterator = constituent_truncated.begin();
                 iterator != constituent_truncated.end();
                 iterator++) {
                if (fabs(angular_range_reduce(
                        iterator->phi_std() - azimuth_window_center)) <
                    azimuth_window_width_nyquist) {
                    count++;
                }
            }
            if (count <= 4) {
                for (int j = -9; j <= 9; j += 3) {
                    fastjet::PseudoJet p;

                    p.reset_PtYPhiM(1, 0.1 * j, azimuth_window_center, 0);
                    constituent_guard.push_back(p);
                }
            }
        }
        constituent_truncated.insert(
            constituent_truncated.end(),
            constituent_guard.begin(), constituent_guard.end());
    }

    std::pair<std::vector<double>, std::vector<double> >
    ue_estimation_truncated_mean(
        fastjet::ClusterSequenceArea cluster_sequence,
        std::vector<double> particle_area,
        size_t order_pseudorapidity_chebyshev,
        size_t order_azimuth_fourier,
        double quantile)
    {
        std::vector<fastjet::PseudoJet> constituent_truncated;
        std::set<int> constituent_truncated_user_index;

        constituent_quantile(constituent_truncated,
                             constituent_truncated_user_index,
                             cluster_sequence, particle_area,
                             order_azimuth_fourier, quantile);

        std::vector<double> pseudorapidity_dependence;
        static const double sqrt_area_empty = 1;

        if (!constituent_truncated.empty()) {
            order_pseudorapidity_chebyshev =
                std::min(order_pseudorapidity_chebyshev,
                         constituent_truncated.size() - 1);

            TMatrixD a(constituent_truncated.size(),
                       order_pseudorapidity_chebyshev + 1);
            TVectorD b(constituent_truncated.size());
            size_t row = 0;

            for (std::vector<fastjet::PseudoJet>::const_iterator
                     iterator = constituent_truncated.begin();
                 iterator != constituent_truncated.end();
                 iterator++) {
                const double sqrt_area =
                    iterator->user_index() >= 0 ?
                    sqrt(particle_area[iterator->user_index()]) :
                    sqrt_area_empty;

                a(row, 0) = sqrt_area;

                // The convenience of ALICE central tracks being from
                // pseudorapidity -0.9 to 0.9 (close to -1 to 1) is
                // taken advantage to avoid a linear transform for the
                // Chebyshev polynomials

                const double x = iterator->pseudorapidity();

                if (order_pseudorapidity_chebyshev >= 1) {
                    a(row, 1) = x * sqrt_area;
                }

                // t[0] is T_n(x), t[1] is T_{n - 1}(x)
                double t[2] = { x, 1 };

                for (size_t j = 2;
                     j < order_pseudorapidity_chebyshev + 1; j++) {
                    const double tn1 = 2 * x * t[0] - t[1];

                    a(row, j) = tn1 * sqrt_area;
                    t[1] = t[0];
                    t[0] = tn1;
                }
                b(row) = iterator->user_index() >= 0 ?
                    iterator->perp() / sqrt_area : 0;
                row++;
            }

            TDecompSVD a_svd(a);
            Bool_t status;
            TVectorD x = a_svd.Solve(b, status);

            if (status != kFALSE) {
                for (size_t i = 0;
                     i < order_pseudorapidity_chebyshev + 1; i++) {
                    pseudorapidity_dependence.push_back(x(i));
                }
            }
        }

        std::vector<double> azimuth_dependence;

        if (!constituent_truncated.empty()) {
            order_azimuth_fourier =
                std::min(order_azimuth_fourier,
                         (constituent_truncated.size() - 1) / 2);

            TMatrixD a(constituent_truncated.size(),
                       2 * order_azimuth_fourier + 1);
            TVectorD b(constituent_truncated.size());
            size_t row = 0;

            for (std::vector<fastjet::PseudoJet>::const_iterator
                     iterator = constituent_truncated.begin();
                 iterator != constituent_truncated.end();
                 iterator++) {
                const double sqrt_area =
                    iterator->user_index() >= 0 ?
                    sqrt(particle_area[iterator->user_index()]) :
                    sqrt_area_empty;

                a(row, 0) = sqrt_area;

                const double azimuth = iterator->phi_std();

                for (size_t j = 0; j < order_azimuth_fourier; j++) {
                    a(row, 2 * j + 1) = cos((j + 1) * azimuth) * sqrt_area;
                    a(row, 2 * j + 2) = sin((j + 1) * azimuth) * sqrt_area;
                }
                b(row) = iterator->user_index() >= 0 ?
                    iterator->perp() / sqrt_area : 0;
                row++;
            }

            TDecompSVD a_svd(a);
            Bool_t status;
            TVectorD x = a_svd.Solve(b, status);

            if (status != kFALSE) {
                for (size_t i = 0; i < 2 * order_azimuth_fourier + 1;
                     i++) {
                    azimuth_dependence.push_back(x(i));
                }
            }
        }

        return std::pair<
            std::vector<double>, std::vector<double> >(
                pseudorapidity_dependence, azimuth_dependence);
    }

    std::set<int> ue_user_index_truncated_mean(
        fastjet::ClusterSequenceArea cluster_sequence,
        std::vector<double> particle_area,
        size_t order_pseudorapidity_chebyshev,
        size_t order_azimuth_fourier,
        double quantile)
    {
        std::vector<fastjet::PseudoJet> constituent_truncated;
        std::set<int> constituent_truncated_user_index;

        constituent_quantile(constituent_truncated,
                             constituent_truncated_user_index,
                             cluster_sequence, particle_area,
                             order_azimuth_fourier, quantile);

        return constituent_truncated_user_index;
    }

    double evaluate_ue(std::pair<std::vector<double>,
                       std::vector<double> > ue_estimate,
                       double pseudorapidity, double azimuth)
    {
        if (ue_estimate.first.empty() ||
            ue_estimate.second.empty()) {
            return 0;
        }

        double p = ue_estimate.first[0];

        if (ue_estimate.first.size() >= 1) {
            p += ue_estimate.first[1] * pseudorapidity;
        }

        const double x = pseudorapidity;
        // t[0] is T_n(x), t[1] is T_{n - 1}(x)
        double t[2] = { x, 1 };

        for (size_t i = 2; i < ue_estimate.first.size(); i++) {
            const double tn1 = 2 * x * t[0] - t[1];

            p += ue_estimate.first[i] * tn1;
            t[0] = tn1;
            t[1] = t[0];
        }

        double a = ue_estimate.second[0];

        for (size_t i = 0; i < (ue_estimate.second.size() - 1) / 2; i++) {
            const double v =
                sqrt(std::pow(ue_estimate.second[2 * i + 2], 2) +
                     std::pow(ue_estimate.second[2 * i + 1], 2));
            const double psi = atan2(ue_estimate.second[2 * i + 2],
                                     ue_estimate.second[2 * i + 1]);
            const double k = i + 1;

            a += v * cos(k * azimuth - psi);
        }

        return std::max(0.0, p * a / ue_estimate.second[0]);
    }

    double evaluate_ue_constant(std::pair<std::vector<double>,
                                std::vector<double> > ue_estimate)
    {
        if (ue_estimate.first.empty() ||
            ue_estimate.second.empty()) {
            return 0;
        }

        const double p = ue_estimate.first[0];

        return std::max(0.0, p);
    }

    double jet_emf(const std::vector<fastjet::PseudoJet> constituent,
                   double scale_em_ghost)
    {
        double sum_hadronic = 0;
        double sum_em = 0;

        for (std::vector<fastjet::PseudoJet>::const_iterator
                 iterator_constituent = constituent.begin();
             iterator_constituent != constituent.end();
             iterator_constituent++) {
            switch (iterator_constituent->user_index()) {
            case USER_INDEX_DEFAULT_OR_TRACK:
                sum_hadronic += iterator_constituent->perp();
                break;
            case USER_INDEX_EM:
                sum_em += iterator_constituent->perp();
                break;
            }
        }

        return sum_hadronic + sum_em > 0 ?
            sum_em / (sum_hadronic * scale_em_ghost + sum_em) :
            NAN;
    }

    size_t jet_multiplicity(const std::vector<fastjet::PseudoJet>
                            constituent)
    {
        size_t multiplicity = 0;

        for (std::vector<fastjet::PseudoJet>::const_iterator
                 iterator_constituent = constituent.begin();
             iterator_constituent != constituent.end();
             iterator_constituent++) {
            switch (iterator_constituent->user_index()) {
            case USER_INDEX_DEFAULT_OR_TRACK:
            case USER_INDEX_EM:
            case USER_INDEX_MUON:
                multiplicity++;
                break;
            }
        }

        return multiplicity;
    }

    double constituent_perp(const fastjet::PseudoJet constituent,
                            double scale_em_ghost)
    {
        switch (constituent.user_index()) {
        case USER_INDEX_DEFAULT_OR_TRACK:
        case USER_INDEX_MUON:
            return constituent.perp();
        case USER_INDEX_EM:
            return constituent.perp() / scale_em_ghost;
        default:
            return 0;
        }
    }

    double jet_ptd(const std::vector<fastjet::PseudoJet> constituent,
                   double scale_em_ghost)
    {
        double sum_1 = 0;
        double sum_2 = 0;

        for (std::vector<fastjet::PseudoJet>::const_iterator
                 iterator_constituent = constituent.begin();
             iterator_constituent != constituent.end();
             iterator_constituent++) {
            const double perp =
                constituent_perp(*iterator_constituent,
                                 scale_em_ghost);

            sum_1 += perp;
            sum_2 += std::pow(perp, 2);
        }

        // The default value is the one particle limit

        return sum_1 > 0 ? sqrt(sum_2) / sum_1 : 1;
    }

    void jet_width_sigma(double sigma[],
                         const fastjet::PseudoJet jet,
                         const std::vector<fastjet::PseudoJet>
                         constituent,
                         double scale_em_ghost)
    {
        double m11 = 0;
        double m22 = 0;
        double m12_m21 = 0;
        double sum_2 = 0;

        for (std::vector<fastjet::PseudoJet>::const_iterator
                 iterator_constituent = constituent.begin();
             iterator_constituent != constituent.end();
             iterator_constituent++) {
            const double perp_2 =
                std::pow(constituent_perp(*iterator_constituent,
                                          scale_em_ghost), 2);
            const double dpseudorapidity =
                iterator_constituent->pseudorapidity() -
                jet.pseudorapidity();
            const double dazimuth = angular_range_reduce(
                iterator_constituent->phi_std() -
                jet.phi_std());

            m11 += perp_2 * std::pow(dpseudorapidity, 2);
            m22 += perp_2 * std::pow(dazimuth, 2);
            m12_m21 -= perp_2 * dpseudorapidity * dazimuth;
            sum_2 += perp_2;
        }

        // a = -1 in the equation for eigenvalues
        const double b = m11 + m22;
        const double c = std::pow(m12_m21, 2) - m11 * m22;
        const double q = -0.5 *
            (b + copysign(1, b) * sqrt(std::pow(b, 2) + 4 * c));

        // Major axis, x2
        sigma[0] = sqrt(c / (q * sum_2));
        // Minor axis, x1
        sigma[1] = sqrt(q / (-sum_2));
    }

}