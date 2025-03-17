set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

enable_testing()

include(FindGoogleTest)

include(GoogleTest)
include(Coverage)
# include(Memcheck)

# add target gathering all unit tests
if (NOT TARGET exec-utests)
  add_custom_target(exec-utests)
endif()

macro(AddTests target)
  AddCoverage(${target})

  add_custom_target(exec-${target}
    COMMAND $<TARGET_FILE:${target}>
    COMMENT "Executing  ${target}"
  )
  add_dependencies(exec-utests exec-${target})

  target_link_libraries(${target}
    PRIVATE
      GTest::gmock_main
  )

  # Disable unity build for tests. There are several files defining the same
  # function in different anonymous namespaces. This is not allowed in unity
  # builds, as it would result in multiple definitions of the same function.
  set_target_properties(${target}  PROPERTIES  UNITY_BUILD OFF)
  gtest_discover_tests(${target}  DISCOVERY_TIMEOUT 600)

  # AddMemcheck(${target})
endmacro()
