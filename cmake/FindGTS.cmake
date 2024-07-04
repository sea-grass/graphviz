include(FindPackageHandleStandardArgs)
if(WIN32)
  find_package(GLIB)

  find_path(GTS_INCLUDE_DIR gts.h)
  find_path(GLIBCONFIG_INCLUDE_DIR glibconfig.h
            PATH_SUFFIXES glib-2.0/include lib/glib-2.0/include)

  find_library(GTS_LIBRARY NAMES gts)

  find_program(GTS_RUNTIME_LIBRARY NAMES gts.dll libgts-0-7-5.dll)

  find_package_handle_standard_args(GTS DEFAULT_MSG
    GTS_INCLUDE_DIR
    GLIB_INCLUDE_DIRS
    GLIBCONFIG_INCLUDE_DIR

    GTS_LIBRARY
    GLIB_LIBRARIES

    GTS_RUNTIME_LIBRARY
    GLIB_RUNTIME_LIBRARIES
  )

  set(GTS_INCLUDE_DIRS
    ${GTS_INCLUDE_DIR}
    ${GLIB_INCLUDE_DIRS}
    ${GLIBCONFIG_INCLUDE_DIR}
  )

  set(GTS_LINK_LIBRARIES
    ${GTS_LIBRARY}
    ${GLIB_LIBRARIES}
  )

  set(GTS_RUNTIME_LIBRARIES
    ${GTS_RUNTIME_LIBRARY}
    ${GLIB_RUNTIME_LIBRARIES}
  )
else()
  find_package(PkgConfig)
  pkg_check_modules(GTS gts)

  find_package_handle_standard_args(GTS DEFAULT_MSG
    GTS_INCLUDE_DIRS
    GTS_LIBRARIES
    GTS_LINK_LIBRARIES
  )
endif()
