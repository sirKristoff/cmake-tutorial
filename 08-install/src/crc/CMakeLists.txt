add_library(crc_obj OBJECT crc.cpp)

# BUILD_INTERFACE  for regular builds
# INSTALL_INTERFACE  for installation
target_include_directories(crc_obj PUBLIC # INTERFACE
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
  "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)
set_target_properties(crc_obj PROPERTIES
  PUBLIC_HEADER src/crc/include/crc/crc.hpp
  POSITION_INDEPENDENT_CODE 1
)

try_compile(
  present_cxx_std_20_concepts
  SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/test/compile_feature_cxx_20_concepts.cpp"
  LOG_DESCRIPTION "Check individual compile feature for c++20 concepts"
  OUTPUT_VARIABLE output
  CXX_STANDARD ${CMAKE_CXX_STANDARD}
  CXX_STANDARD_REQUIRED ${CMAKE_CXX_STANDARD_REQUIRED}
  CXX_EXTENSIONS ${CMAKE_CXX_EXTENSIONS}
)
## note: No individual compile feature for c++20 concepts
# list(FIND CMAKE_CXX_COMPILE_FEATURES  cxx_std_20  present_cxx_std_20_concepts)
# if (NOT present_cxx_std_20_concepts EQUAL -1)
if (present_cxx_std_20_concepts)
    message("cxx_std_20_concepts compile feature  present")
    target_compile_definitions(crc_obj  PRIVATE HAVE_CXX_STD_20_CONCEPTS)
else()
    message("cxx_std_20_concepts compile feature  absent")
    message(VERBOSE "try_compile output:\n\"\"\"\n${output}\n\"\"\"\n")
endif()


add_library(crc_shared SHARED)
target_link_libraries(crc_shared crc_obj)
set_target_properties(crc_shared  PROPERTIES
  OUTPUT_NAME  "crc"
)


add_library(crc_static STATIC)
target_link_libraries(crc_static crc_obj)
set_target_properties(crc_static  PROPERTIES
  OUTPUT_NAME  "crc"
)
