add_executable(prio_containers_test
  "priority_stack_test.cpp")

target_link_libraries(prio_containers_test  PRIVATE prio_containers_lib)

AddTests(prio_containers_test)
EnableCoverage(prio_containers_obj)
