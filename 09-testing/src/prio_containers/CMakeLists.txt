add_library(prio_containers_obj  OBJECT prio_containers.cpp)
target_include_directories(prio_containers_obj
  PRIVATE
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
  INTERFACE
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)
set_target_properties(prio_containers_obj  PROPERTIES
    PUBLIC_HEADER "src/module/include/prio_containers/priority_stack.hpp"
    POSITION_INDEPENDENT_CODE 1
)

add_library(prio_containers_lib  STATIC)
target_link_libraries(prio_containers_lib  prio_containers_obj)
set_target_properties(prio_containers_lib  PROPERTIES
  OUTPUT_NAME  "prio_containers"
)

# include(Coverage)
# CleanCoverage(prio_containers_lib)
# include(CppCheck)
# AddCppCheck(prio_containers_obj)
