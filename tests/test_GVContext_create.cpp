#include <catch2/catch.hpp>

#include <gvc/GVContext.h>

TEST_CASE("GVContext can be constructed without built-in plugins and it's "
          "underlying C data structure can be retrieved") {
  auto gvc = GVC::GVContext::create();
  REQUIRE(gvc->c_struct() != nullptr);
}

TEST_CASE("GVContext can be constructed with built-in plugins and it's "
          "underlying C data structure can be retrieved") {
  const auto demand_loading = false;
  auto gvc = GVC::GVContext::create_with_builtins(lt_preloaded_symbols,
                                                  demand_loading);
  REQUIRE(gvc->c_struct() != nullptr);
}
