set(CPACK_PACKAGE_NAME                  ${PROJECT_NAME}                 )
SET(CPACK_PACKAGE_VERSION_MAJOR         ${GRAPHVIZ_VERSION_MAJOR}       )
SET(CPACK_PACKAGE_VERSION_MINOR         ${GRAPHVIZ_VERSION_MINOR}       )
SET(CPACK_PACKAGE_VERSION_PATCH         ${GRAPHVIZ_VERSION_BUILD}       )
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY   "Graph Visualization Tools"     )
set(CPACK_PACKAGE_VENDOR                Graphviz                        )
set(CPACK_PACKAGE_CONTACT               http://www.graphviz.org/        )
set(CPACK_RESOURCE_FILE_LICENSE         "${TOP_SOURCE_DIR}/LICENSE"     )
set(CPACK_RESOURCE_FILE_README          "${TOP_SOURCE_DIR}/README.md"   )

set(CPACK_GENERATOR ZIP)

find_package(NSIS)
if(NSIS_FOUND)
    set(CPACK_NSIS_MUI_ICON                 "${TOP_SOURCE_DIR}/windows/build/Graphviz.ico"  )
    set(CPACK_NSIS_MUI_UNIICON              "${TOP_SOURCE_DIR}/windows/build/Graphviz.ico"  )
    set(CPACK_NSIS_INSTALLED_ICON_NAME      "Uninstall.exe"                                 )
    set(CPACK_NSIS_HELP_LINK                "http://www.graphviz.org"                       )
    set(CPACK_NSIS_URL_INFO_ABOUT           "http://www.graphviz.org"                       )
    set(CPACK_NSIS_MODIFY_PATH              ON                                              )
    LIST(APPEND CPACK_GENERATOR NSIS)
endif()

# Only install man pages on non-Windows systems
if(NOT WIN32)
    # Specify man pages to be installed
    install(
        FILES ${CMAKE_CURRENT_SOURCE_DIR}/graphviz.7
        DESTINATION ${MAN7_INSTALL_DIR}
    )
endif()

# All platforms: Install PDFs somewhere sensible
install(FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/doc/dotguide.pdf
    ${CMAKE_CURRENT_SOURCE_DIR}/doc/dottyguide.pdf
    ${CMAKE_CURRENT_SOURCE_DIR}/doc/leftyguide.pdf
    ${CMAKE_CURRENT_SOURCE_DIR}/doc/libgraph/Agraph.pdf
    ${CMAKE_CURRENT_SOURCE_DIR}/doc/libguide/libguide.pdf
    ${CMAKE_CURRENT_SOURCE_DIR}/doc/neatoguide.pdf
    ${CMAKE_CURRENT_SOURCE_DIR}/doc/odt/graphviz_plugins.pdf
#    ${CMAKE_CURRENT_SOURCE_DIR}/doc/oldlibguide.pdf # Do we need this?
    ${CMAKE_CURRENT_SOURCE_DIR}/doc/smyrna/smyrna.pdf
    DESTINATION doc/pdf
)

# Only install PDF versions of application and library man pages on Windows systems
# Note: These are all translated from nroff to PDF via pandoc and xelatex
# (part of LaTeX) - see doc/mantopdf/README.md
if(WIN32)
    # Application manual pages
    install(FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/doc/mantopdf/graphviz.7.pdf
        ${CMAKE_CURRENT_SOURCE_DIR}/doc/mantopdf/acyclic.1.pdf
        ${CMAKE_CURRENT_SOURCE_DIR}/doc/mantopdf/bcomps.1.pdf
        ${CMAKE_CURRENT_SOURCE_DIR}/doc/mantopdf/ccomps.1.pdf
        ${CMAKE_CURRENT_SOURCE_DIR}/doc/mantopdf/cluster.1.pdf
        ${CMAKE_CURRENT_SOURCE_DIR}/doc/mantopdf/diffimg.1.pdf
        ${CMAKE_CURRENT_SOURCE_DIR}/doc/mantopdf/dijkstra.1.pdf
        ${CMAKE_CURRENT_SOURCE_DIR}/doc/mantopdf/dot.1.pdf
        ${CMAKE_CURRENT_SOURCE_DIR}/doc/mantopdf/dotty.1.pdf
        ${CMAKE_CURRENT_SOURCE_DIR}/doc/mantopdf/edgepaint.1.pdf
        ${CMAKE_CURRENT_SOURCE_DIR}/doc/mantopdf/gc.1.pdf
        ${CMAKE_CURRENT_SOURCE_DIR}/doc/mantopdf/gml2gv.1.pdf
        ${CMAKE_CURRENT_SOURCE_DIR}/doc/mantopdf/graphml2gv.1.pdf
        ${CMAKE_CURRENT_SOURCE_DIR}/doc/mantopdf/gvcolor.1.pdf
        ${CMAKE_CURRENT_SOURCE_DIR}/doc/mantopdf/gvedit.1.pdf
        ${CMAKE_CURRENT_SOURCE_DIR}/doc/mantopdf/gvgen.1.pdf
        ${CMAKE_CURRENT_SOURCE_DIR}/doc/mantopdf/gvmap.1.pdf
        ${CMAKE_CURRENT_SOURCE_DIR}/doc/mantopdf/gvmap.sh.1.pdf
        ${CMAKE_CURRENT_SOURCE_DIR}/doc/mantopdf/gvpack.1.pdf
        ${CMAKE_CURRENT_SOURCE_DIR}/doc/mantopdf/gvpr.1.pdf
        ${CMAKE_CURRENT_SOURCE_DIR}/doc/mantopdf/gxl2gv.1.pdf
        ${CMAKE_CURRENT_SOURCE_DIR}/doc/mantopdf/inkpot.1.pdf
        ${CMAKE_CURRENT_SOURCE_DIR}/doc/mantopdf/lefty.1.pdf
        ${CMAKE_CURRENT_SOURCE_DIR}/doc/mantopdf/lneato.1.pdf
        ${CMAKE_CURRENT_SOURCE_DIR}/doc/mantopdf/mingle.1.pdf
        ${CMAKE_CURRENT_SOURCE_DIR}/doc/mantopdf/mm2gv.1.pdf
        ${CMAKE_CURRENT_SOURCE_DIR}/doc/mantopdf/nop.1.pdf
        ${CMAKE_CURRENT_SOURCE_DIR}/doc/mantopdf/osage.1.pdf
        ${CMAKE_CURRENT_SOURCE_DIR}/doc/mantopdf/patchwork.1.pdf
        ${CMAKE_CURRENT_SOURCE_DIR}/doc/mantopdf/prune.1.pdf
        ${CMAKE_CURRENT_SOURCE_DIR}/doc/mantopdf/sccmap.1.pdf
        ${CMAKE_CURRENT_SOURCE_DIR}/doc/mantopdf/smyrna.1.pdf
        ${CMAKE_CURRENT_SOURCE_DIR}/doc/mantopdf/tred.1.pdf
        ${CMAKE_CURRENT_SOURCE_DIR}/doc/mantopdf/unflatten.1.pdf
        ${CMAKE_CURRENT_SOURCE_DIR}/doc/mantopdf/vimdot.1.pdf
        DESTINATION doc/pdf/applications
    )

    # Library manual pages
    install(FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/doc/mantopdf/agraph.3.pdf
        ${CMAKE_CURRENT_SOURCE_DIR}/doc/mantopdf/cdt.3.pdf
        ${CMAKE_CURRENT_SOURCE_DIR}/doc/mantopdf/cgraph.3.pdf
        ${CMAKE_CURRENT_SOURCE_DIR}/doc/mantopdf/expr.3.pdf
        ${CMAKE_CURRENT_SOURCE_DIR}/doc/mantopdf/gvc.3.pdf
        ${CMAKE_CURRENT_SOURCE_DIR}/doc/mantopdf/gvpr.3.pdf
        ${CMAKE_CURRENT_SOURCE_DIR}/doc/mantopdf/inkpot.3.pdf
        ${CMAKE_CURRENT_SOURCE_DIR}/doc/mantopdf/lab_gamut.3.pdf
        ${CMAKE_CURRENT_SOURCE_DIR}/doc/mantopdf/pack.3.pdf
        ${CMAKE_CURRENT_SOURCE_DIR}/doc/mantopdf/pathplan.3.pdf
        ${CMAKE_CURRENT_SOURCE_DIR}/doc/mantopdf/xdot.3.pdf
        DESTINATION doc/pdf/library
    )
endif()
