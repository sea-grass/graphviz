find_program(XCODEBUILD NAMES xcodebuild)
find_program(PKGBUILD NAMES pkgbuild)
find_program(PRODUCTBUILD NAMES productbuild)

set(GV_PKG ${CMAKE_BINARY_DIR}/graphviz.pkg)

set(PREFIX ${CMAKE_INSTALL_PREFIX})
file(GLOB_RECURSE sources "*.m")
file(GLOB_RECURSE headers "*.h")
file(GLOB_RECURSE ibfiles "*.xib")

configure_file(Info.plist.in ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist @ONLY)
configure_file(Distribution.xml.in Distribution.xml @ONLY)
file(GENERATE
  OUTPUT Scripts/postinstall
  CONTENT "#!/bin/sh
logger -is -t \"Graphviz Install\" \"register dot plugins\"
${PREFIX}/bin/dot -c
echo \"${PREFIX}/bin\" >/etc/paths.d/graphviz
"
  FILE_PERMISSIONS
    OWNER_READ OWNER_WRITE OWNER_EXECUTE
    GROUP_READ GROUP_EXECUTE
    WORLD_READ WORLD_EXECUTE
)

add_custom_target(pkg
  COMMENT "Built macOS Installer Package"
  DEPENDS ${GV_PKG}
)

add_custom_command(OUTPUT ${GV_PKG}
  COMMENT "Build macOS Installer Package"
  DEPENDS
    ${headers}
    ${sources}
    ${ibfiles}
    $(DESTDIR)${PREFIX}/bin/dot
    ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist
    ${CMAKE_CURRENT_SOURCE_DIR}/Component.plist
    Distribution.xml
    Scripts/postinstall
  COMMAND ${XCODEBUILD}
    -project ${CMAKE_CURRENT_SOURCE_DIR}/graphviz.xcodeproj
    install
    ARCHS="${CMAKE_OSX_ARCHITECTURES}"
    DSTROOT=$(DESTDIR)
    LIBRARY_SEARCH_PATHS=${CMAKE_INSTALL_RPATH}
    LD_RUNPATH_SEARCH_PATHS=${CMAKE_INSTALL_RPATH}
  COMMAND ${PKGBUILD}
    --root $(DESTDIR)
    --scripts ${CMAKE_CURRENT_BINARY_DIR}/Scripts
    --identifier com.att.graphviz
    --component-plist ${CMAKE_CURRENT_SOURCE_DIR}/Component.plist
    ${CMAKE_CURRENT_BINARY_DIR}/app.pkg
  COMMAND ${PRODUCTBUILD}
    --package-path ${CMAKE_CURRENT_BINARY_DIR}
    --distribution ${CMAKE_CURRENT_BINARY_DIR}/Distribution.xml
    ${GV_PKG}
)
