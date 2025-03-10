find_package(GTest 1.10)

if (GTest_FOUND)
  return()
endif()

option(FETCHCONTENT_UPDATES_DISCONNECTED TRUE)
option(FETCHCONTENT_QUIET FALSE)

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY "https://github.com/google/googletest.git"
  GIT_TAG        "v1.16.0"
  GIT_PROGRESS   TRUE
)
# For Windows: Prevent overriding the parent project's
# compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL
  "Use shared (DLL) run-time lib even when Google Test is built as static lib." FORCE)
option(BUILD_GMOCK "Builds the googlemock subproject" ON)
option(INSTALL_GMOCK "Install GMock" OFF)
option(INSTALL_GTEST "Install GTest" OFF)

FetchContent_MakeAvailable(googletest)
