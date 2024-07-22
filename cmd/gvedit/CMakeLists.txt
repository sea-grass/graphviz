if(NOT WITH_GVEDIT STREQUAL "OFF")
  find_package(Qt6 COMPONENTS Core PrintSupport Widgets)
  if(Qt6_FOUND)
    set(QT_LIBS Qt6::Core Qt6::PrintSupport Qt6::Widgets)
  else()
    find_package(Qt5 COMPONENTS Core PrintSupport Widgets)
    if(Qt5_FOUND)
      set(QT_LIBS Qt5::Core Qt5::PrintSupport Qt5::Widgets)
    endif()
  endif()
  if(WITH_GVEDIT STREQUAL "AUTO")
    if(Qt6_FOUND)
      message(STATUS "setting -DWITH_GVEDIT=ON with Qt6")
      set(WITH_GVEDIT ON)
    elseif(Qt5_FOUND)
      message(STATUS "setting -DWITH_GVEDIT=ON with Qt5")
      set(WITH_GVEDIT ON)
    else()
      message(STATUS "setting -DWITH_GVEDIT=OFF")
      set(WITH_GVEDIT OFF)
    endif()
  elseif(NOT Qt6_FOUND AND NOT Qt5_FOUND)
    message(FATAL_ERROR "-DWITH_GVEDIT=ON and neither Qt5 nor Qt6 found")
  endif()
endif()

if(WITH_GVEDIT)
  add_executable(gvedit
    csettings.cpp
    csettings.h
    imageviewer.cpp
    imageviewer.h
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mdi.qrc
    mdichild.cpp
    mdichild.h
    ui_settings.h
  )

  set_target_properties(gvedit PROPERTIES
    AUTOMOC ON
    AUTORCC ON
    AUTOUIC ON
    AUTOUIC_SEARCH_PATHS "${CMAKE_CURRENT_SOURCE_DIR}/ui"
  )

  target_compile_definitions(gvedit PRIVATE
    DEMAND_LOADING=1
    QT_NO_CAST_TO_ASCII
    QT_NO_CAST_FROM_ASCII
    QT_STRICT_ITERATORS
    QT_NO_URL_CAST_FROM_STRING
    QT_NO_CAST_FROM_BYTEARRAY
    QT_USE_QSTRINGBUILDER
    QT_USE_FAST_OPERATOR_PLUS
  )

  target_include_directories(gvedit PRIVATE
    ../../lib
    ../../lib/cdt
    ../../lib/cgraph
    ../../lib/common
    ../../lib/gvc
    ../../lib/pathplan
  )

  target_link_libraries(gvedit PRIVATE
    cdt
    cgraph
    gvc
  )

  target_link_libraries(gvedit PRIVATE ${QT_LIBS})

  if(EXPAT_FOUND)
    target_include_directories(gvedit SYSTEM PRIVATE ${EXPAT_INCLUDE_DIRS})
    target_link_libraries(gvedit PRIVATE ${EXPAT_LIBRARIES})
  endif()

  install(
    TARGETS gvedit
    RUNTIME DESTINATION ${BINARY_INSTALL_DIR}
    LIBRARY DESTINATION ${LIBRARY_INSTALL_DIR}
    ARCHIVE DESTINATION ${LIBRARY_INSTALL_DIR}
  )

  if(WIN32 AND NOT MINGW AND install_win_dependency_dlls)
    set(TOOLS_QT6_BIN ${WINDOWS_DEPENDENCY_VCPKG_DIR}/tools/Qt6/bin)
    set(QTPATHS_DEBUG_BAT ${TOOLS_QT6_BIN}/qtpaths.debug.bat)
    qt_generate_deploy_app_script(
      TARGET gvedit
      OUTPUT_SCRIPT deploy_script
      DEPLOY_TOOL_OPTIONS
        # workaround for https://github.com/microsoft/vcpkg/issues/17840 or
        # https://github.com/microsoft/vcpkg/issues/36250
        $<$<CONFIG:DEBUG>:--qtpaths ${QTPATHS_DEBUG_BAT}>
    )
    install(SCRIPT ${deploy_script})

    find_program(DOUBLE_CONVERSION_RT_LIB NAMES double-conversion.dll)
    find_program(PCRE_DBG_RT_LIB
                 NAMES pcre2-16d.dll
                 PATHS ${WINDOWS_DEPENDENCY_VCPKG_DIR}/debug/bin
    )
    find_program(PCRE_REL_RT_LIB NAMES pcre2-16.dll)
    find_program(ZLIB_DBG_RT_LIB
                 NAMES zlibd1.dll
                 PATHS ${WINDOWS_DEPENDENCY_VCPKG_DIR}/debug/bin
    )
    find_program(ZLIB_REL_RT_LIB NAMES zlib1.dll)
    find_program(LIBPNG_DBG_RT_LIB
                 NAMES libpng16d.dll
                 PATHS ${WINDOWS_DEPENDENCY_VCPKG_DIR}/debug/bin
    )
    find_program(LIBPNG_REL_RT_LIB NAMES libpng16.dll)
    find_program(FREETYPE_DBG_RT_LIB
                 NAMES freetyped.dll
                 PATHS ${WINDOWS_DEPENDENCY_VCPKG_DIR}/debug/bin
    )
    find_program(BZ2_DBG_RT_LIB
                 NAMES bz2d.dll
                 PATHS ${WINDOWS_DEPENDENCY_VCPKG_DIR}/debug/bin
    )
    find_program(BZ2_REL_RT_LIB NAMES bz2.dll)
    find_program(FREETYPE_REL_RT_LIB NAMES freetype.dll)
    find_program(ZSTD_RT_LIB NAMES zstd.dll)
    if(CMAKE_CL_64)
      find_program(LIBCRYPTO_3_RT_LIB NAMES libcrypto-3-x64.dll)
    else()
      find_program(LIBCRYPTO_3_RT_LIB NAMES libcrypto-3.dll)
    endif()
    install(
      FILES
        ${DOUBLE_CONVERSION_RT_LIB}
        $<IF:$<CONFIG:DEBUG>,${PCRE_DBG_RT_LIB},${PCRE_REL_RT_LIB}>
        $<IF:$<CONFIG:DEBUG>,${ZLIB_DBG_RT_LIB},${ZLIB_REL_RT_LIB}>
        $<IF:$<CONFIG:DEBUG>,${LIBPNG_DBG_RT_LIB},${LIBPNG_REL_RT_LIB}>
        $<IF:$<CONFIG:DEBUG>,${BZ2_DBG_RT_LIB},${BZ2_REL_RT_LIB}>
        $<IF:$<CONFIG:DEBUG>,${FREETYPE_DBG_RT_LIB},${FREETYPE_REL_RT_LIB}>
        ${ZSTD_RT_LIB}
        ${LIBCRYPTO_3_RT_LIB}
      DESTINATION ${BINARY_INSTALL_DIR}
    )
  endif()

  install(
    FILES ../../doc/infosrc/attrs
    DESTINATION ${DATA_INSTALL_DIR}/graphviz/gvedit
    RENAME attrs.txt
  )

  if(GZIP)
    add_custom_target(man-gvedit ALL DEPENDS gvedit.1.gz
                      COMMENT "gvedit man page")
    add_custom_command(
      OUTPUT gvedit.1.gz
      COMMAND ${GZIP} -9 --no-name --to-stdout gvedit.1
        >"${CMAKE_CURRENT_BINARY_DIR}/gvedit.1.gz"
      MAIN_DEPENDENCY gvedit.1
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
      COMMENT "compress gvedit man page")
    install(
      FILES ${CMAKE_CURRENT_BINARY_DIR}/gvedit.1.gz
      DESTINATION ${MAN_INSTALL_DIR}/man1)
  else()
    install(
      FILES gvedit.1
      DESTINATION ${MAN_INSTALL_DIR}/man1
    )
  endif()
endif()
