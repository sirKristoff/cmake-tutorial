if (NOT TUTORIAL_BUILD_TESTS AND NOT TUTORIAL_ENABLE_COVERAGE)
  return()
endif()

include(Testing)

add_subdirectory(prio_containers)
