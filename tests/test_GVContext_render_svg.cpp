#include <string_view>

#include <catch2/catch.hpp>

#include "svg_analyzer.h"
#include <cgraph/AGraph.h>
#include <gvc/GVContext.h>
#include <gvc/GVLayout.h>
#include <gvc/GVRenderData.h>

TEST_CASE(
    "Rendering an SVG from a graph without any nodes outputs an SVG containing "
    "a single (transparent) polygon") {
  auto gvc = GVC::GVContext::create_with_builtins(lt_preloaded_symbols, false);

  auto dot = "digraph {}";
  auto g = CGraph::AGraph::create_from_dot_source(dot);

  const auto layout =
      GVC::GVLayout::create(std::move(gvc), std::move(g), "dot");

  const auto result = layout->render("svg");
  SVGAnalyzer svgParser;
  svgParser.loadSvg(result->c_str());
  REQUIRE(result->string_view().find("svg") != std::string_view::npos);
  REQUIRE(svgParser.num_svgs() == 1);
  REQUIRE(svgParser.num_groups() == 1);
  REQUIRE(svgParser.num_circles() == 0);
  REQUIRE(svgParser.num_ellipses() == 0);
  REQUIRE(svgParser.num_lines() == 0);
  REQUIRE(svgParser.num_paths() == 0);
  REQUIRE(svgParser.num_polygons() == 1);
  REQUIRE(svgParser.num_polylines() == 0);
  REQUIRE(svgParser.num_rects() == 0);
  REQUIRE(svgParser.num_titles() == 0);
  REQUIRE(svgParser.num_unknowns() == 0);
}

TEST_CASE("Rendering an SVG from a graph with a single node outputs an SVG "
          "containing an ellipse") {
  auto gvc = GVC::GVContext::create_with_builtins(lt_preloaded_symbols, false);

  auto dot = "digraph {a}";
  auto g = CGraph::AGraph::create_from_dot_source(dot);

  const auto layout =
      GVC::GVLayout::create(std::move(gvc), std::move(g), "dot");

  const auto result = layout->render("svg");
  SVGAnalyzer svgParser;
  svgParser.loadSvg(result->c_str());
  REQUIRE(result->string_view().find("svg") != std::string_view::npos);
  REQUIRE(svgParser.num_svgs() == 1);
  REQUIRE(svgParser.num_groups() == 2);
  REQUIRE(svgParser.num_ellipses() == 1);
  REQUIRE(svgParser.num_circles() == 0);
  REQUIRE(svgParser.num_lines() == 0);
  REQUIRE(svgParser.num_paths() == 0);
  REQUIRE(svgParser.num_polygons() == 1);
  REQUIRE(svgParser.num_polylines() == 0);
  REQUIRE(svgParser.num_rects() == 0);
  REQUIRE(svgParser.num_titles() == 1);
  REQUIRE(svgParser.num_unknowns() == 0);
}

TEST_CASE("Rendering an SVG from a graph with two node outputs an SVG "
          "containing two ellipses") {
  auto gvc = GVC::GVContext::create_with_builtins(lt_preloaded_symbols, false);

  auto dot = "digraph {a b}";
  auto g = CGraph::AGraph::create_from_dot_source(dot);

  const auto layout =
      GVC::GVLayout::create(std::move(gvc), std::move(g), "dot");

  const auto result = layout->render("svg");
  SVGAnalyzer svgParser;
  svgParser.loadSvg(result->c_str());
  REQUIRE(result->string_view().find("svg") != std::string_view::npos);
  REQUIRE(svgParser.num_svgs() == 1);
  REQUIRE(svgParser.num_groups() == 3);
  REQUIRE(svgParser.num_ellipses() == 2);
  REQUIRE(svgParser.num_circles() == 0);
  REQUIRE(svgParser.num_lines() == 0);
  REQUIRE(svgParser.num_paths() == 0);
  REQUIRE(svgParser.num_polygons() == 1);
  REQUIRE(svgParser.num_polylines() == 0);
  REQUIRE(svgParser.num_rects() == 0);
  REQUIRE(svgParser.num_titles() == 2);
  REQUIRE(svgParser.num_unknowns() == 0);
}

TEST_CASE("Rendering an SVG from a graph with two nodes and one edge outputs "
          "an SVG containing two polygons, two ellipses and one path") {
  auto gvc = GVC::GVContext::create_with_builtins(lt_preloaded_symbols, false);

  auto dot = "digraph {a -> b}";
  auto g = CGraph::AGraph::create_from_dot_source(dot);

  const auto layout =
      GVC::GVLayout::create(std::move(gvc), std::move(g), "dot");

  const auto result = layout->render("svg");
  SVGAnalyzer svgParser;
  svgParser.loadSvg(result->c_str());
  REQUIRE(result->string_view().find("svg") != std::string_view::npos);
  REQUIRE(svgParser.num_svgs() == 1);
  REQUIRE(svgParser.num_groups() == 4);
  REQUIRE(svgParser.num_ellipses() == 2);
  REQUIRE(svgParser.num_circles() == 0);
  REQUIRE(svgParser.num_lines() == 0);
  REQUIRE(svgParser.num_paths() == 1);
  REQUIRE(svgParser.num_polygons() == 2);
  REQUIRE(svgParser.num_polylines() == 0);
  REQUIRE(svgParser.num_rects() == 0);
  REQUIRE(svgParser.num_titles() == 3);
  REQUIRE(svgParser.num_unknowns() == 0);
}
