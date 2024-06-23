cmake_minimum_required(VERSION 3.16)
# delimiter (;) is delimiter of arguments
project(Syntax;CXX)

#[[
 Bracket argument is not evaluated
 it's verbatim multiline string
#]]
message([[Testing endianess
  by "TestBigEndian" module]])

# use utility module from CMake distibution
include(TestBigEndian)

# test endianess - result in IS_BIG_ENDIAN variable
test_big_endian(IS_BIG_ENDIAN)
if(IS_BIG_ENDIAN)
    set(endianess "BIG_ENDIAN")
else()
    set(endianess "LITTLE_ENDIAN")
endif()

message("${endianess} system for $ENV{USER}")
unset(endianess)

#[[ NOTE:
variable value set during the configuration
is persisted to the generated build system
]]
