cmake_minimum_required(VERSION 3.20.0)
project(
  TestingSample
  VERSION  0.2
  LANGUAGES  CXX
)

option(TestingSample_BUILD_TESTS
  "Set to ON to build tests" ON)
option(TestingSample_ENABLE_COVERAGE
  "Execute code coverage" ON)
option(TestingSample_DEV_WARNINGS
  "Enable more compiler warnings and warnings as errors" OFF)
option(TestingSample_VERBOSE_BUILD
  "Make build process more verbose and traceable" OFF)


list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

include(Options)

add_subdirectory(src bin)
add_subdirectory(test)
