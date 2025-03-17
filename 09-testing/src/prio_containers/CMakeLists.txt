add_library(prio_containers_lib  INTERFACE)

target_include_directories(prio_containers_lib
  INTERFACE
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

set_target_properties(prio_containers_lib  PROPERTIES
    PUBLIC_HEADER "src/prio_containers/include/prio_containers/priority_stack.hpp"
    POSITION_INDEPENDENT_CODE 1
)

include(Coverage)
CleanCoverage(prio_containers_lib)
# include(CppCheck)
# AddCppCheck(prio_containers_obj)
