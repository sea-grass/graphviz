#include "svg_analyzer.h"
#include "svgpp_document_traverser.h"

// most of this is based on
// http://svgpp.org/lesson01.html#handling-shapes-geometry

void SVGAnalyzer::loadSvg(char *text) {
  SvgppDocumentTraverser traverser;
  traverser.traverseSvg(*this, text);
}

void SVGAnalyzer::on_enter_element(svgpp::tag::element::svg e) {
  (void)e;
  m_num_svgs++;
}

void SVGAnalyzer::on_enter_element(svgpp::tag::element::g e) {
  (void)e;
  m_num_groups++;
}

void SVGAnalyzer::on_enter_element(svgpp::tag::element::circle e) {
  (void)e;
  m_num_circles++;
}

void SVGAnalyzer::on_enter_element(svgpp::tag::element::ellipse e) {
  (void)e;
  m_num_ellipses++;
}

void SVGAnalyzer::on_enter_element(svgpp::tag::element::line e) {
  (void)e;
  m_num_lines++;
}

void SVGAnalyzer::on_enter_element(svgpp::tag::element::path e) {
  (void)e;
  m_num_paths++;
}

void SVGAnalyzer::on_enter_element(svgpp::tag::element::polygon e) {
  (void)e;
  m_num_polygons++;
}

void SVGAnalyzer::on_enter_element(svgpp::tag::element::polyline e) {
  (void)e;
  m_num_polylines++;
}

void SVGAnalyzer::on_enter_element(svgpp::tag::element::rect e) {
  (void)e;
  m_num_rects++;
}

void SVGAnalyzer::on_enter_element(svgpp::tag::element::title e) {
  (void)e;
  m_num_titles++;
}

void SVGAnalyzer::on_enter_element(svgpp::tag::element::any) {
  m_num_unknowns++;
}

void SVGAnalyzer::on_exit_element() {
  // ignore for now
}

void SVGAnalyzer::path_move_to(double x, double y,
                               svgpp::tag::coordinate::absolute c) {
  (void)x;
  (void)y;
  (void)c;
  // ignore for now
}

void SVGAnalyzer::path_line_to(double x, double y,
                               svgpp::tag::coordinate::absolute c) {
  (void)x;
  (void)y;
  (void)c;
  // ignore for now
}

void SVGAnalyzer::path_cubic_bezier_to(double x1, double y1, double x2,
                                       double y2, double x, double y,
                                       svgpp::tag::coordinate::absolute c) {
  (void)x1;
  (void)y1;
  (void)x2;
  (void)y2;
  (void)x;
  (void)y;
  (void)c;
  // ignore for now
}

void SVGAnalyzer::path_quadratic_bezier_to(double x1, double y1, double x,
                                           double y,
                                           svgpp::tag::coordinate::absolute c) {
  (void)x1;
  (void)y1;
  (void)x;
  (void)y;
  (void)c;
  // ignore for now
}

void SVGAnalyzer::path_elliptical_arc_to(double rx, double ry,
                                         double x_axis_rotation,
                                         bool large_arc_flag, bool sweep_flag,
                                         double x, double y,
                                         svgpp::tag::coordinate::absolute c) {
  (void)rx;
  (void)ry;
  (void)x_axis_rotation;
  (void)large_arc_flag;
  (void)sweep_flag;
  (void)x;
  (void)y;
  (void)c;
  // ignore for now
}

void SVGAnalyzer::path_close_subpath() {
  // ignore for now
}

void SVGAnalyzer::path_exit() {
  // ignore for now
}

void SVGAnalyzer::set(svgpp::tag::attribute::cy a, const double &v) {
  (void)a;
  (void)v;
  // ignore for now
}

void SVGAnalyzer::set(svgpp::tag::attribute::cx a, const double &v) {
  (void)a;
  (void)v;
  // ignore for now
}

void SVGAnalyzer::set(svgpp::tag::attribute::r a, const double &v) {
  (void)a;
  (void)v;
  // ignore for now
}

void SVGAnalyzer::set(svgpp::tag::attribute::rx a, const double &v) {
  (void)a;
  (void)v;
  // ignore for now
}

void SVGAnalyzer::set(svgpp::tag::attribute::ry a, const double &v) {
  (void)a;
  (void)v;
  // ignore for now
}

void SVGAnalyzer::set(svgpp::tag::attribute::x1 a, const double &v) {
  (void)a;
  (void)v;
  // ignore for now
}

void SVGAnalyzer::set(svgpp::tag::attribute::y1 a, const double &v) {
  (void)a;
  (void)v;
  // ignore for now
}

void SVGAnalyzer::set(svgpp::tag::attribute::x2 a, const double &v) {
  (void)a;
  (void)v;
  // ignore for now
}

void SVGAnalyzer::set(svgpp::tag::attribute::y2 a, const double &v) {
  (void)a;
  (void)v;
  // ignore for now
}

void SVGAnalyzer::set(svgpp::tag::attribute::x a, const double &v) {
  (void)a;
  (void)v;
  // ignore for now
}

void SVGAnalyzer::set(svgpp::tag::attribute::y a, const double &v) {
  (void)a;
  (void)v;
  // ignore for now
}

void SVGAnalyzer::set(svgpp::tag::attribute::width a, const double &v) {
  (void)a;
  (void)v;
  // ignore for now
}

void SVGAnalyzer::set(svgpp::tag::attribute::height a, const double &v) {
  (void)a;
  (void)v;
  // ignore for now
}

void SVGAnalyzer::set(svgpp::tag::attribute::points &points,
                      const std::any &range) {
  (void)points;
  (void)range;
  // ignore for now
}

void SVGAnalyzer::set_text(const std::any &range) {
  (void)range;
  // ignore for now
}
