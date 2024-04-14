add_library(tclplan SHARED
  find_ints.c
  intersect.c
  makecw.c
  tclpathplan.c
  wrapper.c
)

target_include_directories(tclplan PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
  ../../lib
  ../../lib/pathplan
)

target_link_libraries(tclplan PRIVATE
  pathplan
  tclhandle
  tclstubs
)

target_include_directories(tclplan SYSTEM PRIVATE ${TCL_INCLUDE_PATH})
target_link_libraries(tclplan PRIVATE ${TCL_LIBRARY})

install(
  TARGETS tclplan
  RUNTIME DESTINATION ${BINARY_INSTALL_DIR}
  LIBRARY DESTINATION ${LIBRARY_INSTALL_DIR}/graphviz/tcl
  ARCHIVE DESTINATION ${LIBRARY_INSTALL_DIR}
)

add_custom_command(
  OUTPUT pkgIndex.tcl
  COMMAND sh "${CMAKE_CURRENT_SOURCE_DIR}/../mkpkgindex.sh" libtclplan.so
    Tclpathplan "${GRAPHVIZ_VERSION_STRING}"
  MAIN_DEPENDENCY tclpathplan
  DEPENDS ../mkpkgindex.sh
  COMMENT "create tclpathplan package index"
)
