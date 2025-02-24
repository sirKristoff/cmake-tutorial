# enable_testing()



set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/test/bin)
include(GoogleTest)

add_subdirectory(chunk)



try_run(
  run_result
  compile_result
  ${CMAKE_BINARY_DIR}/try_run_test
  ${CMAKE_SOURCE_DIR}/lang_features_test_main.cpp
  RUN_OUTPUT_VARIABLE output)

message("run_result: ${run_result}")
message("compile_result: ${compile_result}")
message("output:\n\"\"\"\n${output}\n\"\"\"")
