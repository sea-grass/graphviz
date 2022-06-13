if(CAIRO_FOUND AND PANGOCAIRO_FOUND)

  add_library(gvplugin_pango_obj OBJECT
    # Header files
    gvgetfontlist.h
    gvplugin_pango.h

    # Source files
    gvgetfontlist_pango.c
    gvloadimage_pango.c
    gvplugin_pango.c
    gvrender_pango.c
    gvtextlayout_pango.c
  )

  target_include_directories(gvplugin_pango_obj PRIVATE
    ../../lib
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}/lib/common
    ../../lib/cdt
    ../../lib/cgraph
    ../../lib/common
    ../../lib/gvc
    ../../lib/pathplan
  )

  target_include_directories(gvplugin_pango_obj SYSTEM PRIVATE
    ${CAIRO_INCLUDE_DIRS}/cairo
    ${PANGOCAIRO_INCLUDE_DIRS}
  )

  set_target_properties(gvplugin_pango_obj
    PROPERTIES POSITION_INDEPENDENT_CODE 1)

  add_library(gvplugin_pango SHARED $<TARGET_OBJECTS:gvplugin_pango_obj>)
  add_library(gvplugin_pango_static STATIC $<TARGET_OBJECTS:gvplugin_pango_obj>)
  target_link_libraries(gvplugin_pango
    cgraph
    gvc
    ${CAIRO_LINK_LIBRARIES}
    ${PANGOCAIRO_LINK_LIBRARIES}
  )
  target_link_libraries(gvplugin_pango_static
    cgraph
    gvc
    ${CAIRO_LINK_LIBRARIES}
    ${PANGOCAIRO_LINK_LIBRARIES}
  )

  # Installation location of library files
  install(
    TARGETS gvplugin_pango
    RUNTIME DESTINATION ${BINARY_INSTALL_DIR}
    LIBRARY DESTINATION ${PLUGIN_INSTALL_DIR}
    ARCHIVE DESTINATION ${LIBRARY_INSTALL_DIR}
  )

  # Include DLLs with this library on Windows
  if(WIN32 AND install_win_dependency_dlls)
    install(
      FILES
          ${CAIRO_RUNTIME_LIBRARIES}
          ${PANGOCAIRO_RUNTIME_LIBRARIES}
      DESTINATION ${BINARY_INSTALL_DIR}
    )
  endif()

  # Specify library version and soversion
  set_target_properties(gvplugin_pango PROPERTIES
    VERSION ${GRAPHVIZ_PLUGIN_VERSION}.0.0
    SOVERSION ${GRAPHVIZ_PLUGIN_VERSION}
  )

  if(MINGW)
    # work around https://gitlab.kitware.com/cmake/cmake/-/issues/21716
    set_target_properties(gvplugin_pango PROPERTIES
      RUNTIME_OUTPUT_NAME gvplugin_pango-${GRAPHVIZ_PLUGIN_VERSION}
    )
  endif()

endif()
