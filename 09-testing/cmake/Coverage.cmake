
if (${PROJECT_NAME}_ENABLE_COVERAGE AND NOT TARGET coverage)
  find_program(LCOV_PATH lcov REQUIRED)
  find_program(GENHTML_PATH genhtml REQUIRED)

  # this target has to be build manually: `cmake --build './build' -t coverage`
  add_custom_target(coverage # ALL # if coverage should be included in the all default target
    # Collect metrics for the solution from the current directory.
    COMMAND ${LCOV_PATH}  --directory "." --capture --output-file "coverage.info"
    # Remove unwanted coverage data on system headers.
    COMMAND ${LCOV_PATH}  --remove "coverage.info" '/usr/include/*'
                          --output-file "filtered-coverage.info"
    # Generate HTML report in the coverage directory
    COMMAND ${GENHTML_PATH} --output-directory "coverage" "filtered-coverage.info"
                            --title "${PROJECT_NAME}"
                            --show-details  --legend  --frames  --highlight  --demangle-cpp
    # Remove temporary .info files.
    COMMAND rm -rf  coverage.info  filtered-coverage.info
    BYPRODUCTS  ${CMAKE_BINARY_DIR}/coverage
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Collecting coverage metrics and generating HTML report in `${CMAKE_BINARY_DIR}/coverage'."
  )
endif()


## Enable code coverage by adding extra compiler/linker flags
##   Compilation process will generate coverage note (.gcno) files.
function(EnableCoverage target)
  if (NOT ${PROJECT_NAME}_ENABLE_COVERAGE)
    return()
  endif()
  if (NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(WARNING "Enabling coverage for not Debug build. "
            "Should configure project with flag: `-D CMAKE_BUILD_TYPE=Debug`")
  endif()
  if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
    get_target_property(library_type ${target} TYPE)
    if (${library_type} STREQUAL "OBJECT_LIBRARY")  # TODO: change to INTERFACE_LIBRARY
      # for header only library '--coverage' should be propagated
      # to target compiling test sources
      target_compile_options(${target} PUBLIC --coverage -fno-inline)
    else()  # STATIC_LIBRARY or SHARED_LIBRARY or OBJECT_LIBRARY
      target_compile_options(${target} PRIVATE --coverage -fno-inline)
    endif()
    # this link option has to be propagated to target linking test executable
    target_link_options(${target} PUBLIC --coverage)
  else()
    message(AUTHOR_WARNING "No known compiler/linker flags for enabling coverage,"
            " CMAKE_CXX_COMPILER_ID : '${CMAKE_CXX_COMPILER_ID}'")
  endif()
endfunction()


## Before building the ${target} remove all coverage data (.gcda) files
function(CleanCoverage target)
  if (NOT ${PROJECT_NAME}_ENABLE_COVERAGE)
    return()
  endif()
  add_custom_command(
    TARGET ${target}
    PRE_BUILD COMMAND
      find ${CMAKE_BINARY_DIR} -type f -name '*.gcda' -exec rm {} +
    COMMENT "Removing all coverage data (.gcda) files."
  )
endfunction()


## Add target for collecting coverage metrics and generating coverage report
function(AddCoverage target)
  if (NOT ${PROJECT_NAME}_ENABLE_COVERAGE)
    return()
  endif()

  add_custom_target(coverage-${target}
    # Clear the counters from any previous runs
    COMMAND ${LCOV_PATH} -d . --zerocounters
    # Run the ${target} executable (unit tests).
    # TARGET_FILE generator will implicitly add a dependency on ${target}
    #  causing it to be build before executing command.
    COMMAND $<TARGET_FILE:${target}>
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Running ${target} to generate coverage data (.gcda)."
  )
  add_dependencies(coverage coverage-${target})
endfunction()
