# Set reference archive and base directory name
# set(RTEST2_REFERENCE_BASEZIPDIR "test_reference_2.21")
set(RTEST2_REFERENCE_BASEZIPDIR "test_reference_2.38")
set(RTEST2_REFERENCE_BASEZIPFILE "${RTEST2_REFERENCE_BASEZIPDIR}.zip")

# Copy minimal required test files from source directory
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/TESTS.json"
          "${CMAKE_CURRENT_SOURCE_DIR}/rtest2.py"
          "${CMAKE_CURRENT_SOURCE_DIR}/${RTEST2_REFERENCE_BASEZIPFILE}"
          DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# Note that CMake's 'tar' command will unpack zip files
execute_process(WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                COMMAND "${CMAKE_COMMAND}" -E tar x ${RTEST2_REFERENCE_BASEZIPFILE}
)

add_test(NAME rtest2
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
         COMMAND ${PYTHON_EXECUTABLE} rtest2.py -x $<TARGET_FILE:dot> -r ${RTEST2_REFERENCE_BASEZIPDIR}
         CONFIGURATIONS "Debug" "Release" ""
)