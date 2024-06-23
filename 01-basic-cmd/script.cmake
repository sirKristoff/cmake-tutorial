# An example of a script
# cmake -P script.cmake
cmake_minimum_required(VERSION 3.16)
message([[Testing endianess
  by "TestBigEndian" module]])

# use utility module from CMake distibution
include(TestBigEndian)

# test endianess - result in IS_BIG_ENDIAN variable
test_big_endian(IS_BIG_ENDIAN)
if(IS_BIG_ENDIAN)
    message("BIG_ENDIAN")
else()
    message("LITTLE_ENDIAN")
endif()
