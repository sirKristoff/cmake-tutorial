if (NOT ${PROJECT_NAME}_BUILD_TESTS)
  return()
endif()

include(Testing)

add_subdirectory(prio_containers)
