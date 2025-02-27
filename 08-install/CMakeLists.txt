cmake_minimum_required(VERSION 3.20.0)
project(
    InstallSample
    VERSION  1.4.2
    LANGUAGES  CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)  ## Explicitly require supporting standard by compiler
set(CMAKE_CXX_EXTENSIONS OFF)  ## Disallows using extensions like gnu++14

add_compile_options(-Wall -Wcast-qual -Wextra -Wshadow -fconcepts)
add_link_options(-Wall -Wcast-qual -Wextra -Wshadow)


list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

add_subdirectory(src bin)

include(Install)
