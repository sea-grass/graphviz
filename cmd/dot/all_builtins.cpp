/// \file
/// \brief plugin registry with everything registered at build time

#include "config.h"
#include <gvc/gvplugin.h>

#if defined(GVDLL)
#define IMPORT __declspec(dllimport)
#else
#define IMPORT /* nothing */
#endif

extern "C" {

IMPORT extern gvplugin_library_t gvplugin_dot_layout_LTX_library;
IMPORT extern gvplugin_library_t gvplugin_neato_layout_LTX_library;
IMPORT extern gvplugin_library_t gvplugin_core_LTX_library;
IMPORT extern gvplugin_library_t gvplugin_devil_LTX_library;
IMPORT extern gvplugin_library_t gvplugin_gd_LTX_library;
IMPORT extern gvplugin_library_t gvplugin_gdk_LTX_library;
IMPORT extern gvplugin_library_t gvplugin_gdiplus_LTX_library;
IMPORT extern gvplugin_library_t gvplugin_gs_LTX_library;
IMPORT extern gvplugin_library_t gvplugin_kitty_LTX_library;
IMPORT extern gvplugin_library_t gvplugin_lasi_LTX_library;
IMPORT extern gvplugin_library_t gvplugin_pango_LTX_library;
IMPORT extern gvplugin_library_t gvplugin_poppler_LTX_library;
IMPORT extern gvplugin_library_t gvplugin_webp_LTX_library;
IMPORT extern gvplugin_library_t gvplugin_quartz_LTX_library;
IMPORT extern gvplugin_library_t gvplugin_rsvg_LTX_library;
IMPORT extern gvplugin_library_t gvplugin_vt_LTX_library;
IMPORT extern gvplugin_library_t gvplugin_xlib_LTX_library;

lt_symlist_t lt_preloaded_symbols[] = {
    {"gvplugin_dot_layout_LTX_library", &gvplugin_dot_layout_LTX_library},
    {"gvplugin_neato_layout_LTX_library", &gvplugin_neato_layout_LTX_library},
    {"gvplugin_core_LTX_library", &gvplugin_core_LTX_library},
#if defined(HAVE_DEVIL) && defined(HAVE_PANGOCAIRO)
    {"gvplugin_devil_LTX_library", &gvplugin_devil_LTX_library},
#endif
#ifdef HAVE_LIBGD
    {"gvplugin_gd_LTX_library", &gvplugin_gd_LTX_library},
#endif
#if defined(HAVE_GDK) && defined(HAVE_GDK_PIXBUF) && defined(HAVE_PANGOCAIRO)
    {"gvplugin_gdk_LTX_library", &gvplugin_gdk_LTX_library},
#endif
#ifdef _WIN32
    {"gvplugin_gdiplus_LTX_library", &gvplugin_gdiplus_LTX_library},
#endif
#if defined(HAVE_GS) && defined(HAVE_PANGOCAIRO)
    {"gvplugin_gs_LTX_library", &gvplugin_gs_LTX_library},
#endif
#ifdef HAVE_PANGOCAIRO
    {"gvplugin_kitty_LTX_library", &gvplugin_kitty_LTX_library},
#if defined(HAVE_FREETYPE) && defined(HAVE_LASI)
    {"gvplugin_lasi_LTX_library", &gvplugin_lasi_LTX_library},
#endif
    {"gvplugin_pango_LTX_library", &gvplugin_pango_LTX_library},
#ifdef HAVE_POPPLER
    {"gvplugin_poppler_LTX_library", &gvplugin_poppler_LTX_library},
#endif
#ifdef HAVE_WEBP
    {"gvplugin_webp_LTX_library", &gvplugin_webp_LTX_library},
#endif
#endif
#ifdef __APPLE__
    {"gvplugin_quartz_LTX_library", &gvplugin_quartz_LTX_library},
#endif
#if defined(HAVE_PANGOCAIRO) && defined(HAVE_RSVG)
    {"gvplugin_rsvg_LTX_library", &gvplugin_rsvg_LTX_library},
#endif
    {"gvplugin_vt_LTX_library", &gvplugin_vt_LTX_library},
#if defined(HAVE_PANGOCAIRO) && defined(HAVE_X11) && defined(HAVE_XRENDER)
    {"gvplugin_xlib_LTX_library", &gvplugin_xlib_LTX_library},
#endif
    {0, 0}};
}
