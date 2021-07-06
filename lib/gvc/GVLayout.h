#pragma once

#include <memory>

#include <cgraph/AGraph.h>
#include <gvc/GVContext.h>

#ifdef _WIN32
#if gvc_EXPORTS
#define GVLAYOUT_API __declspec(dllexport)
#else
#define GVLAYOUT_API __declspec(dllimport)
#endif
#else
#define GVLAYOUT_API /* nothing */
#endif

/**
 * @brief The GVlayout class represents a graph layout
 */

class GVRenderData;

namespace GVC {

class GVLAYOUT_API GVLayout {
public:
  // named constructor
  static std::unique_ptr<GVLayout> create(std::shared_ptr<GVContext> gvc,
                                          std::shared_ptr<CGraph::AGraph> g,
                                          const std::string &engine);

  // do not use this constructor directly. Use the named constructor instead
  GVLayout(std::shared_ptr<GVContext> gvc, std::shared_ptr<CGraph::AGraph> g);
  ~GVLayout();
  std::unique_ptr<GVRenderData> render(const std::string &format);

private:
  std::shared_ptr<GVContext> m_gvc;
  std::shared_ptr<CGraph::AGraph> m_g;
};

} //  namespace GVC
