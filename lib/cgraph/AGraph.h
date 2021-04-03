#pragma once

#include <memory>

#include <cgraph/cgraph.h>

#ifdef _WIN32
#if cgraph_EXPORTS
#define AGRAPH_API __declspec(dllexport)
#else
#define AGRAPH_API __declspec(dllimport)
#endif
#else
#define AGRAPH_API /* nothing */
#endif

/**
 * @brief The AGraph class represents an abstract graph
 */

namespace CGraph {

class AGRAPH_API AGraph {
public:
  // named constructors
  static std::unique_ptr<AGraph> create_from_dot_source(const std::string &dot);

  // do not use this constructor directly. Use a named constructor instead
  explicit AGraph(Agraph_t *g);
  ~AGraph();
  // disallow copy for now since we manage a C struct using a raw pointer
  AGraph(AGraph &) = delete;
  AGraph &operator=(AGraph &) = delete;

  // get the underlying C data structure
  Agraph_t *c_struct() const { return m_g; };

private:
  // the underlying C data structure
  Agraph_t *const m_g = nullptr;
};

} // namespace CGraph
