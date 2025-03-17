add_library(circular_buffer_obj  OBJECT
  "circular_buffer_impl.cpp"
  "circular_buffer.cpp"
  )
target_include_directories(circular_buffer_obj
  PRIVATE
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
  INTERFACE
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)
set_target_properties(circular_buffer_obj  PROPERTIES
    PUBLIC_HEADER "src/circular_buffer/include/circular_buffer/circular_buffer.hpp"
    PUBLIC_HEADER "src/circular_buffer/include/circular_buffer/istorage.hpp"
    PUBLIC_HEADER "src/circular_buffer/include/circular_buffer/storage.hpp"
    POSITION_INDEPENDENT_CODE 1
)

add_library(circular_buffer_lib  STATIC)
target_link_libraries(circular_buffer_lib  circular_buffer_obj)
set_target_properties(circular_buffer_lib  PROPERTIES
  OUTPUT_NAME  "circular_buffer"
)


include(Coverage)
CleanCoverage(circular_buffer_lib)
# include(CppCheck)
# AddCppCheck(circular_buffer_obj)
