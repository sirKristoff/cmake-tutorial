cmake_minimum_required(VERSION 3.20.0)
project(
  TestingSample
  VERSION  0.2
  LANGUAGES  CXX
)

option(TUTORIAL_BUILD_TESTS
  "Set to ON to build tests" ON)
option(TUTORIAL_ENABLE_COVERAGE
  "Execute code coverage" ON)
option(TUTORIAL_DEV_WARNINGS
  "Enable more compiler warnings and warnings as errors" OFF)


list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

include(Options)

add_subdirectory(src bin)
add_subdirectory(test)
