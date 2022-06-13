add_library(gvplugin_neato_layout_obj OBJECT
  # Source files
  gvplugin_neato_layout.c
  gvlayout_neato_layout.c
)

target_include_directories(gvplugin_neato_layout_obj PRIVATE
  ../../lib
  ${CMAKE_CURRENT_SOURCE_DIR}
  ../../lib/cdt
  ../../lib/cgraph
  ../../lib/common
  ../../lib/gvc
  ../../lib/pathplan
)

set_target_properties(gvplugin_neato_layout_obj
  PROPERTIES POSITION_INDEPENDENT_CODE 1)

add_library(gvplugin_neato_layout SHARED
  $<TARGET_OBJECTS:gvplugin_neato_layout_obj>)
add_library(gvplugin_neato_layout_static STATIC
  $<TARGET_OBJECTS:gvplugin_neato_layout_obj>)
target_link_libraries(gvplugin_neato_layout
  circogen
  fdpgen
  gvc
  neatogen
  osage
  patchwork
  rbtree
  sparse
  twopigen
)
target_link_libraries(gvplugin_neato_layout_static
  circogen
  fdpgen
  gvc
  neatogen
  osage
  patchwork
  rbtree
  sparse
  twopigen
)

if(with_ipsepcola)
  target_link_libraries(gvplugin_neato_layout vpsc)
  target_link_libraries(gvplugin_neato_layout_static vpsc)
endif()

if(with_sfdp)
  target_link_libraries(gvplugin_neato_layout sfdpgen)
  target_link_libraries(gvplugin_neato_layout_static sfdpgen)
endif()

# Installation location of library files
install(
  TARGETS gvplugin_neato_layout
  RUNTIME DESTINATION ${BINARY_INSTALL_DIR}
  LIBRARY DESTINATION ${PLUGIN_INSTALL_DIR}
  ARCHIVE DESTINATION ${LIBRARY_INSTALL_DIR}
)

# Specify library version and soversion
set_target_properties(gvplugin_neato_layout PROPERTIES
  VERSION ${GRAPHVIZ_PLUGIN_VERSION}.0.0
  SOVERSION ${GRAPHVIZ_PLUGIN_VERSION}
)

if(MINGW)
  # work around https://gitlab.kitware.com/cmake/cmake/-/issues/21716
  set_target_properties(gvplugin_neato_layout PROPERTIES
    RUNTIME_OUTPUT_NAME gvplugin_neato_layout-${GRAPHVIZ_PLUGIN_VERSION}
  )
endif()
