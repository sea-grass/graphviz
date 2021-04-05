#include <catch2/catch.hpp>

#include <cgraph/AGraph.h>
#include <gvc/GVContext.h>
#include <gvc/GVLayout.h>
#include <gvc/GVRenderData.h>

TEST_CASE("Rendered SVG can be retrieved as a string_view or as a C-string") {
  const auto demand_loading = false;
  auto gvc = GVC::GVContext::create_with_builtins(lt_preloaded_symbols,
                                                  demand_loading);

  auto dot = "digraph {}";
  auto g = CGraph::AGraph::create_from_dot_source(dot);

  const auto layout =
      GVC::GVLayout::create(std::move(gvc), std::move(g), "dot");

  const auto result = layout->render("svg");
  REQUIRE(result->string_view().compare(result->c_str()) == 0);
}

TEST_CASE("Rendering in an unknown format throws an exception") {
  const auto demand_loading = false;
  auto gvc = GVC::GVContext::create_with_builtins(lt_preloaded_symbols,
                                                  demand_loading);

  auto dot = "digraph {}";
  auto g = CGraph::AGraph::create_from_dot_source(dot);

  const auto layout =
      GVC::GVLayout::create(std::move(gvc), std::move(g), "dot");

  REQUIRE_THROWS_AS(layout->render("UNKNOWN_FORMAT"), std::runtime_error);
}
