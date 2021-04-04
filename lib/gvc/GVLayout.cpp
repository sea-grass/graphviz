#include <memory>

#include <cgraph/AGraph.h>
#include <gvc/GVLayout.h>

#ifdef _WIN32
#if gvc_EXPORTS
#define GVLAYOUT_API __declspec(dllexport)
#else
#define GVLAYOUT_API __declspec(dllimport)
#endif
#else
#define GVLAYOUT_API /* nothing */
#endif

namespace GVC {

std::unique_ptr<GVLayout> GVLayout::create(std::shared_ptr<GVContext> gvc,
                                           std::shared_ptr<CGraph::AGraph> g,
                                           const std::string &engine) {
  const auto rc = gvLayout(gvc->c_struct(), g->c_struct(), engine.c_str());
  if (rc) {
    throw std::runtime_error("Layout failed");
  }
  return std::make_unique<GVLayout>(std::move(gvc), std::move(g));
}

GVLayout::GVLayout(std::shared_ptr<GVContext> gvc,
                   std::shared_ptr<CGraph::AGraph> g)
    : m_gvc(gvc), m_g(std::move(g)) {}

GVLayout::~GVLayout() { gvFreeLayout(m_gvc->c_struct(), m_g->c_struct()); }

} // namespace GVC
