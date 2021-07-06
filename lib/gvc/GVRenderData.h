#pragma once

#include <cstddef>
#include <string_view>

#ifdef _WIN32
#if gvc_EXPORTS
#define GVRENDER_API __declspec(dllexport)
#else
#define GVRENDER_API __declspec(dllimport)
#endif
#else
#define GVRENDER_API /* nothing */
#endif

/**
 * @brief The GVRenderData class represents a rendered layout in a specific text
 * format
 */

class GVRENDER_API GVRenderData {
public:
  // do not use this constructor directly. Use GVLayout::render instead
  GVRenderData(char *rendered_data, size_t length);
  ~GVRenderData();
  // disallow copy for now since we manage a C string using a raw pointer
  GVRenderData(GVRenderData &) = delete;
  GVRenderData &operator=(GVRenderData &) = delete;
  char *c_str() const { return m_data; }
  std::string_view string_view() const {
    return std::string_view{m_data, m_length};
  }

private:
  // the underlying C data structure
  char *m_data = nullptr;
  std::size_t m_length = 0;
};
