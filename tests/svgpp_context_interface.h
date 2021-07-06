#pragma once

#include <any>

#include <svgpp/svgpp.hpp>

/**
 * @brief Interface for class for an SVG++ context.
 * The ISvgppContext class provides a context containing SVG element
 * callbacks for the SVG++ parser which is called from the SVG document
 * traverser. It's separated from the SVG analyzer to avoid the very time
 * consuming recompilation of the SVG document traverser when then SVG analyzer
 * header is changed, which is expected to happen often as new functionality is
 * added.
 *
 * Mostly based on http://svgpp.org/lesson01.html#handling-shapes-geometry
 */

class ISvgppContext {
public:
  virtual void on_enter_element(svgpp::tag::element::svg e) = 0;
  virtual void on_enter_element(svgpp::tag::element::g e) = 0;
  virtual void on_enter_element(svgpp::tag::element::circle e) = 0;
  virtual void on_enter_element(svgpp::tag::element::ellipse e) = 0;
  virtual void on_enter_element(svgpp::tag::element::line e) = 0;
  virtual void on_enter_element(svgpp::tag::element::path e) = 0;
  virtual void on_enter_element(svgpp::tag::element::polygon e) = 0;
  virtual void on_enter_element(svgpp::tag::element::polyline e) = 0;
  virtual void on_enter_element(svgpp::tag::element::rect e) = 0;
  virtual void on_enter_element(svgpp::tag::element::title e) = 0;
  virtual void on_enter_element(svgpp::tag::element::any e) = 0;
  virtual void on_exit_element() = 0;
  virtual void path_move_to(double x, double y,
                            svgpp::tag::coordinate::absolute) = 0;
  virtual void path_line_to(double x, double y,
                            svgpp::tag::coordinate::absolute) = 0;
  virtual void path_cubic_bezier_to(double x1, double y1, double x2, double y2,
                                    double x, double y,
                                    svgpp::tag::coordinate::absolute) = 0;
  virtual void path_quadratic_bezier_to(double x1, double y1, double x,
                                        double y,
                                        svgpp::tag::coordinate::absolute) = 0;
  virtual void path_elliptical_arc_to(double rx, double ry,
                                      double x_axis_rotation,
                                      bool large_arc_flag, bool sweep_flag,
                                      double x, double y,
                                      svgpp::tag::coordinate::absolute) = 0;
  virtual void path_close_subpath() = 0;
  virtual void path_exit() = 0;
  virtual void set(svgpp::tag::attribute::cy cy, const double &v) = 0;
  virtual void set(svgpp::tag::attribute::cx cx, const double &v) = 0;
  virtual void set(svgpp::tag::attribute::r r, const double &v) = 0;
  virtual void set(svgpp::tag::attribute::rx rx, const double &v) = 0;
  virtual void set(svgpp::tag::attribute::ry ry, const double &v) = 0;
  virtual void set(svgpp::tag::attribute::x1 x1, const double &v) = 0;
  virtual void set(svgpp::tag::attribute::y1 y1, const double &v) = 0;
  virtual void set(svgpp::tag::attribute::x2 x2, const double &v) = 0;
  virtual void set(svgpp::tag::attribute::y2 y2, const double &v) = 0;
  template <class Range>
  void set(svgpp::tag::attribute::points points, Range const &range) {
    set(points, std::any{range});
  }
  virtual void set(svgpp::tag::attribute::x a, const double &v) = 0;
  virtual void set(svgpp::tag::attribute::y y, const double &v) = 0;
  virtual void set(svgpp::tag::attribute::width width, const double &v) = 0;
  virtual void set(svgpp::tag::attribute::height height, const double &v) = 0;
  template <class Range> void set_text(const Range &range) {
    set_text(std::any{range});
  }

protected:
  virtual void set(svgpp::tag::attribute::points &points,
                   const std::any &range) = 0;
  virtual void set_text(const std::any &range) = 0;
};
