#include <stdexcept>

#include <gvc/GVContext.h>
#include <gvc/GVLayout.h>

namespace GVC {

GVContext::GVContext(GVC_t *gvc) : m_gvc(gvc) {}

std::unique_ptr<GVContext> GVContext::create() {
  return std::make_unique<GVContext>(gvContext());
}

std::unique_ptr<GVContext>
GVContext::create_with_builtins(const lt_symlist_t *builtins,
                                bool demand_loading) {
  return std::make_unique<GVContext>(
      gvContextPlugins(builtins, demand_loading));
}

GVContext::~GVContext() { gvFreeContext(m_gvc); }

} // namespace GVC
