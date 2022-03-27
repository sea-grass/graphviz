set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
find_package(Qt5 COMPONENTS Core Gui)

if(Qt5_FOUND)

  add_executable(gvedit
    csettings.h
    imageviewer.h
    mainwindow.h
    mdichild.h
    ui_settings.h 
    csettings.cpp
    imageviewer.cpp
    main.cpp
    mainwindow.cpp
    mdichild.cpp
    mdi.qrc
  )

  target_compile_definitions(gvedit
    -DDEMAND_LOADING=1
    -DGVEDIT_DATADIR="${DATA_INSTALL_DIR}/graphviz/gvedit"
  )

  target_include_directories(gvedit PRIVATE
    ${GRAPHVIZ_LIB_DIR}
    ${GRAPHVIZ_LIB_DIR}/cgraph
    ${GRAPHVIZ_LIB_DIR}/common
    ${GRAPHVIZ_LIB_DIR}/gvc
    ${GRAPHVIZ_LIB_DIR}/pathplan
  )

  target_link_libraries(gvedit
    cdt
    cgraph
    gvc
    Qt5::Core
    Qt5::Gui
  )

  find_program(GZIP gzip)
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

  install(
    TARGETS gvedit
    RUNTIME DESTINATION ${BINARY_INSTALL_DIR}
    LIBRARY DESTINATION ${LIBRARY_INSTALL_DIR}
    ARCHIVE DESTINATION ${LIBRARY_INSTALL_DIR}
  )

  install(
    FILES ../../doc/infosrc/attrs
    RENAME attrs.txt
    DESTINATION ${DATA_INSTALL_DIR}/graphviz/gvedit
  )

endif()
