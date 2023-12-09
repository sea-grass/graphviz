add_definitions(-DDEMAND_LOADING=1)

include_directories(
  ../../lib
  ${CMAKE_CURRENT_BINARY_DIR}
  ../../lib/cdt
  ../../lib/cgraph
  ../../lib/common
  ../../lib/gvc
  ../../lib/pathplan
)

add_executable(dot
  # Source files
  dot.c
  all_builtins.cpp
)

# MinGW does not deal with `TARGET_OPTIONS` well and link errors occur
if(MINGW)
  target_link_libraries(dot PRIVATE
    gvplugin_core
    gvplugin_dot_layout
    gvplugin_neato_layout
    gvplugin_vt
  )
else()
  target_sources(dot PRIVATE
    $<TARGET_OBJECTS:gvplugin_core>
    $<TARGET_OBJECTS:gvplugin_dot_layout>
    $<TARGET_OBJECTS:gvplugin_neato_layout>
    $<TARGET_OBJECTS:gvplugin_vt>
  )
endif()

target_link_libraries(dot PRIVATE
  cgraph
  circogen
  dotgen
  fdpgen
  gvc
  neatogen
  osage
  patchwork
  pathplan
  rbtree
  sparse
  twopigen
  xdot
)

if(with_ipsepcola)
  target_link_libraries(dot PRIVATE vpsc)
endif()

if(with_sfdp)
  target_link_libraries(dot PRIVATE sfdpgen)
endif()

if(DevIL_FOUND AND PANGOCAIRO_FOUND)
  if(MINGW)
    target_link_libraries(dot PRIVATE gvplugin_devil)
  else()
    target_sources(dot PRIVATE $<TARGET_OBJECTS:gvplugin_devil>)
  endif()
  target_link_libraries(dot PRIVATE
    ${PANGOCAIRO_LIBRARIES}
    ${IL_LIBRARIES}
    ${ILU_LIBRARIES}
  )
endif()

if(GD_FOUND)
  if(MINGW)
    target_link_libraries(dot PRIVATE gvplugin_gd)
  else()
    target_sources(dot PRIVATE $<TARGET_OBJECTS:gvplugin_gd>)
  endif()
  target_link_libraries(dot PRIVATE ${GD_LIBRARIES})
  if(CAIRO_FOUND)
    target_link_libraries(dot PRIVATE ${CAIRO_LINK_LIBRARIES})
    if(PANGOCAIRO_FOUND)
      target_link_libraries(dot PRIVATE ${PANGOCAIRO_LINK_LIBRARIES})
    endif()
  endif()
endif()

if(GDK_FOUND AND GDK_PIXBUF_FOUND AND PANGOCAIRO_FOUND)
  if(MINGW)
    target_link_libraries(dot PRIVATE gvplugin_gdk)
  else()
    target_sources(dot PRIVATE $<TARGET_OBJECTS:gvplugin_gdk>)
  endif()
  target_link_libraries(dot PRIVATE
    ${GDK_LINK_LIBRARIES}
    ${GDK_PIXBUF_LINK_LIBRARIES}
    ${PANGOCAIRO_LINK_LIBRARIES}
  )
endif()

if(WIN32)
  if(MINGW)
    target_link_libraries(dot PRIVATE gvplugin_gdiplus)
  else()
    target_sources(dot PRIVATE $<TARGET_OBJECTS:gvplugin_gdiplus>)
  endif()
  target_link_libraries(dot PRIVATE gdiplus)
endif()

if(GS_FOUND AND PANGOCAIRO_FOUND)
  if(MINGW)
    target_link_libraries(dot PRIVATE gvplugin_gs)
  else()
    target_sources(dot PRIVATE $<TARGET_OBJECTS:gvplugin_gs>)
  endif()
  target_link_libraries(dot PRIVATE
    ${GS_LIBRARIES}
    ${CAIRO_LINK_LIBRARIES}
    ${PANGOCAIRO_LINK_LIBRARIES}
  )
endif()

if(PANGOCAIRO_FOUND)
  if(MINGW)
    target_link_libraries(dot PRIVATE gvplugin_kitty)
  else()
    target_sources(dot PRIVATE $<TARGET_OBJECTS:gvplugin_kitty>)
  endif()
  if(ZLIB_FOUND)
    target_link_libraries(dot PRIVATE ${ZLIB_LIBRARIES})
  endif()
endif()

if(Freetype_FOUND AND LASI_FOUND AND PANGOCAIRO_FOUND)
  if(MINGW)
    target_link_libraries(dot PRIVATE gvplugin_lasi)
  else()
    target_sources(dot PRIVATE $<TARGET_OBJECTS:gvplugin_lasi>)
  endif()
  target_link_libraries(dot PRIVATE
    ${FREETYPE_LIBRARIES}
    ${LASI_LIBRARIES}
    ${PANGOCAIRO_LIBRARIES}
  )
endif()

find_package(PkgConfig)
if(PkgConfig_FOUND)
  pkg_check_modules(GTS gts)
  if(GTS_FOUND)
    target_include_directories(dot SYSTEM PRIVATE ${GTS_INCLUDE_DIRS})
    target_link_libraries(dot PRIVATE ${GTS_LINK_LIBRARIES})
  endif()
endif()

if(PANGOCAIRO_FOUND)
  if(MINGW)
    target_link_libraries(dot PRIVATE gvplugin_pango)
  else()
    target_sources(dot PRIVATE $<TARGET_OBJECTS:gvplugin_pango>)
  endif()
  target_link_libraries(dot PRIVATE
    ${CAIRO_LINK_LIBRARIES}
    ${PANGOCAIRO_LINK_LIBRARIES}
  )
endif()

if(PANGOCAIRO_FOUND AND POPPLER_FOUND)
  if(MINGW)
    target_link_libraries(dot PRIVATE gvplugin_poppler)
  else()
    target_sources(dot PRIVATE $<TARGET_OBJECTS:gvplugin_poppler>)
  endif()
  target_link_libraries(dot PRIVATE
    ${PANGOCAIRO_LIBRARIES}
    ${POPPLER_LIBRARIES}
  )
endif()

if(APPLE)
  target_sources(dot PRIVATE $<TARGET_OBJECTS:gvplugin_quartz>)
  target_link_options(dot PRIVATE -framework ApplicationServices)
endif()

if(PANGOCAIRO_FOUND AND RSVG_FOUND)
  if(MINGW)
    target_link_libraries(dot PRIVATE gvplugin_rsvg)
  else()
    target_sources(dot PRIVATE $<TARGET_OBJECTS:gvplugin_rsvg>)
  endif()
  target_link_libraries(dot PRIVATE
    ${PANGOCAIRO_LINK_LIBRARIES}
    ${RSVG_LINK_LIBRARIES}
  )
endif()

if(PANGOCAIRO_FOUND AND X11_FOUND AND XRENDER_FOUND)
  if(MINGW)
    target_link_libraries(dot PRIVATE gvplugin_xlib)
  else()
    target_sources(dot PRIVATE $<TARGET_OBJECTS:gvplugin_xlib>)
  endif()
  target_link_libraries(dot PRIVATE
    ${PANGOCAIRO_LINK_LIBRARIES}
    ${X11_LINK_LIBRARIES}
    ${XRENDER_LINK_LIBRARIES}
  )
endif()

if(WEBP_FOUND)
  if(MINGW)
    target_link_libraries(dot PRIVATE gvplugin_webp)
  else()
    target_sources(dot PRIVATE $<TARGET_OBJECTS:gvplugin_webp>)
  endif()
  target_link_libraries(dot PRIVATE ${WEBP_LINK_LIBRARIES})
  if(CAIRO_FOUND)
    target_link_libraries(dot PRIVATE ${CAIRO_LINK_LIBRARIES})
    if(PANGOCAIRO_FOUND)
      target_link_libraries(dot PRIVATE ${PANGOCAIRO_LINK_LIBRARIES})
    endif()
  endif()
endif()

find_library(SOCKET socket)
if(SOCKET)
  target_link_libraries(dot PRIVATE ${SOCKET})
endif()

if(APPLE)
  set_target_properties(dot PROPERTIES LINK_FLAGS -Wl,-stack_size,0x2000000)
elseif(MINGW)
  set_target_properties(dot PROPERTIES LINK_FLAGS -Wl,--stack,0x2000000)
elseif(WIN32)
  set_target_properties(dot PROPERTIES LINK_FLAGS /STACK:"33554432")
endif()

# Installation location of executables
install(
  TARGETS dot
  RUNTIME DESTINATION ${BINARY_INSTALL_DIR}
  LIBRARY DESTINATION ${LIBRARY_INSTALL_DIR}
  ARCHIVE DESTINATION ${LIBRARY_INSTALL_DIR}
)

# Aliases to the dot executable (not including '.exe' suffix)
list(APPEND dot_aliases circo fdp neato osage patchwork sfdp twopi)

# We use copying instead of symlinking for Cygwin to avoid
# https://gitlab.com/graphviz/graphviz/-/issues/2123

foreach(cmd_alias IN LISTS dot_aliases)
  set(DOTCOPY
      "${CMAKE_CURRENT_BINARY_DIR}/${cmd_alias}${CMAKE_EXECUTABLE_SUFFIX}")
  if(WIN32 OR CYGWIN)
    # Copy dot executable to each alias name then install copies to bindir
    add_custom_command(
      TARGET dot
      POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:dot> ${DOTCOPY}
      COMMENT "Copying dot to ${DOTCOPY}"
    )
    install(
      PROGRAMS ${DOTCOPY}
      DESTINATION ${BINARY_INSTALL_DIR}
    )
  else()
    # For not-WIN32, install symlinks from dot_aliases -> dot_executable in
    # bindir. Note: This may be brittle. This builds a symlink from
    # ./cmd_alias -> ./dot in ${CMAKE_CURRENT_BINARY_DIR}, then installs that
    # symlink into ${BINARY_INSTALL_DIR}. This presumes
    # ${CMAKE_CURRENT_BINARY_DIR}/dot is installed to ${BINARY_INSTALL_DIR}/dot.
    # There is a (small?) risk of dangling symlinks
    add_custom_command(
      TARGET dot
      POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E create_symlink $<TARGET_FILE_NAME:dot>
        ${cmd_alias}
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
      COMMENT "Linking dot as ${cmd_alias}"
    )
    install(
      FILES ${DOTCOPY}
      DESTINATION ${BINARY_INSTALL_DIR}
    )
  endif()
endforeach()

# Specify man pages to be installed
if(GZIP)
  add_custom_target(man-dot ALL DEPENDS dot.1.gz
                    COMMENT "dot man page")
  add_custom_command(
    OUTPUT dot.1.gz
    COMMAND ${GZIP} -9 --no-name --to-stdout dot.1
      >"${CMAKE_CURRENT_BINARY_DIR}/dot.1.gz"
    MAIN_DEPENDENCY dot.1
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "compress dot man page")
  install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/dot.1.gz
    DESTINATION ${MAN_INSTALL_DIR}/man1)
  add_custom_target(man-osage ALL DEPENDS osage.1.gz
                    COMMENT "osage man page")
  add_custom_command(
    OUTPUT osage.1.gz
    COMMAND ${GZIP} -9 --no-name --to-stdout osage.1
      >"${CMAKE_CURRENT_BINARY_DIR}/osage.1.gz"
    MAIN_DEPENDENCY osage.1
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "compress osage man page")
  install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/osage.1.gz
    DESTINATION ${MAN_INSTALL_DIR}/man1)
  add_custom_target(man-patchwork ALL DEPENDS patchwork.1.gz
                    COMMENT "patchwork man page")
  add_custom_command(
    OUTPUT patchwork.1.gz
    COMMAND ${GZIP} -9 --no-name --to-stdout patchwork.1
      >"${CMAKE_CURRENT_BINARY_DIR}/patchwork.1.gz"
    MAIN_DEPENDENCY patchwork.1
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "compress patchwork man page")
  install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/patchwork.1.gz
    DESTINATION ${MAN_INSTALL_DIR}/man1)
else()
  install(
    FILES dot.1 osage.1 patchwork.1
    DESTINATION ${MAN_INSTALL_DIR}/man1
  )
endif()

# run `dot -c`, which is not needed for dot itself but does create a config6
# file for programmatic use of libgvc to then find
if(NOT CMAKE_CROSSCOMPILING)
  configure_file(
    ../../cmake/configure_plugins.cmake.in configure_plugins.cmake @ONLY)

  install(SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/configure_plugins.cmake)
endif()
