cmake_minimum_required(VERSION 3.20.0)
project(InstallSample CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)  ## Explicitly require supporting standard by compiler
set(CMAKE_CXX_EXTENSIONS OFF)  ## Disallows using extensions like gnu++14

# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
# set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
# set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)

# add_compile_options(-Wall -Wcast-qual -Wextra -Wshadow)
# add_link_options(-Wall -Wcast-qual -Wextra -Wshadow)
# enable_testing()

add_subdirectory(src)
add_subdirectory(test)

# add_executable(main main.cpp)
