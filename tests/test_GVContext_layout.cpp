#include <catch2/catch.hpp>

#include <cgraph/AGraph.h>
#include <gvc/GVContext.h>
#include <gvc/GVLayout.h>

TEST_CASE("Layout of a graph with dynamically loaded plugins succeeds") {
  auto gvc = GVC::GVContext::create();

  auto dot = "graph {}";
  auto g = CGraph::AGraph::create_from_dot_source(dot);

  const auto layout =
      GVC::GVLayout::create(std::move(gvc), std::move(g), "dot");
}

TEST_CASE("Layout of a graph with built-in plugins succeeds") {
  const auto demand_loading = false;
  auto gvc = GVC::GVContext::create_with_builtins(lt_preloaded_symbols,
                                                  demand_loading);

  auto dot = "graph {}";
  auto g = CGraph::AGraph::create_from_dot_source(dot);

  const auto layout =
      GVC::GVLayout::create(std::move(gvc), std::move(g), "dot");
}

TEST_CASE("Layout of a graph with two nodes and one edge succeds") {
  const auto demand_loading = false;
  auto gvc = GVC::GVContext::create_with_builtins(lt_preloaded_symbols,
                                                  demand_loading);

  auto dot = "graph {a -- b}";
  auto g = CGraph::AGraph::create_from_dot_source(dot);

  const auto layout =
      GVC::GVLayout::create(std::move(gvc), std::move(g), "dot");
}

TEST_CASE("Layout with an unknown engine throws an exception") {
  auto dot = "digraph {}";
  auto g = CGraph::AGraph::create_from_dot_source(dot);

  const auto demand_loading = false;
  auto gvc = GVC::GVContext::create_with_builtins(lt_preloaded_symbols,
                                                  demand_loading);

  REQUIRE_THROWS_AS(
      GVC::GVLayout::create(std::move(gvc), std::move(g),
                            "THIS ERROR MESSAGE IS EXPECTED IN THIS TEST"),
      std::runtime_error);
}
