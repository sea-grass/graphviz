include(FindPackageHandleStandardArgs)

find_package(PkgConfig)
pkg_check_modules(GLIB glib-2.0)

if(MINGW)
  find_program(GLIB_RUNTIME_LIBRARY NAMES libglib-2.0-0.dll)
  find_package_handle_standard_args(GLIB DEFAULT_MSG
    GLIB_INCLUDE_DIRS
    GLIB_LIBRARIES
    GLIB_LINK_LIBRARIES
    GLIB_RUNTIME_LIBRARY
  )
  set(GLIB_RUNTIME_LIBRARIES
    ${GLIB_RUNTIME_LIBRARY}
  )
endif()
