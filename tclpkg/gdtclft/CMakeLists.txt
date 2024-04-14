if(GD_FOUND)
  add_library(gdtclft SHARED
    gdtclft.c
  )

  target_include_directories(gdtclft SYSTEM PRIVATE
    ${GD_INCLUDE_DIRS}
  )
  target_link_libraries(gdtclft PRIVATE ${GD_LIBRARIES})

  target_include_directories(gdtclft SYSTEM PRIVATE ${TCL_INCLUDE_PATH})
  target_link_libraries(gdtclft PRIVATE ${TCL_LIBRARY})

  install(
    TARGETS gdtclft
    RUNTIME DESTINATION ${BINARY_INSTALL_DIR}
    LIBRARY DESTINATION ${LIBRARY_INSTALL_DIR}/graphviz/tcl
    ARCHIVE DESTINATION ${LIBRARY_INSTALL_DIR}
  )

  add_custom_command(
    OUTPUT pkgIndex.tcl
    COMMAND sh "${CMAKE_CURRENT_SOURCE_DIR}/../mkpkgindex.sh" libgdtclft.so
      Gdtclft "${GRAPHVIZ_VERSION_STRING}"
    MAIN_DEPENDENCY gdtclft
    DEPENDS ../mkpkgindex.sh
    COMMENT "create gdtclft package index"
  )
endif()
