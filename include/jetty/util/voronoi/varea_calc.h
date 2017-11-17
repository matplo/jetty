#ifndef __JETTY_VAREA_CALC_HH
#define __JETTY_VAREA_CALC_HH

// based on code from Yue Shi https://github.com/yslai/ntuple-gj

#include <vector>
#include <set>
#include <map>

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
		const double &det_eta_max);

	void voronoi_insert_alice_tpc(
		voronoi_diagram_t &diagram,
		std::map<voronoi_diagram_t::Face_handle, size_t> &face_index,
		const std::vector<point_2d_t> particle_pseudorapidity_azimuth);

	void voronoi_area_incident(
		std::vector<double> &particle_area,
		std::vector<std::set<size_t> > &particle_incident,
		const std::vector<point_2d_t> particle_pseudorapidity_azimuth,
		const double &det_eta_max);

	void voronoi_area_incident_alice_tpc(
		std::vector<double> &particle_area,
		std::vector<std::set<size_t> > &particle_incident,
		const std::vector<point_2d_t> particle_pseudorapidity_azimuth);

	void voronoi_polygon(
		std::vector<TPolyLine> &polyline,
		const std::vector<point_2d_t> &
		particle_pseudorapidity_azimuth,
		const double &det_eta_max);

};
#endif
