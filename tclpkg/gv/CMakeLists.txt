if(enable_swig)
  set(UseSWIG_MODULE_VERSION 2) # cmake-lint: disable=C0103
  include(UseSWIG)

  set_property(SOURCE gv.i PROPERTY CPLUSPLUS ON)

  if(enable_tcl)
    swig_add_library(gv_tcl
      LANGUAGE tcl
      SOURCES gv.i gv_tcl_init.c
    )
    set_property(TARGET gv_tcl PROPERTY SWIG_COMPILE_OPTIONS -namespace)
    target_include_directories(gv_tcl PRIVATE SYSTEM ${TCL_INCLUDE_PATH})
    swig_link_libraries(gv_tcl PRIVATE gvc tclstubs ${TCL_LIBRARY})

    install(
      TARGETS gv_tcl
      RUNTIME DESTINATION ${BINARY_INSTALL_DIR}
      LIBRARY DESTINATION ${LIBRARY_INSTALL_DIR}/graphviz/tcl
      ARCHIVE DESTINATION ${LIBRARY_INSTALL_DIR}
    )

    add_custom_target(gv_tcl_pkgindex
      COMMAND sh "${CMAKE_CURRENT_SOURCE_DIR}/../mkpkgindex.sh" libgv_tcl.so
        gv 0
      DEPENDS ../mkpkgindex.sh
      BYPRODUCTS pkgIndex.tcl
      COMMENT "create gv_tcl package index"
    )
    add_dependencies(gv_tcl_pkgindex gv_tcl)

    add_custom_target(man-bindings ALL
      DEPENDS gv.3go gv.3guile gv.3java gv.3lua gv.3perl gv.3php gv.3python
        gv.3r gv.3ruby gv.3sharp gv.3tcl
      COMMENT "man pages for language bindings"
    )
    add_custom_command(
      OUTPUT gv.3go gv.3guile gv.3java gv.3lua gv.3perl gv.3php gv.3python gv.3r
        gv.3ruby gv.3sharp gv.3tcl
      COMMAND ${TCL_TCLSH} "${CMAKE_CURRENT_SOURCE_DIR}/gv_doc_writer.tcl"
        ${CMAKE_CURRENT_SOURCE_DIR}
      MAIN_DEPENDENCY gv_doc_writer.tcl
      DEPENDS gv_doc_langs.tcl gv_doc_template.tcl
      COMMENT "generate man pages for language bindings"
    )

    if(GZIP)
      add_custom_target(man-gv_tcl ALL DEPENDS gv.3tcl.gz
                        COMMENT "gv_tcl man page")
      add_custom_command(
        OUTPUT gv.3tcl.gz
        COMMAND ${GZIP} -9 --no-name --to-stdout gv.3tcl
          >"${CMAKE_CURRENT_BINARY_DIR}/gv.3tcl.gz"
        MAIN_DEPENDENCY gv.3tcl
        COMMENT "compress gv_tcl man page")
      install(
        FILES ${CMAKE_CURRENT_BINARY_DIR}/gv.3tcl.gz
        DESTINATION ${MAN_INSTALL_DIR}/man3)
    else()
      install(
        FILES ${CMAKE_CURRENT_BINARY_DIR}/gv.3tcl
        DESTINATION ${MAN_INSTALL_DIR}/man3
      )
    endif()
  endif()
endif()
