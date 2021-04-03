#include <catch2/catch.hpp>

#include <cgraph/AGraph.h>

TEST_CASE("AGraph can be constructed from DOT source and has an underlying C "
          "data structure") {
  auto g = CGraph::AGraph::create_from_dot_source("graph {}");
  REQUIRE(g->c_struct() != nullptr);
}

TEST_CASE("AGraph constructed from an empty string throws exception") {
  REQUIRE_THROWS_AS(CGraph::AGraph::create_from_dot_source(""),
                    std::runtime_error);
}

TEST_CASE("AGraph constructed from bad DOT source throws exception") {
  REQUIRE_THROWS_AS(CGraph::AGraph::create_from_dot_source(
                        "THIS_SHOULD_GENERATE_A_SYNTAX_ERROR"),
                    std::runtime_error);
}
