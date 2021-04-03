BISON_TARGET(Grammar ${CMAKE_CURRENT_SOURCE_DIR}/grammar.y ${CMAKE_CURRENT_BINARY_DIR}/grammar.c)
FLEX_TARGET(Scan ${CMAKE_CURRENT_SOURCE_DIR}/scan.l  ${CMAKE_CURRENT_BINARY_DIR}/scan.c)
ADD_FLEX_BISON_DEPENDENCY(Scan Grammar)

add_definitions(-DEXPORT_CGRAPH -DEXPORT_AGXBUF -DEXPORT_CGHDR -DYY_NO_UNISTD_H)

add_library(cgraph SHARED
    # Header files
    agxbuf.h
    cghdr.h
    cgraph.h
    itos.h
    likely.h
    sprint.h
    strcasecmp.h
    unreachable.h

    # Source files
    agerror.c
    agxbuf.c
    apply.c
    attr.c
    edge.c
    flatten.c
    graph.c
    id.c
    imap.c
    io.c
    mem.c
    node.c
    obj.c
    pend.c
    rec.c
    refstr.c
    subg.c
    utils.c
    write.c

    ${cxx_api_files}

    # Generated files
    ${BISON_Grammar_OUTPUTS}
    ${FLEX_Scan_OUTPUTS}
)

target_include_directories(cgraph PRIVATE
    ${GRAPHVIZ_LIB_DIR}
    ${GRAPHVIZ_LIB_DIR}/cdt
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(cgraph cdt)

# Installation location of library files
install(
    TARGETS cgraph
    RUNTIME DESTINATION ${BINARY_INSTALL_DIR}
    LIBRARY DESTINATION ${LIBRARY_INSTALL_DIR}
    ARCHIVE DESTINATION ${LIBRARY_INSTALL_DIR}
)

# Specify headers to be installed
install(
    FILES cgraph.h
    DESTINATION ${HEADER_INSTALL_DIR}
)

# Specify man pages to be installed
install(
    FILES cgraph.3
    DESTINATION ${MAN_INSTALL_DIR}
)

# Specify library version and soversion
set_target_properties(cgraph PROPERTIES
    VERSION 6.0.0
    SOVERSION 6
)

# C++ API

if (with_cxx_api)
    add_library(cgraph++ SHARED
        AGraph.h
        AGraph.cpp
        )
    set_property(TARGET cgraph++ PROPERTY CXX_STANDARD 20)
    set_property(TARGET cgraph++ PROPERTY CXX_STANDARD_REQUIRED ON)
    if (NOT ${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
        set_target_properties(cgraph++ PROPERTIES COMPILE_OPTIONS -Wextra)
    endif()

    target_include_directories(cgraph++ PRIVATE
        ${GRAPHVIZ_LIB_DIR}
        ${GRAPHVIZ_LIB_DIR}/cdt
        ${CMAKE_CURRENT_SOURCE_DIR}
    )

    target_link_libraries(cgraph++ cgraph)

    # Installation location of library files
    install(
        TARGETS cgraph++
        RUNTIME DESTINATION ${BINARY_INSTALL_DIR}
        LIBRARY DESTINATION ${LIBRARY_INSTALL_DIR}
        ARCHIVE DESTINATION ${LIBRARY_INSTALL_DIR}
    )

    # Specify headers to be installed
    install(
        FILES AGraph.h
        DESTINATION ${HEADER_INSTALL_DIR}
    )

    # Specify library version and soversion
    set_target_properties(cgraph++ PROPERTIES
        VERSION 6.0.0
        SOVERSION 6
    )
endif()
