#ifndef __JETTY_VAREA_CALC_HH
#define __JETTY_VAREA_CALC_HH

// based on code from Yue Shi https://github.com/yslai/ntuple-gj

#include <vector>
#include <set>
#include <map>
#include <algorithm>

#include <TDecompSVD.h>
#include <TPolyLine.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Weffc++"
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/Voronoi_diagram_2.h>
#include <CGAL/Delaunay_triangulation_adaptation_traits_2.h>
#include <CGAL/Delaunay_triangulation_adaptation_policies_2.h>
#include <CGAL/Polygon_2.h>

namespace VoronoiUtil
{

	typedef CGAL::Delaunay_triangulation_2<CGAL::Exact_predicates_inexact_constructions_kernel>
		delaunay_triangulation_t;
	typedef delaunay_triangulation_t::Point
		point_2d_t;
	typedef CGAL::Voronoi_diagram_2<delaunay_triangulation_t, CGAL::Delaunay_triangulation_adaptation_traits_2<delaunay_triangulation_t>, CGAL::Delaunay_triangulation_caching_degeneracy_removal_policy_2<delaunay_triangulation_t> >
		voronoi_diagram_t;
	typedef CGAL::Polygon_2<CGAL::Exact_predicates_inexact_constructions_kernel>
		polygon_t;

	void voronoi_insert_det_eta_max(
		voronoi_diagram_t &diagram,
		std::map<voronoi_diagram_t::Face_handle, size_t> &face_index,
		const std::vector<point_2d_t> particle_pseudorapidity_azimuth,
		const double &det_eta_max)
	{
		for (std::vector<point_2d_t>::const_iterator iterator =
				 particle_pseudorapidity_azimuth.begin();
			 iterator != particle_pseudorapidity_azimuth.end();
			 iterator++) {
			// Reflect at the boundary of |eta| = det_eta_max, to
			// cut-off the tesselation at the boundary condition via
			// "mirror tracks"
			for (int j = -1; j <= 1; j++) {
				// Make two additional replicas with azimuth +/- 2 pi
				// (and use only the middle) to mimick the cyclical
				// boundary condition
				for (int k = -1; k <= 1; k++) {
					const point_2d_t
						p(iterator->x() * (1 - 2 * (j & 1)) + j * (2 * det_eta_max),
						  iterator->y() + k * (2 * M_PI));
					const voronoi_diagram_t::Face_handle
						handle = diagram.insert(p);

					face_index[handle] = iterator -
						particle_pseudorapidity_azimuth.begin();
				}
			}
		}
	}

	void voronoi_insert_alice_tpc(
		voronoi_diagram_t &diagram,
		std::map<voronoi_diagram_t::Face_handle, size_t> &face_index,
		const std::vector<point_2d_t> particle_pseudorapidity_azimuth)
	{
		voronoi_insert_det_eta_max(diagram,
		                           face_index,
		                           particle_pseudorapidity_azimuth,
		                           0.9);
	}

	void voronoi_area_incident(
		std::vector<double> &particle_area,
		std::vector<std::set<size_t> > &particle_incident,
		const std::vector<point_2d_t> particle_pseudorapidity_azimuth,
		const double &det_eta_max)
	{
		voronoi_diagram_t diagram;
		std::map<voronoi_diagram_t::Face_handle, size_t> face_index;

		voronoi_insert_det_eta_max(diagram,
		                           face_index,
		                           particle_pseudorapidity_azimuth,
		                           det_eta_max);

		particle_area.clear();
		particle_incident = std::vector<std::set<size_t> >(
			particle_pseudorapidity_azimuth.size(),
			std::set<size_t>());

		// Extract the Voronoi cells as polygon and calculate the
		// area associated with individual particles

		for (std::vector<point_2d_t>::const_iterator iterator =
				 particle_pseudorapidity_azimuth.begin();
			 iterator != particle_pseudorapidity_azimuth.end();
			 iterator++) {
			const voronoi_diagram_t::Locate_result result =
				diagram.locate(*iterator);
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
						polygon.push_back(
							circulator->target()->point());
						particle_incident[face_index[*face]].insert(
							face_index[circulator->twin()->face()]);
					}
					else {
						unbounded = true;
						break;
					}
				}
				while (++circulator != circulator_start);
				polygon_area = unbounded ?
					INFINITY : polygon.area();
			}
			else {
				polygon_area = NAN;
			}
			particle_area.push_back(fabs(polygon_area));
		}
	}

	void voronoi_area_incident_alice_tpc(
		std::vector<double> &particle_area,
		std::vector<std::set<size_t> > &particle_incident,
		const std::vector<point_2d_t> particle_pseudorapidity_azimuth)
	{
		voronoi_area_incident(particle_area,
		                      particle_incident,
		                      particle_pseudorapidity_azimuth,
		                      0.9);
	}

	void voronoi_polygon(
		std::vector<TPolyLine> &polyline,
		const std::vector<point_2d_t> &
		particle_pseudorapidity_azimuth,
		const double &det_eta_max)
	{
		voronoi_diagram_t diagram;
		std::map<voronoi_diagram_t::Face_handle, size_t> face_index;

		voronoi_insert_det_eta_max(diagram,
		                           face_index,
		                           particle_pseudorapidity_azimuth,
		                           det_eta_max);

		// Extract the Voronoi cells as polygon and calculate the
		// area associated with individual particles

		for (std::vector<point_2d_t>::const_iterator iterator =
				 particle_pseudorapidity_azimuth.begin();
			 iterator != particle_pseudorapidity_azimuth.end();
			 iterator++) {
			const voronoi_diagram_t::Locate_result result =
				diagram.locate(*iterator);
			const voronoi_diagram_t::Face_handle *face =
				boost::get<voronoi_diagram_t::Face_handle>(&result);
			std::vector<double> x;
			std::vector<double> y;

			if (face != NULL) {
				voronoi_diagram_t::Ccb_halfedge_circulator
					circulator_start = (*face)->outer_ccb();

				voronoi_diagram_t::Ccb_halfedge_circulator
					circulator = circulator_start;

				// Circle around the edges and extract the polygon
				// vertices
				do {
					if (circulator->has_target()) {
						x.push_back(circulator->target()->point().x());
						y.push_back(circulator->target()->point().y());
					}
				}
				while (++circulator != circulator_start);
			}
			if (!x.empty()) {
				x.push_back(x.front());
				y.push_back(y.front());
			}
			polyline.push_back(TPolyLine(x.size(), &x[0], &y[0]));
		}
	}

};
#endif
