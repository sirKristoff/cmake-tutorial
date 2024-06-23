cmake_minimum_required(VERSION 3.16)
project(Variables)

# Long var names
set([[My String 1]] "text 1")
set("My String 2" "text 2")
message(${My\ String\ 1})
message("${My\ String\ 2}")
message("")

# unset variable
unset([[My String 1]])

# Automatic variables
message("\
CMAKE_SOURCE_DIR: ${CMAKE_SOURCE_DIR}
CMAKE_BINARY_DIR: ${CMAKE_BINARY_DIR}
CMAKE_CURRENT_SOURCE_DIR: ${CMAKE_CURRENT_SOURCE_DIR}
CMAKE_CURRENT_BINARY_DIR: ${CMAKE_CURRENT_BINARY_DIR}\
")
message("")

# Environment variables
message("USER: $ENV{USER}")
message("")

set(ENV{CXX} "clang++")
unset(ENV{VERBOSE})

#[[
variable value set during the configuration
is persisted to the generated build system
#]]

message("generated with: " $ENV{my_env})
add_custom_target(EchoEnv ALL
    COMMAND echo "build with: " $ENV{my_env})
set(ENV{my_env} "post_config") # it will not impact outher environment
