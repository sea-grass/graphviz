if(GD_FOUND)

  add_library(gvplugin_gd_obj OBJECT
    # Source files
    gdioctx_wrapper.h
    gvdevice_gd.c
    gvloadimage_gd.c
    gvplugin_gd.c
    gvrender_gd.c
    gvrender_gd_vrml.c
    gvtextlayout_gd.c
  )

  target_include_directories(gvplugin_gd_obj PRIVATE
    ../../lib
    ${CMAKE_CURRENT_SOURCE_DIR}
    ../../lib/cdt
    ../../lib/cgraph
    ../../lib/common
    ../../lib/gvc
    ../../lib/pathplan
  )

  target_include_directories(gvplugin_gd_obj SYSTEM PRIVATE
    ${GD_INCLUDE_DIRS}
  )

  set_target_properties(gvplugin_gd_obj PROPERTIES POSITION_INDEPENDENT_CODE 1)

  add_library(gvplugin_gd SHARED $<TARGET_OBJECTS:gvplugin_gd_obj>)
  add_library(gvplugin_gd_static STATIC $<TARGET_OBJECTS:gvplugin_gd_obj>)
  target_link_libraries(gvplugin_gd
    cgraph
    gvc
    pathplan
    ${GD_LIBRARIES}
  )
  target_link_libraries(gvplugin_gd_static
    cgraph
    gvc
    pathplan
    ${GD_LIBRARIES}
  )

  # Installation location of library files
  install(
    TARGETS gvplugin_gd
    RUNTIME DESTINATION ${BINARY_INSTALL_DIR}
    LIBRARY DESTINATION ${PLUGIN_INSTALL_DIR}
    ARCHIVE DESTINATION ${LIBRARY_INSTALL_DIR}
  )

  # Include DLLs with this library on Windows
  if(WIN32 AND install_win_dependency_dlls)
    install(
      FILES
        ${GD_RUNTIME_LIBRARIES}
      DESTINATION ${BINARY_INSTALL_DIR}
    )
  endif()

  # Specify library version and soversion
  set_target_properties(gvplugin_gd PROPERTIES
    VERSION ${GRAPHVIZ_PLUGIN_VERSION}.0.0
    SOVERSION ${GRAPHVIZ_PLUGIN_VERSION}
  )

  if(MINGW)
    # work around https://gitlab.kitware.com/cmake/cmake/-/issues/21716
    set_target_properties(gvplugin_gd PROPERTIES
      RUNTIME_OUTPUT_NAME gvplugin_gd-${GRAPHVIZ_PLUGIN_VERSION}
    )
  endif()

endif()
