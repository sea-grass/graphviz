set(SOURCES
  tcldot.c
  tcldot.h
  tcldot-graphcmd.c
  tcldot-nodecmd.c
  tcldot-edgecmd.c
  tcldot-util.c
  tcldot-id.c
  tcldot-io.c
  no_builtins.c
)

if(GD_FOUND)
  list(APPEND SOURCES $<TARGET_OBJECTS:gdtclft>)
endif()

add_library(tcldot SHARED ${SOURCES})

target_compile_definitions(tcldot PRIVATE -DDEMAND_LOADING=1)

target_link_libraries(tcldot PRIVATE
  tclhandle
  tclstubs
  cdt
  cgraph
  pathplan
  gvc
)
target_include_directories(tcldot SYSTEM PRIVATE ${TCL_INCLUDE_PATH})
target_link_libraries(tcldot PRIVATE ${TCL_LIBRARY})
if(GD_FOUND)
  target_include_directories(tcldot SYSTEM PRIVATE ${GD_INCLUDE_DIRS})
  target_link_libraries(tcldot PRIVATE ${GD_LIBRARIES})
endif()

install(
  TARGETS tcldot
  RUNTIME DESTINATION ${BINARY_INSTALL_DIR}
  LIBRARY DESTINATION ${LIBRARY_INSTALL_DIR}/graphviz/tcl
  ARCHIVE DESTINATION ${LIBRARY_INSTALL_DIR}
)

add_custom_target(tcldot_pkgindex
  COMMAND sh "${CMAKE_CURRENT_SOURCE_DIR}/../mkpkgindex.sh" libtcldot.so
    Tcldot "${GRAPHVIZ_VERSION_STRING}"
  DEPENDS ../mkpkgindex.sh
  BYPRODUCTS pkgIndex.tcl
  COMMENT "create tcldot package index"
)
add_dependencies(tcldot_pkgindex tcldot)

if(GZIP)
  add_custom_target(man-tcldot ALL DEPENDS tcldot.3tcl.gz
                    COMMENT "tcldot man page")
  add_custom_command(
    OUTPUT tcldot.3tcl.gz
    COMMAND ${GZIP} -9 --no-name --to-stdout tcldot.3tcl
      >"${CMAKE_CURRENT_BINARY_DIR}/tcldot.3tcl.gz"
    MAIN_DEPENDENCY tcldot.3tcl
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "compress tcldot man page")
  install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/tcldot.3tcl.gz
    DESTINATION ${MAN_INSTALL_DIR}/man3)
else()
  install(
    FILES tcldot.3tcl
    DESTINATION ${MAN_INSTALL_DIR}/man3
  )
endif()
