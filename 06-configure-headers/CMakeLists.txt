cmake_minimum_required(VERSION 3.16)
project(Configure CXX)

add_executable(configure configure.cpp)
# add_custom_command(
#     TARGET configure POST_BUILD
#     COMMAND $<TARGET_FILE:configure>
#     COMMENT "Execute binary after build"
# )
add_custom_target(run DEPENDS configure
    COMMAND $<TARGET_FILE:configure>
    COMMENT "Execute binary"
)

set(FOO_ENABLE ON)
set(FOO_STRING1 "abc")
set(FOO_STRING2 "def")

configure_file(
    "configure.h.in"
    "configured/configure.h"
    NEWLINE_STYLE WIN32)  # Windows newline style for the output file

    # configure.h will be generated in binary dir
target_include_directories(configure PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
