cmake_minimum_required(VERSION 3.20.0)
project(TestRun CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)  ## Explicitly require supporting standard by compiler
set(CMAKE_CXX_EXTENSIONS OFF)  ## Disallows using extensions like gnu++14

#[[
  Configure-time commands, try_compile() and try_run(),
  to verify that everything is supported on target platform.
  Checks if code is compiling and executing correctly.
#]]
try_run(
  run_result
  compile_result
  ${CMAKE_BINARY_DIR}/test_output
  ${CMAKE_SOURCE_DIR}/main.cpp
  RUN_OUTPUT_VARIABLE output)

message("run_result: ${run_result}")
message("compile_result: ${compile_result}")
message("output:\n\n${output}")
