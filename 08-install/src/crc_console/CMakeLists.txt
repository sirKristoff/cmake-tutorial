add_executable(crc_console  main.cpp)
target_link_libraries(crc_console  crc_static)
set_target_properties(crc_console  PROPERTIES
  OUTPUT_NAME  "crc"
)
