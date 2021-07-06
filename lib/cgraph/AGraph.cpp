#include <stdexcept>
#include <string>

#include <cgraph/AGraph.h>

namespace CGraph {

AGraph::AGraph(Agraph_t *g) : m_g(g){};

std::unique_ptr<AGraph> AGraph::create_from_dot_source(const std::string &dot) {
  const auto g = agmemread(dot.c_str());
  if (!g) {
    throw std::runtime_error("Could not read graph");
  }
  return std::make_unique<AGraph>(g);
}

AGraph::~AGraph() { agclose(m_g); }

} // namespace CGraph
