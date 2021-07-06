#pragma once

#include <memory>
#include <string>

#include <cgraph/AGraph.h>
#include <gvc/gvc.h>

#ifdef _WIN32
#if gvc_EXPORTS
#define GVCONTEXT_API __declspec(dllexport)
#else
#define GVCONTEXT_API __declspec(dllimport)
#endif
#else
#define GVCONTEXT_API /* nothing */
#endif

/**
 * @brief The GVContext class represents a Graphviz context
 */

namespace GVC {

class GVLayout;

class GVCONTEXT_API GVContext {
public:
  // named constructors
  static std::unique_ptr<GVContext> create();
  static std::unique_ptr<GVContext>
  create_with_builtins(const lt_symlist_t *builtins, bool demand_loading);

  // do not use this constructor directly. Use a named constructor instead
  explicit GVContext(GVC_t *gvc);
  ~GVContext();
  // disallow copy for now since we manage a C struct using a raw pointer
  GVContext(GVContext &) = delete;
  GVContext &operator=(GVContext &) = delete;
  GVC_t *c_struct() const { return m_gvc; }

private:
  // the underlying C data structure
  GVC_t *const m_gvc = nullptr;
};

} // namespace GVC
