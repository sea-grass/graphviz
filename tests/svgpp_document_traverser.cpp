#include <rapidxml_ns/rapidxml_ns.hpp>
#include <svgpp/policy/xml/rapidxml_ns.hpp>
#include <svgpp/svgpp.hpp>

#include "svgpp_context_interface.h"
#include "svgpp_document_traverser.h"

// most of this is taken from
// http://svgpp.org/lesson01.html#handling-shapes-geometry

void SvgppDocumentTraverser::traverseSvg(ISvgppContext &context, char *text) {
  typedef rapidxml_ns::xml_node<> const *xml_element_t;

  typedef boost::mpl::set<
      // SVG Structural Elements
      svgpp::tag::element::svg, svgpp::tag::element::g,
      // SVG Shape Elements
      svgpp::tag::element::circle, svgpp::tag::element::ellipse,
      svgpp::tag::element::line, svgpp::tag::element::path,
      svgpp::tag::element::polygon, svgpp::tag::element::polyline,
      svgpp::tag::element::rect, svgpp::tag::element::title>::type
      processed_elements_t;

  rapidxml_ns::xml_document<> doc;
  doc.parse<0>(text);
  if (rapidxml_ns::xml_node<> *svg_element = doc.first_node("svg")) {
    xml_element_t xml_root_element = svg_element;
    svgpp::document_traversal<
        svgpp::processed_elements<processed_elements_t>,
        svgpp::processed_attributes<
            svgpp::traits::shapes_attributes_by_element>,
        svgpp::basic_shapes_policy<svgpp::policy::basic_shapes::raw>>::
        load_document(xml_root_element, context);
  }
}
