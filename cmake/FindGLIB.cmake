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
elseif(WIN32)
  find_path(GLIB_INCLUDE_DIR glib.h PATH_SUFFIXES glib-2.0)

  find_library(GLIB_LIBRARY NAMES glib-2.0)

  find_program(GLIB_RUNTIME_LIBRARY NAMES glib-2.dll)

  find_package_handle_standard_args(GLIB DEFAULT_MSG
    GLIB_INCLUDE_DIR
    GLIB_LIBRARY
    GLIB_LINK_LIBRARY
    GLIB_RUNTIME_LIBRARY
  )

  set(GLIB_INCLUDE_DIRS
    ${GLIB_INCLUDE_DIR}
  )

  set(GLIB_LIBRARIES
    glib-2.0
  )

  set(GLIB_LINK_LIBRARIES
    ${GLIB_LIBRARY}
  )

  set(GLIB_RUNTIME_LIBRARIES
    ${GLIB_RUNTIME_LIBRARY}
  )
endif()
