#pragma once

#include <cstddef>

#include <svgpp/svgpp.hpp>

#include "svgpp_context_interface.h"

/**
 * @brief The SVGParser class analyzes the contents of an SVG document.
 *
 * The ISvgppContext functions are based on
 * http://svgpp.org/lesson01.html#handling-shapes-geometry
 */

class SVGAnalyzer : public ISvgppContext {
public:
  void on_enter_element(svgpp::tag::element::svg e) override;
  void on_enter_element(svgpp::tag::element::g e) override;
  void on_enter_element(svgpp::tag::element::circle e) override;
  void on_enter_element(svgpp::tag::element::ellipse e) override;
  void on_enter_element(svgpp::tag::element::line e) override;
  void on_enter_element(svgpp::tag::element::path e) override;
  void on_enter_element(svgpp::tag::element::polygon e) override;
  void on_enter_element(svgpp::tag::element::polyline e) override;
  void on_enter_element(svgpp::tag::element::rect e) override;
  void on_enter_element(svgpp::tag::element::title e) override;
  void on_enter_element(svgpp::tag::element::any e) override;
  void on_exit_element() override;
  void path_move_to(double x, double y,
                    svgpp::tag::coordinate::absolute) override;
  void path_line_to(double x, double y,
                    svgpp::tag::coordinate::absolute) override;
  void path_cubic_bezier_to(double x1, double y1, double x2, double y2,
                            double x, double y,
                            svgpp::tag::coordinate::absolute) override;
  void path_quadratic_bezier_to(double x1, double y1, double x, double y,
                                svgpp::tag::coordinate::absolute) override;
  void path_elliptical_arc_to(double rx, double ry, double x_axis_rotation,
                              bool large_arc_flag, bool sweep_flag, double x,
                              double y,
                              svgpp::tag::coordinate::absolute) override;
  void path_close_subpath() override;
  void path_exit() override;
  void set(svgpp::tag::attribute::cy cy, const double &v) override;
  void set(svgpp::tag::attribute::cx cx, const double &v) override;
  void set(svgpp::tag::attribute::r r, const double &v) override;
  void set(svgpp::tag::attribute::rx rx, const double &v) override;
  void set(svgpp::tag::attribute::ry ry, const double &v) override;
  void set(svgpp::tag::attribute::x1 x1, const double &v) override;
  void set(svgpp::tag::attribute::y1 y1, const double &v) override;
  void set(svgpp::tag::attribute::x2 x2, const double &v) override;
  void set(svgpp::tag::attribute::y2 y2, const double &v) override;
  void set(svgpp::tag::attribute::x a, const double &v) override;
  void set(svgpp::tag::attribute::y y, const double &v) override;
  void set(svgpp::tag::attribute::width width, const double &v) override;
  void set(svgpp::tag::attribute::height height, const double &v) override;

  void loadSvg(char *text);

  std::size_t num_svgs() const { return m_num_svgs; };
  std::size_t num_groups() const { return m_num_groups; };
  std::size_t num_circles() const { return m_num_circles; };
  std::size_t num_ellipses() const { return m_num_ellipses; };
  std::size_t num_lines() const { return m_num_lines; };
  std::size_t num_paths() const { return m_num_paths; };
  std::size_t num_polygons() const { return m_num_polygons; };
  std::size_t num_polylines() const { return m_num_polylines; };
  std::size_t num_rects() const { return m_num_rects; };
  std::size_t num_titles() const { return m_num_titles; };
  std::size_t num_unknowns() const { return m_num_unknowns; };

protected:
  virtual void set(svgpp::tag::attribute::points &points,
                   const std::any &range) override;
  virtual void set_text(const std::any &range) override;

private:
  std::size_t m_num_svgs = 1; // the top level svg is implicit. See
                              // https://github.com/svgpp/svgpp/issues/98
  std::size_t m_num_groups = 0;
  std::size_t m_num_circles = 0;
  std::size_t m_num_ellipses = 0;
  std::size_t m_num_lines = 0;
  std::size_t m_num_paths = 0;
  std::size_t m_num_polygons = 0;
  std::size_t m_num_polylines = 0;
  std::size_t m_num_rects = 0;
  std::size_t m_num_titles = 0;
  std::size_t m_num_unknowns = 0;
};
