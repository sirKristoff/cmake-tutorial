add_executable(circular_buffer_test
  "circular_buffer_test.cpp")

target_link_libraries(circular_buffer_test  PRIVATE circular_buffer_lib)

AddTests(circular_buffer_test)
EnableCoverage(circular_buffer_obj)
