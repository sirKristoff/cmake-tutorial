
if (TUTORIAL_ENABLE_COVERAGE AND NOT TARGET coverage)
  find_program(LCOV_PATH lcov REQUIRED)
  find_program(GENHTML_PATH genhtml REQUIRED)

  # this target has to be build manually: `cmake --build './build' -t coverage`
  add_custom_target(coverage ALL
    # Collect metrics for the solution from the current directory.
    COMMAND ${LCOV_PATH} -d . --capture  -o coverage.info
    # Remove unwanted coverage data on system headers.
    COMMAND ${LCOV_PATH} -r coverage.info '/usr/include/*'  -o filtered.info
    # Generate HTML report in the coverage directory
    COMMAND ${GENHTML_PATH} -o coverage filtered.info --legend
    # Remove temporary .info files.
    COMMAND rm -rf  coverage.info  filtered.info
    BYPRODUCTS  ${CMAKE_BINARY_DIR}/coverage
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Collecting coverage metrics and generating HTML report in `${CMAKE_BINARY_DIR}/coverage'."
  )
endif()

## Enable code coverage by adding extra compiler/linker flags
##   Compilation process will generate coverage note (.gcno) files.
function(EnableCoverage target)
  if (NOT TUTORIAL_ENABLE_COVERAGE)
    return()
  endif()
  if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
    target_compile_options(${target} PUBLIC --coverage -fno-inline -O0 -g)
    target_link_options(${target} PUBLIC --coverage)
  endif()
endfunction()


## Before building the ${target} remove all coverage data (.gcda) files
function(CleanCoverage target)
  if (NOT TUTORIAL_ENABLE_COVERAGE)
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
  if (NOT TUTORIAL_ENABLE_COVERAGE)
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
